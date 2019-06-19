// ExifToolWrapper.h
//
// Wrapper class for calling the command-line version of 
// ExifTool (http://www.sno.phy.queensu.ca/~phil/exiftool/)
// 
// Copyright 2007-2009 Mark Borg
//
// This code is free software; you can redistribute it and/or modify it
// under the same terms as Perl itself.

#ifndef __EXIF_TOOL_WRAPPER_H
#define __EXIF_TOOL_WRAPPER_H


#include <string>
#include <vector>


struct ExifTagItem
{
	std::string group;
	std::string name;
	std::string value;
};


class ExifToolWrapper : public std::vector< ExifTagItem >
{
public:

	static bool check_tool_exists();

	void run( std::string filename, bool removeWhitespaceInTagNames = false );

	bool has_exif_data() const
	{
		return this->size() > 0;
	}


	ExifTagItem* find( std::string tagname );


	// This method saves EXIF data to an external file (<file>.exif).
	// Only tags with group EXIF are saved.
	static bool saveExifData( std::string source_image, std::string destination_exif_file );

	// This method writes EXIF data to the given destination image file (must exist beforehand).
	static bool writeExifData( std::string source_exif_file, std::string destination_image );
};


#endif