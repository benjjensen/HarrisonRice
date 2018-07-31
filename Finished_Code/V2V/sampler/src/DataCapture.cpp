#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "DataCapture.h"

DataCapture::DataCapture()
{
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
	filename = "";
}

std::string DataCapture::get_date_string()
{
	std::stringstream ss;
	ss << year << '-' << month << '-' << date;
	return ss.str();
}
std::string DataCapture::get_time_string()
{
	std::stringstream ss;
	ss << hour << ':' << minute << ':' << second;
	return ss.str();
}

int DataCapture::write_to_file(std::string filename)
{
	if(filename == "")
	{
		filename = this->filename;
	}
	else
	{
		this->filename = filename;
	}
	
	std::ofstream file;
	file.open(filename.c_str());
	if(!file.is_open()) 
	{
		std::cerr << "ERROR: unable to open file " << filename << std::endl;
		return 1;
	}
	
	file << "name " << name << std::endl;
	file << "date " << get_date_string() << std::endl;
	file << "time " << get_time_string() << std::endl;
	file << "duration " << duration << std::endl;
	file << "size " << size << std::endl;
	file << "notes " << notes << "`";
	
	file.close();
	return 0;
}

int DataCapture::read_from_file(std::string filename)
{
	if(filename == "")
	{
		filename = this->filename;
	}
	else
	{
		this->filename = filename;
	}
	
	std::ifstream file;
	file.open(filename.c_str());
	if(!file.is_open()) 
	{
		std::cerr << "ERROR: unable to open file " << filename << std::endl;
		return 1;
	}
	
	std::string line_str;
	while(std::getline(file, line_str))
	{
		std::stringstream line(line_str);
		std::string type;
		line >> type;
		
		if(type == "name")
		{
			line >> name;
		}
		else if(type == "date")
		{
			line >> year;
			line.ignore();
			line >> month;
			line.ignore();
			line >> date;
		}
		else if(type == "time")
		{
			line >> hour;
			line.ignore();
			line >> minute;
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
			std::string remaining_in_line;
			std::string rest_of_notes;
			std::getline(line, remaining_in_line);
			std::getline(file, rest_of_notes, '`');
			notes = remaining_in_line + '\n' + rest_of_notes;
		}
		else
		{
			std::cerr << "ERROR: unkown line in capture meta file: " << line_str << std::endl;
		}
	}
	
	file.close();
	return 0;
}
