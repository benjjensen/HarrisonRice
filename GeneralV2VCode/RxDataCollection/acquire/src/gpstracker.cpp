#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef __linux__
#include <sys/wait.h>
#elif _WIN32
#include <windows.h>
#include <tchar.h>
#endif

#include "DataCapture.h"

const std::string DEFAULT_OUTPUT_FILENAME = "gps.kml";
#ifdef __linux__
const std::string DEFAULT_GPSBABEL_CALL = "gpsbabel";
#elif _WIN32
const std::string DEFAULT_GPSBABEL_CALL = "C:\\Program Files (x86)\\GPSBabel\\gpsbabel.exe";
#endif
const std::string DEFAULT_XCSV_FORMAT_FILENAME = "gpsbabel_xcsv_format.txt";

const std::string DEFAULT_OUTPUT_EXTENSION = ".kml";

struct GPSTrackerEnvironment {
    std::string outputFilename;
    std::string gpsbabelCall;
    std::string xcsvFormatFilename;
    bool timestamping;
};

/**
 * Print the usage of this program to the command line.
 */
static void printUsage();

/**
 * Parse the command-line arguments and return them.
 */
static GPSTrackerEnvironment GPSTrackerParser(int argc, char** argv);

/**
 * Returns the given filename with a timestamp added just before the file
 * extension.
 *
 * The timestamp is based on the given GPSPosition.
 */
static std::string applyTimestamp(std::string filename,
        GPSPosition position);
/**
 * If the given filename has a file extension, return the argument.
 * If the given filename does not have a file extension, append the default
 * file extension and return the result.
 */
static std::string applyDefaultFileExtension(std::string filename);

#ifdef _WIN32

bool gpsbabel_running = false;


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal. 
    case CTRL_C_EVENT:
        if (gpsbabel_running)
        {
            gpsbabel_running = false;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    default:
        return FALSE;
    }
}

#endif

int main(int argc, char** argv) {
    // Parse the command-line arguments.
    GPSTrackerEnvironment environment = GPSTrackerParser(argc, argv);

    std::cout << "System call to gpsbabel.\n"
            "WARNING: IF THE COMPUTER GOES TO SLEEP, GPS DATA CAPTURE WILL STOP"
            "\nPress ctrl-c to finish." <<
            std::endl;

    // Call gpsbabel.
    std::stringstream command;
    command << '"' << environment.gpsbabelCall << "\" -T -i garmin -f usb: -o" <<
            " xcsv,style=" << environment.xcsvFormatFilename << " -F " <<
            environment.outputFilename;

    std::time_t t = std::time(0);
    std::tm* start_time = std::localtime(&t);

#ifdef __linux__
    int status = system(command.str().c_str());

    // Check the return status for errors.
    if(!WIFEXITED(status)) {
        //        std::cerr << "ERROR: failed to capture gps data. Exiting." << std::endl;
  //      std::exit(1);
    }
    else if(WEXITSTATUS(status)) {
        int exitCode = WEXITSTATUS(status);
        if(exitCode == 127) {
            // Exit code of 127 means that either the shell could not find the
            // gpsbabel executable or the gpsbabel process exited with code
            // 127. (In practice, I don't think gpsbabel ever exits with code
            // 127, but you never know.)
            std::cerr << "ERROR: probably unable to locate gpsbabel executable."
                    " Exiting." << std::endl;
        }
        else {
            std::cerr << "ERROR: gpsbabel failure. Exiting." << std::endl;
        }
        std::exit(1);
    }
#elif _WIN32
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        printf("\nERROR: Could not set control handler. Exiting.");
        return 1;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    TCHAR* command_line = new TCHAR[command.str().size() + 1];
    for (unsigned int i = 0; i < command.str().size(); ++i)
    {
        command_line[i] = command.str()[i];
    }
    command_line[command.str().size()] = '\0';

    if (!CreateProcess(NULL,   // No module name (use command line)
        command_line,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        false,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        std::cerr << "ERROR: failed to capture gps data. Exiting." << std::endl;
        std::exit(1);
    }

    gpsbabel_running = true;
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    delete[] command_line;
    command_line = nullptr;
#endif

    // Check to make sure that gpsbabel actually gave some output:

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
        // Delete the empty file.
        remove(environment.outputFilename.c_str());
        std::exit(1);
    }

    std::vector<GPSPosition> positions;

    set_gps_position_time(firstPosition, firstPosition, false, start_time);
    positions.push_back(firstPosition);
    GPSPosition position, previousPosition = firstPosition;
    while(GPSFile >> position) {
        set_gps_position_time(position, previousPosition, true);
        positions.push_back(position);
        previousPosition = position;
    }

    GPSFile.close();

    std::ofstream GPSFileRetimed(environment.outputFilename.c_str());
    for(unsigned int i = 0; i < positions.size(); ++i) {
        GPSFileRetimed << positions.at(i) << std::endl;
    }
    GPSFileRetimed.close();

    // At this point, we're confident that gpsbabel succeeded in capturing the
    // gps data.

    // This is the filename that our output will actually have. It's based on
    // the filename given by the user, but it contains an optional timestamp
    // and also has a file extension added if none was provided.
    std::string newFilename = environment.timestamping ?
            applyTimestamp(environment.outputFilename, firstPosition) :
            applyDefaultFileExtension(environment.outputFilename);

    std::cout << "\nReturned from gpsbabel. Converting output to kml... " <<
            std::flush;

    // Convert the gpsbabel output to kml format.
    command.str("");
    command << '"' << environment.gpsbabelCall << "\" -i " <<
            "xcsv,style=" << environment.xcsvFormatFilename << " -f " <<
            environment.outputFilename << " -x transform,trk=w,del -o kml -F "
            << newFilename;
    system(command.str().c_str());

    // Remove the xcsv gpsbabel output (if it wasn't overwritten already).
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

    // If there is no file extension,
    if(extensionIndex == std::string::npos ||
            (folderIndex != std::string::npos &&
            folderIndex >= extensionIndex)) {
        // Add the timestamp and default file extension to the end of the
        // filename.
        filename += timestamp;
        filename += DEFAULT_OUTPUT_EXTENSION;
    }
    else {
        // Insert the timestamp immediately before the file extension.
        filename.insert(extensionIndex, timestamp);
    }
    return filename;
}

static std::string applyDefaultFileExtension(std::string filename) {
    size_t extensionIndex = filename.rfind('.');
    size_t folderIndex = filename.find_last_of("/\\");

    // If there's no file extension,
    if(extensionIndex == std::string::npos ||
            (folderIndex != std::string::npos &&
            folderIndex >= extensionIndex)) {
        // Append the default file extension.
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

    // Parse the command-line arguments.
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
            // Only accept up to one positional argument.
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
