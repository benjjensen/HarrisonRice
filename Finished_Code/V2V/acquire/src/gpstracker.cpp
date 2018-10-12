#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#ifdef __linux__
#include <sys/wait.h>
#endif


const int MIN_ARG_COUNT = 2;
const int ARG_POSITION_OUTPUT_FILENAME = 1;

const std::string DEFAULT_GPSBABEL_CALL = "gpsbabel";
const std::string DEFAULT_XCSV_FORMAT_FILENAME = "gpsbabel_xcsv_format.txt";

struct GPSTrackerEnvironment {
    std::string outputFilename;
    std::string gpsbabelCall;
    std::string xcsvFormatFilename;
};

static void printUsage();
static GPSTrackerEnvironment GPSTrackerParser(int argc, char** argv);

int main(int argc, char** argv) {
    GPSTrackerEnvironment environment = GPSTrackerParser(argc, argv);

    std::cout << "System call to gpsbabel. Press ctrl-c to finish." <<
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

    std::cout << "\nReturned from gpsbabel. Converting output to kml... " <<
            std::flush;

    command.str("");
    command << environment.gpsbabelCall << " -i " <<
            "xcsv,style=" << environment.xcsvFormatFilename << " -f " <<
            environment.outputFilename << " -x transform,trk=w,del -o kml -F "
            << environment.outputFilename;
    system(command.str().c_str());

    std::cout << "done." << std::endl;

    return 0;
}

static void printUsage() {
    std::cout <<
            "Usage: gpstracker <filename> [-c <gpsbabel-command>] [-x <xcsv-format-file>]\n"
            "    <filename>            The name of the output file\n"
            "    <gpsbabel-command>    Location of the gpsbabel executable\n"
            "                          [default: gpsbabel]\n"
            "    <xcsv-format-file>    File to use for the intermediate xcsv format. Don't \n"
            "                          specify this unless you know what you're doing.\n"
            "                          [default: gpsbabel_xcsv_format.txt]"
            << std::endl;
}

static GPSTrackerEnvironment GPSTrackerParser(int argc, char** argv) {
    if(argc < MIN_ARG_COUNT) {
        printUsage();
        std::exit(1);
    }

    GPSTrackerEnvironment environment;
    environment.gpsbabelCall = DEFAULT_GPSBABEL_CALL;
    environment.xcsvFormatFilename = DEFAULT_XCSV_FORMAT_FILENAME;

    environment.outputFilename = argv[ARG_POSITION_OUTPUT_FILENAME];

    for(int i = ARG_POSITION_OUTPUT_FILENAME + 1; i < argc; ++i) {
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
        else {
            printUsage();
            std::exit(1);
        }
    }

    return environment;
}