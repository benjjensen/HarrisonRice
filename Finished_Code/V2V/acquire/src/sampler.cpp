#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <dirent.h>
#include <ext/stdio_filebuf.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "DataCapture.h"
#include "NamingConvention.h"

/**
 * Constants for the columns of the capture table.
 */
const unsigned int COLS_COUNT = 8;
const unsigned int COL_NAME = 0;
const unsigned int COL_DATE = 1;
const unsigned int COL_TIME = 2;
const unsigned int COL_DURATION = 3;
const unsigned int COL_SIZE = 4;
const unsigned int COL_NOTES = 5;
const unsigned int COL_EDIT_NOTES = 6;
const unsigned int COL_DELETE_CAPTURE = 7;

/**
 * The width of the border on all the windows.
 */
const int WINDOW_BORDER_WIDTH = 10;

/**
 * The default height and width of the main window.
 */
const int MAIN_WINDOW_DEFAULT_HEIGHT = 400;
const int MAIN_WINDOW_DEFAULT_WIDTH = 1000;

/**
 * The padding between fields.
 */
const int FIELDS_PADDING = 10;

/**
 * The minimum width of a minor button.
 */
const int MINOR_BUTTON_MIN_WIDTH = 100;

/**
 * Default height of a widget.
 */
const int DEFAULT_HEIGHT = -1;

/**
 * Spacing for the columns and rows in the capture table.
 */
const int COL_SPACING = 20;
const int ROW_SPACING = 10;

/**
 * Padding values.
 */
const int READY_BUTTON_PADDING = 5;
const int NO_PADDING = 0;

struct GuiElements {

    GuiElements() {
        main_window = NULL;
        capture_table_scroll_window = NULL;
        capture_table = NULL;
        capture_table_current_rows = 0;
        current_capture_box = NULL;
        current_capture_name = NULL;
        current_capture_status = NULL;
        label_capture_error = NULL;
        label_total_data_captured = NULL;
        start_button = NULL;
        stop_button = NULL;
        new_button = NULL;
        reset_button = NULL;
        new_capture_window = NULL;
        fields_parent_box = NULL;
        add_option_window = NULL;
        add_option_field_label = NULL;
        add_option_entry_code = NULL;
        add_option_entry_name = NULL;
        add_option_label_error = NULL;
        edit_notes_window = NULL;
        edit_notes_text_view = NULL;
        edit_notes_label_error = NULL;
        edit_notes_label_name = NULL;
    }

    /**
     * The main window.
     */
    GtkWidget *main_window;

    /**
     * The scrolled window that the table is inside.
     */
    GtkWidget *capture_table_scroll_window;
    /**
     * The table where the captures are displayed.
     */
    GtkWidget *capture_table;
    /**
     * The current number of rows in the capture table.
     */
    size_t capture_table_current_rows;
    /**
     * The list of labels for the notes of each entry in the capture table.
     */
    std::vector<GtkWidget*> notes_labels;

    /**
     * The box that contains the current capture info.
     */
    GtkWidget *current_capture_box;
    /**
     * The label that shows the name of the current capture.
     */
    GtkWidget *current_capture_name;
    /**
     * The label that shows the status of the current capture.
     */
    GtkWidget *current_capture_status;

    /**
     * The label that shows an error message to the user when there's an issue
     * writing the captured data to disk.
     */
    GtkWidget *label_capture_error;

    /**
     * The label that shows the total amount of data captured.
     */
    GtkWidget *label_total_data_captured;

    /**
     * The button in the main window that starts the data capture.
     */
    GtkWidget *start_button;
    /**
     * The button in the main window that stops the current data capture.
     */
    GtkWidget *stop_button;
    /**
     * The button in the main window that opens the new capture window.
     */
    GtkWidget *new_button;
    /**
     * The button in the main window that clears the current capture.
     */
    GtkWidget *reset_button;



    /**
     * The window that allows the user to set up a new capture.
     */
    GtkWidget *new_capture_window;

    /**
     * The box in the new capture window that contains the fields and options
     * for the naming convention.
     */
    GtkWidget *fields_parent_box;



    /**
     * The window that allows the user to add a new option to a naming convention
     * field.
     */
    GtkWidget *add_option_window;

    /**
     * The label in the add option window that tells the user the name of the
     * field they're adding to.
     */
    GtkWidget *add_option_field_label;
    /**
     * The entry in the add option window where the user enters the code for the
     * entry.
     */
    GtkWidget *add_option_entry_code;
    /**
     * The entry in the add option window where the user enters the name for the
     * entry.
     */
    GtkWidget *add_option_entry_name;
    /**
     * The label in the add option window where we display any problems with the
     * user's input in the add_option_entry_code and add_option_entry_name fields.
     */
    GtkWidget *add_option_label_error;

    /**
     * The window where the user edits the notes for a certain capture.
     */
    GtkWidget* edit_notes_window;
    /**
     * The text view in the edit notes window where the user enters and modifies
     * notes for a certain capture.
     *
     * This needs to be a text view and not an entry because text view widgets can
     * accept multiline entries, where as entry widgets accept only a single line.
     */
    GtkWidget* edit_notes_text_view;
    /**
     * The label in the edit notes window where we display problems with the input
     * in edit_notes_text_view.
     */
    GtkWidget* edit_notes_label_error;
    /**
     * Label that holds the name of the capture for which the notes are being
     * edited.
     */
    GtkWidget* edit_notes_label_name;
};

struct SamplerState {

    SamplerState() {
        total_data_captured = 0;
        all_initialized = false;
        field_being_extended = "";
        acquire_process_id = -1;
        acquire_output_fd = -1;
        notes_being_edited = -1;
    }

    /**
     * The naming convention used to store the values the user has selected for
     * each field.
     */
    NamingConvention naming_convention;

    /**
     * The total amount of data captured.
     */
    int total_data_captured;

    /**
     * The current data capture. This stores the name as determined in the
     * new capture window and then retrieves the information that acquire.cpp
     * saves to the metadata file.
     */
    DataCapture current_capture;

    /**
     * The list of previously completed data captures.
     */
    std::vector<DataCapture> finished_captures;

    /**
     * This indicates whether the GUI has finished initializing.
     */
    bool all_initialized;

    /**
     * The name of the field that is currently being extended.
     */
    std::string field_being_extended;

    /**
     * The process id for the child process where the acquire program is running.
     */
    int acquire_process_id;
    /**
     * The file descriptor for the output end of the pipe that the acquire program
     * is feeding with its output.
     */
    int acquire_output_fd;

    /**
     * The index of the capture in finished_captures for which the user is currently
     * editing the notes.
     */
    int notes_being_edited;

    GuiElements gui;

};

/**
 * A callback function for when the user closes the main window.
 */
static void cb_destroy(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the start button.
 */
static void cb_start_capture(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the stop button.
 */
static void cb_stop_capture(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the new capture button.
 */
static void cb_new_capture(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the reset button.
 */
static void cb_reset_capture(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the create button in the
 * new capture window.
 */
static void cb_create_new_capture(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the cancel button in the
 * new capture window.
 */
static void cb_cancel_new_capture(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks a button to add a new
 * option to a naming convention field.
 */
static void cb_add_new_option(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the cancel button in
 * the add option window.
 */
static void cb_add_option_cancel(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the add button in the
 * add option window.
 */
static void cb_add_option_add(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks on an option in the
 * new capture window.
 */
static void cb_field_selection_changed(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the button to edit the
 * notes for a capture in the capture table.
 */
static void cb_edit_notes(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the save button in the
 * edit notes window.
 */
static void cb_edit_notes_save(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the cancel button in
 * the edit notes window.
 */
static void cb_edit_notes_cancel(GtkWidget *widget, gpointer data);
/**
 * A callback function for when the user clicks the delete button for a
 * row in the capture table.
 */
static void cb_delete_capture(GtkWidget *widget, gpointer data);

/**
 * Validates the code and the name for a new option that the user has
 * entered. Also trims whitespace off.
 */
static bool validate_new_option_and_trim(SamplerState& state, std::string &new_option_code,
        std::string &new_option_name);

/**
 * The function that initializes the main window.
 */
static void init_main_window(SamplerState& state);
/**
 * The function that initializes the new capture window.
 */
static void init_new_capture_window(SamplerState& state);
/**
 * The function that initializes the add option window.
 */
static void init_add_option_window(SamplerState& state);
/**
 * The function that initializes the edit notes window.
 */
static void init_edit_notes_window(SamplerState& state);

/**
 * Populates the fields and options for the naming convention in the new capture
 * window.
 *
 * This clears what is already in fields_parent_box and refills it with new
 * radio button options.
 */
static void populate_fields_and_options(SamplerState& state);
/**
 * Inserts a capture into the table at a specific row.
 */
static void insert_capture_into_table_row(SamplerState& state, DataCapture capture, const int row);
/**
 * Inserts a capture into the table at the end of the table.
 *
 * This function resizes the capture table and then calls
 * insert_capture_into_table_row to insert the capture into the
 * last row.
 */
static void insert_capture_into_table(SamplerState& state, DataCapture capture);
/**
 * Hides the given row from the capture table.
 */
static void hide_capture_from_table(GuiElements& gui, const int capture_index);

/**
 * This finds all the "*-meta.txt" files in DATA_FOLDER and reads them in
 * as DataCapture objects and puts those objects into finished_captures.
 *
 * Note: this does not put the captures into the captures table.
 */
static void load_all_captures_from_files(SamplerState& state);

/**
 * Starts the acquire program in a child process, with the child's
 * output piped into this program via the acquire_output_fd file
 * descriptor.
 *
 * The process id of the new child process is stored in
 * acquire_process_id.
 *
 * The arguments given to acquire specify that the name of the output
 * file is current_capture.name.
 */
static bool start_acquire_in_child_process(SamplerState& state);
/**
 * Stops the acquire program, which is running in a child process
 * with process id acquire_process_id.
 */
static void stop_acquiring(SamplerState& state);
/**
 * Processes the output of the acquire program. Should only be called after the
 * acquire program has stopped.
 */
static void process_acquire_output(SamplerState& state);
/**
 * Handles the SIGUSR1 signal from the acquire program.
 *
 * The acquire program should send this signal to this program when it
 * encounters an error while writing the captured data to a file.
 */
static void handler_signal_from_child(int signal);
/**
 * Handles the SIGUSR1 signal from the acquire program.
 *
 * The acquire program should send this signal to this program when it
 * encounters an error while writing the captured data to a file.
 */
static void handler_signal_from_child(int signal, SamplerState* state_pointer);

int main(int argc, char **argv) {
    SamplerState state;

    system("sudo ./reserve_acquire_cpus.sh >/dev/null 2>/dev/null");

    handler_signal_from_child(SIGUSR1, &state);
    signal(SIGUSR1, &handler_signal_from_child);

    gtk_init(&argc, &argv);

    load_all_captures_from_files(state);

    // Initialize all of the windows we use, before any of them are visible.
    init_main_window(state);
    init_new_capture_window(state);
    init_add_option_window(state);
    init_edit_notes_window(state);

    state.all_initialized = true;

    gtk_widget_show(state.gui.main_window);
    // Run the GUI until it's done.
    gtk_main();

    std::cout << "After gtk_main() function" << std::endl;
    system("sudo ./revert_cpus.sh >/dev/null 2>/dev/null");

    return 0;
}

static bool start_acquire_in_child_process(SamplerState& state) {
    const int READ_FD = 0;
    const int WRITE_FD = 1;

    const int SUCCESS = 0;
    const int FAILURE = -1;

    const int CHILD_PID = 0;

    // A pipe to get info from acquire and read it in sampler (via acquire's
    // standard output):
    int output_pipe[2];
    if(pipe(output_pipe) != SUCCESS) {
        std::cerr << "ERROR: UNABLE TO OPEN PIPE FOR ACQUIRE'S OUTPUT" <<
                std::endl;
        return false;
    }

    std::stringstream parent_pid_stringstream;
    parent_pid_stringstream << getpid();

    pid_t pid;
    switch(pid = fork()) {
        // -1 means that the fork failed.
    case FAILURE:
        std::cerr << "ERROR: FAILURE TO FORK FOR ACQUIRE" << std::endl;
        return FALSE;
        // If we get 0, we're the child process.
    case CHILD_PID:
        // Set up the process interaction by replacing standard out with the
        // pipe to the parent process:
        if(dup2(output_pipe[WRITE_FD], STDOUT_FILENO) == FAILURE) {
            std::cerr << "ERROR: UNABLE TO PIPE ACQUIRE'S STDOUT TO SAMPLER" <<
                    std::endl;
            std::exit(-1);
        }
        // Close the side of the pipe we (the child process) won't use:
        if(close(output_pipe[READ_FD]) != SUCCESS) {
            std::cerr << "ERROR: UNABLE TO CLOSE READ SIDE OF PIPE FROM ACQUIRE"
                    << std::endl;
        }


        // Not necessary to change directories because acquire is in the same
        // directory as this.
        execl("/usr/bin/sudo", "sudo", "/bin/nice", "-n", "-20", "./acquire", "-f",
                state.current_capture.name.c_str(), "-pid",
                parent_pid_stringstream.str().c_str(), "-gps", NULL);

        std::cerr << "ERROR: EXEC CALL FAILURE IN ACQUIRE CHILD PROCESS" <<
                std::endl;
        std::exit(-1);
    default:
        // Otherwise, we're the parent process, so break out of this switch
        // statement and continue on:
        break;
    }

    // Close the side of the pipe that we won't use in the parent process:
    if(close(output_pipe[WRITE_FD]) != SUCCESS) {
        std::cerr << "ERROR: UNABLE TO CLOSE WRITE SIDE OF PIPE IN SAMPLER" <<
                std::endl;
    }

    // Save this info for future use:
    state.acquire_process_id = pid;
    state.acquire_output_fd = output_pipe[READ_FD];

    return true;
}

static void stop_acquiring(SamplerState& state) {
    // Sent the SIGINT signal to acquire--this is the same as if the user had
    // pressed ctrl-c at the terminal where acquire is running.
    kill(state.acquire_process_id, SIGINT);

    // Wait for acquire to be done. (This is necessary--otherwise it ends up as
    // a zombie process.)
    // TODO better wait function
    wait(NULL);

    process_acquire_output(state);

    state.acquire_output_fd = -1;
    state.acquire_process_id = -1;
}

static void handler_signal_from_child(int signal) {
    handler_signal_from_child(signal, NULL);
}

static void handler_signal_from_child(int signal, SamplerState* state_pointer) {
    static SamplerState& state = *state_pointer;
    if(state_pointer != NULL) {
        return;
    }

    if(signal != SIGUSR1) {
        std::cerr << "ERROR: WRONG SIGNAL " << signal << " PASSED TO SIGUSR1 "
                << SIGUSR1 << " HANDLER" << std::endl;
        return;
    }

    // We wait for the acquire process to finish. We know that it will finish on
    // its own because it sent us the SIGUSR1 signal, which means that it
    // encountered an error and is exiting.
    // TODO better version of wait
    wait(NULL);

    gtk_widget_hide(state.gui.current_capture_box);
    process_acquire_output(state);

    state.acquire_output_fd = -1;
    state.acquire_process_id = -1;

    state.current_capture.name = "";

    // The only active button when there is no current capture should be the new
    // capture button:
    gtk_widget_set_sensitive(state.gui.start_button, FALSE);
    gtk_widget_set_sensitive(state.gui.stop_button, FALSE);
    gtk_widget_set_sensitive(state.gui.new_button, TRUE);
    gtk_widget_set_sensitive(state.gui.reset_button, FALSE);

    gtk_widget_show(state.gui.label_capture_error);
}

static void process_acquire_output(SamplerState& state) {
    // Convert the file descriptor acquire_output_fd that reads what acquire
    // wrote to its standard output into a stream we can easily read from:
    __gnu_cxx::stdio_filebuf<char> *sb =
            new __gnu_cxx::stdio_filebuf<char>(state.acquire_output_fd, std::ios::in);
    std::istream input_from_acquire(sb);

    // For every line in acquire's output:
    std::string line_str = "";
    while(std::getline(input_from_acquire, line_str)) {
        std::stringstream line(line_str);
        std::string first_word = "";
        line >> first_word;

        // Check to see if it's the line that tells us the metadata filename:
        if(first_word == CAPTURE_META_FILENAME_HANDOFF_TAG) {
            // If it is, read the rest in as the filename:
            line >> state.current_capture.meta_filename;
            // Save the notes we already have, because otherwise they'll be
            // overwritten as empty when we read the capture info in from the
            // file.
            std::string notes = state.current_capture.notes;
            state.current_capture.read_from_file();
            state.current_capture.notes = notes;

            // Add the new capture to the table and to the list of captures:
            state.finished_captures.push_back(state.current_capture);
            insert_capture_into_table(state, state.current_capture);
        }
    }

    if(close(state.acquire_output_fd) != 0) {
        std::cerr << "ERROR: failed to close acquire_output_fd" << std::endl;
    }
    delete sb;
}

static void init_main_window(SamplerState& state) {
    GuiElements& gui = state.gui;
    gui.main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(gui.main_window), "Sampler");
    gtk_container_set_border_width(GTK_CONTAINER(gui.main_window),
            WINDOW_BORDER_WIDTH);
    gtk_window_set_position(GTK_WINDOW(gui.main_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_default_size(GTK_WINDOW(gui.main_window),
            MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT);
    // Set the callback for when the user closes this window:
    std::cout << "About to connect signal" << std::endl;
    g_signal_connect(gui.main_window, "destroy", G_CALLBACK(cb_destroy), (gpointer)(&state));
    std::cout << "Connected signal" << std::endl;

    GtkWidget *layout_box = gtk_vbox_new(FALSE, NO_PADDING);
    gtk_container_add(GTK_CONTAINER(gui.main_window), layout_box);

    gui.capture_table_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    // This allows the scroll window to scroll vertically and horizontally:
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gui.capture_table_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(layout_box), gui.capture_table_scroll_window, TRUE, TRUE, NO_PADDING);

    // 0, 0, 0, 0 means that what's inside of this alignment widget will not expand to fill its parent,
    // which is what we want with the capture table--it should have its own height and width independent
    // from the height and width of the scrolled window.
    GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(gui.capture_table_scroll_window), align);
    gtk_widget_show(align);

    // Removes the "shade" lines from around the scrolled window:
    gtk_viewport_set_shadow_type(GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(gui.capture_table_scroll_window))), GTK_SHADOW_NONE);

    gui.capture_table_current_rows = 1;
    gui.capture_table = gtk_table_new(gui.capture_table_current_rows, COLS_COUNT, FALSE);
    gtk_container_add(GTK_CONTAINER(align), gui.capture_table);
    gtk_table_set_col_spacings(GTK_TABLE(gui.capture_table), COL_SPACING);
    gtk_table_set_row_spacings(GTK_TABLE(gui.capture_table), ROW_SPACING);

    // The row which we're inserting the labels below:
    int row = 0;

    // Inserting each label into the table:
    GtkWidget *label = gtk_label_new("Name");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Name</b>");
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(gui.capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);
    label = gtk_label_new("Date");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Date</b>");
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(gui.capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);
    label = gtk_label_new("Time");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Time</b>");
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(gui.capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);
    label = gtk_label_new("Duration");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Duration</b>");
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(gui.capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);
    label = gtk_label_new("Size");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Size</b>");
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(gui.capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);
    label = gtk_label_new("Notes");
    gtk_label_set_markup(GTK_LABEL(label), "<b>Notes</b>");
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(gui.capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);

    gtk_widget_show(gui.capture_table);
    gtk_widget_show(gui.capture_table_scroll_window);

    GtkWidget* footer_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_start(GTK_BOX(layout_box), footer_box, FALSE, FALSE, NO_PADDING);
    gtk_widget_show(footer_box);

    // This box contains the labels that display information about the current captures:
    gui.current_capture_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_start(GTK_BOX(footer_box), gui.current_capture_box, FALSE, FALSE, NO_PADDING);
    // Intentionally don't show current_capture_box yet

    gui.current_capture_name = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(gui.current_capture_box), gui.current_capture_name, FALSE, FALSE, NO_PADDING);
    gtk_widget_show(gui.current_capture_name);

    gui.current_capture_status = gtk_label_new("ready...");
    gtk_box_pack_start(GTK_BOX(gui.current_capture_box), gui.current_capture_status, FALSE, FALSE, READY_BUTTON_PADDING);
    gtk_widget_show(gui.current_capture_status);

    // This label tells the user when there's an error saving the captured data to the disk.
    gui.label_capture_error = gtk_label_new("ERROR: Problem saving data. Is the disk full?");
    GdkColor color;
    gdk_color_parse("red", &color);
    // Set the error label's text color to red:
    gtk_widget_modify_fg(gui.label_capture_error, GTK_STATE_NORMAL, &color);
    gtk_box_pack_start(GTK_BOX(footer_box), gui.label_capture_error, FALSE, FALSE, NO_PADDING);
    // Intentionally don't show this yet.

    gui.label_total_data_captured = gtk_label_new("0 GB captured in total");
    gtk_box_pack_end(GTK_BOX(footer_box), gui.label_total_data_captured, FALSE, FALSE, NO_PADDING);
    gtk_widget_show(gui.label_total_data_captured);

    GtkWidget *sub_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, NO_PADDING);

    // The new capture button. The callback opens the new capture screen.
    gui.new_button = gtk_button_new_with_label("New Capture...");
    gtk_box_pack_start(GTK_BOX(sub_box), gui.new_button, TRUE, TRUE, NO_PADDING);
    g_signal_connect(gui.new_button, "clicked", G_CALLBACK(cb_new_capture), (gpointer)(&state));
    gtk_widget_show(gui.new_button);

    // The reset button. The callback clears the current capture.
    gui.reset_button = gtk_button_new_with_label("Reset");
    gtk_widget_set_size_request(gui.reset_button, MINOR_BUTTON_MIN_WIDTH, DEFAULT_HEIGHT);
    gtk_box_pack_start(GTK_BOX(sub_box), gui.reset_button, FALSE, FALSE, NO_PADDING);
    // Not active until there's a current capture:
    gtk_widget_set_sensitive(gui.reset_button, FALSE);
    g_signal_connect(gui.reset_button, "clicked", G_CALLBACK(cb_reset_capture), (gpointer)(&state));
    gtk_widget_show(gui.reset_button);

    gtk_widget_show(sub_box);

    sub_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, NO_PADDING);

    // The start button starts the current capture when it's clicked:
    gui.start_button = gtk_button_new_with_label("Start");
    gtk_box_pack_start(GTK_BOX(sub_box), gui.start_button, TRUE, TRUE, NO_PADDING);
    gtk_widget_set_sensitive(gui.start_button, FALSE);
    g_signal_connect(gui.start_button, "clicked", G_CALLBACK(cb_start_capture), (gpointer)(&state));
    gtk_widget_show(gui.start_button);

    // The stop button stops the current capture when it's clicked:
    gui.stop_button = gtk_button_new_with_label("Stop");
    gtk_box_pack_start(GTK_BOX(sub_box), gui.stop_button, TRUE, TRUE, NO_PADDING);
    gtk_widget_set_sensitive(gui.stop_button, FALSE);
    g_signal_connect(gui.stop_button, "clicked", G_CALLBACK(cb_stop_capture), (gpointer)(&state));
    gtk_widget_show(gui.stop_button);

    gtk_widget_show(sub_box);

    gtk_widget_show(layout_box);

    // Populate the list with the captures that were loaded from the files in the data folder.
    // finished_captures should already have been initialized by now:
    for(std::vector<DataCapture>::iterator it = state.finished_captures.begin(); it != state.finished_captures.end(); ++it) {
        insert_capture_into_table(state, *it);
    }
}

static void init_new_capture_window(SamplerState& state) {
    GuiElements& gui = state.gui;
    gui.new_capture_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // This makes the new capture window always appear above the main window:
    gtk_window_set_transient_for(GTK_WINDOW(gui.new_capture_window), GTK_WINDOW(gui.main_window));
    gtk_window_set_title(GTK_WINDOW(gui.new_capture_window), "New Capture");
    gtk_container_set_border_width(GTK_CONTAINER(gui.new_capture_window), WINDOW_BORDER_WIDTH);
    gtk_window_set_position(GTK_WINDOW(gui.new_capture_window), GTK_WIN_POS_CENTER_ALWAYS);
    // The user can't close this window by pressing the x button at the top right:
    gtk_window_set_deletable(GTK_WINDOW(gui.new_capture_window), FALSE);

    GtkWidget *layout_box = gtk_vbox_new(FALSE, NO_PADDING);
    gtk_container_add(GTK_CONTAINER(gui.new_capture_window), layout_box);

    // This box contains the fields--each field has a box with a radio list of options.
    gui.fields_parent_box = gtk_hbox_new(FALSE, FIELDS_PADDING);
    gtk_box_pack_start(GTK_BOX(layout_box), gui.fields_parent_box, FALSE, FALSE, NO_PADDING);

    // Call the separate function that populates the fields into fields_parent_box:
    populate_fields_and_options(state);

    GtkWidget *sub_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, NO_PADDING);

    // The new capture button sends the capture info to the main window so that it can
    // begin capturing.
    GtkWidget* create_new_capture_button = gtk_button_new_with_label("Prepare");
    gtk_box_pack_start(GTK_BOX(sub_box), create_new_capture_button, TRUE, TRUE, NO_PADDING);
    g_signal_connect(create_new_capture_button, "clicked", G_CALLBACK(cb_create_new_capture), (gpointer)(&state));
    gtk_widget_show(create_new_capture_button);

    // The cancel new capture button cancels making a new capture and returns to the main
    // window.
    GtkWidget* cancel_new_capture_button = gtk_button_new_with_label("Cancel");
    gtk_widget_set_size_request(cancel_new_capture_button, MINOR_BUTTON_MIN_WIDTH, DEFAULT_HEIGHT);
    gtk_box_pack_start(GTK_BOX(sub_box), cancel_new_capture_button, FALSE, FALSE, NO_PADDING);
    g_signal_connect(cancel_new_capture_button, "clicked", G_CALLBACK(cb_cancel_new_capture), (gpointer)(&state));
    gtk_widget_show(cancel_new_capture_button);

    gtk_widget_show(sub_box);

    gtk_widget_show(layout_box);
}

static void init_add_option_window(SamplerState& state) {
    GuiElements& gui = state.gui;
    gui.add_option_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // This window will always appear above the new capture window:
    gtk_window_set_transient_for(GTK_WINDOW(gui.add_option_window), GTK_WINDOW(gui.new_capture_window));
    gtk_window_set_title(GTK_WINDOW(gui.add_option_window), "Add Option");
    gtk_container_set_border_width(GTK_CONTAINER(gui.add_option_window), WINDOW_BORDER_WIDTH);
    gtk_window_set_position(GTK_WINDOW(gui.add_option_window), GTK_WIN_POS_CENTER_ALWAYS);
    // The user can't close this window except through our GUI:
    gtk_window_set_deletable(GTK_WINDOW(gui.add_option_window), FALSE);

    GtkWidget *layout_box = gtk_vbox_new(FALSE, NO_PADDING);
    gtk_container_add(GTK_CONTAINER(gui.add_option_window), layout_box);

    GtkWidget *frame = gtk_frame_new("Field:");
    gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, NO_PADDING);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

    // The label that displays the name of the field that is being extended:
    gui.add_option_field_label = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(frame), gui.add_option_field_label);
    // Sets the alignment to left-align:
    gtk_misc_set_alignment(GTK_MISC(gui.add_option_field_label), 0, 0);
    gtk_widget_show(gui.add_option_field_label);

    gtk_widget_show(frame);

    frame = gtk_frame_new("Code:");
    gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, NO_PADDING);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

    // The entry where the user enters the code for the new option:
    gui.add_option_entry_code = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(frame), gui.add_option_entry_code);
    gtk_widget_show(gui.add_option_entry_code);

    gtk_widget_show(frame);

    frame = gtk_frame_new("Name:");
    gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, NO_PADDING);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

    // The entry where the user enters the name of the new option:
    gui.add_option_entry_name = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(frame), gui.add_option_entry_name);
    gtk_widget_show(gui.add_option_entry_name);

    gtk_widget_show(frame);

    // The label where error text is displayed:
    gui.add_option_label_error = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(layout_box), gui.add_option_label_error, FALSE, FALSE, NO_PADDING);

    GdkColor color;
    gdk_color_parse("red", &color);
    // Changes the color of the error text to red:
    gtk_widget_modify_fg(gui.add_option_label_error, GTK_STATE_NORMAL, &color);
    gtk_widget_show(gui.add_option_label_error);

    GtkWidget* sub_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, NO_PADDING);

    // The button the user clicks to confirm their new option.
    // When it's clicked, the user's input is validated before it's
    // accepted; if it's not acceptable, an error message is displayed
    // in add_option_label_error.
    GtkWidget* add_option_add_button = gtk_button_new_with_label("Add");
    gtk_box_pack_start(GTK_BOX(sub_box), add_option_add_button, TRUE, TRUE, NO_PADDING);
    g_signal_connect(add_option_add_button, "clicked", G_CALLBACK(cb_add_option_add), (gpointer)(&state));
    gtk_widget_show(add_option_add_button);

    // The button that cancels the new option:
    GtkWidget* add_option_cancel_button = gtk_button_new_with_label("Cancel");
    gtk_box_pack_start(GTK_BOX(sub_box), add_option_cancel_button, FALSE, FALSE, NO_PADDING);
    gtk_widget_set_size_request(add_option_cancel_button, MINOR_BUTTON_MIN_WIDTH, DEFAULT_HEIGHT);
    g_signal_connect(add_option_cancel_button, "clicked", G_CALLBACK(cb_add_option_cancel), (gpointer)(&state));
    gtk_widget_show(add_option_cancel_button);

    gtk_widget_show(sub_box);

    gtk_widget_show(layout_box);
}

static void init_edit_notes_window(SamplerState& state) {
    GuiElements& gui = state.gui;
    gui.edit_notes_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // The edit notes window always appears on top of the main window:
    gtk_window_set_transient_for(GTK_WINDOW(gui.edit_notes_window), GTK_WINDOW(gui.main_window));
    gtk_window_set_title(GTK_WINDOW(gui.edit_notes_window), "Edit notes");
    gtk_container_set_border_width(GTK_CONTAINER(gui.edit_notes_window), WINDOW_BORDER_WIDTH);
    gtk_window_set_position(GTK_WINDOW(gui.edit_notes_window), GTK_WIN_POS_CENTER_ALWAYS);
    // The user can't close this window except through our GUI:
    gtk_window_set_deletable(GTK_WINDOW(gui.edit_notes_window), FALSE);

    GtkWidget *layout_box = gtk_vbox_new(FALSE, NO_PADDING);
    gtk_container_add(GTK_CONTAINER(gui.edit_notes_window), layout_box);

    GtkWidget *frame = gtk_frame_new("Name:");
    gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, NO_PADDING);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

    // The label where the name of the capture is displayed:
    gui.edit_notes_label_name = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(frame), gui.edit_notes_label_name);
    // Set the alignment to top-left:
    gtk_misc_set_alignment(GTK_MISC(gui.edit_notes_label_name), 0, 0);
    gtk_widget_show(gui.edit_notes_label_name);

    gtk_widget_show(frame);

    frame = gtk_frame_new("Notes:");
    gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, NO_PADDING);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

    // The text view where the user inputs their notes:
    gui.edit_notes_text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(frame), gui.edit_notes_text_view);
    gtk_widget_show(gui.edit_notes_text_view);

    gtk_widget_show(frame);

    // The label where errors are displayed:
    gui.edit_notes_label_error = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(layout_box), gui.edit_notes_label_error, FALSE, FALSE, NO_PADDING);

    GdkColor color;
    gdk_color_parse("red", &color);
    // Set the error label's text color to red:
    gtk_widget_modify_fg(gui.edit_notes_label_error, GTK_STATE_NORMAL, &color);
    gtk_widget_show(gui.edit_notes_label_error);

    GtkWidget* sub_box = gtk_hbox_new(FALSE, NO_PADDING);
    gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, NO_PADDING);

    // This button saves the notes:
    GtkWidget* save_button = gtk_button_new_with_label("Save");
    gtk_box_pack_start(GTK_BOX(sub_box), save_button, TRUE, TRUE, NO_PADDING);
    g_signal_connect(save_button, "clicked", G_CALLBACK(cb_edit_notes_save), (gpointer)(&state));
    gtk_widget_show(save_button);

    // This button cancels editing the notes:
    GtkWidget* cancel_button = gtk_button_new_with_label("Cancel");
    gtk_box_pack_start(GTK_BOX(sub_box), cancel_button, FALSE, FALSE, NO_PADDING);
    gtk_widget_set_size_request(cancel_button, MINOR_BUTTON_MIN_WIDTH, DEFAULT_HEIGHT);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(cb_edit_notes_cancel), (gpointer)(&state));
    gtk_widget_show(cancel_button);

    gtk_widget_show(sub_box);

    gtk_widget_show(layout_box);
}

static void populate_fields_and_options(SamplerState& state) {
    if(state.gui.fields_parent_box == NULL) {
        std::cerr << "ERROR: populate_fields_and_options() called before fields_parent_box was set." << std::endl;
    }

    {
        // Remove all of the old fields from fields_parent_box:
        GList *children, *iter;

        children = gtk_container_get_children(GTK_CONTAINER(state.gui.fields_parent_box));
        for(iter = children; iter != NULL; iter = g_list_next(iter))
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        g_list_free(children);
        children = NULL;
        iter = NULL;
    }

    // For each field in the naming convention:
    std::list<std::string> field_order = state.naming_convention.get_field_order();
    for(std::list<std::string>::iterator field_it = field_order.begin();
            field_it != field_order.end(); ++field_it) {
        GtkWidget* field_box = gtk_vbox_new(FALSE, NO_PADDING);
        gtk_box_pack_start(GTK_BOX(state.gui.fields_parent_box), field_box, TRUE, TRUE, NO_PADDING);

        std::string field_code = *field_it;
        // Store the code for the field in the field_box object so that it can be
        // retrieved later by cb_field_selected_changed:
        gtk_widget_set_name(field_box, field_code.c_str());

        // This label shows the name of the field:
        GtkWidget* field_label = gtk_label_new(state.naming_convention.get_field_name(field_code).c_str());
        gtk_box_pack_start(GTK_BOX(field_box), field_label, FALSE, FALSE, NO_PADDING);
        gtk_widget_show(field_label);

        std::string selected_option = state.naming_convention.get_field_value(field_code);

        GtkWidget* option_radio_button = NULL;
        std::map<std::string, std::string> options = state.naming_convention.get_options(field_code);
        for(std::map<std::string, std::string>::iterator option_it = options.begin();
                option_it != options.end(); ++option_it) {
            std::string option_code = option_it->first;
            std::string option_name = option_it->second;
            if(option_radio_button == NULL) {
                // If this is the first radio button in this field, create the new radio button in
                // a new radio group (with the option name as its label):
                option_radio_button = gtk_radio_button_new_with_label(NULL, option_name.c_str());
            }
            else {
                // Otherwise, if this is not the first radio button, create a the new radio button in
                // the same radio group as the previous one (with the option name as its label):
                option_radio_button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(option_radio_button), option_name.c_str());
            }

            if(option_code == selected_option) {
                // If this is the selected option, set this as the active radio button:
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option_radio_button), TRUE);
            }

            gtk_box_pack_start(GTK_BOX(field_box), option_radio_button, FALSE, FALSE, NO_PADDING);

            // Store the code for this option in the radio button widget so that it can be retreived by
            // cb_field_selection_changed
            gtk_widget_set_name(option_radio_button, option_code.c_str());
            g_signal_connect(option_radio_button, "toggled", G_CALLBACK(cb_field_selection_changed), (gpointer)(&state));

            gtk_widget_show(option_radio_button);
        }

        // This button opens the add option window:
        GtkWidget* add_option_button = gtk_button_new_with_label("Add...");
        gtk_widget_set_name(add_option_button, field_code.c_str());
        gtk_widget_set_size_request(add_option_button, MINOR_BUTTON_MIN_WIDTH, DEFAULT_HEIGHT);
        g_signal_connect(add_option_button, "clicked", G_CALLBACK(cb_add_new_option), (gpointer)(&state));

        // Use this alignment widget to prevent the button from expanding (so that all of
        // the add option buttons are the same size.
        // 0, 0, 0, 0 makes it top-left aligned with no option for expanding to fill all the
        // available space.
        GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
        gtk_container_add(GTK_CONTAINER(align), add_option_button);
        gtk_widget_show(add_option_button);

        gtk_box_pack_end(GTK_BOX(field_box), align, FALSE, FALSE, NO_PADDING);
        gtk_widget_show(align);

        gtk_widget_show(field_box);
    }

    gtk_widget_show(state.gui.fields_parent_box);
}

static void insert_capture_into_table(SamplerState& state, DataCapture capture) {
    // Add a row to the table:
    gtk_table_resize(GTK_TABLE(state.gui.capture_table), ++state.gui.capture_table_current_rows, COLS_COUNT);
    // Put the capture into that row:
    insert_capture_into_table_row(state, capture, state.gui.capture_table_current_rows - 1);
}

static void insert_capture_into_table_row(SamplerState& state, DataCapture capture, const int row) {
    // This label shows the name of the capture:
    GtkWidget *label = gtk_label_new(capture.name.c_str());
    // Top-left align:
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);

    // This label shows the date of the capture:
    label = gtk_label_new(capture.get_date_string().c_str());
    // Top-left align:
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);

    // This label shows the time of the capture:
    label = gtk_label_new(capture.get_time_string().c_str());
    // Top-left align:
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);

    std::stringstream ss;
    ss << std::setprecision(3) << capture.duration << " s";

    // This label shows the duration of the capture:
    label = gtk_label_new(ss.str().c_str());
    // Top-right align:
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0);
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);

    ss.str("");
    ss << capture.size << " MB";

    // This label shows the size of the capture:
    label = gtk_label_new(ss.str().c_str());
    // Top-right align:
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0);
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);

    // This label shows the user's notes on the capture:
    label = gtk_label_new(capture.notes.c_str());
    // Top-left align:
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);
    // The notes labels need to be saved so that they can be changed later:
    state.gui.notes_labels.push_back(label);

    // This button opens the edit notes window:
    GtkWidget *edit_notes_button = gtk_button_new_with_label("Edit...");
    ss.str("");
    ss << (row - 1);
    gtk_widget_show(edit_notes_button);
    gtk_widget_set_name(edit_notes_button, ss.str().c_str());
    g_signal_connect(edit_notes_button, "clicked", G_CALLBACK(cb_edit_notes), (gpointer)(&state));

    // This alignment widget prevents the edit notes button from expanding.
    // 0, 0, 0, 0 means top-left alignment and no expansion.
    GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(align), edit_notes_button);
    gtk_widget_show(align);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), align, COL_EDIT_NOTES, COL_EDIT_NOTES + 1, row, row + 1);

    // This button deletes the capture:
    GtkWidget *delete_capture_button = gtk_button_new_with_label("Delete");
    gtk_widget_show(delete_capture_button);
    gtk_widget_set_name(delete_capture_button, ss.str().c_str());
    g_signal_connect(delete_capture_button, "clicked", G_CALLBACK(cb_delete_capture), (gpointer)(&state));

    // This alignment widget prevents the delete capture button from expanding.
    // 0, 0, 0, 0 means top-left alignment and no expansion.
    align = gtk_alignment_new(0, 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(align), delete_capture_button);
    gtk_widget_show(align);
    gtk_table_attach_defaults(GTK_TABLE(state.gui.capture_table), align, COL_DELETE_CAPTURE, COL_DELETE_CAPTURE + 1, row, row + 1);

    // Update the total data label:
    state.total_data_captured += capture.size;
    ss.str("");
    ss << state.total_data_captured / 1024 << " GB captured in total";
    gtk_label_set_text(GTK_LABEL(state.gui.label_total_data_captured), ss.str().c_str());

    // Scroll to the bottom:
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(state.gui.capture_table_scroll_window));
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
    gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(state.gui.capture_table_scroll_window), adjustment);
}

static void hide_capture_from_table(GuiElements& gui, const int capture_index) {
    // The first row is always the column headings:
    const int row = capture_index + 1;


    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(gui.capture_table));
    for(iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget* child = (GtkWidget*)iter->data;
        unsigned int child_row = 0;
        gtk_container_child_get(GTK_CONTAINER(gui.capture_table), child, "top-attach", &child_row, NULL);
        if(child_row == row) {
            gtk_widget_hide(child);
        }
    }
    children = NULL;
    iter = NULL;

    gtk_table_set_row_spacing(GTK_TABLE(gui.capture_table), row, 0);
}

static void cb_destroy(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    std::cerr << "In cb_destroy, acquire_process_id == " << state.acquire_process_id << std::endl;
    fflush(stdout);

    // If we are currently acquiring data,
    if(state.acquire_process_id != -1) {
        // Stop the acquire program.
        stop_acquiring(state);
    }

    // When the main window is closed by the user, quit the program:
    gtk_main_quit();

    // Return false to signify that the window destroy event should proceed.
    // (Returning true would prevent the window from actually being closed.)
    // return FALSE;
}

static void cb_start_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    gtk_label_set_text(GTK_LABEL(state.gui.current_capture_status), "capturing data... (click stop to finish)");
    start_acquire_in_child_process(state);

    // The only active button when acquire is running should be the stop button:
    gtk_widget_set_sensitive(state.gui.start_button, FALSE);
    gtk_widget_set_sensitive(state.gui.stop_button, TRUE);
    gtk_widget_set_sensitive(state.gui.new_button, FALSE);
    gtk_widget_set_sensitive(state.gui.reset_button, FALSE);

    gtk_widget_hide(state.gui.label_capture_error);
}

static void cb_stop_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }


    gtk_widget_set_sensitive(state.gui.start_button, FALSE);
    gtk_widget_set_sensitive(state.gui.stop_button, FALSE);
    gtk_widget_set_sensitive(state.gui.new_button, FALSE);
    gtk_widget_set_sensitive(state.gui.reset_button, FALSE);

    gtk_widget_hide(state.gui.current_capture_box);
    stop_acquiring(state);

    // The only active button when there is no current capture should be the new capture button:
    gtk_widget_set_sensitive(state.gui.new_button, TRUE);

    gtk_widget_hide(state.gui.label_capture_error);
}

static void cb_new_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    // Setting the new capture window as modal prevents other windows in the program
    // from receiving focus (so the only window that will accept input is the
    // new capture window).
    gtk_window_set_modal(GTK_WINDOW(state.gui.new_capture_window), TRUE);
    gtk_widget_show(state.gui.new_capture_window);

    gtk_widget_hide(state.gui.label_capture_error);
}

static void cb_reset_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }
    state.current_capture.name = "";
    gtk_widget_hide(state.gui.current_capture_box);

    // The only active button when there is no current capture should be the new capture button:
    gtk_widget_set_sensitive(state.gui.start_button, FALSE);
    gtk_widget_set_sensitive(state.gui.stop_button, FALSE);
    gtk_widget_set_sensitive(state.gui.new_button, TRUE);
    gtk_widget_set_sensitive(state.gui.reset_button, FALSE);

    gtk_widget_hide(state.gui.label_capture_error);
}

static void cb_create_new_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    // The input from the user in the radio group for each field in the naming convention
    // is already in the naming_convention object because it's stored there by
    // cb_field_selection_changed whenever the user clicks on a radio button.
    // So, here, all we have to do is have the naming convention build the name:
    state.current_capture.name = state.naming_convention.name();

    // Display to the user the name of the current capture and that it's ready to begin:
    gtk_label_set_text(GTK_LABEL(state.gui.current_capture_name), state.current_capture.name.c_str());
    gtk_label_set_text(GTK_LABEL(state.gui.current_capture_status), "ready to capture");
    gtk_widget_show(state.gui.current_capture_box);

    // At this point, the user can either cancel the current capture or start the current
    // capture, so those are the buttons that are active right now:
    gtk_widget_set_sensitive(state.gui.start_button, TRUE);
    gtk_widget_set_sensitive(state.gui.stop_button, FALSE);
    gtk_widget_set_sensitive(state.gui.new_button, FALSE);
    gtk_widget_set_sensitive(state.gui.reset_button, TRUE);

    gtk_widget_hide(state.gui.new_capture_window);
    // Allow the main window to receive input again:
    gtk_window_set_modal(GTK_WINDOW(state.gui.new_capture_window), FALSE);
}

static void cb_cancel_new_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    // The only active button when there is no current capture should be the new capture button:
    gtk_widget_set_sensitive(state.gui.start_button, FALSE);
    gtk_widget_set_sensitive(state.gui.stop_button, FALSE);
    gtk_widget_set_sensitive(state.gui.new_button, TRUE);
    gtk_widget_set_sensitive(state.gui.reset_button, FALSE);

    gtk_widget_hide(state.gui.new_capture_window);
    // Allow the main window to receive input again:
    gtk_window_set_modal(GTK_WINDOW(state.gui.new_capture_window), FALSE);
}

static void cb_add_new_option(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    // The field code was stored in the add option button by populate_fields_and_options:
    state.field_being_extended = gtk_widget_get_name(widget);

    // Show the user the name of the current field being extended and clear the entry fields:
    gtk_label_set_text(GTK_LABEL(state.gui.add_option_field_label), ("  " + state.naming_convention.get_field_name(state.field_being_extended)).c_str());
    gtk_entry_set_text(GTK_ENTRY(state.gui.add_option_entry_code), "");
    gtk_entry_set_text(GTK_ENTRY(state.gui.add_option_entry_name), "");

    gtk_widget_show(state.gui.add_option_window);
    // Prevent windows other from add_option_window from receiving input:
    gtk_window_set_modal(GTK_WINDOW(state.gui.add_option_window), TRUE);
}

static void cb_field_selection_changed(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    // widget is the radio button that was clicked. Its name was assigned to the code of the
    // option in populate_fields_and_options. The parent of widget is the field box, and
    // its name was assigned to the code of the field in populate_fields_and_options.
    std::string field_code = gtk_widget_get_name(gtk_widget_get_parent(widget));
    std::string option_code = gtk_widget_get_name(widget);

    // Store the user's selection in naming_convention so that when we need to get the chosen
    // name, we don't have to crawl over the radio buttons to find the selected one.
    state.naming_convention.set_field_value(field_code, option_code);
}

static void cb_add_option_cancel(GtkWidget* widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }

    state.field_being_extended = "";
    gtk_widget_hide(state.gui.add_option_window);
    // Allow windows other than add_option_window to receive input.
    gtk_window_set_modal(GTK_WINDOW(state.gui.add_option_window), FALSE);
}

static void cb_add_option_add(GtkWidget* widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    if(!state.all_initialized) {
        return;
    }
    std::string new_option_code = gtk_entry_get_text(GTK_ENTRY(state.gui.add_option_entry_code));
    std::string new_option_name = gtk_entry_get_text(GTK_ENTRY(state.gui.add_option_entry_name));

    if(!validate_new_option_and_trim(state, new_option_code, new_option_name)) {
        // The new option is not valid, so we can't do anything here. The user was informed
        // by the validate_new_option_and_trim function, so we don't need to worry about that
        // here. Just return from this function and the user will have another chance to change
        // their input to be valid.
        return;
    }

    state.naming_convention.add_option(state.field_being_extended, new_option_code, new_option_name);
    // Re-populate the fields and option radio buttons now that there's a new option:
    populate_fields_and_options(state);

    state.field_being_extended = "";
    gtk_widget_hide(state.gui.add_option_window);
    // Allow other windows to receive input:
    gtk_window_set_modal(GTK_WINDOW(state.gui.add_option_window), FALSE);
}

static void cb_edit_notes(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    const int capture_index = atoi(gtk_widget_get_name(widget));

    DataCapture capture = state.finished_captures[capture_index];
    state.notes_being_edited = capture_index;

    // Put the name of the capture being edited up:
    gtk_label_set_text(GTK_LABEL(state.gui.edit_notes_label_name), ("  " + capture.name).c_str());

    // Fill the text view with the current notes on the capture:
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state.gui.edit_notes_text_view)), capture.notes.c_str(), -1);
    // Clear the error label:
    gtk_label_set_text(GTK_LABEL(state.gui.edit_notes_label_error), "");

    gtk_widget_show(state.gui.edit_notes_window);
    // Move the edit notes window to the center:
    gtk_window_set_gravity(GTK_WINDOW(state.gui.edit_notes_window), GDK_GRAVITY_CENTER);
    gtk_window_move(GTK_WINDOW(state.gui.edit_notes_window), 0, 0);

    // Prevent other windows from receiving input:
    gtk_window_set_modal(GTK_WINDOW(state.gui.edit_notes_window), TRUE);
}

static void cb_edit_notes_save(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    // This is the only way to read all of the text out of the text view:
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state.gui.edit_notes_text_view));
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    std::string new_notes = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

    if(new_notes.find('`') != std::string::npos) {
        // Backquote characters are not allowed in the notes text. Tell the user.
        gtk_label_set_text(GTK_LABEL(state.gui.edit_notes_label_error), "Backquote (`) character not allowed");
        return;
    }

    state.finished_captures[state.notes_being_edited].notes = new_notes;
    // Save the notes to file so that they'll be persistent:
    state.finished_captures[state.notes_being_edited].write_to_file();

    // Display the new notes in the capture table:
    gtk_label_set_text(GTK_LABEL(state.gui.notes_labels[state.notes_being_edited]), new_notes.c_str());

    state.notes_being_edited = -1;
    gtk_widget_hide(state.gui.edit_notes_window);
    // Allow other windows to receive focus:
    gtk_window_set_modal(GTK_WINDOW(state.gui.edit_notes_window), FALSE);
}

static void cb_edit_notes_cancel(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    state.notes_being_edited = -1;
    gtk_widget_hide(state.gui.edit_notes_window);
    // Allow other windows to receive focus:
    gtk_window_set_modal(GTK_WINDOW(state.gui.edit_notes_window), FALSE);
}

static void cb_delete_capture(GtkWidget *widget, gpointer data) {
    SamplerState& state = *((SamplerState*)data);

    const int capture_index = atoi(gtk_widget_get_name(widget));
    hide_capture_from_table(state.gui, capture_index);

    // Delete the capture data and metadata files:
    DataCapture capture = state.finished_captures[capture_index];
    int err = 0;
    err |= remove(capture.data_filename.c_str());
    err |= remove(capture.meta_filename.c_str());
    // TODO remove gps file as well
    if(err) {
        std::cerr << "ERROR: UNABLE TO DELETE " << capture.data_filename << " and/or "
                << capture.meta_filename << std::endl;
    }

    // Update the total data label:
    state.total_data_captured -= capture.size;
    // TODO refactor this into a function
    std::stringstream ss;
    ss << state.total_data_captured / 1024 << " GB captured in total";
    gtk_label_set_text(GTK_LABEL(state.gui.label_total_data_captured), ss.str().c_str());
}

static bool validate_new_option_and_trim(SamplerState& state, std::string &new_option_code, std::string &new_option_name) {
    // Empty strings and strings that are only space characters are not allowed for either the name
    // or the code:
    if(new_option_code == "" || new_option_name == "" || new_option_code.find_first_not_of(' ') == std::string::npos
            || new_option_name.find_first_not_of(' ') == std::string::npos) {
        gtk_label_set_text(GTK_LABEL(state.gui.add_option_label_error), "No empty parameters allowed");
        return FALSE;
    }
    // At this point, neither string is empty, nor is either string only space characters

    // Trim the whitespace off of option code
    if(new_option_code.find(' ') != -1) {
        size_t first_not_space = new_option_code.find_first_not_of(' ');
        // This is safe because we know that the string is not only space characters.
        new_option_code = new_option_code.substr(first_not_space, new_option_code.find_last_not_of(' ') - first_not_space + 1);
    }

    // Trim the whitespace off of option name
    if(new_option_name.find(' ') != -1) {
        size_t first_not_space = new_option_name.find_first_not_of(' ');
        // This is safe because we know that the string is not only space characters.
        new_option_name = new_option_name.substr(first_not_space, new_option_name.find_last_not_of(' ') - first_not_space + 1);
    }

    if(new_option_code.find(' ') != -1) {
        // After stripping the whitespace off of the option code, if it still has spaces in it, it's invalid:
        gtk_label_set_text(GTK_LABEL(state.gui.add_option_label_error), "No spaces allowed in code");
        return FALSE;
    }

    // Check each currently existing option in the field being extended to see if the new option shares a name
    // or a code with it. If it does, the new option is not valid.
    std::map<std::string, std::string> current_options = state.naming_convention.get_options(state.field_being_extended);
    for(std::map<std::string, std::string>::iterator option_it = current_options.begin();
            option_it != current_options.end(); ++option_it) {
        std::string option_code = option_it->first;
        std::string option_name = option_it->second;

        if(option_code == new_option_code) {
            gtk_label_set_text(GTK_LABEL(state.gui.add_option_label_error), ("This code already in use for " + option_name).c_str());
            return FALSE;
        }
        else if(option_name == new_option_name) {
            gtk_label_set_text(GTK_LABEL(state.gui.add_option_label_error), "This name already in use");
            return FALSE;
        }
    }
    // If it's valid, clear the error text:
    gtk_label_set_text(GTK_LABEL(state.gui.add_option_label_error), "");
    return TRUE;
}

/**
 * This function returns true if the first DataCapture happened before the second one, false otherwise.
 */
static bool compare_captures(const DataCapture &first, const DataCapture &second) {
    if(first.year < second.year) {
        return TRUE;
    }
    else if(first.year > second.year) {
        return FALSE;
    }

    if(first.month < second.month) {
        return TRUE;
    }
    else if(first.month > second.month) {
        return FALSE;
    }

    if(first.date < second.date) {
        return TRUE;
    }
    else if(first.date > second.date) {
        return FALSE;
    }

    if(first.hour < second.hour) {
        return TRUE;
    }
    else if(first.hour > second.hour) {
        return FALSE;
    }

    if(first.minute < second.minute) {
        return TRUE;
    }
    else if(first.minute > second.minute) {
        return FALSE;
    }

    if(first.second < second.second) {
        return TRUE;
    }
    else if(first.second > second.second) {
        return FALSE;
    }

    return FALSE;
}

static void load_all_captures_from_files(SamplerState& state) {
    std::vector<std::string> capture_files;

    {
        // Crawl the data folder to find all of the files that end with CAPTURE_META_ENDING
        DIR *directory_pointer = NULL;
        struct dirent *item = NULL;

        directory_pointer = opendir(DATA_FOLDER.c_str());
        if(directory_pointer) {
            while((item = readdir(directory_pointer)) != NULL) {
                std::string filename = item->d_name;
                int found_index = filename.find(CAPTURE_META_ENDING);
                int expected_index = filename.size() - CAPTURE_META_ENDING.size();
                if(found_index != std::string::npos && found_index == expected_index) {
                    // If the filename ends with CAPTURE_META_ENDING, add it
                    // to the list of capture data files:
                    capture_files.push_back(DATA_FOLDER + "/" + filename);
                }
            }
            closedir(directory_pointer);
        }
        directory_pointer = NULL;
    }

    // For each file found, read the capture into memory and add it to finished_captures.
    DataCapture capture;
    for(std::vector<std::string>::iterator it = capture_files.begin(); it != capture_files.end(); ++it) {
        capture.read_from_file(*it);
        state.finished_captures.push_back(capture);
    }
    // Sort finished_captures by the time they happened, with older captures at the beginning:
    std::sort(state.finished_captures.begin(), state.finished_captures.end(), compare_captures);
}
