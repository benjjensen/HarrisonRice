/**
 * @file acquire.cpp
 *
 * To run without breaks in the acquired data:
 * sudo nice -n -20 ./acquire -f trigger_stopped
 */

// TODO: Clean up commented-out sections, remove windows-specific sections (?),
// remove global variables

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>

#include <ext/stdio_filebuf.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/core/ref.hpp>

#include "DataCapture.h"
#include "uvAPI.h"



const int NO_SIGNAL_PROCESS = -1;

const size_t BUFFER_ALIGNMENT = 4096;
const size_t BLOCKS_PER_BUFFER = 200;
const size_t BUFFER_SIZE = BLOCKS_PER_BUFFER * DIG_BLOCK_SIZE;

const size_t BLOCKS_PER_READ = 1;
const size_t READ_SIZE = DIG_BLOCK_SIZE * BLOCKS_PER_READ;

const size_t BLOCKS_PER_SETUP_ACQUIRE = 8000000;

const int SECONDS_TO_SLEEP = 3;

const std::string DEFAULT_OUTPUT_EXTENSION = ".dat";
const std::string DEFAULT_FILENAME = "uvdma.dat";



struct AcquireEnvironment {

    AcquireEnvironment() {
        // Default values for SetupBoard. These may change in acquire_parser.
        board_num = 0;
        internal_clock = CLOCK_EXTERNAL;
        single_channel_mode = 1; // Default to single channel mode.
        single_channel_select = 0;
        desiq = 0;
        desclkiq = 0;
        ecl_trigger = 0;
        ecl_trigger_delay = 0;
        dual_channel_mode = 0;
        dual_channel_select = 01;
        first_channel = 0; // Default to channel 0.
        second_channel = 3; // Default to channel 3.
        channels = 9; // Default bitwise channel 0 and 3.
        capture_count = 0;
        capture_depth = 0;
        decimation = 1;
        pretrigger_memory = 0;
        ttl_invert = 0;
        trigger_mode = NO_TRIGGER;
        trigger_channel = 0;
        trigger_slope = FALLING_EDGE;
        trigger_threshold_16 = 32768;
        trigger_threshold_14 = 8192;
        trigger_threshold_12 = 2048;
        trigger_hysteresis = 100;
        num_averages = 0;
        averager_length = 4096;
        fiducial = 0;
        force_calibration = 0;
        internal_frequency = 0.0;

        record_gps_data = false;
        signal_pid = NO_SIGNAL_PROCESS;
        user_outputfile = NULL;
    }
    unsigned short board_num;
    unsigned int internal_clock;
    unsigned int single_channel_mode;
    unsigned int single_channel_select;
    unsigned int desiq;
    unsigned int desclkiq;
    unsigned int ecl_trigger;
    unsigned int ecl_trigger_delay;
    unsigned int dual_channel_mode;
    unsigned int dual_channel_select;
    unsigned int first_channel;
    unsigned int second_channel;
    unsigned int channels;
    unsigned int capture_count;
    unsigned int capture_depth;
    unsigned int decimation;
    unsigned int pretrigger_memory;
    unsigned int ttl_invert;
    unsigned int trigger_mode;
    unsigned int trigger_channel;
    unsigned int trigger_slope;
    unsigned int trigger_threshold_16;
    unsigned int trigger_threshold_14;
    unsigned int trigger_threshold_12;
    unsigned int trigger_hysteresis;
    unsigned int num_averages;
    unsigned int averager_length;
    unsigned int fiducial;
    unsigned int force_calibration;
    double internal_frequency;

    bool record_gps_data;
    int signal_pid;
    char * user_outputfile;
};

/**
 * Reads the command sent in by the user and parses what board settings the user
 * wishes to set.
 */
AcquireEnvironment acquire_parser(int argc, char** argv);

/**
 * If "acquire" was sent with no arguments, a list of printf statements will
 * display to guide the user.
 */
void acquire_parser_printf();

/**
 * Takes the arguments read by acquire_parser and sets the board to run with the
 * desired settings.
 */
void acquire_set_session(uvAPI& pUV, AcquireEnvironment& environment);

/**
 * Handles the exit signal given by pressing ctrl-c.
 */
void exit_signal_handler(int signal, boost::atomic<bool>* flag_pointer);
void exit_signal_handler(int signal) {
    exit_signal_handler(signal, NULL);
}

/**
 * Adds a timestamp to the given filename, before any file type.
 *
 * The timestamp will be of the format
 * _YYYY-mm-dd__HH-MM-SS
 */
void timestamp_filename(std::string& selected_name, DataCapture& capture);

/**
 * The thread that writes the buffers to the file.
 *
 * This thread modifies all of its arguments except for the constant ones and
 * finished_capturing. However, it never modifies any arguments except for when
 * ready_to_save_buffer is true. Thus, it is safe for other threads to modify
 * the arguments only when ready_to_save_buffer is false.
 *
 * When finished_capturing becomes false, this thread finishes saving the buffer
 * it's currently saving and then exits.
 */
void write_thread_main_function(const HANDLE disk_fd,
        boost::atomic<bool>& finished_capturing,
        boost::atomic<bool>& ready_to_save_buffer,
        volatile bool& writing_to_first_buffer,
        volatile size_t& blocks_in_buffer_ready_to_save,
        const uint8_t* first_buffer, const uint8_t* second_buffer,
        volatile ssize_t& write_thread_status);

/**
 * Starts a gpsbabel process and the gps thread.
 *
 * The gps thread listens for input from the gpsbabel process and reads it into
 * GPSPosition objects, which can later be saved to a file.
 *
 * The gps thread only modifies capture_info, and may do so at any time. Thus,
 * it is not safe for any other thread to modify capture_info as long as the gps
 * thread is running.
 *
 * When continue_recording_gps becomes false, the gps thread kills the gpsbabel
 * process and exits.
 */
boost::thread* start_gps_thread(DataCapture& capture_info,
        boost::atomic_uint_fast32_t& blocks_acquired,
        boost::atomic<bool>& continue_recording_gps);

/**
 * The main function of the gps thread.
 *
 * This thread listens for input from the gpsbabel process and reads it into
 * GPSPosition objects, which can later be saved to a file.
 *
 * This thread only modifies capture_info, and may do so at any time. Thus, it
 * is not safe for any other thread to modify capture_info as long as this
 * thread is running.
 *
 * When continue_recording_gps becomes false, this thread kills the gpsbabel
 * process and exits.
 */
void gps_thread_main_function(const pid_t gps_process_id,
        const HANDLE gps_output_fd, DataCapture& capture_info,
        boost::atomic_uint_fast32_t& blocks_acquired,
        boost::atomic<bool>& continue_recording_gps);

/**
 * This function sends the abort signal to the parent process, if there is one.
 */
void send_abort_signal(const AcquireEnvironment environment);

/**
 * This function free()s the memory pointed to by its non-NULL arguments and then
 * sets those arguments to NULL.
 */
void free_memory(uint8_t*& first_buffer, uint8_t*& second_buffer);

/**
 * This function deletes any metadata or gps files associated with capture_info.
 */
void remove_meta_gps_files(DataCapture& capture_info);

int main(int argc, char** argv) {
    //--------------------------------------------------------------------------
    //                          Initialize the program
    //--------------------------------------------------------------------------

    // Move this process to its own cpuset, assuming that one was created for it
    // previously.
    pid_t process_id = getpid();
    std::stringstream command;
    // This is a safe command to pass to system() because it contains the
    // absolute path of echo.
    command << "/bin/echo " << process_id << " > /cgroup/cpuset/acquire/tasks";
    system(command.str().c_str());

    // This is the flag that tells whether we're still reading data or not. It
    // will remain true until either there's an error writing the data to file
    // or the exit signal handler function sets it to false.
    boost::atomic<bool> continue_reading_data(true);

    // Give continue_reading_data to exit_signal_handler() so that the function
    // can store the flag in its local static variable. This assignment only
    // works once; after that, exit_signal_handler is stuck with the flag we
    // gave it the first time.
    exit_signal_handler(0, &continue_reading_data);
    // Set the signal handler for when the user presses ctrl-c.
    // (Sneakily, this passes the pointer to exit_signal_handler(int), not
    // exit_signal_handler(int, boost::atomic<bool>*), so don't be confused.)
    signal(SIGINT, &exit_signal_handler);

    std::cout << "acquire continuously v1.1" << std::endl << std::endl;

    // The environment that the user defined with the command line arguments.
    const AcquireEnvironment environment;
    // The API that wraps the DAQ card.
    uvAPI uv;
    {
        // Get the settings that the user passed in with the command line
        // arguments.
        AcquireEnvironment mutable_environment = acquire_parser(argc, argv);

        // Initialize settings.
        if(mutable_environment.force_calibration) {
            // Force full setup.
            uv.setSetupDoneBit(mutable_environment.board_num, 0);
        }
        uv.setupBoard(mutable_environment.board_num);

        // Write user settings to the board.
        acquire_set_session(uv, mutable_environment);

        // After this point, the environment is not changing.
        environment = mutable_environment;
    }

    // Read the clock frequency.
    const unsigned int clock_frequency =
            uv.getAdcClockFreq(environment.board_num);
    std::cout << "ADC clock frequency ~= " << clock_frequency << "MHz" <<
            std::endl;
    fflush(stdout);


    // The first buffer where incoming data is stored.
    uint8_t *first_buffer = NULL;
    // The second buffer where incoming data is stored.
    uint8_t *second_buffer = NULL;

    // Allocate page-aligned buffers for DMA.
    int error = posix_memalign((void**)&first_buffer, BUFFER_ALIGNMENT,
            BUFFER_SIZE);
    error |= posix_memalign((void**)&second_buffer, BUFFER_ALIGNMENT,
            BUFFER_SIZE);
    if(error) {
        std::cerr << "ERROR: UNABLE TO ALLOCATE MEMORY" << std::endl;
        send_abort_signal(environment);
        free_memory(first_buffer, second_buffer);
        return -1;
    }



    //--------------------------------------------------------------------------
    //                             Setup DAQ Card
    //--------------------------------------------------------------------------

    // Tell the board that we're going to acquire data.
    // After this function is called, the beginning of the data we're acquiring
    // does not start until we call uv.X_Read, and so all the data that should
    // be captured during the intervening time will be lost. (This amounts to
    // quite a bit of data because the SetupAcquire function sleeps for 50 ms.)
    // Thus, this function should only be called once, with an argument so large
    // that we will never acquire more than that much data in a single run of
    // this program. We have chosen 8,000,000 blocks, which is just under 8 TB
    // of data. If this is not enough, change the value of
    // BLOCKS_PER_SETUP_ACQUIRE.
    uv.SetupAcquire(environment.board_num, BLOCKS_PER_SETUP_ACQUIRE);

    // Acquire and discard the first block of data. Once this happens, the DAQ
    // card starts saving the incoming data to its internal buffer until it has
    // saved BLOCKS_PER_SETUP_ACQUIRE blocks.
    uv.X_Read(environment.board_num, first_buffer, READ_SIZE);

    // ----------IMPORTANT----------
    // For some reason, the DAQ card does not always work properly if we don't
    // give it a little bit of a head start on acquiring data. It doesn't make
    // any sense at all, but it's true. Once we give it a head start by sleeping
    // for a few seconds, we can start saving data and stay caught up with it
    // indefinitely and it won't skip data. If we jump right in and start
    // acquiring data as fast as we can as soon as we call SetupAcquire (above),
    // the DAQ card can't take the pressure and often skips some data in the
    // first 8 GB of captured data.
    sleep(SECONDS_TO_SLEEP); // Sleep for 3 seconds.



    //--------------------------------------------------------------------------
    //                                 Open Files
    //--------------------------------------------------------------------------

    // We have to wait until now to open the data files because we want to give
    // them a timestamp accurate to the second, and so we wait until after we've
    // slept to give the DAQ card a head start.

    // An object containing information about the data we're about to capture.
    DataCapture capture_info;
    capture_info.name = (environment.user_outputfile == NULL ?
            "Unnamed" : environment.user_outputfile);

    // If the user specified a filename, use that name, else use the default
    // name.
    std::string filename = DATA_FOLDER + "/" +
            (environment.user_outputfile == NULL ?
            DEFAULT_FILENAME : environment.user_outputfile);

    // Add the timestamp to the filename.
    timestamp_filename(filename, capture_info);

    capture_info.data_filename = filename;

    // Reserve space on disk for the metadata file, in case we completely
    // fill up the disk with data.
    if(capture_info.reserve_meta_file_space() != 0) {
        std::cerr << "ERROR: UNABLE TO RESERVE SPACE FOR THE METADATA FILE" <<
                std::endl;

        remove_meta_gps_files(capture_info);
        send_abort_signal(environment);
        free_memory(first_buffer, second_buffer);

        return -1;
    }
    // Reserve space on disk for the gps file, in case we completely fill up the
    // disk with data.
    // Short circuiting ensures that we don't reserve space for the gps file if
    // record_gps_data == false.
    if(environment.record_gps_data &&
            capture_info.reserve_gps_file_space() != 0) {
        std::cerr << "ERROR: UNABLE TO RESERVE SPACE FOR THE GPS FILE" <<
                std::endl;

        remove_meta_gps_files(capture_info);
        send_abort_signal(environment);
        free_memory(first_buffer, second_buffer);

        return -1;
    }

    // Open the data disk file.
    HANDLE outfile_fd = uv.X_CreateFile((char*)filename.c_str());
    // disk_fd will be less than zero if the open operation failed.
    if(outfile_fd < 0) {
        std::cerr << "ERROR: UNABLE TO OPEN OUTPUT FILE " << filename <<
                std::endl;

        remove_meta_gps_files(capture_info);
        send_abort_signal(environment);
        free_memory(first_buffer, second_buffer);

        return -1;
    }



    //--------------------------------------------------------------------------
    //                           Setup write thread
    //--------------------------------------------------------------------------

    // We must wait until now to setup the write thread so that the output file
    // descriptor in disk_fd is already initialized. (disk_fd is initialized
    // just above here.)

    // The flag that indicates whether we're done capturing new data or not.
    boost::atomic<bool> finished_capturing(false);
    // The flag that indicates whether there's a buffer that's ready to be saved
    // to file.
    boost::atomic<bool> ready_to_save_buffer(false);

    // The flag that indicates whether the main thread is writing new data to
    // the first buffer. (If not, it's writing new data to the second buffer.)
    volatile bool writing_to_first_buffer = true;
    // How many blocks were stored in the buffer that's ready to save to file.
    volatile size_t blocks_in_buffer_ready_to_save = 0;

    // The status of the write thread. This is how the write thread keeps the
    // main thread informed about any errors that occurred while writing the
    // data to file.
    volatile ssize_t write_thread_status = 0;
    // The expected status of the write thread.
    ssize_t expected_write_thread_status = 0;

    //----------------MULTITHREADING START----------------

    // The following variables and objects must not be changed or accessed by
    // the main thread during the multithreaded portion of the program:
    //     capture_info (except as an argument to start the gps thread)
    //
    // The following variables and objects must not be changed or accessed by
    // the main thread during the multithreaded portion of the program except
    // when ready_to_save_buffer is false:
    //     ready_to_save_buffer (can be accessed because it's atomic)
    //     finished_capturing (can be accessed because it's atomic)
    //     writing_to_first_buffer
    //     blocks_in_buffer_ready_to_save
    //     write_thread_status
    // Additionally, the main thread must only write to the buffer indicated by
    // writing_to_first_buffer; that is, when writing_to_first_buffer is true,
    // the main thread must not write to the second buffer, and when
    // writing_to_first_buffer is false, the main thread must not write to the
    // first buffer.

    //----------------WRITE THREAD START----------------

    // Start up the thread that will write to disk the data that we read in this
    // thread.
    boost::thread write_thread(write_thread_main_function, outfile_fd,
            boost::ref(finished_capturing), boost::ref(ready_to_save_buffer),
            boost::ref(writing_to_first_buffer),
            boost::ref(blocks_in_buffer_ready_to_save), first_buffer,
            second_buffer, boost::ref(write_thread_status));



    //--------------------------------------------------------------------------
    //                            Setup gps thread
    //--------------------------------------------------------------------------

    // This is the number of blocks that we've captured so far. It needs to be
    // atomic because the gps thread accesses this value while the main thread
    // writes to it.
    boost::atomic_uint_fast32_t blocks_captured(0);

    //----------------GPS THREAD START----------------

    boost::thread* gps_thread = NULL;
    if(environment.record_gps_data) {
        // Start up the thread that will record the gps data.
        gps_thread = start_gps_thread(capture_info, blocks_captured,
                continue_reading_data);
        if(gps_thread == NULL) {
            std::cerr << "ERROR: UNABLE TO START GPS THREAD" << std::endl;
        }
    }



    //--------------------------------------------------------------------------
    //                             Capture the data
    //--------------------------------------------------------------------------

    // The number of blocks in the buffer currently being written to.
    unsigned int blocks_in_current_buffer = 0;
    // The number of times that uv.SetupAcquire has been called.
    unsigned int setup_acquire_call_count = 1;
    // Whether we had to abort reading the data because of an error saving it to
    // file.
    bool reading_aborted = false;

    // Safe to change this because ready_to_save_buffer is false.
    writing_to_first_buffer = true;
    // The buffer that needs to be written to next.
    uint8_t* current_buffer = first_buffer;

    // The time when we started acquiring data.
    timespec start_time;
    // The time when we finished acquiring data.
    timespec end_time;

    // Measure the time when we started capturing data. This should be after we
    // sleep (above) because most of the data that is captured while we're
    // sleeping is lost.
    clock_gettime(CLOCK_REALTIME, &start_time);

    // The continue_reading_data flag will be true until the user presses
    // ctrl-c (or the SIGINT signal is received).
    // (The exit_signal_handler function changes continue_reading_data.)
    while(continue_reading_data.load()) {
        // This is the total number of blocks we will have acquired when we run
        // out of the current call to SetupAcquire; in other words, once we've
        // acquired block_limit total blocks, it's time to call SetupAcquire
        // again.
        unsigned int block_limit =
                BLOCKS_PER_SETUP_ACQUIRE * setup_acquire_call_count;
        for(; blocks_captured.load() < block_limit &&
                continue_reading_data.load();
                blocks_captured.add(BLOCKS_PER_READ)) {
            // Read a block from the board.
            uv.X_Read(environment.board_num, current_buffer, READ_SIZE);
            blocks_in_current_buffer += BLOCKS_PER_READ;

            // If we've filled up the current buffer,
            if(blocks_in_current_buffer >= BLOCKS_PER_BUFFER) {
                // Change which buffer we're writing to.
                current_buffer = (writing_to_first_buffer ?
                        second_buffer : first_buffer);

                // If ready_to_save_buffer is still true, it means that the
                // write thread hasn't finished saving the last buffer we filled
                // up.
                // If this happens, there's a problem, and we might be skipping
                // data because we can't write it to disk as fast as we can read
                // it in.
                if(ready_to_save_buffer.load()) {
                    // Print an error message, then wait until the write thread
                    // is done writing.
                    std::cerr << "ERROR: BUFFER NOT EMPTIED BEFORE IT WAS " <<
                            "NEEDED AGAIN" << std::endl;
                    while(ready_to_save_buffer.load()) {
                        SLEEP(1); // Sleep for 1 millisecond.
                    }
                }

                //----------------CRITICAL SECTION START----------------

                // At this point, it's safe to access and change variables that
                // the write thread uses because we know that it is waiting for
                // ready_to_save_buffer to become true.

                // If there was an error saving the data from the last buffer,
                if(write_thread_status != expected_write_thread_status) {
                    std::cerr << "ERROR: WRITE OPERATION FAIL" << std::endl;

                    // Forget about the blocks we were about to save from this
                    // buffer.
                    blocks_captured.sub(blocks_in_current_buffer);
                    // If the write thread managed to save any data at all,
                    if(write_thread_status > 0) {
                        // Calculate how many blocks the write thread managed
                        // to save and forget about the blocks it didn't manage
                        // to save.
                        int size = DIG_BLOCK_SIZE;
                        int blocks_written = write_thread_status / size;
                        int blocks_not_written =
                                blocks_in_buffer_ready_to_save - blocks_written;
                        blocks_captured.sub(blocks_not_written);
                    }
                        // If the write thread didn't save any data,
                    else {
                        // Forget about all the blocks we thought we saved from
                        // the last buffer.
                        blocks_captured.sub(
                                blocks_in_buffer_ready_to_save);
                    }

                    reading_aborted = true;
                    continue_reading_data.store(false);
                    break;
                }

                // Tell the write thread how many blocks are currently in this
                // buffer.
                blocks_in_buffer_ready_to_save = blocks_in_current_buffer;

                // The write_thread_status variable contains the number of bytes
                // that the write thread was able to write to the disk. Thus,
                // the status we expect is equal to the number of 1 MB blocks
                // we're asking it to write multiplied by the number of bytes
                // per MB.
                expected_write_thread_status = blocks_in_buffer_ready_to_save *
                        DIG_BLOCK_SIZE;

                // (Reset the count of blocks in the current buffer for in a
                // minute when we start writing to the next buffer.)
                blocks_in_current_buffer = 0;

                // Tell the write thread which buffer we're going to be writing
                // on (so it knows to read from the other buffer).
                writing_to_first_buffer = !writing_to_first_buffer;

                // Tell the write thread that we're ready for it to start
                // reading and saving the data from the buffer we just finished
                // writing.
                ready_to_save_buffer.store(true);

                //----------------CRITICAL SECTION END----------------
            }
            else {
                // The current buffer is not full, so just move the pointer up
                // by how much we just read.
                current_buffer += READ_SIZE;
            }
        }

        // Let the user know if there were any data overruns.
        unsigned long overruns = uv.getOverruns(environment.board_num);
        if(overruns > 0) {
            std::cerr << "WARNING: " << overruns << " OVERRUNS OCCURRED" <<
                    std::endl;
        }

        // If we will go through this loop again,
        if(continue_reading_data.load()) {
            // Call SetupAcquire again to get more data.
            ++setup_acquire_call_count;
            uv.SetupAcquire(environment.board_num, BLOCKS_PER_SETUP_ACQUIRE);
        }
    }
    // Measure the time when we stopped capturing new data.
    clock_gettime(CLOCK_REALTIME, &end_time);

    current_buffer = NULL;

    // Wait for the most recent buffer to finish being saved.
    while(ready_to_save_buffer.load()) {
        SLEEP(1); // Sleep for 1 millisecond.
    }

    //----------------CRITICAL SECTION START----------------
    // This critical section has three possible control paths, each of which end
    // this critical section and end the write thread.

    // At this point, it's safe to change variables that the other thread uses
    // because we know that it is waiting for ready_to_save_buffer to become
    // true.

    // If there wasn't previously a problem with saving data,
    if(!reading_aborted) {
        // If there was a problem with saving the most recent complete buffer,
        if(write_thread_status != expected_write_thread_status) {
            std::cerr << "ERROR: WRITE OPERATION FAIL" << std::endl;

            // Forget about the blocks we were about to save from the
            // partially-full buffer.
            blocks_captured.sub(blocks_in_current_buffer);

            // If the write thread managed to save any data at all,
            if(write_thread_status > 0) {
                // Calculate how many blocks the write thread managed to save
                // and forget about the blocks it didn't manage to save.
                int size = DIG_BLOCK_SIZE;
                int blocks_written = write_thread_status / size;
                int blocks_not_written = blocks_in_buffer_ready_to_save -
                        blocks_written;
                blocks_captured.sub(blocks_not_written);
            }
                // If the write thread didn't save any data,
            else {
                // Forget about all the blocks we thought we saved from the last
                // buffer.
                blocks_captured.sub(blocks_in_buffer_ready_to_save);
            }

            // Have the write thread go ahead and exit.
            blocks_in_buffer_ready_to_save = 0;
            blocks_in_current_buffer = 0;
            finished_capturing.store(true);
            ready_to_save_buffer.store(true);

            //----------------CRITICAL SECTION END----------------

            // Wait for the write thread to finish up.
            write_thread.join();

            //----------------WRITE THREAD END----------------

            reading_aborted = true;
        }
        else {
            // Save to file the blocks that have already been read into the
            // current buffer:

            // Let the other thread know which buffer to read from.
            writing_to_first_buffer = !writing_to_first_buffer;

            // Let the other thread know how many blocks we wrote into that
            // buffer.
            blocks_in_buffer_ready_to_save = blocks_in_current_buffer;

            expected_write_thread_status = blocks_in_buffer_ready_to_save *
                    DIG_BLOCK_SIZE;

            blocks_in_current_buffer = 0;

            // Let the other thread know that we're done capturing new data, so
            // it can finish once it's done saving this most recent buffer.
            finished_capturing.store(true);

            // Tell the other thread to start saving the data we captured.
            ready_to_save_buffer.store(true);

            //----------------CRITICAL SECTION END----------------

            // Wait for the writing thread to finish.
            write_thread.join();

            //----------------WRITE THREAD END----------------

            // At this point, the other thread no longer exists, so we don't
            // need to worry about not accessing variables it might be changing.

            // If there was a problem with saving this last partially-full
            // buffer,
            if(write_thread_status != expected_write_thread_status) {
                std::cerr << "ERROR: WRITE OPERATION FAIL" << std::endl;

                // If the write thread managed to save any data at all,
                if(write_thread_status > 0) {
                    // Calculate how many blocks the write thread managed to
                    // save and forget about the blocks it didn't manage to
                    // save.
                    int size = DIG_BLOCK_SIZE;
                    int blocks_written = write_thread_status / size;
                    int blocks_not_written = blocks_in_buffer_ready_to_save -
                            blocks_written;
                    blocks_captured.sub(blocks_not_written);
                }
                    // If the write thread didn't save any data,
                else {
                    // Forget about all the blocks we thought we saved from the
                    // last buffer.
                    blocks_captured.sub(blocks_in_buffer_ready_to_save);
                }

                reading_aborted = true;
            }
        }
    }
        // Otherwise, if we exited the capture loop because of an error writing,
    else {
        // Have the write thread go ahead and exit.
        blocks_in_buffer_ready_to_save = 0;
        blocks_in_current_buffer = 0;
        finished_capturing.store(true);
        ready_to_save_buffer.store(true);

        //----------------CRITICAL SECTION END----------------

        // Wait for the write thread to finish up.
        write_thread.join();

        //----------------WRITE THREAD END----------------
    }

    // If there was an error writing the data to file at all (whether in the
    // capture loop or after it),
    if(reading_aborted) {
        // Send the signal to the parent process (sampler) to let it know that
        // we had to abort.
        send_abort_signal(environment);
    }

    if(environment.record_gps_data && gps_thread != NULL) {
        // The gps thread got the signal to finish when we marked
        // continueReadingData as false.
        // Wait for the gps thread to finish.
        gps_thread->join();
        delete gps_thread;
        gps_thread = NULL;
    }

    //----------------GPS THREAD END----------------
    //----------------MULTITHREADING END----------------

    //--------------------------------------------------------------------------
    //                           Output and cleanup
    //--------------------------------------------------------------------------

    std::cout << std::endl << "Data capture and writing to disk complete." <<
            std::endl;

    // Calculate the time spent capturing data.
    double elapsed_time_seconds = (end_time.tv_sec - start_time.tv_sec) +
            (end_time.tv_nsec - start_time.tv_nsec) / (double)1000000000;

    capture_info.duration = elapsed_time_seconds;
    capture_info.size = blocks_captured.load();

    printf("Data captured: %lu MB\n", blocks_captured.load());
    printf("Elapsed time: %.4f s\n", elapsed_time_seconds);
    printf("Average data capture rate: %.4f MB/s\n\n\n",
            blocks_captured.load() / elapsed_time_seconds);


    // This EXPECTED_RATE is the data transfer rate, in MB/s, that ideally would
    // happen if we're sampling at adcClock MHz. The granularity of adcClock is
    // 1 MHz, so this calculated rate won't be exactly correct, but it can give
    // us a good idea.

    // Multiply by 1,000,000 because adcClock is in MHz; multiply by 2 because
    // we capture two bytes per sample; divide by (1024 * 1024) because 1 MB =
    // 1024 * 1024 bytes.
    const double EXPECTED_RATE = clock_frequency * 1000000.0 * 2 / 1024 / 1024;

    std::cout << "--------------------------------------\n";
    printf("Proportion of data points gathered:\n%.5f%%\n",
            blocks_captured.load() / elapsed_time_seconds / EXPECTED_RATE
            * 100);
    std::cout << "--------------------------------------\n\n\n";


    // Save the metadata to a file.
    // We know that there's enough space on disk to store the metadata, even if
    // the disk filled up completely, because we reserved it earlier.
    capture_info.write_to_file();
    // Output the name of the metadata file so that any program that knows what
    // to look for (e.g. sampler) can know what it is.
    std::cout << CAPTURE_META_FILENAME_HANDOFF_TAG << " " <<
            capture_info.meta_filename << std::endl << std::endl;

    if(environment.record_gps_data) {
        // Save the gps data to a file.
        capture_info.save_gps_to_google_earth_file();
    }

    fflush(stdout);

    // Close output file.
    if(outfile_fd) {
        uv.X_Close(outfile_fd);
        outfile_fd = INVALID_HANDLE_VALUE;
    }

    // Deallocate resources.
    free_memory(first_buffer, second_buffer);

    return 0;
}

void write_thread_main_function(const HANDLE disk_fd,
        boost::atomic<bool>& finished_capturing,
        boost::atomic<bool>& ready_to_save_buffer,
        volatile bool& writing_to_first_buffer,
        volatile size_t& blocks_in_buffer_ready_to_save,
        const uint8_t* first_buffer, const uint8_t* second_buffer,
        volatile ssize_t& write_thread_status) {
    // The flag finished_capturing should be false until the capture thread
    // (main thread) finishes capturing all of the data.
    while(!finished_capturing.load()) {
        // The capture thread changes ready_to_save_buffer when it has filled a
        // buffer with data.
        while(!ready_to_save_buffer.load()) {
            // Wait for the capture thread.
            SLEEP(1); // Sleep for 1 millisecond.
        }

        //----------------CRITICAL SECTION START----------------

        // At this point, it's safe to access and modify the variables we share
        // between threads, because the write thread is either (a) busy reading
        // new data into a buffer that we're not going to access or (b) waiting
        // for us to mark ready_to_save_buffer as false.

        // Figure out which buffer is full. If the capture thread is writing to
        // the first buffer, that means that the second buffer is full, and
        // vice-versa.
        const void* buffer =
                (writing_to_first_buffer ? second_buffer : first_buffer);

        // Write the data in the buffer to the output file.
        write_thread_status = write(disk_fd, buffer,
                blocks_in_buffer_ready_to_save * DIG_BLOCK_SIZE);

        // Let the capture thread know that we're done saving the data to the
        // output file.
        ready_to_save_buffer.store(false);

        //----------------CRITICAL SECTION END----------------
    }
}

boost::thread* start_gps_thread(DataCapture& capture_info,
        boost::atomic_uint_fast32_t& blocks_acquired,
        boost::atomic<bool>& continue_recording_gps) {
    const int READ_FD = 0;
    const int WRITE_FD = 1;

    const int SUCCESS = 0;
    const int FAILURE = -1;

    const int CHILD_PID = 0;

    // A pipe to get info from acquire and read it in sampler (via acquire's
    // standard output):
    HANDLE pipe[2];
    if(pipe(pipe) != SUCCESS) {
        std::cerr << "ERROR: UNABLE TO OPEN PIPE FOR GPSBABEL OUTPUT" <<
                std::endl;
        return NULL;
    }

    pid_t pid;
    switch(pid = fork()) {
        // -1 means that the fork failed.
    case FAILURE:
        std::cerr << "ERROR: FAILURE TO FORK FOR GPSBABEL" << std::endl;
        return NULL;
        // If we get 0, we're the child process.
    case CHILD_PID:
        // Set up the process interaction by replacing standard out with the
        // pipe to the parent process.
        if(dup2(pipe[WRITE_FD], STDOUT_FILENO) == FAILURE) {
            std::cerr << "ERROR: UNABLE TO PIPE STDOUT FROM GPS PROCESS INTO" <<
                    " PARENT PROCESS" << std::endl;
        }
        // Close the side of the pipe we (the child process) won't use:
        if(close(pipe[READ_FD]) != SUCCESS) {
            std::cerr << "ERROR: UNABLE TO CLOSE UNUSED SIDE OF PIPE IN GPS " <<
                    "PROCESS" << std::endl;
        }

        // TODO change to the sys core group?

        // Not necessary to change directories because gpsbabel is on the path
        // and gpsbabel_xcsv_format.txt is in the current directory.
        execl("/usr/local/bin/gpsbabel", "gpsbabel", "-T", "-i", "garmin", "-f",
                "usb:", "-o", "xcsv,style=gpsbabel_xcsv_format.txt", "-F",
                "/dev/stdout", NULL);

        std::cerr << "ERROR: EXEC CALL FAILURE IN GPS CHILD PROCESS" <<
                std::endl;
        std::exit(-1);
        // Otherwise, we're the parent process, so break out of this switch
        // statement and continue on.
    default:
        break;
    }

    // Close the side of the pipe that we won't use in the parent process.
    if(close(pipe[WRITE_FD]) != SUCCESS) {
        std::cerr << "ERROR: UNABLE TO CLOSE UNUSED SIDE OF PIPE IN ACQUIRE " <<
                "PROCESS" << std::endl;
    }

    const pid_t gps_process_id = pid;
    const HANDLE gps_output_fd = pipe[READ_FD];

    // Start the gps thread and pass in the information it needs to function.
    boost::thread *gps_thread = new boost::thread(gps_thread_main_function,
            gps_process_id, gps_output_fd, boost::ref(capture_info),
            boost::ref(blocks_acquired), boost::ref(continue_recording_gps));
    return gps_thread;
}

void gps_thread_main_function(const pid_t gps_process_id,
        const HANDLE gps_output_fd, DataCapture& capture_info,
        boost::atomic_uint_fast32_t& blocks_acquired,
        boost::atomic<bool>& continue_recording_gps) {
    // Get an istream from the pipe file descriptor.
    __gnu_cxx::stdio_filebuf<char> *sb =
            new __gnu_cxx::stdio_filebuf<char>(gps_output_fd, std::ios::in);
    std::istream input_from_gps(sb);

    GPSPosition position;
    // The flag continue_recording_gps will be set to false by another thread;
    // possibly the main thread, but more likely by the SIGINT signal handler.
    while(continue_recording_gps.load()) {
        // Read the input from gpsbabel into a position object.
        input_from_gps >> position;

        // Check to make sure that the main thread is still not done capturing;
        // the previous line blocks for about a second each time through the
        // loop, and a lot can happen in a second. Also check to be sure that we
        // didn't run out of gps fixes from gpsbabel.
        if(!continue_recording_gps.load() || input_from_gps.eof()) {
            break;
        }

        position.blocks_captured = blocks_acquired.load();
        capture_info.gps_positions.push_back(position);
    }

    // Stop the gpsbabel process, as if by pressing ctrl-c.
    kill(gps_process_id, SIGINT);
    // Wait for the gpsbabel process to finish.
    // This function call is necessary to prevent gpsbabel from becoming a
    // zombie process.
    int error = waitid(P_PID, gps_process_id, NULL, WEXITED);
    if(error < 0) {
        std::cerr << "ERROR: FAILED TO CLOSE GPSBABEL PROCESS" << std::endl;
    }

    if(close(gps_output_fd) != 0) {
        std::cerr << "ERROR: FAILED TO CLOSE GPS STDOUT FILE DESCRIPTOR" <<
                std::endl;
    }
    delete sb;
}

std::string get_current_timestamp(DataCapture& capture) {
    std::time_t t = std::time(0);
    std::tm* time = std::localtime(&t);

    std::stringstream timestamp;

    capture.year = time->tm_year + 1900;
    capture.month = time->tm_mon + 1;
    capture.date = time->tm_mday;
    capture.hour = time->tm_hour;
    capture.minute = time->tm_min;
    capture.second = time->tm_sec;

    timestamp << std::setfill('0');
    timestamp << "_" << capture.year << "-" << std::setw(2) << capture.month <<
            "-" << std::setw(2) << capture.date;
    timestamp << "__" << std::setw(2) << capture.hour << "-" << std::setw(2) <<
            capture.minute << "-" << std::setw(2) << capture.second;

    return timestamp.str();
}

void timestamp_filename(std::string& selected_name, DataCapture& capture) {
    std::string timestamp = get_current_timestamp(capture);

    size_t extension_index = selected_name.rfind('.');

    if(extension_index == std::string::npos) {
        selected_name += timestamp;
        selected_name += DEFAULT_OUTPUT_EXTENSION;
    }
    else {
        selected_name.insert(extension_index, timestamp);
    }
}

void exit_signal_handler(int signal, boost::atomic<bool>* flag_pointer) {
    static boost::atomic<bool> &continue_reading_data = *flag_pointer;
    if(flag_pointer != NULL) {
        return;
    }

    if(continue_reading_data.load()) {
        continue_reading_data.store(false);
    }
    else {
        std::cout << "----Exit signal received outside of loop----" <<
                std::endl;
    }
}

void send_abort_signal(const AcquireEnvironment environment) {
    if(environment.signal_pid != NO_SIGNAL_PROCESS) {
        std::cout << "Sending abort signal to signal process " <<
                environment.signal_pid << std::endl;
        kill(environment.signal_pid, SIGUSR1);
    }
}

void free_memory(uint8_t*& first_buffer, uint8_t*& second_buffer) {
    if(first_buffer) {
        free(first_buffer);
        first_buffer = NULL;
    }
    if(second_buffer) {
        free(second_buffer);
        second_buffer = NULL;
    }
}

void remove_meta_gps_files(DataCapture& capture_info) {
    if(capture_info.meta_filename != "") {
        remove(capture_info.meta_filename.c_str());
        capture_info.meta_filename = "";
    }
    if(capture_info.gps_filename != "") {
        remove(capture_info.gps_filename.c_str());
        capture_info.gps_filename = "";
    }
}

AcquireEnvironment acquire_parser(int argc, char** argv) {
    AcquireEnvironment environment;
    int arg_index = 0;

    // check how many arguments, if run without arguements prints usage.
    if(argc == 1) {
        acquire_parser_printf();
        exit(1);
    }
    else {
        // starting at second argument look for options
        for(arg_index = 1; arg_index < argc; arg_index++) {
            if(strcmp(argv[arg_index], "-pid") == 0) {
                // Make sure option is followed by the process id
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.signal_pid = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "Invalid process id with -pid option" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-gps") == 0) {
                environment.record_gps_data = true;
            }
            else if(strcmp(argv[arg_index], "-b") == 0) {
                // make sure option is followed by (BoardNum)
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.board_num = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "Invalid board number with ""-b"" option" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-forceCal") == 0) {
                environment.force_calibration = 1;
                std::cout << "force calibration selected" << std::endl;
            }
            else if(strcmp(argv[arg_index], "-ic") == 0) {
                environment.internal_clock = CLOCK_INTERNAL;
                std::cout << "internal clock selected" << std::endl;
            }
            else if(strcmp(argv[arg_index], "-freq") == 0) {
                // make sure option is followed by (Chan N)user_outputfile  = argv[arg_index];
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taking care of the next arguement
                    environment.internal_frequency = atoi(argv[arg_index]);
                    if(environment.internal_frequency <= 2000000000 && environment.internal_frequency >= 50000000) {
                        std::cout << "Internal clock frequency " << environment.internal_frequency << std::endl;
                    }
                    else {
                        std::cout << "Frequency selected must be between 300MHz and 2GHz for AD12 and 50MHz and 250MHz for AD16" << std::endl;
                        exit(1);
                    }
                }
                else {
                    std::cout << "Frequency selected must be between 300MHz and 2GHz for AD12 and 50MHz and 250MHz for AD16" << std::endl;
                    exit(1);
                }
            }

            else if(strcmp(argv[arg_index], "-f") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.user_outputfile = argv[arg_index];
                }
            }
            else if(strcmp(argv[arg_index], "-scm") == 0) {
                environment.single_channel_mode = 1;
                environment.single_channel_select = 0;
            }
            else if(strcmp(argv[arg_index], "-scs") == 0) {
                // make sure option is followed by (Chan N)
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.single_channel_select = atoi(argv[arg_index]);
                    std::cout << "SingleChannelSelect " << environment.single_channel_select << std::endl;
                }
                else {
                    std::cout << "(Chan N) must be in range 0,1 for 2-channel boards and 0,1,2,3 for 4-channel boards" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-desiq") == 0) {
                environment.desiq = 1;
                environment.single_channel_mode = 1;
                environment.single_channel_select = 0;
            }
            else if(strcmp(argv[arg_index], "-desclkiq") == 0) {
                environment.desclkiq = 1;
                environment.single_channel_mode = 1;
                environment.single_channel_select = 0;
            }
            else if(strcmp(argv[arg_index], "-ecltrig") == 0) {
                environment.ecl_trigger = 1;
            }
            else if(strcmp(argv[arg_index], "-ecldelay") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.ecl_trigger_delay = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "ECL trigger delay must be [0,2^32]" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-dcm") == 0) {
                environment.dual_channel_mode = 1;
                environment.dual_channel_select = 3; // Default DualChannelSelect = channel 0 channel 1
            }
            else if(strcmp(argv[arg_index], "-dcs") == 0) {
                // make sure option is followed by (Chan N)
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.dual_channel_select = atoi(argv[arg_index]);
                    environment.first_channel = environment.dual_channel_select / 10;
                    environment.second_channel = environment.dual_channel_select % 10;
                    environment.channels = (1 << environment.first_channel) + (1 << environment.second_channel); //bitwise representation of channels
                }
                else {
                    std::cout << "(Chan NM) must be 01, 02, 03, 12, 13, 23" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-capture_count") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.capture_count = atoi(argv[arg_index]);
                }
                else {
                    printf("setCaptureDepth must be [0,2^32] \n");
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-capture_depth") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.capture_depth = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "capture_depth must a multiple of 8 up to 2^32. 0 = normal acquisition." << std::endl;
                    exit(1);
                }

                if((environment.capture_depth % 8) != 0) {
                    std::cout << "capture_depth must a multiple of 8 up to 2^32. 0 = normal acquisition." << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-dec") == 0) {
                // Make sure option is followed by the desired decimation amount
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.decimation = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "Decimation must either be 1, 2, 4, 8" << std::endl;
                    exit(1);
                }

                if(!((environment.decimation == 1) ||
                        (environment.decimation == 2) ||
                        (environment.decimation == 4) ||
                        (environment.decimation == 8))) {
                    std::cout << "Decimation must either be 1, 2, 4, 8" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-pretrigger") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.pretrigger_memory = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "pretrigger must be 0 to 4096" << std::endl;
                    exit(1);
                }
                if(environment.pretrigger_memory > 4096) {
                    std::cout << "pretrigger must be 0 to 4096" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-ttledge") == 0) {
                environment.trigger_mode = TTL_TRIGGER_EDGE;
            }
            else if(strcmp(argv[arg_index], "-ttlinv") == 0) {
                environment.ttl_invert = 1;
            }
            else if(strcmp(argv[arg_index], "-hdiode") == 0) {
                environment.trigger_mode = HETERODYNE;
            }
            else if(strcmp(argv[arg_index], "-syncselrecord") == 0) {
                environment.trigger_mode = SYNC_SELECTIVE_RECORDING;
            }
            else if(strcmp(argv[arg_index], "-analog") == 0) {
                environment.trigger_mode = WAVEFORM_TRIGGER;
            }
            else if(strcmp(argv[arg_index], "-analog_ch") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.trigger_channel = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "analog_ch must be either 0,1,2,3 (4-channel boards), or 0,1 (2-channel boards)" << std::endl;
                    exit(1);
                }
                if(environment.pretrigger_memory > 2047) {
                    std::cout << "analog_ch must be either 0,1,2,3 (4-channel boards), or 0,1 (2-channel boards)" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-threshold") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.trigger_threshold_12 = atoi(argv[arg_index]);
                    environment.trigger_threshold_16 = environment.trigger_threshold_12; //this may need a better scheme
                    environment.trigger_threshold_14 = environment.trigger_threshold_12; //this may need a better scheme
                }
                else {
                    std::cout << "thresh_a must be 0 to 65535 (16bit)" << std::cout;
                    std::cout << "thresh_a must be 0 to 16383 (14bit)" << std::cout;
                    std::cout << "thresh_a must be 0 to 4095 (12bit)" << std::cout;
                    exit(1);
                }
            }

            else if(strcmp(argv[arg_index], "-hysteresis") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.trigger_hysteresis = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "thresh_b must be 0 to 65535" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-avg") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taking care of the next arguement
                    environment.num_averages = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "NumAverages must be [0,2^32]" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-avg_len") == 0) {
                if(argc > (arg_index + 1)) {
                    arg_index++; // increment the arguement index b/c we have now taken care of the next arguement
                    environment.averager_length = atoi(argv[arg_index]);
                }
                else {
                    std::cout << "AveragerLength must be [0, 2^17]" << std::endl;
                    exit(1);
                }
            }
            else if(strcmp(argv[arg_index], "-fiducial") == 0) {
                environment.fiducial = 1;
            }
        }
    }
    return environment;
}

void acquire_set_session(uvAPI& uv, AcquireEnvironment& environment) {
    uv.selClock(environment.board_num, environment.internal_clock);
    int defaultchannels = uv.GetAllChannels(environment.board_num);
    if(uv.IS_ISLA216P(environment.board_num)) {
        if(uv.HAS_microsynth(environment.board_num) && environment.internal_frequency != 0) {
            if(environment.internal_frequency <= 250000000) {
                uv.MICROSYNTH_freq(environment.board_num, environment.internal_frequency);
            }
            else {
                printf("Frequency outside range 50MHz-250MHz, using previous frequency\n");
            }
        }
        unsigned int selectedChannels = 0;

        if(environment.single_channel_mode == 1) {
            switch(environment.single_channel_select) {
            case 0:
                selectedChannels = IN0;
                break;
            case 1:
                selectedChannels = IN3; // due to part poulation on 2ch verion ch1 is on IN3
                break;
            case 2:
                selectedChannels = IN1;
                break;
            case 3:
                selectedChannels = IN2;
                break;
            default:
                selectedChannels = IN0;
                break;
            }
        }
        else if(environment.dual_channel_mode == 1 || defaultchannels == 2) {
            selectedChannels = (environment.channels);
        }
        else if(defaultchannels == 4) {
            selectedChannels = (IN0 | IN1 | IN2 | IN3);
        }
        else {
            printf("channel info not found, exiting\n");
            exit(1);
        }
        uv.selectAdcChannels(environment.board_num, selectedChannels);

        uv.selectTrigger(environment.board_num, environment.trigger_mode, environment.trigger_slope, environment.trigger_channel);
        uv.configureWaveformTrigger(environment.board_num, environment.trigger_threshold_16, environment.trigger_hysteresis);
        uv.configureSegmentedCapture(environment.board_num, environment.capture_count, environment.capture_depth, 0);
        if(environment.num_averages > 0) {
            uv.configureAverager(environment.board_num, environment.num_averages, environment.averager_length, 0);
        }
        uv.setFiducialMarks(environment.board_num, environment.fiducial);
        unsigned int trigVal = uv.isTriggerEnabled(environment.board_num);

    }
    if(uv.IS_adc12d2000(environment.board_num)) {
        if(uv.HAS_microsynth(environment.board_num) && environment.internal_frequency != 0) {
            if(environment.internal_frequency >= 300000000) {
                uv.MICROSYNTH_freq(environment.board_num, environment.internal_frequency);
            }
            else {
                printf("Frequency outside range 300MHz-2GHz, using previous frequency\n");
            }
        }
        unsigned int selectedChannels = 0;
        if(environment.single_channel_mode == 1) // One channel mode
        {
            if(environment.desiq == 1) {
                selectedChannels = 8;
            }
            else if(environment.desclkiq == 1) {
                selectedChannels = 4;
            }
            else if(environment.single_channel_select == 0) {
                selectedChannels = 1;
            }
            else if(environment.single_channel_select == 1) {
                selectedChannels = 2;
            }
            else {
                selectedChannels = 1;
            }
            uv.selectAdcChannels(environment.board_num, selectedChannels);
        }
        else {
            uv.selectAdcChannels(environment.board_num, IN0 | IN1); // Two channel mode
            printf("two channel mode\n");
        }

        // Set ECL Trigger Delay
        uv.SetECLTriggerDelay(environment.board_num, environment.ecl_trigger_delay);
        // Set Decimation
        uv.setAdcDecimation(environment.board_num, environment.decimation);
        // Set ECL Trigger
        uv.SetECLTriggerEnable(environment.board_num, environment.ecl_trigger);
        uv.selectTrigger(environment.board_num, environment.trigger_mode, environment.trigger_slope, environment.trigger_channel);
        uv.configureWaveformTrigger(environment.board_num, environment.trigger_threshold_12, environment.trigger_hysteresis);
        uv.configureSegmentedCapture(environment.board_num, environment.capture_count, environment.capture_depth, 0);
        if(environment.num_averages > 0) {
            if(environment.num_averages > 64) {
                environment.num_averages = 64;
                printf("!!CAUTION!!: Averages reduced to maximum for AD12 (64)\n");
            }
            uv.configureAverager(environment.board_num, environment.num_averages, environment.averager_length, 0);
        }
        uv.setFiducialMarks(environment.board_num, environment.fiducial);
    }

    if(uv.IS_AD5474(environment.board_num)) {
        printf("AD14 found\n");
        unsigned int selectedChannels = 0;

        if(environment.single_channel_mode == 1) // One channel mode
        {
            printf("Setting board to 1 channel mode. ");

            if(environment.single_channel_select == 0) {
                printf("Acquire IN0.\n");
                selectedChannels = IN0;
            }
            else if(environment.single_channel_select == 1) {
                printf("Acquire IN1.\n");
                selectedChannels = IN1;
            }
            else {
                printf("Invalid channel. Defaulting to acquire IN0.\n");
                selectedChannels = IN0;
            }

            uv.selectAdcChannels(environment.board_num, selectedChannels);
        }
        else {
            selectedChannels = 3; //1 || 2 = 3
            printf("Setting board to 2 channel mode\n");
            uv.selectAdcChannels(environment.board_num, selectedChannels);
        }

        // Configure Trigger
        uv.selectTrigger(environment.board_num, environment.trigger_mode, environment.trigger_slope, environment.trigger_channel);

        //Configure Waveform Trigger
        uv.configureWaveformTrigger(environment.board_num, environment.trigger_threshold_14, environment.trigger_hysteresis);

        // Configure Segmented Capture
        uv.configureSegmentedCapture(environment.board_num, environment.capture_count, environment.capture_depth, 0);

        // Configure Averager
        uv.configureAverager(environment.board_num, environment.num_averages, environment.averager_length, 0);

        // Set Decimation
        uv.setAdcDecimation(environment.board_num, environment.decimation);

        // Set Fiducial Marks
        uv.setFiducialMarks(environment.board_num, environment.fiducial);

    }

    if(!uv.IS_adc12d2000(environment.board_num) && !uv.IS_ISLA216P(environment.board_num) && !uv.IS_AD5474(environment.board_num)) {
        printf("AD8 found\n");


        unsigned int selectedChannels = 0;

        if(environment.single_channel_mode == 1) { // One channel mode
            if(environment.single_channel_select == 1) {
                selectedChannels = IN1;
            }
            else {
                selectedChannels = IN0;
            }
        }
        else {
            selectedChannels = 3;
        }
        //pUV.selectAdcChannels(environment.BoardNum, selectedChannels);
        uv.selectAdcChannels(environment.board_num, selectedChannels);
        // Set ECL Trigger Delay
        uv.SetECLTriggerDelay(environment.board_num, environment.ecl_trigger_delay);
        // Set Decimation
        uv.setAdcDecimation(environment.board_num, environment.decimation);
        // Set ECL Trigger
        uv.SetECLTriggerEnable(environment.board_num, environment.ecl_trigger);
        // Configure Segmented Capture
        uv.configureSegmentedCapture(environment.board_num, environment.capture_count, environment.capture_depth, 0);
    }

    //       pUV.SetAdcDecimation(environment.BoardNum, environment.Decimation);
    uv.SetTTLInvert(environment.board_num, environment.ttl_invert);
    uv.setPreTriggerMemory(environment.board_num, environment.pretrigger_memory);
}

void acquire_parser_printf() {
    printf("\n");
    printf("acquire will setup the requested board with desired settings, initiate\nacquisition and store data to disk file. \n\nUsage:\n\n");
    printf("acquire [OPTIONS]\n");
    printf("\t\t\tAcquires and saves blocks continuously.\n\n");

    printf("The following [OPTIONS] may follow the number of blocks option in any order:\n\n");

    printf("-f (filename)\t\tUse this argument to specify the name of the file to write. Must append .dat at the end of filename\n");
    printf("\n");
    printf("-gps\t\t\tSave gps data from garmin USB gps device\n");
    printf("-pid (pid)\t\tUse this argument to specify the process id to send the error signal to in case writing the data\n\t\t\tto file fails partway through.\n");
    printf("-ic\t\t\tBoard will use the internal clock. If not specified board uses external clock.\n");
    printf("-freq\t\t\tSpecifies the internal clock frequency for boards equipped with a microsynth programmable oscillator\n");
    printf("\t\t\t300MHz-2GHz for AD12, 50MHz-250MHz for AD16\n");
    printf("\n");
    printf("-scm\t\t\tRun multi-channel boards in single channel mode. Not required for single channel boards.\n");
    printf("-scs (Chan N)\t\tWhen running multi-channel boards in single channel mode use channel (Chan N).\n");
    printf("-desiq\t\t\tdesiq mode for AD12 board.\n");
    printf("-desclkiq\t\tdesclkiq mode for AD12 board.\n");
    printf("\n");
    printf("-dcm\t\t\tRun quad channel boards in dual channel mode. Not required for dual channel boards.\n");
    printf("-dcs <NM>\t\tWhen running in dual channel mode, specify the channels to acquire from. \n\t\t\tWhere channels NM can be 01, 02, 03, 12, 13, 23.\n");
    printf("\n");
    printf("-b (BoardNum)\t\tAcquires from the specified board. Do not specify the same board\n\t\t\tfrom two separate consoles. BoardNum starts at 0.\n");
    printf("\n");
    printf("-forceCal\t\tRuns full setup and re-reads calibration info from ultra_config.dat.\n");
    printf("-hdiode\t\t\tHeterodyne trigger.\n");
    printf("-syncselrecord\t\tSelective recording.\n");
    printf("-ttledge\t\tSets acquisition to await TTL trigger edge.\n");
    printf("-ttlinv\t\t\tInverts the selective recording feature. Default is active LOW.\n");
    printf("-ecltrig\t\tAcquire once ecl trigger has been received.\n");
    printf("-ecldelay (N)\t\tSet the ECL trigger delay. Default = 8000 (64us). See manual for details.\n");
    printf("-dec (factor)\t\tEnables input sample decimation. (factor) can be 1,2,4,8 or 16. (AD8/AD12/AD16)\n");
    printf("-capture_count (X)\tEnable segmented captures with X number of records of length Y.\n");
    printf("-capture_depth (Y)\tEnable segmented captures with X number of records of length Y. Y must be a multiple of 8 samples.\n");
    printf("-analog\t\t\tFor analog waveform trigger. Use -ttlinv to specify rising or falling edge.\n");
    printf("-analog_ch\t\tanalog waveform trigger channel");
    printf("-threshold <N>\t\tFor analog waveform trigger threshold. <N> = [0,65535].\n");
    printf("-hysteresis <N>\t\tFor analog waveform trigger hysteresis. The hysteresis value = threshold + <N>. <N> = [0,65535].\n");
    printf("-fiducial\t\tPlaces Fiducial marks in data when trig/sel record starts recording\n");
    printf("-pretrigger\t\tFor pretrigger memory after trigger is acquired. pretrigger must be 0 to 512.\n");
    printf("-avg <N> \t\t(N=0-65535)\tAverages N+1 cycles,  N=0 is flow through mode\n");
    printf("-avg_len <N>\t\tFor 50T/110T:\t1-channel mode: N=16384,8192,4096,2048,1024,512,256,128,64,32,16\n");
    printf("\t\t\t\t\t2-channel mode: N=8192,4096,2048,1024,512,256,128,64,32,16,8\n");
    printf("\t\t\t\t\t4-channel mode: N=4096,2048,1024,512,256,128,64,32,16,8,4\n");
    printf("\t\t\tFor 155T:\t1-channel mode:\tN=131072,65536,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16\n");
    printf("\t\t\t\t\t2-channel mode: N=65536,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8\n");
    printf("\t\t\t\t\t4-channel mode: N=32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8,4\n");
    printf("\t\t\tavg_len default is 4096.\n");
}
