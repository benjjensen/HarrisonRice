#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef __linux__
#include <sys/wait.h>
#endif

#include "DataCapture.h"

const std::string DEFAULT_OUTPUT_FILENAME = "gps.kml";
const std::string DEFAULT_GPSBABEL_CALL = "gpsbabel";
const std::string DEFAULT_XCSV_FORMAT_FILENAME = "gpsbabel_xcsv_format.txt";

const std::string DEFAULT_OUTPUT_EXTENSION = ".kml";

struct GPSTrackerEnvironment {
    std::string outputFilename;
    std::string gpsbabelCall;
    std::string xcsvFormatFilename;
    bool timestamping;
};

static void printUsage();
static GPSTrackerEnvironment GPSTrackerParser(int argc, char** argv);
static std::string applyTimestamp(std::string filename,
        GPSPosition position);
static std::string applyDefaultFileExtension(std::string filename);

int main(int argc, char** argv) {
    GPSTrackerEnvironment environment = GPSTrackerParser(argc, argv);

    std::cout << "System call to gpsbabel.\n"
            "WARNING: IF THE COMPUTER GOES TO SLEEP, GPS DATA CAPTURE WILL STOP"
            "\nPress ctrl-c to finish." <<
            std::endl;

    std::stringstream command;
    command << environment.gpsbabelCall << " -T -i garmin -f usb: -o" <<
            " xcsv,style=" << environment.xcsvFormatFilename << " -F " <<
            environment.outputFilename;
    int status = system(command.str().c_str());

#ifdef __linux__
    if(!WIFEXITED(status)) {
        std::cerr << "ERROR: failed to capture gps data. Exiting." << std::endl;
        std::exit(1);
    }
    else if(WEXITSTATUS(status)) {
        int exitCode = WEXITSTATUS(status);
        if(exitCode == 127) {
            std::cerr << "ERROR: unable to locate gpsbabel executable. Exiting."
                    << std::endl;
        }
        else {
            std::cerr << "ERROR: gpsbabel failure. Exiting." << std::endl;
        }
        std::exit(1);
    }
#elif _WIN32
    if(status) {
        std::cerr << "ERROR: failed to capture gps data. Exiting." << std::endl;
        std::exit(1);
    }
#endif

    std::ifstream GPSFile(environment.outputFilename.c_str());
    if(!GPSFile.good()) {
        std::cerr << "ERROR: no output file was created by gpsbabel. Exiting."
                << std::endl;
        std::exit(1);
    }

    GPSPosition firstPosition;
    if(!(GPSFile >> firstPosition)) {
        std::cerr << "ERROR: no gps data recorded. Exiting." << std::endl;
        GPSFile.close();
        // Delete the empty file
        remove(environment.outputFilename.c_str());
        std::exit(1);
    }
    GPSFile.close();

    // At this point, we're confident that gpsbabel succeeded in capturing the
    // gps data.

    std::string newFilename = environment.timestamping ?
            applyTimestamp(environment.outputFilename, firstPosition) :
            applyDefaultFileExtension(environment.outputFilename);

    std::cout << "\nReturned from gpsbabel. Converting output to kml... " <<
            std::flush;

    command.str("");
    command << environment.gpsbabelCall << " -i " <<
            "xcsv,style=" << environment.xcsvFormatFilename << " -f " <<
            environment.outputFilename << " -x transform,trk=w,del -o kml -F "
            << newFilename;
    system(command.str().c_str());

    if(environment.outputFilename != newFilename) {
        remove(environment.outputFilename.c_str());
    }

    std::cout << "done.\nOutput stored in " << newFilename << std::endl;

    return 0;
}

static std::string applyTimestamp(std::string filename,
        GPSPosition position) {
    std::string timestamp = position.get_timestamp();

    size_t extensionIndex = filename.rfind('.');
    size_t folderIndex = filename.find_last_of("/\\");

    if(extensionIndex == std::string::npos ||
            (folderIndex != std::string::npos &&
            folderIndex >= extensionIndex)) {
        filename += timestamp;
        filename += DEFAULT_OUTPUT_EXTENSION;
    }
    else {
        filename.insert(extensionIndex, timestamp);
    }
    return filename;
}

static std::string applyDefaultFileExtension(std::string filename) {
    size_t extensionIndex = filename.rfind('.');
    size_t folderIndex = filename.find_last_of("/\\");

    if(extensionIndex == std::string::npos ||
            (folderIndex != std::string::npos &&
            folderIndex >= extensionIndex)) {
        filename += DEFAULT_OUTPUT_EXTENSION;
    }

    return filename;
}

static void printUsage() {
    std::cout <<
            "Usage: gpstracker [options] [<filename>]\n"
            "    <filename>            The name of the output file. [default: gps.kml]\n"
            "Options:\n"
            "    -d                       Disable timestamping of output file.\n"
            "    -c <gpsbabel-command>    Location of the gpsbabel executable.\n"
            "                             [default: gpsbabel]\n"
            "    -x <xcsv-format-file>    File to use for the intermediate xcsv format.\n"
            "                             Don't specify this unless you know what you're\n"
            "                             doing. [default: gpsbabel_xcsv_format.txt]\n\n"
            "    -h, --help               Print this help message"
            << std::endl;
}

static GPSTrackerEnvironment GPSTrackerParser(int argc, char** argv) {
    GPSTrackerEnvironment environment;
    environment.gpsbabelCall = DEFAULT_GPSBABEL_CALL;
    environment.xcsvFormatFilename = DEFAULT_XCSV_FORMAT_FILENAME;
    environment.outputFilename = DEFAULT_OUTPUT_FILENAME;
    environment.timestamping = true;

    bool assignedOutputFilename = false;

    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == "-c") {
            if(++i >= argc) {
                printUsage();
                std::exit(1);
            }
            environment.gpsbabelCall = argv[i];
        }
        else if(arg == "-x") {
            if(++i >= argc) {
                printUsage();
                std::exit(1);
            }
            environment.xcsvFormatFilename = argv[i];
        }
        else if(arg == "-d") {
            environment.timestamping = false;
        }
        else if(arg == "-h" || arg == "--help") {
            printUsage();
            std::exit(1);
        }
        else {
            if(assignedOutputFilename) {
                printUsage();
                std::exit(1);
            }
            environment.outputFilename = arg;
            assignedOutputFilename = true;
        }
    }

    return environment;
}