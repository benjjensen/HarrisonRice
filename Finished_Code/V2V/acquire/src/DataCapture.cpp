#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

#include <unistd.h>
#include <fcntl.h>

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
	// If the meta_filename is also empty, generate it from the data_filename.
	// If generating the meta_filename failed, print an error message and return 1.
	if(filename == "")
	{
		if(meta_filename == "" && !generate_meta_filename())
		{
			std::cerr << "ERROR: no file to write this DataCapture to" << std::endl;
			return 1;
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
	out << "notes " << notes << "`" << std::endl;
	out << "positions " << gps_positions.size() << std::endl;
	for(std::vector<GPSPosition>::iterator it = gps_positions.begin(); it != gps_positions.end(); ++it)
	{
		out << (*it) << std::endl;
	}
}

void DataCapture::read_from_stream(std::istream &in)
{
	gps_positions.clear();
	
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
			// Strip whitespace away from the data filename.
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
		else if(type == "positions")
		{
			int num_positions;
			line >> num_positions;
			gps_positions.reserve(num_positions);
			for(int index = 0; index < num_positions; ++index)
			{
				GPSPosition position;
				in >> position;
				gps_positions.push_back(position);
			}
			// Ignore the last endline character after the list of positions.
			in.ignore();
		}
		else
		{
			std::cerr << "ERROR: unkown line in capture stream: " << line_str << std::endl;
		}
	}
}
bool DataCapture::generate_meta_filename()
{
	if(data_filename != "")
	{
		size_t extension_index = data_filename.rfind('.');
		size_t last_slash_index = data_filename.find_last_of("\\/");
		
		if(extension_index != std::string::npos && last_slash_index != std::string::npos)
		{
			if(extension_index <= last_slash_index)
			{
				// If the last period is before the last slash in the filename, we know
				// that that period does not represent the beginning of a file extension.
				extension_index = std::string::npos;
			}
		}

		if(extension_index == std::string::npos)
		{
			meta_filename = data_filename + CAPTURE_META_ENDING;
		}
		else
		{
			meta_filename = data_filename.substr(0, extension_index) + CAPTURE_META_ENDING;
		}
		return true;
	}
	else
	{
		return false;
	}
}
	
int DataCapture::reserve_meta_file_space()
{
	// If there's no meta_filename, we can't reserve space for the file:
	if(meta_filename == "" && !generate_meta_filename())
	{
		return -1;
	}
	
	int meta_file_descriptor = open(meta_filename.c_str(), O_WRONLY | O_CREAT);
	if(meta_file_descriptor < 0)
	{
		return -1;
	}
	
	// Reserve a full megabyte to be sure we have enough.
	int result = posix_fallocate(meta_file_descriptor, 0, 1024 * 1024);
	close(meta_file_descriptor);
	meta_file_descriptor = -1;
	
	return result;
}









GPSPosition::GPSPosition()
{
	year = 2018;
	month = 8;
	date = 27;
	hour = 12;
	minute = 0;
	second = 0;
	
	blocks_captured = 0;
	
	latitude = 4500;
	
	longitude = 11100;
	
	altitude_feet = 4500;
}
	
void GPSPosition::write_to_stream(std::ostream &out)
{
	out << std::setfill('0');
	out << std::setw(2) << year << '-' << std::setw(2) << month << '-' << std::setw(2) << date << ' ';
	out << std::setw(2) << hour << ':' << std::setw(2) << minute << ':' << std::setw(2) << second << FIELD_DELIMITER;
	
	out << blocks_captured << " blocks" << FIELD_DELIMITER;
	
	out << latitude << FIELD_DELIMITER;
	out << longitude << FIELD_DELIMITER;
	
	out << altitude_feet;
}
void GPSPosition::read_from_stream(std::istream &in)
{
	in >> year;
	// Ignore the '-' character.
	in.ignore();
	in >> month;
	// Ignore the '-' character.
	in.ignore();
	in >> date;
	
	in >> hour;
	// Ignore the ':' character.
	in.ignore();
	in >> minute;
	// Ignore the ':' character.
	in.ignore();
	in >> second;
	
	// Ignore the field delimiter.
	in.ignore();
	
	in >> blocks_captured;
	// Ignore the " blocks" string here.
	in.ignore(7);
	
	// Ignore the field delimiter.
	in.ignore();
	
	in >> latitude;
	
	// Ignore the field delimiter.
	in.ignore();
	
	in >> longitude;
	
	// Ignore the field delimiter.
	in.ignore();
	
	in >> altitude_feet;
}

std::istream & operator >> (std::istream &in, GPSPosition &position)
{
	position.read_from_stream(in);
	return in;
}
std::ostream & operator << (std::ostream &out, GPSPosition &position)
{
	position.write_to_stream(out);
	return out;
}
