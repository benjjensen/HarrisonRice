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
		std::cerr << "ERROR: failure to open file " << filename << std::endl;
		return 1;
	}
	// Wait to clear the naming convention until after we know we have a file to read from.
	clear_naming_convention();
	
	char line_marker = ' ';
	std::string current_field;
	std::string code;
	std::string name;
	std::string separator;
	
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
			// The default option was read in as the code:
			p_add_field(name, name, code, separator);
			current_field = name;
			break;
		case OPTION_MARKER:
			p_add_option(current_field, code, name);
			break;
		default:
			std::cerr << "ERROR: Unkown line marker in file " << filename << ": " << line_marker << std::endl;
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
	
	file << "# Naming conventions are stored in a text file. The format of this file is as follows:\n"
	<< "# Each line is one of a comment, a blank line, a field entry, an option entry, or the end of file marker line.\n"
	<< "# \n"
	<< "# Comment lines start with '#' and are ignored.\n"
	<< "#     # This is a comment. I can write anything here.\n"
	<< "# Note: comment lines are not preserved when a member of this class overwrites the naming convention file.\n"
	<< "#\n"
	<< "# Field entries start with '$' and are in this format:\n"
	<< "#     $ [default value] [separator] [field name]\n"
	<< "# Where each item is exactly one word, aside from the field name, which can be any number of words.\n"
	<< "# The default value must be a code for one of the options listed below this field.\n"
	<< "# The separator here cannot be empty, but if an empty separator in the name is desired use the string \"`\" here.\n"
	<< "# Field entries should appear in the order in which their codes are used for the naming convention.\n"
	<< "# \n"
	<< "# Option entries start with '-' and are in this format:\n"
	<< "#     - [option code] [option name]\n"
	<< "# Where option code is exactly one word and option name is any number of words.\n"
	<< "# Option entries are listed under the field which they are options for.\n"
	<< "#\n"
	<< "# The end of file marker line is mandatory and consists of exactly `~`:\n"
	<< "#     ~\n"
	<< "# Behavior is undefined if this line is not present or if there exist lines in the file after this line.\n"
	<< "#\n"
	<< "# If any line begins with a character other than these four options (leading whitespace and empty lines\n"
	<< "# are ignored), the naming convention class will be unable to read the naming convention file.\n\n\n" << std::endl;
		
	// For each field, make an entry for it and each of its options.
	for(std::list<std::string>::iterator fields_it = mFieldOrder.begin(); fields_it != mFieldOrder.end(); fields_it++)
	{
		std::string field_code = *fields_it;
		std::string separator = mFieldSeparators[field_code];
		if(separator == "")
		{
			separator = SEPARATOR_EMPTY;
		}
		file << FIELD_MARKER << ' ' << mFieldDefaults[field_code] << ' ' <<
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
	std::map<std::string, std::string> options = mFieldOptions[field_code];
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

void NamingConvention::set_field_value(std::string field_code, std::string option_code)
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
