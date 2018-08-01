#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

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
	if(filename == "")
	{
		filename = meta_filename;
	}
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
	std::string line_str;
	while(std::getline(in, line_str))
	{
		std::stringstream line(line_str);
		std::string type;
		line >> type;
		
		if(type == "name")
		{
			std::getline(line, name);
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
			std::getline(line, remaining_in_line, '`');
			if(line_str.find('`') == -1)
			{
				std::string rest_of_notes;	
				std::getline(in, rest_of_notes, '`');
				notes = remaining_in_line + '\n' + rest_of_notes;
			}
			else
			{
				notes = remaining_in_line;
			}
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
