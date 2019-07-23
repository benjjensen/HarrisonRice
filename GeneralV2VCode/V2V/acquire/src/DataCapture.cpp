/**
 * @file DataCapture.cpp Defines the DataCapture and GPSPosition classes.
 */


#include "DataCapture.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>

DataCapture::DataCapture() {
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

std::string DataCapture::get_date_string() {
    std::stringstream ss;
    ss << std::setfill('0') << year << '-' << std::setw(2) << month << '-' <<
            std::setw(2) << date;
    return ss.str();
}

std::string DataCapture::get_time_string() {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hour << ':' << std::setw(2) <<
            minute << ':' << std::setw(2) << second;
    return ss.str();
}

int DataCapture::write_to_file(std::string filename) {
    // If the filename provided is empty, use the meta_filename.
    // If the meta_filename is also empty, generate it from the data_filename.
    // If generating the meta_filename failed, print an error message and return
    // 1.
    if(filename == "") {
        if(meta_filename == "" && !generate_meta_filename()) {
            std::cerr << "ERROR: no file to write this DataCapture to" <<
                    std::endl;
            return 1;
        }
        filename = meta_filename;
    }
        // If the user provided a filename, save that as the meta_filename:
    else {
        meta_filename = filename;
    }

    std::ofstream file;
    file.open(filename.c_str());
    if(!file.is_open()) {
        std::cerr << "ERROR: unable to open file " << filename << std::endl;
        return 1;
    }

    write_to_stream(file);

    file.close();
    return 0;
}

int DataCapture::read_from_file(std::string filename) {
    if(filename == "") {
        filename = meta_filename;
    }
        // If the user provided a filename, save that as the meta_filename:
    else {
        meta_filename = filename;
    }

    std::ifstream file;
    file.open(filename.c_str());
    if(!file.is_open()) {
        std::cerr << "ERROR: unable to open file " << filename << std::endl;
        return 1;
    }

    read_from_stream(file);

    file.close();
    return 0;
}

void DataCapture::write_to_stream(std::ostream &out) {
    out << "name " << name << std::endl;
    out << "date " << get_date_string() << std::endl;
    out << "time " << get_time_string() << std::endl;
    out << "duration " << duration << std::endl;
    out << "size " << size << std::endl;
    out << "datafile " << data_filename << std::endl;
    out << "gpsfile " << gps_filename << std::endl;
    out << "notes " << notes << "`" << std::endl;
    out << "positions " << gps_positions.size() << std::endl;
    for(std::vector<GPSPosition>::iterator it = gps_positions.begin();
            it != gps_positions.end(); ++it) {
        out << (*it) << std::endl;
    }
}

void DataCapture::read_from_stream(std::istream &in) {
    gps_positions.clear();

    // For each line in the stream, read the first word to figure out the type.
    // Based on the type, read in the rest of the info for that type.
    std::string line_str;
    while(std::getline(in, line_str)) {
        std::stringstream line(line_str);
        std::string type;
        line >> type;

        if(type == "name") {
            std::getline(line, name);
            // Strip whitespace away from the name:
            if(name != "") {
                int start_index = name.find_first_not_of(" \n\t\r");
                if(start_index != -1) {
                    int end_index = name.find_last_not_of(" \n\t\r");
                    name = name.substr(start_index,
                            end_index - start_index + 1);
                }
                else {
                    name = "";
                }
            }
        }
        else if(type == "date") {
            line >> year;
            // Ignore the '-' character:
            line.ignore();
            line >> month;
            // Ignore the '-' character:
            line.ignore();
            line >> date;
        }
        else if(type == "time") {
            line >> hour;
            // Ignore the ':' character:
            line.ignore();
            line >> minute;
            // Ignore the ':' character:
            line.ignore();
            line >> second;
        }
        else if(type == "duration") {
            line >> duration;
        }
        else if(type == "size") {
            line >> size;
        }
        else if(type == "notes") {
            // Read in the rest of the stream until the backquote character:
            std::string remaining_in_line;
            std::getline(line, remaining_in_line, '`');
            // If this line didn't have the backquote in it, read until we
            // find it:
            if(line_str.find('`') == std::string::npos) {
                std::string rest_of_notes;
                std::getline(in, rest_of_notes, '`');
                notes = remaining_in_line + '\n' + rest_of_notes;
            }
                // If this line had the backquote in it, that's it:
            else {
                notes = remaining_in_line;
            }
            // Strip whitespace away from the notes:
            if(notes != "") {
                int start_index = notes.find_first_not_of(" \n\t\r");
                if(start_index != -1) {
                    int end_index = notes.find_last_not_of(" \n\t\r");
                    notes = notes.substr(start_index,
                            end_index - start_index + 1);
                }
                else {
                    notes = "";
                }
            }
        }
        else if(type == "datafile") {
            std::getline(line, data_filename);
            // Strip whitespace away from the data filename.
            if(data_filename != "") {
                int start_index = data_filename.find_first_not_of(" \n\t\r");
                if(start_index != -1) {
                    int end_index = data_filename.find_last_not_of(" \n\t\r");
                    data_filename = data_filename.substr(start_index,
                            end_index - start_index + 1);
                }
                else {
                    data_filename = "";
                }
            }
        }
        else if(type == "gpsfile") {
            std::getline(line, gps_filename);
            // Strip whitespace away from the data filename.
            if(gps_filename != "") {
                int start_index = gps_filename.find_first_not_of(" \n\t\r");
                if(start_index != -1) {
                    int end_index = gps_filename.find_last_not_of(" \n\t\r");
                    gps_filename = gps_filename.substr(start_index,
                            end_index - start_index + 1);
                }
                else {
                    gps_filename = "";
                }
            }
        }
        else if(type == "positions") {
            int num_positions;
            line >> num_positions;
            gps_positions.reserve(num_positions);
            for(int index = 0; index < num_positions; ++index) {
                GPSPosition position;
                in >> position;
                gps_positions.push_back(position);
            }
            // Ignore the last endline character after the list of positions.
            in.ignore();
        }
        else {
            std::cerr << "ERROR: unkown line in capture stream: " << line_str <<
                    std::endl;
        }
    }
}

bool DataCapture::generate_meta_filename() {
    if(data_filename != "") {
        size_t extension_index = data_filename.rfind('.');
        size_t last_slash_index = data_filename.find_last_of("\\/");

        if(extension_index != std::string::npos &&
                last_slash_index != std::string::npos) {
            if(extension_index <= last_slash_index) {
                // If the last period is before the last slash in the filename,
                // we know that that period does not represent the beginning of
                // a file extension.
                extension_index = std::string::npos;
            }
        }

        if(extension_index == std::string::npos) {
            meta_filename = data_filename + CAPTURE_META_ENDING;
        }
        else {
            meta_filename = data_filename.substr(0, extension_index) +
                    CAPTURE_META_ENDING;
        }
        return true;
    }
    else {
        return false;
    }
}

int DataCapture::reserve_meta_file_space() {
    // If there's no meta_filename, we can't reserve space for the file:
    if(meta_filename == "" && !generate_meta_filename()) {
        return -1;
    }

    int meta_file_descriptor = open(meta_filename.c_str(), O_WRONLY | O_CREAT);
    if(meta_file_descriptor < 0) {
        return -1;
    }

    // Reserve a full 10 megabytes to be sure we have enough.
    int result = posix_fallocate(meta_file_descriptor, 0,
            META_FILE_RESERVE_SIZE);
    close(meta_file_descriptor);
    meta_file_descriptor = -1;

    return result;
}

bool DataCapture::generate_gps_filename() {
    if(data_filename != "") {
        size_t extension_index = data_filename.rfind('.');
        size_t last_slash_index = data_filename.find_last_of("\\/");

        if(extension_index != std::string::npos &&
                last_slash_index != std::string::npos) {
            if(extension_index <= last_slash_index) {
                // If the last period is before the last slash in the filename,
                // we know that that period does not represent the beginning of
                // a file extension.
                extension_index = std::string::npos;
            }
        }

        if(extension_index == std::string::npos) {
            gps_filename = data_filename + CAPTURE_GPS_ENDING;
        }
        else {
            gps_filename = data_filename.substr(0, extension_index) +
                    CAPTURE_GPS_ENDING;
        }
        return true;
    }
    else {
        return false;
    }
}

int DataCapture::save_gps_to_google_earth_file(std::string filename) {
    // If the filename provided is empty, use gps_filename.
    // If the gps_filename is also empty, generate it from data_filename.
    // If generating gps_filename failed, print an error message and return 1.
    if(filename == "") {
        if(gps_filename == "" && !generate_gps_filename()) {
            std::cerr << "ERROR: NO FILE TO WRITE THE GPS DATA TO" << std::endl;
            return 1;
        }
        filename = gps_filename;
    }
        // If the user provided a filename, save that as the gps_filename:
    else {
        gps_filename = filename;
    }

    // Save the gps data in its current format to an xcsv file:

    std::ofstream file;
    file.open(filename.c_str());
    if(!file.is_open()) {
        std::cerr << "ERROR: UNABLE TO OPEN FILE " << filename << std::endl;
        return 1;
    }

    for(std::vector<GPSPosition>::iterator it = gps_positions.begin();
            it != gps_positions.end(); ++it) {
        file << (*it) << std::endl;
    }

    file.close();

    // Convert the xcsv file to a kml file:

    std::stringstream command;
    command <<
            "/usr/local/bin/gpsbabel -i xcsv,style=gpsbabel_xcsv_format.txt -f "
            << gps_filename << " -x transform,trk=w,del -o kml -F " <<
            gps_filename;
    system(command.str().c_str());

    return 0;
}

int DataCapture::reserve_gps_file_space() {
    // If there's no meta_filename, we can't reserve space for the file:
    if(gps_filename == "" && !generate_gps_filename()) {
        return -1;
    }

    int gps_file_descriptor = open(gps_filename.c_str(), O_WRONLY | O_CREAT);
    if(gps_file_descriptor < 0) {
        return -1;
    }

    // Reserve a full megabyte to be sure we have enough.
    int result = posix_fallocate(gps_file_descriptor, 0, GPS_FILE_RESERVE_SIZE);
    close(gps_file_descriptor);
    gps_file_descriptor = -1;

    return result;
}

GPSPosition::GPSPosition() {
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

    fix_type = "None";
}

void GPSPosition::write_to_stream(std::ostream &out) {
    out << std::setfill('0');
    out << std::setw(2) << year << '-' << std::setw(2) << month << '-' <<
            std::setw(2) << date << ' ';
    out << std::setw(2) << hour << ':' << std::setw(2) << minute << ':' <<
            std::setw(2) << second << FIELD_DELIMITER;

    out << blocks_captured << " blocks" << FIELD_DELIMITER;

    out << std::setprecision(12) << latitude << FIELD_DELIMITER;
    out << std::setprecision(13) << longitude << FIELD_DELIMITER;

    out << altitude_feet << FIELD_DELIMITER;

    out << fix_type;
}

bool GPSPosition::read_from_stream(std::istream &in) {
    time_t year = 2018;
    time_t month = 8;
    time_t date = 27;
    time_t hour = 12;
    time_t minute = 0;
    time_t second = 0;

    size_t blocks_captured = 0;

    double latitude = 4500;
    double longitude = 11100;
    double altitude_feet = 4500;

    std::string fix_type = "None";

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

    // Ignore the field delimiter and the space before the fix type.
    in.ignore();
    in.ignore();
    in >> fix_type;

    if(in) {
        this->year = year;
        this->month = month;
        this->date = date;
        this->hour = hour;
        this->minute = minute;
        this->second = second;

        this->blocks_captured = blocks_captured;

        this->latitude = latitude;
        this->longitude = longitude;
        this->altitude_feet = altitude_feet;

        this->fix_type = fix_type;

        return true;
    }
    else {
        return false;
    }
}

std::string GPSPosition::get_timestamp() {
    std::stringstream timestamp;

    timestamp << std::setfill('0');
    timestamp << "_" << year << "-" << std::setw(2) << month <<
            "-" << std::setw(2) << date;
    timestamp << "__" << std::setw(2) << hour << "-" << std::setw(2) <<
            minute << "-" << std::setw(2) << second;

    return timestamp.str();
}

bool GPSPosition::is_valid_fix() {
    return boost::algorithm::to_lower_copy(fix_type) != "none";
}

std::istream & operator>>(std::istream &in, GPSPosition &position) {
    position.read_from_stream(in);
    return in;
}

std::ostream & operator<<(std::ostream &out, GPSPosition &position) {
    position.write_to_stream(out);
    return out;
}

int days_in_month(int month, int year) {
    switch(month) {
    case 2:
        if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
            return 29;
        }
        else {
            return 28;
        }
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    default:
        return 31;
    }
}

void set_gps_position_time(GPSPosition& position,
        GPSPosition one_second_previous, bool previous_valid,
        std::tm* time) {
    if(previous_valid) {
        position.year = one_second_previous.year;
        position.month = one_second_previous.month;
        position.date = one_second_previous.date;

        position.hour = one_second_previous.hour;

        if(position.minute < one_second_previous.minute) {
            position.hour += 1;

            position.date += position.hour / 24;
            position.hour %= 24;

            int month_length = days_in_month(position.month, position.year);
            if(position.date > month_length) {
                ++position.month;
                position.date = 1;

                if(position.month > 12) {
                    ++position.year;
                    position.month = 1;
                }
            }
        }
    }
    else {
        if(time == NULL) {
            std::time_t t = std::time(0);
            time = std::localtime(&t);
        }

        if(position.year != time->tm_year + 1900 ||
                position.month != time->tm_mon + 1 ||
                position.date != time->tm_mday) {
            // GPS Satellites use a week counter that resets every 19.6 years,
            // and so the GPS unit sometimes returns a date that is 19.6 years
            // off. This isn't a problem if we already know the current date.

            position.year = time->tm_year + 1900;
            position.month = time->tm_mon + 1;
            position.date = time->tm_mday;

            // The hour might be incorrect if the original (incorrect) date was
            // during daylight savings time and the correct date is not or
            // vice-versa.
            position.hour = time->tm_hour;
            if(time->tm_min != position.minute) {
                if(position.minute >= 50 && time->tm_min <= 10) {
                    ++position.hour;
                }
                else if(position.minute <= 10 &&
                        time->tm_min >= 50) {
                    --position.hour;
                }
            }
            // No need to update the minute or hour because those are always
            // accurate in the given GPS Position.
        }
    }
}