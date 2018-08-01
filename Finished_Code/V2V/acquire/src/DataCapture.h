#pragma once

#include <string>
#include <ctime>

class DataCapture
{
public:
	std::string name;

	time_t year;
	time_t month;
	time_t date;
	time_t hour;
	time_t minute;
	time_t second;
	
	size_t duration;
	size_t size;
	
	std::string notes;
	
	std::string meta_filename;
	std::string data_filename;
	
	std::string get_date_string();
	std::string get_time_string();
	
	DataCapture();
	
	int write_to_file(std::string filename = "");
	int read_from_file(std::string filename = "");
	void write_to_stream(std::ostream &out);
	void read_from_stream(std::istream &in);
};
