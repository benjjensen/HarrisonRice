#pragma once

#include <string>
#include <list>
#include <map>


/**
The default filename to store the naming convention specification.
*/
const std::string DEFAULT_FILENAME = "name_fields.txt";
/**
Represents a separator that is empty.
*/
const std::string SEPARATOR_EMPTY = "`";

/**
NamingConvention manages the naming convention for capture files.

Members of this class load a naming convention from a file, manage that convention, manage values of the
various fields in the convention, and convert those values to a name that fits the convention.

Naming conventions are stored in a text file. The format of this file is as follows:
Each line is one of a comment, a blank line, a field entry, an option entry, or the end of file marker line.

Comment lines start with '#' and are ignored.
	# This is a comment. I can write anything here.

Field entries start with '$' and are in this format:
	$ [field code] [default value] [separator] [field name]
Where each item is exactly one word, aside from the field name, which can be any number of words.
The default value must be a code for one of the options listed below this field.
The separator here cannot be empty, but if an empty separator in the name is desired use the string "`" here.
Field entries should appear in the order in which their codes are used for the naming convention.

Option entries start with '-' and are in this format:
	- [option code] [option name]
Where option code is exactly one word and option name is any number of words.
Option entries are listed under the field which they are options for.

The end of file marker line is mandatory and consists of exactly `~`:
	~
Behavior is undefined if this line is not present or if there exist lines in the file after this line.

If any line begins with a character other than these four options (leading whitespace and empty lines
are ignored), this class will be unable to read the naming convention file.
*/
class NamingConvention
{
	public:
	/**
	Loads the naming convention from a file.
	
	This clears out the old naming convention, but does not clear the current values for the various fields.
	For each field in the given file, if there is not yet a value for it in this object, the default value
	is applied.
	
	If no filename is provided, this loads from the most recent file used or from DEFAULT_FILENAME if no
	files have been used yet.
	
	This is called by the constructor of this class, unless the filename given to the constructor is empty.
	
	Returns zero on successful load, nonzero otherwise.
	*/
	int load_fields_from_file(std::string filename = "");
	
	/**
	Saves the naming convention to a file.
	
	If no filename is provided, this saves to the most recent file used if there has been one.
	
	If no files have been used yet and no filename is provided, this function does nothing and returns
	nonzero.
	
	Returns zero on successful save, nonzero otherwise.
	*/
	int save_fields_to_file(std::string filename = "");
	
	/**
	Add an option to a field.
	
	This function saves the new option to the file currently in use, unless no file has been used yet,
	in which case no save is made.
	
	Returns zero if the save was successful, nonzero otherwise.
	*/
	int add_option(std::string field_code, std::string new_option_code, std::string new_option_name);
	/**
	Add a field to the naming convention.
	
	This function saves the new option to the file currently in use, unless no file has been used yet,
	in which case no save is made.
	
	Returns zero if the save was successful, nonzero otherwise.
	*/
	int add_field(std::string field_code, std::string field_name, std::string default_option, std::string field_separator = "");
	
	/**
	Gets the name of a certain option in a field.
	*/
	std::string get_field_option_name(std::string field_code, std::string option_code);
	/**
	Gets the name of a field.
	*/
	std::string get_field_name(std::string field_code);
	
	/**
	Returns a list of the field codes in the naming convention, in the proper order.
	*/
	std::list<std::string> get_field_order();
	/**
	Returns a map of field codes to field names for all the fields in the convention.
	*/
	std::map<std::string, std::string> get_fields();
	/**
	Returns a map of option codes to option names for all the options in a field.
	*/
	std::map<std::string, std::string> get_options(std::string field_code);
	
	/**
	Initializes the NamingConvention by loading the convention values from a file.
	
	If the given filename is the empty string, no file is loaded and the convention is empty. A file
	can later be loaded by calling load_fields_from_file().
	*/
	NamingConvention(std::string filename = DEFAULT_FILENAME);
	
	/**
	Gets the code for the currently selected option for a certain field.
	*/
	std::string get_field_value(std::string field_code);
	/**
	Sets the currently selected option for a certain field.
	*/
	std::string set_field_value(std::string field_code, std::string option_code);
	
	/**
	Clears out all of the current values.
	*/
	void clear_values();
	/**
	Clears the values, then applies the default values for each field.
	*/
	void apply_defaults();
	
	/**
	Returns the name that matches the naming convention given the currently selected fields.
	*/
	std::string name();
	
	private:
	/**
	Line markers for the four types of lines in a naming convention file.
	*/
	static const char COMMENT_MARKER = '#';
	static const char FIELD_MARKER = '$';
	static const char OPTION_MARKER = '-';
	static const char EOF_MARKER = '~';
	
	/**
	Whether a file has been loaded yet or not.
	*/
	bool mFileLoaded;
	
	/**
	The current filename that is being used to store and load the naming convention.
	*/
	std::string mCurrentFilename;
	
	/**
	The naming convention that's loaded into memory.
	*/
	std::list<std::string> mFieldOrder;
	std::map<std::string, std::string> mFieldNames;
	std::map<std::string, std::string> mFieldDefaults;
	std::map<std::string, std::string> mFieldSeparators;
	std::map<std::string, std::map<std::string, std::string> > mFieldOptions;
	
	/**
	The currently selected values for various fields.
	*/
	std::map <std::string, std::string> mFieldValues;
	
	/**
	Private functions to add options and fields that do not save the new options/fields to the naming convention file.
	*/
	void p_add_option(std::string field_code, std::string new_option_code, std::string new_option_name);
	void p_add_field(std::string field_code, std::string field_name, std::string default_option, std::string field_separator = "");
	
	/**
	Clears the current naming convention from this instance. This does not affect the file currently in use.
	*/
	void clear_naming_convention();
};
