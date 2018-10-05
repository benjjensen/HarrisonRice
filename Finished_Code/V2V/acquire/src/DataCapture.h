#pragma once

#include <ctime>
#include <string>
#include <vector>

/**
 * The ending of the filename for a capture metadata file.
 */
const std::string CAPTURE_META_ENDING = "-meta.txt";

/**
 * The ending of the filename for a capture metadata file.
 */
const std::string CAPTURE_GPS_ENDING = "-GPS.kml";

/**
 * The tag to use for acquire.cpp to pass the name of the capture metadata
 * file to sampler.cpp via acquire's standard output.
 */
const std::string CAPTURE_META_FILENAME_HANDOFF_TAG =
        "DataCapture.meta_filename";

/**
 * The tag to use for acquire.cpp to tell sampler.cpp that no data was captured.
 */
const std::string CAPTURE_CANCELED_TAG = "DataCapture.canceled";

/**
 * The path to the folder where the data is stored.
 */
const std::string DATA_FOLDER = "/media/V2V";

/**
 * The delimiter between fields in the gps data.
 */
const std::string FIELD_DELIMITER = ", ";

/**
 * The size of the file to reserve for the metadata file.
 */
const int META_FILE_RESERVE_SIZE = 1024 * 1024;
/**
 * The size of the file to reserve for the gps data.
 */
const int GPS_FILE_RESERVE_SIZE = 10 * 1024 * 1024;

/**
 * A class to represent a single gps position in a data capture.
 */
class GPSPosition {
public:
    /**
     * The year of the gps position.
     */
    time_t year;
    /**
     * The month (1-12) of the gps position.
     */
    time_t month;
    /**
     * The date (1-31) of the gps position.
     */
    time_t date;

    /**
     * The hour (0-23) of the gps position.
     */
    time_t hour;
    /**
     * The minute (0-59) of the gps position.
     */
    time_t minute;
    /**
     * The second (0-59) of the gps position.
     */
    time_t second;

    /**
     * The number of blocks captured before this position fix.
     */
    size_t blocks_captured;

    /**
     * The latitude of this position.
     */
    double latitude;
    /**
     * The longitude of this position.
     */
    double longitude;
    /**
     * The altitude of this position.
     */
    double altitude_feet;


    /**
     * Default constructor.
     */
    GPSPosition();

    /**
     * Writes this gps position to a stream.
     */
    void write_to_stream(std::ostream &out);
    /**
     * Reads this gps position in from a stream.
     */
    void read_from_stream(std::istream &in);
};

/**
 * A class to represent the metadata for a single data capture.
 */
class DataCapture {
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
     * The filename where the GPS data is stored.
     */
    std::string gps_filename;

    std::vector<GPSPosition> gps_positions;


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

    /**
     * Generates the meta_filename from data_filename.
     *
     * Returns true on success, false on failure (if there's no data_filename).
     */
    bool generate_meta_filename();

    /**
     * Reserves space for the meta file.
     *
     * Returns 0 on success, nonzero on failure.
     */
    int reserve_meta_file_space();

    /**
     * Generates the gps_filename from data_filename.
     *
     * Returns true on success, false on failure (if there's no data_filename).
     */
    bool generate_gps_filename();

    int save_gps_to_google_earth_file(std::string filename = "");

    /**
     * Reserves space for the gps file.
     *
     * Returns 0 on success, nonzero on failure.
     */
    int reserve_gps_file_space();
};

std::istream & operator>>(std::istream &in, GPSPosition &position);
std::ostream & operator<<(std::ostream &out, GPSPosition &position);