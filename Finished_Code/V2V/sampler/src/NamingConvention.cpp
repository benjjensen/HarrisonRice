#include <string>
#include <list>
#include <map>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>

#include "NamingConvention.h"

void NamingConvention::p_add_option(std::string field_code, std::string new_option_code, std::string new_option_name)
{
	mFieldOptions[field_code][new_option_code] = new_option_name;
}
void NamingConvention::p_add_field(std::string field_code, std::string field_name, std::string default_option, std::string field_separator)
{
	mFieldOrder.push_back(field_code);
	mFieldNames[field_code] = field_name;
	// Create the list of options (intentionally don't assign anything here):
	mFieldOptions[field_code];
	mFieldDefaults[field_code] = default_option;
	mFieldSeparators[field_code] = field_separator;
	
	if(mFieldValues[field_code] == "")
	{
		// Set the value for this field to the default value if no other value is already present.
		mFieldValues[field_code] = default_option;
	}
}

void NamingConvention::clear_naming_convention()
{
	mFieldOrder.clear();
	mFieldNames.clear();
	mFieldDefaults.clear();
	mFieldSeparators.clear();
	mFieldOptions.clear();
}

int NamingConvention::load_fields_from_file(std::string filename)
{
	if(filename == "")
	{
		// Default to continue using the current file.
		filename = mCurrentFilename;
	}
	else
	{
		// If the user specifies a file, that becomes the new current file.
		mCurrentFilename = filename;
	}
	std::ifstream file;
	file.open(filename.c_str());
	if(!file.is_open()) {
		std::cout << "ERROR: failure to open file " << filename << std::endl;
		return 1;
	}
	// Wait to clear the naming convention until after we know we have a file to read from.
	clear_naming_convention();
	
	char line_marker;
	std::string current_field;
	std::string code;
	std::string name;
	std::string separator;
	std::string default_option;
	
	// Loop until we run out of things to read. The presence of file.eof() here is more of a
	// failsafe than anything else--every file should end with '~', and we watch for that and
	// quit the loop from inside once it appears.
	while(!file.eof() && line_marker != EOF_MARKER) {
		file >> line_marker;
		
		if(line_marker == COMMENT_MARKER)
		{
			// Read the rest of the line and ignore it because it's a comment.
			std::getline(file, name);
			continue;
		}
		if(line_marker == EOF_MARKER)
		{
			// Stop reading from the file because we've reached the end.
			break;	
		}
		
		file >> code;
		
		if(line_marker == FIELD_MARKER)
		{
			// Extra things to read before the name if it's a field entry.
			file >> default_option;
			file >> separator;
		}
		
		// The remainder of the line (except leading whitespace) is used as the name
		// of the entry.
		file.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
		std::getline(file, name);
		
		switch(line_marker) 
		{
		case FIELD_MARKER:
			if(separator == SEPARATOR_EMPTY) {
				// Can't read in an empty string, so the SEPARATOR_EMPTY value
				// represents it.
				separator = "";
			}
			p_add_field(code, name, default_option, separator);
			current_field = code;
			break;
		case OPTION_MARKER:
			p_add_option(current_field, code, name);
			break;
		default:
			std::cout << "ERROR: Unkown line marker in file " << filename << ": " << line_marker << std::endl;
			clear_naming_convention();
			return 1;
		case EOF_MARKER:
			// We should never get here because of the check earlier in the loop
			break;
		}
	}
	
	file.close();
	mFileLoaded = true;
	return 0;
}
int NamingConvention::save_fields_to_file(std::string filename)
{	
	if(filename == "")
	{
		// Default to continue using the current file, but if no file has been used yet, there is no current file,
		// so cancel saving.
		if(!mFileLoaded) {
			return 1;	
		}
		filename = mCurrentFilename;
	}
	else
	{
		// If the user supplied a filename, it becomes the current file.
		mCurrentFilename = filename;
	}	
	std::ofstream file;
	file.open(filename.c_str());
	if(!file.is_open()) {
		return 1;
	}
	
	// For each field, make an entry for it and each of its options.
	for(std::list<std::string>::iterator fields_it = mFieldOrder.begin(); fields_it != mFieldOrder.end(); fields_it++)
	{
		std::string field_code = *fields_it;
		std::string separator = mFieldSeparators[field_code];
		if(separator == "")
		{
			separator = SEPARATOR_EMPTY;
		}
		file << FIELD_MARKER << ' ' << field_code << ' ' << mFieldDefaults[field_code] << ' ' <<
				separator << ' ' << mFieldNames[field_code] << std::endl;
		
		for(std::map<std::string, std::string>::iterator options_it = mFieldOptions[field_code].begin();
				options_it != mFieldOptions[field_code].end(); ++options_it)
		{
			file << OPTION_MARKER << ' ' << options_it->first << ' ' << options_it->second << std::endl;
		}
	}
	// Write the end of file line.
	file << EOF_MARKER;
	
	file.close();
	return 0;
}

int NamingConvention::add_option(std::string field_code, std::string new_option_code, std::string new_option_name)
{
	p_add_option(field_code, new_option_code, new_option_name);
	return save_fields_to_file();
}
int NamingConvention::add_field(std::string field_code, std::string field_name, std::string default_option, std::string field_separator)
{
	p_add_field(field_code, field_name, default_option, field_separator);
	return save_fields_to_file();
}

std::string NamingConvention::get_field_option_name(std::string field_code, std::string option_code)
{
	return mFieldOptions[field_code][option_code];
}
std::string NamingConvention::get_field_name(std::string field_code)
{
	return mFieldNames[field_code];
}

std::list<std::string> NamingConvention::get_field_order()
{
	return mFieldOrder;
}
std::map<std::string, std::string> NamingConvention::get_fields()
{
	return mFieldNames;
}
std::map<std::string, std::string> NamingConvention::get_options(std::string field_code)
{
	return mFieldOptions[field_code];
}

NamingConvention::NamingConvention(std::string filename) : mFileLoaded(false), mCurrentFilename(DEFAULT_FILENAME)
{
	if(filename != "") {
		load_fields_from_file(filename);
	}
	// Don't load any file if the user explicity provides the empty string as the argument.
}

std::string NamingConvention::get_field_value(std::string field_code)
{
	return mFieldValues[field_code];
}
std::string NamingConvention::set_field_value(std::string field_code, std::string option_code)
{
	mFieldValues[field_code] = option_code;
}

void NamingConvention::clear_values()
{
	mFieldValues.clear();
}
void NamingConvention::apply_defaults()
{
	clear_values();
	for(std::map<std::string, std::string>::iterator it = mFieldDefaults.begin();
			it != mFieldDefaults.end(); ++it)
	{
		mFieldValues[it->first] = it->second;
	}
}

std::string NamingConvention::name()
{
	std::stringstream name;
	// For each field, add the option code and the separator.
	for(std::list<std::string>::iterator fields_it = mFieldOrder.begin(); fields_it != mFieldOrder.end(); fields_it++)
	{
		std::string field_code = *fields_it;
		name << get_field_value(field_code) << mFieldSeparators[field_code];
	}
	return name.str();
}
