#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

#include "DataCapture.h"

DataCapture::DataCapture()
{
	// Set some reasonable defaults for the member fields:
	name = "";
	year = 2018;
	month = 8;
	date = 1;
	hour = 12;
	minute = 0;
	second = 0;
	duration = 0;
	size = 0;
	notes = "";
	data_filename = "";
	meta_filename = "";
}

std::string DataCapture::get_date_string()
{
	std::stringstream ss;
	ss << std::setfill('0') << year << '-' << std::setw(2) << month << '-' << std::setw(2) << date;
	return ss.str();
}
std::string DataCapture::get_time_string()
{
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << hour << ':' << std::setw(2) << minute << ':' << std::setw(2) << second;
	return ss.str();
}

int DataCapture::write_to_file(std::string filename)
{
	// If the filename provided is empty, use the meta_filename.
	// If the meta_filename is also empty, use the data_filename with
	// the file extension removed and CAPTURE_META_ENDING appended.
	// If the data_filename is also empty, print an error message and return.
	if(filename == "")
	{
		if(meta_filename == "")
		{
			if(data_filename != "")
			{
				size_t extension_index = data_filename.rfind('.');
	
				if(extension_index == std::string::npos)
				{
					meta_filename = data_filename + CAPTURE_META_ENDING;
				}
				else
				{
					meta_filename = data_filename.substr(0, extension_index) + CAPTURE_META_ENDING;
				}
			}
			else
			{
				std::cerr << "ERROR: no file to write this DataCapture to" << std::endl;
				return 1;
			}
		}
		filename = meta_filename;
	}
	// If the user provided a filename, save that as the meta_filename:
	else
	{
		meta_filename = filename;
	}
	
	std::ofstream file;
	file.open(filename.c_str());
	if(!file.is_open()) 
	{
		std::cerr << "ERROR: unable to open file " << filename << std::endl;
		return 1;
	}
	
	write_to_stream(file);
	
	file.close();
	return 0;
}

int DataCapture::read_from_file(std::string filename)
{
	if(filename == "")
	{
		filename = meta_filename;
	}
	// If the user provided a filename, save that as the meta_filename:
	else
	{
		meta_filename = filename;
	}
	
	std::ifstream file;
	file.open(filename.c_str());
	if(!file.is_open()) 
	{
		std::cerr << "ERROR: unable to open file " << filename << std::endl;
		return 1;
	}
	
	read_from_stream(file);
	
	file.close();
	return 0;
}

void DataCapture::write_to_stream(std::ostream &out)
{
	out << "name " << name << std::endl;
	out << "date " << get_date_string() << std::endl;
	out << "time " << get_time_string() << std::endl;
	out << "duration " << duration << std::endl;
	out << "size " << size << std::endl;
	out << "datafile " << data_filename << std::endl;
	out << "notes " << notes << "`";
}

void DataCapture::read_from_stream(std::istream &in)
{
	// For each line in the stream, read the first word to figure out the type.
	// Based on the type, read in the rest of the info for that type.
	std::string line_str;
	while(std::getline(in, line_str))
	{
		std::stringstream line(line_str);
		std::string type;
		line >> type;
		
		if(type == "name")
		{
			std::getline(line, name);
			// Strip whitespace away from the name:
			if(name != "")
			{
				int start_index = name.find_first_not_of(" \n\t\r");
				if(start_index != -1)
				{
					int end_index = name.find_last_not_of(" \n\t\r");
					name = name.substr(start_index, end_index - start_index + 1);
				}
				else
				{
					name = "";
				}
			}
		}
		else if(type == "date")
		{
			line >> year;
			// Ignore the '-' character:
			line.ignore();
			line >> month;
			// Ignore the '-' character:
			line.ignore();
			line >> date;
		}
		else if(type == "time")
		{
			line >> hour;
			// Ignore the ':' character:
			line.ignore();
			line >> minute;
			// Ignore the ':' character:
			line.ignore();
			line >> second;
		}
		else if(type == "duration")
		{
			line >> duration;
		}
		else if(type == "size")
		{
			line >> size;
		}
		else if(type == "notes")
		{
			// Read in the rest of the stream until the backquote character:
			std::string remaining_in_line;
			std::getline(line, remaining_in_line, '`');
			// If this line didn't have the backquote in it, read until we
			// find it:
			if(line_str.find('`') == std::string::npos)
			{
				std::string rest_of_notes;	
				std::getline(in, rest_of_notes, '`');
				notes = remaining_in_line + '\n' + rest_of_notes;
			}
			// If this line had the backquote in it, that's it:
			else
			{
				notes = remaining_in_line;
			}
			// Strip whitespace away from the notes:
			if(notes != "")
			{
				int start_index = notes.find_first_not_of(" \n\t\r");
				if(start_index != -1)
				{
					int end_index = notes.find_last_not_of(" \n\t\r");
					notes = notes.substr(start_index, end_index - start_index + 1);
				}
				else
				{
					notes = "";
				}
			}
		}
		else if(type == "datafile")
		{
			std::getline(line, data_filename);
			// Strip whitespace away from the data filename:
			if(data_filename != "")
			{
				int start_index = data_filename.find_first_not_of(" \n\t\r");
				if(start_index != -1)
				{
					int end_index = data_filename.find_last_not_of(" \n\t\r");
					data_filename = data_filename.substr(start_index, end_index - start_index + 1);
				}
				else
				{
					data_filename = "";
				}
			}
		}
		else
		{
			std::cerr << "ERROR: unkown line in capture stream: " << line_str << std::endl;
		}
	}
}
