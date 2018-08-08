#pragma once

#include <string>
#include <ctime>

/**
 * The ending of the filename for a capture metadata file.
 */
const std::string CAPTURE_META_ENDING = "-meta.txt";

/**
 * The tag to use for acquire.cpp to pass the name of the capture metafile to sampler.cpp via acquire's
 * standard output.
 */
const std::string CAPTURE_META_FILENAME_HANDOFF_TAG = "DataCapture.meta_filename";

/**
 * The path to the folder where the data is stored.
 */
const std::string DATA_FOLDER = "/media/V2V";

/**
 * A class to represent the metadata for a single data capture.
 */
class DataCapture
{
public:
	/**
	 * The name of the capture.
	 */
	std::string name;
	
	/**
	 * The year when the capture happened.
	 */
	time_t year;
	/**
	 * The month (1-12) when the capture happened.
	 */
	time_t month;
	/**
	 * The day of the month (1-31) when the capture happened.
	 */
	time_t date;
	/**
	 * The hour (0-23) when the capture happened.
	 */
	time_t hour;
	/**
	 * The minute (0-59) when the capture happened.
	 */
	time_t minute;
	/**
	 * The second (0-59) when the capture happened.
	 */
	time_t second;
	
	/**
	 * The duration of the capture, in seconds.
	 */
	double duration;
	/**
	 * The size of the capture, in megabytes.
	 */
	size_t size;
	
	/**
	 * Any notes that the user has taken concerning the capture.
	 */
	std::string notes;
	
	/**
	 * The filename where the metadata is stored.
	 *
	 * This filename should end with CAPTURE_META_ENDING.
	 */
	std::string meta_filename;
	/**
	 * The filename where the data of the capture is stored.
	 */
	std::string data_filename;
	
	
	/**
	 * Gets a formatted string for the date when the capture happened.
	 *
	 * The string is in the format "YYYY-MM-DD".
	 */
	std::string get_date_string();
	/**
	 * Gets a formatted string for the time when the capture happened.
	 *
	 * The string is in the format "hh:mm:ss".
	 */
	std::string get_time_string();
	
	DataCapture();
	
	/**
	 * Saves the data capture to a file.
	 *
	 * If no filename is specified, meta_filename is used.
	 *
	 * If no filename is specified and meta_filename is empty,
	 * data_filename with the file extension removed and
	 * CAPTURE_META_ENDING appended is used.
	 *
	 * If no filename is specified and meta_filename is empty and
	 * data_filename is empty, nothing happens.
	 *
	 * Returns zero on success, nonzero on failure.
	 */
	int write_to_file(std::string filename = "");
	/**
	 * Reads the data capture from a file.
	 *
	 * If no filename is specified, meta_filename is used.
	 *
	 * Returns zero on success, nonzero on failure.
	 */
	int read_from_file(std::string filename = "");
	
	/**
	 * Writes the data capture to a stream.
	 */
	void write_to_stream(std::ostream &out);
	/**
	 * Reads the data capture from a stream.
	 */
	void read_from_stream(std::istream &in);
};