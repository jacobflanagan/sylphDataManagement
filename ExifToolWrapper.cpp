// ExifToolWrapper.cpp
//
// Wrapper class for calling the command-line version of 
// ExifTool (http://www.sno.phy.queensu.ca/~phil/exiftool/)
// 
// Copyright 2007-2009 Mark Borg
//
// Revisions:   2016-01-20 - Vandmoon fixed some compile problems
//              (see http://u88.n24.queensu.ca/exiftool/forum/index.php/topic,8009.0.html)
//
// This code is free software; you can redistribute it and/or modify it
// under the same terms as Perl itself.

#define _AFXDLL
#include "ExifToolWrapper.h"
//#include "error_codes.h"
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#include <windows.h>
#include <iostream>
#include <afxwin.h>

#define CREATE_PIPE_ERROR -8
#define DUPLICATE_HANDLE_ERROR -9
#define CREATE_PROCESS_ERROR -10

using namespace std;

// gets the path from where the executable is being run
string GetAppPath()
{
	char AppPath[ MAX_PATH ];
	::GetModuleFileName( NULL, AppPath, MAX_PATH );
	string str( AppPath );
	int pos = str.find_last_of( "\\" );
	if ( pos != string::npos )
		return str.substr( 0, pos );
	return str;
}


//<<<2009/12/13 Richard0610@seed.net.tw add ; We need 2 thread,so prepare it.
/*
For ex:
Standard Output: print("Image Height                    : 2304")
Error Output:print("File not found: D:/tmp/RAW/SpeedTest/t0053.jpg")
In the worse case,it will mix and become
ImaFile not found: D:/tmp/RAW/SpeedTest/t0053.jpgge Height                    : 2304
*/
struct ReadThreadParam
{
	LPCTSTR		pszThreadName;
	HANDLE		hReadPipe;
	string*		poutput;
};

UINT AFX_CDECL ReadThread(LPVOID pData)
{
	ReadThreadParam*	pParam = (ReadThreadParam*)pData;
	HANDLE	hReadPipe = pParam->hReadPipe;
	string*	poutput = pParam->poutput;

	for (;;)     
	{     
		char chReadBuffer[65];		// pipe read buffer 
		DWORD cchReadBuffer;		// number of bytes read or to be written 

		memset( chReadBuffer, 0, sizeof(chReadBuffer) );
		BOOL ok = ReadFile(	hReadPipe,					// read handle 
							chReadBuffer,				// buffer for incoming data 
							sizeof(chReadBuffer)-1,		// number of bytes to read 
							&cchReadBuffer,				// number of bytes actually read 
							NULL );						// no overlapped reading 
		
		if ( ! ok && ( GetLastError() == ERROR_BROKEN_PIPE ) )         
			break;  // child has died 
		if ( ok && cchReadBuffer )       
			*poutput += chReadBuffer;
	}   
	//each thread close their read pipe themself.
	CloseHandle(hReadPipe);
	
	return 0; 
}
//2009/12/13 Richard0610@seed.net.tw add ; We need 2 thread,so prepare it. >>>

// the function below is the same as _popen, except that the created child process's 
// console is created with the SW_HIDE startup setting.
string my_popen( string cmd )
{
	// set up the security attributes for the anonymous pipe 
	SECURITY_ATTRIBUTES saPipe;  
	saPipe.nLength = sizeof( SECURITY_ATTRIBUTES );   
	saPipe.lpSecurityDescriptor = NULL;   
	saPipe.bInheritHandle = TRUE;		// In order for the child to be able to write to the pipe, the handle must be marked as inheritable by setting this flag
	
	// create the anonymous pipe
	HANDLE hReadPipe, hWritePipe;
	BOOL ok = CreatePipe(	&hReadPipe,		// read handle 
							&hWritePipe,	// write handle, used as stdout by child 
							&saPipe,		// security descriptor 
							0 );			// pipe buffer size 
	if ( ! ok )
		throw CREATE_PIPE_ERROR;

	//2009/12/13 Richard0610@seed.net.tw add ; We need to separate Error Code to avoid they mix together
	//But I don't know if I should change readerr pipe to ' not inherit' , however,if you think that is good for you,do it yourslef.
	HANDLE hReadErrPipe, hWriteErrPipe;
	ok = CreatePipe(		&hReadErrPipe,		// read handle 
							&hWriteErrPipe,	// write handle, used as stdout by child 
							&saPipe,		// security descriptor 
							0 );			// pipe buffer size 
	if ( ! ok )
		throw CREATE_PIPE_ERROR;
	
	// Now we need to change the inheritable property for the readable end of the pipe so 
	// that the child will not inherit that handle as a "garbage" handle. This will keep us 
	// from having extra, unclosable handles to the pipe. 
	ok = DuplicateHandle(	GetCurrentProcess(),	// source process 
							hReadPipe,				// handle to duplicate
							GetCurrentProcess(),	// destination process 
							NULL,					// new handle - don't want one, change original handle
							0,						// new access flags - ignored since DUPLICATE_SAME_ACCESS
							FALSE,					// make it *not* inheritable 
							DUPLICATE_SAME_ACCESS );   
	if ( ! ok )
		throw DUPLICATE_HANDLE_ERROR;
	
	////2009/12/13 Richard0610@seed.net.tw delete ; we had hWriteErrPipe instead
	//// In most cases you can get away with using the same anonymous pipe write handle for 
	//// both the child's standard output and standard error, but this may cause problems if 
	//// the child app explicitly closes one of its standard output or error handles. If that 
	//// happens, the anonymous pipe will close, since the child's standard output and error 
	//// handles are really the same handle. The child won't be able to write to the other 
	//// write handle since the pipe is now gone, and parent reads from the pipe will return   
	//// ERROR_BROKEN_PIPE and child output will be lost. To solve this problem, simply 
	//// duplicate the write end of the pipe to create another distinct, separate handle to 
	//// the write end of the pipe. One pipe write handle will serve as standard out, the 
	//// other as standard error. Now *both* write handles must be closed before the write 
	//// end of the pipe actually closes.
	//HANDLE hWritePipe2;
	//ok = DuplicateHandle(	GetCurrentProcess(),	// source process
	//						hWritePipe,				// handle to duplicate 
	//						GetCurrentProcess(),	// destination process
	//						&hWritePipe2,			// new handle, used as stderr by child 
	//						0,		// new access flags - ignored since DUPLICATE_SAME_ACCESS 
	//						TRUE,					// it's inheritable 
	//						DUPLICATE_SAME_ACCESS );   
	//if ( ! ok )
	//	throw DUPLICATE_HANDLE_ERROR;
	
	// Set up the STARTUPINFO structure for the CreateProcess() call 
	STARTUPINFO si;  
	memset( &si, 0, sizeof(si) );   
	si.cb = sizeof(si);    
	
	// If using the STARTUPINFO STARTF_USESTDHANDLES flag, be sure to set the CreateProcess 
	// fInheritHandles parameter too TRUE so that the file handles specified in the 
	// STARTUPINFO structure will be inheritied by the child. Note that we don't specify a 
	// standard input handle; the child will not inherit a valid input handle, so if it 
	// reads from stdin, it will encounter errors. 
	si.hStdInput = hWriteErrPipe; 
	si.hStdOutput = hWritePipe;			// write end of the pipe 
	si.hStdError = hWriteErrPipe;		// duplicate of write end of the pipe 

	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;


	PROCESS_INFORMATION pi;  
	char szArgs[256];  
	strcpy_s( szArgs, cmd.c_str() );


	// Now create the child process, inheriting handles 
	ok = CreateProcess(	NULL,			// filename 
						szArgs,			// full command line for child 
						NULL,			// process security descriptor 
						NULL,			// thread security descriptor 
						TRUE,			// inherit handles? Also use if STARTF_USESTDHANDLES 
						0,				// creation flags 
						NULL,			// inherited environment address 
						NULL,			// startup dir; NULL = start in current 
						&si,			// pointer to startup info (input) 
						&pi );			// pointer to process info (output) 
	if ( ! ok )
		throw CREATE_PROCESS_ERROR;

//	DWORD rc;
//	ok = GetExitCodeProcess( pi.hProcess, &rc );

	// We can close the returned child process handle and thread handle as we won't be 
	// needing them; you could, however, wait on the process handle to wait until the 
	// child process terminates. 
	CloseHandle( pi.hThread );   
	CloseHandle( pi.hProcess );    

	
	// We need to close our instances of the inheritable pipe write handle now that it's 
	// been inherited so that all open handles to the pipe are closed when the child 
	// process ends and closes its handles to the pipe. 
	ok = CloseHandle( hWritePipe );   
	ok = CloseHandle( hWriteErrPipe );   

	//<<<2009/12/13 Richard0610@seed.net.tw change ; We need 2 thread to read both of them
	ReadThreadParam Param, Param2;
	string			output1, output2;

	Param.pszThreadName = _T("Read");	//it's helpful for your debuging
	Param.hReadPipe = hReadPipe;
	Param.poutput = &output1;

	Param2.pszThreadName = _T("Err");
	Param2.hReadPipe = hReadErrPipe;
	Param2.poutput = &output2;

	CWinThread*		pReadThread = AfxBeginThread(ReadThread, &Param);
	CWinThread*		pReadThread2 = AfxBeginThread(ReadThread, &Param2);

	//wait both of them
	//Wait 2 thread till 1 error or all pass
	HANDLE hWait[] = {pReadThread->m_hThread, pReadThread2->m_hThread};
	WaitForMultipleObjects(2, hWait, true, INFINITE);

	//then combine.
	string	output;
	output = output1 + output2;
	
	/*
	// read from the pipe until we get an ERROR_BROKEN_PIPE 
	string output;
	for (;;)     
	{     
		char chReadBuffer[65];		// pipe read buffer 
		DWORD cchReadBuffer;		// number of bytes read or to be written 

		memset( chReadBuffer, 0, sizeof(chReadBuffer) );
		ok = ReadFile(	hReadPipe,					// read handle 
						chReadBuffer,				// buffer for incoming data 
						sizeof(chReadBuffer)-1,		// number of bytes to read 
						&cchReadBuffer,				// number of bytes actually read 
						NULL );						// no overlapped reading 
		
		if ( ! ok && ( GetLastError() == ERROR_BROKEN_PIPE ) )         
			break;  // child has died 
		if ( ok && cchReadBuffer )       
			output += chReadBuffer;
	}   

	// close the trace file, pipe handles 
	CloseHandle( hReadPipe ); 
	*/
	//2009/12/13 Richard0610@seed.net.tw change ; We need 2 thread to read both of them >>>

	return output;
}

void ExifToolWrapper::run( string filename, bool removeWhitespaceInTagNames )
{
	// exiftool command
	string toolPath = GetAppPath();
	toolPath += "/exiftool.exe ";
	if ( removeWhitespaceInTagNames )
		toolPath += "-s ";
	toolPath += "-fast -G -t -m -q -q ";
	toolPath += "\"" + filename + "\"";

	string output = my_popen( toolPath );

	// parse the output into tags
	this->clear();
	while ( output.length() > 0 )
	{
		int epos = output.find_first_of( '\r' );

		if ( epos == string::npos )
			epos = output.length();
		string tmp = output.substr( 0, epos );
		int tpos1 = tmp.find_first_of( '\t' );
		int tpos2 = tmp.find_first_of( '\t', tpos1+1 );

		if ( tpos1 != string::npos && tpos2 != string::npos )
		{
			string taggroup = tmp.substr( 0, tpos1 );
			++tpos1;
			string tagname = tmp.substr( tpos1, tpos2 - tpos1 );
			++tpos2;
			string tagvalue = tmp.substr( tpos2, tmp.length() - tpos2 );

			// special processing for tags with binary data 
			tpos1 = tagvalue.find( ", use -b option to extract" );
			if ( tpos1 != string::npos )
				tagvalue.erase( tpos1, 26 );

			ExifTagItem itm;
			itm.name = tagname;
			itm.value = tagvalue;
			itm.group = taggroup;
			this->push_back( itm );
		}

		// is \r followed by \n ?
		if ( epos < output.length() )
			epos += ( output[ epos+1 ] == '\n' ) ? 2 : 1;
		output = output.substr( epos, output.length() - epos );
	}

}


bool ExifToolWrapper::check_tool_exists()
{
	string toolPath = GetAppPath();
	toolPath += "/exiftool.exe ";
	toolPath += "-ver";

	string output;

	try
	{
		output = my_popen( toolPath );
	}
	catch(...)
	{
	}

	// check the output
	if ( output.length() < 4 )
		return false;

    // (could check version number here if you care)
	return true;
}


// This method saves EXIF data to an external file (<file>.exif).
// Only tags with group EXIF are saved.
bool ExifToolWrapper::saveExifData( string source_image, string destination_exif_file )
{
	// exiftool command
	string toolPath = GetAppPath();
	toolPath += "/exiftool.exe ";
	toolPath += "-fast -m -q -q -tagsfromfile ";
	toolPath += "\"" + source_image + "\" -exif ";
	toolPath += "\"" + destination_exif_file + "\"";

	string output = my_popen( toolPath );

	if ( output.find( "Error" ) != string::npos )
		return false;

	return true;
}


// This method writes EXIF data to the given destination image file (must exist beforehand).
bool ExifToolWrapper::writeExifData( string source_exif_file, string destination_image )
{
	// exiftool command
	string toolPath = GetAppPath();
	toolPath += "/exiftool.exe ";
	toolPath += "-fast -m -q -q -TagsFromFile ";
	toolPath += "\"" + source_exif_file + "\"";
	toolPath += " -all:all ";
	toolPath += "\"" + destination_image + "\"";

	string output = my_popen( toolPath );

	if ( output.find( "Error" ) != string::npos )
		return false;

	return true;
}



ExifTagItem* ExifToolWrapper::find( string tagname )
{
	ExifToolWrapper::iterator i = begin();
	for ( ; i != end(); ++i )
	{
		if ( i->name.compare( tagname ) == 0 )
			return & (*i);
	}

	return 0;
}

