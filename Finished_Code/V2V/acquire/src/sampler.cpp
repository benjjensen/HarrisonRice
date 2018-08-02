#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <dirent.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <ext/stdio_filebuf.h>

#include <gtk/gtk.h>

#include "NamingConvention.h"
#include "DataCapture.h"

/**
 * Constants for the columns of the capture table.
 */
const unsigned int COLS_COUNT = 7;
const unsigned int COL_NAME = 0;
const unsigned int COL_DATE = 1;
const unsigned int COL_TIME = 2;
const unsigned int COL_DURATION = 3;
const unsigned int COL_SIZE = 4;
const unsigned int COL_NOTES = 5;
const unsigned int COL_EDIT_NOTES = 6;

/**
 * The folder where the data files are stored.
 * 
 * TODO extract this to a header file (here and in acquire.cpp)
 */
const std::string DATA_FOLDER = "../acquire/data";

/**
 * The naming convention used to store the values the user has selected for
 * each field.
 */
NamingConvention naming_convention;

/**
 * The main window.
 */
GtkWidget *main_window = NULL;
/**
 * The table where the captures are displayed.
 */
GtkWidget *capture_table = NULL;
/**
 * The current number of rows in the capture table.
 */
size_t capture_table_current_rows = 0;

/**
 * The box that contains the current capture info.
 */
GtkWidget *current_capture_box = NULL;
/**
 * The label that shows the name of the current capture.
 */
GtkWidget *current_capture_name = NULL;
/**
 * The label that shows the status of the current capture.
 */
GtkWidget *current_capture_status = NULL;

/**
 * The current data capture. This stores the name as determined in the
 * new capture window and then retrieves the information that acquire.cpp
 * saves to the metadata file.
 */
DataCapture current_capture;

/**
 * The list of previously completed data captures.
 * 
 * TODO change this to a vector?
 */
std::list<DataCapture> finished_captures;

/**
 * The button in the main window that starts the data capture.
 */
GtkWidget *start_button = NULL;
/**
 * The button in the main window that stops the current data capture.
 */
GtkWidget *stop_button = NULL;
/**
 * The button in the main window that opens the new capture window.
 */
GtkWidget *new_button = NULL;
/**
 * The button in the main window that clears the current capture.
 */
GtkWidget *reset_button = NULL;

/**
 * The window that allows the user to set up a new capture.
 */
GtkWidget *new_capture_window = NULL;
/**
 * The box in the new capture window that contains the fields and options
 * for the naming convention.
 */
GtkWidget *fields_parent_box = NULL;

/**
 * The window that allows the user to add a new option to a naming convention
 * field.
 */
GtkWidget *add_option_window = NULL;
/**
 * The label in the add option window that tells the user the name of the
 * field they're adding to.
 */
GtkWidget *add_option_field_label = NULL;
/**
 * The entry in the add option window where the user enters the code for the entry.
 */
GtkWidget *add_option_entry_code = NULL;
/**
 * The entry in the add option window where the user enters the name for the entry.
 */
GtkWidget *add_option_entry_name = NULL;
/**
 * The label in the add option window where we display any problems with the
 * user's input in the add_option_entry_code and add_option_entry_name fields.
 */
GtkWidget *add_option_label_error = NULL;

/**
 * The window where the user edits the notes for a certain capture.
 */
GtkWidget* edit_notes_window = NULL;
/**
 * The text view in the edit notes window where the user enters and modifies notes
 * for a certain capture.
 * 
 * This needs to be a text view and not an entry because text view widgets can
 * accept multiline entries, where as entry widgets accept only a single line.
 */
GtkWidget* edit_notes_text_view = NULL;
/**
 * The label in the edit notes window where we display problems with the input in
 * edit_notes_text_view.
 */
GtkWidget* edit_notes_label_error = NULL;
/**
 * The list of labels for the notes of each entry in the capture table.
 */
std::vector<GtkWidget*> notes_labels;

/**
 * This indicates whether the GUI has finished initializing.
 */
bool all_initialized = false;

/**
 * The name of the field that is currently being extended.
 */
std::string field_being_extended = "";

/**
 * The process id for the child process where the acquire program is running.
 */
int acquire_process_id = -1;
/**
 * The file descriptor for the output end of the pipe that the acquire program
 * is feeding with its output.
 */
int acquire_output_fd = -1;

/**
 * The index of the capture in finished_captures for which the user is currently
 * editing the notes.
 */
int notes_being_edited = -1;

/**
 * A callback function for when the user closes the main window.
 */
static gboolean cb_destroy(GtkWidget *widget, GdkEvent *event, gpointer data);
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
 * Validates the code and the name for a new option that the user has
 * entered. Also trims whitespace off.
 */
static bool validate_new_option_and_trim(std::string &new_option_code, std::string &new_option_name);

/**
 * The function that initializes the main window.
 */
static void init_main_window();
/**
 * The function that initializes the new capture window.
 */
static void init_new_capture_window();
/**
 * The function that initializes the add option window.
 */
static void init_add_option_window();
/**
 * The function that initializes the edit notes window.
 */
static void init_edit_notes_window();

/**
 * Populates the fields and options for the naming convention in the new capture window.
 * 
 * This clears what is already in fields_parent_box and refills it with new radio button
 * options.
 */
static void populate_fields_and_options();
/**
 * Inserts a capture into the table at a specific row.
 */
static void insert_capture_into_table_row(DataCapture capture, int row);
/**
 * Inserts a capture into the table at the end of the table.
 * 
 * This function resizes the capture table and then calls
 * insert_capture_into_table_row to insert the capture into the
 * last row.
 */
static void insert_capture_into_table(DataCapture capture);
/**
 * This finds all the "*-meta.txt" files in DATA_FOLDER and reads them in
 * as DataCapture objects and puts those objects into finished_captures.
 * 
 * Note: this does not put the captures into the captures table.
 */
static void load_all_captures_from_files();

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
static bool start_acquire_in_child_process();
/**
 * Stops the acquire program, which is running in a child process
 * with process id acquire_process_id.
 */
static bool stop_acquiring();

// TODO add a label that shows the total space used by all of the captures
// TODO add a button to remove a capture
// TODO update the format of the notes text view
// TODO show the name of the capture that's being edited in the edit notes window
// TODO add a line in the name_fields.txt file that explains that comments will disappear each time
// TODO get rid of the "processing first word" output

int main(int argc, char **argv)
{	
	gtk_init(&argc, &argv);
	
	load_all_captures_from_files();
	
	// Initialize all of the windows we use, before any of them are visible:
	init_main_window();
	init_new_capture_window();
	init_add_option_window();
	init_edit_notes_window();
	
	all_initialized = TRUE;
	
	gtk_widget_show(main_window);
	gtk_main();
	
	return 0;
}



static bool start_acquire_in_child_process()
{
	const int READ_FD = 0;
	const int WRITE_FD = 1;
	
	// A pipe to send info from sampler to acquire (via acquire's standard input):
	int parent_to_child[2];
	if(pipe(parent_to_child) != 0)
	{
		std::cerr << "ERROR: unable to open pipe for parent to child" << std::endl;
	}
	
	// A pipe to get info from acquire and read it in sampler (via acquire's standard output):
	int child_to_parent[2];
	if(pipe(child_to_parent) != 0)
	{
		std::cerr << "ERROR: unable to open pipe for child to parent" << std::endl;
	}
	
	pid_t pid;
	switch ( pid = fork() )
	{
	// -1 means that the fork failed.
	case -1:
		std::cerr << "ERROR: failure to fork for acquire" << std::endl;
		return FALSE;
	// If we get 0, we're the child process.
	case 0:
		// Set up the process interaction by replacing standard in and out with the
		// respective pipes from and to the parent process:
		if(dup2(parent_to_child[READ_FD], STDIN_FILENO) == -1)
		{
			std::cerr << "ERROR: unable to attach std::cin" << std::endl;
		}
		if(dup2(child_to_parent[WRITE_FD], STDOUT_FILENO) == -1)
		{
			std::cerr << "ERROR: unable to attach std::cout" << std::endl;
		}
		// Close the side of the pipes we won't use:
		if(close(parent_to_child[WRITE_FD]) != 0)
		{
			std::cerr << "ERROR: unable to close other side of parent_to_child from child process" << std::endl;
		}
		if(close(child_to_parent[READ_FD]) != 0)
		{
			std::cerr << "ERROR: unable to close other side of child_to_parent from child process" << std::endl;
		}
		
		// Not necessary to change directories because acquire is in the same directory as this.
		execlp("acquire", "acquire", "20", "-f", current_capture.name.c_str(), "-scm", "-scs", "0", NULL);

		std::cerr << "ERROR: this line should never be reached!" << std::endl;
		std::exit(-1);
	// Otherwise, we're the parent process, so break out of this switch statement and continue on:
	default:
		break;
	}
	std::cout << "Child " << pid << " process running..." << std::endl;
	
	// Close the side of the pipes that we won't use in the parent process:
	if(close(parent_to_child[READ_FD]) != 0)
	{
		std::cerr << "ERROR: failed to close parent_to_child read" << std::endl;
	}
	if(close(child_to_parent[WRITE_FD]) != 0)
	{
		std::cerr << "ERROR: failed to close child_to_parent write" << std::endl;
	}
	
	// Save this info for future use:
	acquire_process_id = pid;
	acquire_output_fd = child_to_parent[READ_FD];
	// TODO save acquire input file descriptor
	
	return true;
}

static bool stop_acquiring()
{
	// Convert the file descriptor acquire_output_fd that reads what acquire wrote to its standard output
	// into a stream we can easily read from:
	__gnu_cxx::stdio_filebuf<char> *sb = new __gnu_cxx::stdio_filebuf<char>(acquire_output_fd, std::ios::in);
	std::istream input_from_acquire(sb);
	
	// Sent the SIGINT signal to acquire--this is the same as if the user had pressed ctrl-c at the terminal
	// where acquire is running.
	kill(acquire_process_id, SIGINT);
	
	int err_code = 0;
	// Wait for acquire to be done. (This is necessary--otherwise it ends up as a zombie process.)
	wait(&err_code);
	std::cout << "Error code: " << err_code << std::endl;
	
	// For every line in acquire's output:
	std::string line_str = "";
	while(std::getline(input_from_acquire, line_str))
	{
		std::stringstream line(line_str);
		std::string first_word = "";
		line >> first_word;
		
		// TODO put this string in a header file.
		// Check to see if it's the line that tells us the metadata filename:
		if(first_word == "DataCapture.meta_filename")
		{
			// If it is, read the rest in as the filename:
			line >> current_capture.meta_filename;
			// TODO allow user to take notes before the capture?
			// Save the notes we already have, because otherwise they'll be overwritten as empty
			// when we read the capture info in from the file.
			std::string notes = current_capture.notes;
			current_capture.read_from_file();
			current_capture.notes = notes;
			
			// Add the new capture to the table and to the list of captures:
			finished_captures.push_back(current_capture);
			insert_capture_into_table(current_capture);
		}
	}
	
	if(close(acquire_output_fd) != 0)
	{
		std::cerr << "ERROR: failed to close acquire_output_fd" << std::endl;
	}
	delete sb;
	acquire_output_fd = -1;
	acquire_process_id = -1;
}



static void init_main_window()
{
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title(GTK_WINDOW(main_window), "Sampler");
	// TODO magic number
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER_ALWAYS);
	// TODO magic numbers
	gtk_window_set_default_size(GTK_WINDOW(main_window), 300, 400);
	// Set the callback for when the user closes this window:
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy), NULL);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), layout_box);
	
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	// This allows the scroll window to scroll vertically and horizontally:
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX(layout_box), scrolled_window, TRUE, TRUE, 0);
	
	// 0, 0, 0, 0 means that what's inside of this alignment widget will not expand to fill its parent,
	// which is what we want with the capture table--it should have its own height and width independent
	// from the height and width of the scrolled window.
	GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), align);
	gtk_widget_show(align);
	
	// Removes the "shade" lines from around the scrolled window:
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(scrolled_window))), GTK_SHADOW_NONE);
	
	capture_table_current_rows = 1;
	capture_table = gtk_table_new(capture_table_current_rows, COLS_COUNT, FALSE);
	gtk_container_add(GTK_CONTAINER(align), capture_table);
	gtk_table_set_col_spacings(GTK_TABLE(capture_table), 20);
	gtk_table_set_row_spacings(GTK_TABLE(capture_table), 10);
	
	// The row into which we're inserting the labels below:
	int row = 0;
	
	// Inserting each label into the table:
	GtkWidget *label = gtk_label_new("Name");
	gtk_label_set_markup(GTK_LABEL(label), "<b>Name</b>");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);
	label = gtk_label_new("Date");
	gtk_label_set_markup(GTK_LABEL(label), "<b>Date</b>");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);
	label = gtk_label_new("Time");
	gtk_label_set_markup(GTK_LABEL(label), "<b>Time</b>");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);
	label = gtk_label_new("Duration");
	gtk_label_set_markup(GTK_LABEL(label), "<b>Duration</b>");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);
	label = gtk_label_new("Size");
	gtk_label_set_markup(GTK_LABEL(label), "<b>Size</b>");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);
	label = gtk_label_new("Notes");
	gtk_label_set_markup(GTK_LABEL(label), "<b>Notes</b>");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);
	
	// Populate the list with the captures that were loaded from the files in the data folder.
	// finished_captures should already have been initialized by now:
	for(std::list<DataCapture>::iterator it = finished_captures.begin(); it != finished_captures.end(); ++it)
	{
		insert_capture_into_table(*it);
	}
	
	gtk_widget_show(capture_table);
	gtk_widget_show(scrolled_window);
	
	// This box contains the labels that display information about the current captures:
	current_capture_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), current_capture_box, FALSE, FALSE, 0);
	
	current_capture_name = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(current_capture_box), current_capture_name, FALSE, FALSE, 0);
	gtk_widget_show(current_capture_name);
	
	current_capture_status = gtk_label_new("ready...");
	gtk_box_pack_start(GTK_BOX(current_capture_box), current_capture_status, FALSE, FALSE, 5);
	gtk_widget_show(current_capture_status);
	
	// Intentionally don't show current_capture_box yet
		
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	// The new capture button. The callback opens the new capture screen.
	new_button = gtk_button_new_with_label("New Capture...");
	gtk_box_pack_start(GTK_BOX(sub_box), new_button, TRUE, TRUE, 0);
	g_signal_connect (new_button, "clicked", G_CALLBACK(cb_new_capture), NULL);
	gtk_widget_show(new_button);
	
	// The reset button. The callback clears the current capture.
	reset_button = gtk_button_new_with_label("Reset");
	gtk_widget_set_size_request(reset_button, 100, -1);
	gtk_box_pack_start(GTK_BOX(sub_box), reset_button, FALSE, FALSE, 0);
	// Not active until there's a current capture:
	gtk_widget_set_sensitive(reset_button, FALSE);
	g_signal_connect (reset_button, "clicked", G_CALLBACK(cb_reset_capture), NULL);
	gtk_widget_show(reset_button);
	
	gtk_widget_show(sub_box);	
		
	sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	// The start button starts the current capture when it's clicked:
	start_button = gtk_button_new_with_label("Start");
	gtk_box_pack_start(GTK_BOX(sub_box), start_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(start_button, FALSE);
	g_signal_connect(start_button, "clicked", G_CALLBACK(cb_start_capture), NULL);
	gtk_widget_show(start_button);
	
	// The stop button stops the current capture when it's clicked:
	stop_button = gtk_button_new_with_label("Stop");
	gtk_box_pack_start(GTK_BOX(sub_box), stop_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(stop_button, FALSE);
	g_signal_connect (stop_button, "clicked", G_CALLBACK(cb_stop_capture), NULL);
	gtk_widget_show(stop_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
}

static void init_new_capture_window()
{
	new_capture_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	// This makes the new capture window always appear above the main window:
	gtk_window_set_transient_for(GTK_WINDOW(new_capture_window), GTK_WINDOW(main_window));
	gtk_window_set_title(GTK_WINDOW(new_capture_window), "New Capture");
	// TODO magic number
	gtk_container_set_border_width(GTK_CONTAINER(new_capture_window), 10);
	gtk_window_set_position(GTK_WINDOW(new_capture_window), GTK_WIN_POS_CENTER_ALWAYS);
	// The user can't close this window by pressing the x button at the top right:
	gtk_window_set_deletable(GTK_WINDOW(new_capture_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(new_capture_window), layout_box);
	
	// This box contains the fields--each field has a box with a radio list of options.
	// TODO magic number
	fields_parent_box = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(layout_box), fields_parent_box, FALSE, FALSE, 0);
	
	// Call the separate function that populates the fields into fields_parent_box:
	populate_fields_and_options();
	
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	// The new capture button sends the capture info to the main window so that it can
	// begin capturing.
	GtkWidget* create_new_capture_button = gtk_button_new_with_label("Prepare");
	gtk_box_pack_start(GTK_BOX(sub_box), create_new_capture_button, TRUE, TRUE, 0);
	g_signal_connect(create_new_capture_button, "clicked", G_CALLBACK(cb_create_new_capture), NULL);
	gtk_widget_show(create_new_capture_button);
	
	// The cancel new capture button cancels making a new capture and returns to the main
	// window.
	GtkWidget* cancel_new_capture_button = gtk_button_new_with_label("Cancel");
	// TODO magic numbers
	gtk_widget_set_size_request(cancel_new_capture_button, 100, -1);
	gtk_box_pack_start(GTK_BOX(sub_box), cancel_new_capture_button, FALSE, FALSE, 0);
	g_signal_connect(cancel_new_capture_button, "clicked", G_CALLBACK(cb_cancel_new_capture), NULL);
	gtk_widget_show(cancel_new_capture_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
}

static void init_add_option_window()
{
	add_option_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	// This window will always appear above the new capture window:
	gtk_window_set_transient_for(GTK_WINDOW(add_option_window), GTK_WINDOW(new_capture_window));
	gtk_window_set_title(GTK_WINDOW(add_option_window), "Add Option");
	// TODO magic number
	gtk_container_set_border_width(GTK_CONTAINER(add_option_window), 10);
	gtk_window_set_position(GTK_WINDOW(add_option_window), GTK_WIN_POS_CENTER_ALWAYS);
	// The user can't close this window except through our GUI:
	gtk_window_set_deletable(GTK_WINDOW(add_option_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(add_option_window), layout_box);
	
	GtkWidget *frame = gtk_frame_new("Field:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	// The label that displays the name of the field that is being extended:
	add_option_field_label = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(frame), add_option_field_label);
	// Sets the alignment to left-align:
	gtk_misc_set_alignment(GTK_MISC(add_option_field_label), 0, 0);
	gtk_widget_show(add_option_field_label);
	
	gtk_widget_show(frame);
	
	frame = gtk_frame_new("Code:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	// The entry where the user enters the code for the new option:
	add_option_entry_code = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(frame), add_option_entry_code);
	gtk_widget_show(add_option_entry_code);
	
	gtk_widget_show(frame);
	
	frame = gtk_frame_new("Name:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	// The entry where the user enters the name of the new option:
	add_option_entry_name = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(frame), add_option_entry_name);
	gtk_widget_show(add_option_entry_name);
	
	gtk_widget_show(frame);
	
	// The label where error text is displayed:
	add_option_label_error = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(layout_box), add_option_label_error, FALSE, FALSE, 0);
	
	GdkColor color;
  	gdk_color_parse ("red", &color);
  	// Changes the color of the error text to red:
  	gtk_widget_modify_fg (add_option_label_error, GTK_STATE_NORMAL, &color);
  	gtk_widget_show(add_option_label_error);
	
	GtkWidget* sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	// The button the user clicks to confirm their new option.
	// When it's clicked, the user's input is validated before it's
	// accepted; if it's not acceptable, an error message is displayed
	// in add_option_label_error.
	GtkWidget* add_option_add_button = gtk_button_new_with_label("Add");
	gtk_box_pack_start(GTK_BOX(sub_box), add_option_add_button, TRUE, TRUE, 0);
	g_signal_connect(add_option_add_button, "clicked", G_CALLBACK(cb_add_option_add), NULL);
	gtk_widget_show(add_option_add_button);
	
	// The button that cancels the new option:
	GtkWidget* add_option_cancel_button = gtk_button_new_with_label("Cancel");
	gtk_box_pack_start(GTK_BOX(sub_box), add_option_cancel_button, FALSE, FALSE, 0);
	// TODO magic numbers
	gtk_widget_set_size_request(add_option_cancel_button, 100, -1);
	g_signal_connect(add_option_cancel_button, "clicked", G_CALLBACK(cb_add_option_cancel), NULL);
	gtk_widget_show(add_option_cancel_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
}

static void init_edit_notes_window()
{
	edit_notes_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	// The edit notes window always appears on top of the main window:
	gtk_window_set_transient_for(GTK_WINDOW(edit_notes_window), GTK_WINDOW(main_window));
	gtk_window_set_title(GTK_WINDOW(edit_notes_window), "Edit notes");
	// TODO magic numbers
	gtk_container_set_border_width(GTK_CONTAINER(edit_notes_window), 10);
	gtk_window_set_position(GTK_WINDOW(edit_notes_window), GTK_WIN_POS_CENTER_ALWAYS);
	// The user can't close this window except through our GUI:
	gtk_window_set_deletable(GTK_WINDOW(edit_notes_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(edit_notes_window), layout_box);
	
	GtkWidget *frame = gtk_frame_new("Notes:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	// The text view where the user inputs their notes:
	edit_notes_text_view = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(frame), edit_notes_text_view);
	gtk_widget_show(edit_notes_text_view);
	
	gtk_widget_show(frame);
	
	// The label where errors are displayed:
	edit_notes_label_error = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(layout_box), edit_notes_label_error, FALSE, FALSE, 0);
	
	GdkColor color;
  	gdk_color_parse ("red", &color);
  	// Set the error label's text color to red:
  	gtk_widget_modify_fg(edit_notes_label_error, GTK_STATE_NORMAL, &color);
	gtk_widget_show(edit_notes_label_error);
	
	GtkWidget* sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	// This button saves the notes:
	GtkWidget* save_button = gtk_button_new_with_label("Save");
	gtk_box_pack_start(GTK_BOX(sub_box), save_button, TRUE, TRUE, 0);
	g_signal_connect(save_button, "clicked", G_CALLBACK(cb_edit_notes_save), NULL);
	gtk_widget_show(save_button);
	
	// This button cancels editing the notes:
	GtkWidget* cancel_button = gtk_button_new_with_label("Cancel");
	gtk_box_pack_start(GTK_BOX(sub_box), cancel_button, FALSE, FALSE, 0);
	// TODO magic numbers
	gtk_widget_set_size_request(cancel_button, 100, -1);
	g_signal_connect(cancel_button, "clicked", G_CALLBACK(cb_edit_notes_cancel), NULL);
	gtk_widget_show(cancel_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
}


static void populate_fields_and_options()
{
	if(fields_parent_box == NULL)
	{
		std::cerr << "ERROR: populate_fields_and_options() called before fields_parent_box was set." << std::endl;
	}
	
	{
		// Remove all of the old fields from fields_parent_box:
		GList *children, *iter;

		children = gtk_container_get_children(GTK_CONTAINER(fields_parent_box));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
			gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);
		children = NULL;
		iter = NULL;
	}
	
	// For each field in the naming convention:
	std::list<std::string> field_order = naming_convention.get_field_order();
	for(std::list<std::string>::iterator field_it = field_order.begin();
			field_it != field_order.end(); ++field_it)
	{
		GtkWidget* field_box = gtk_vbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(fields_parent_box), field_box, TRUE, TRUE, 0);
		
		std::string field_code = *field_it;
		// Store the code for the field in the field_box object so that it can be
		// retrieved later by cb_field_selected_changed:
		gtk_widget_set_name(field_box, field_code.c_str());
		
		// This label shows the name of the field:
		GtkWidget* field_label = gtk_label_new(naming_convention.get_field_name(field_code).c_str());
		gtk_box_pack_start(GTK_BOX(field_box), field_label, FALSE, FALSE, 0);
		gtk_widget_show(field_label);
		
		std::string selected_option = naming_convention.get_field_value(field_code);
		
		GtkWidget* option_radio_button = NULL;
		std::map<std::string, std::string> options = naming_convention.get_options(field_code);
		for(std::map<std::string, std::string>::iterator option_it = options.begin();
				option_it != options.end(); ++option_it)
		{
			std::string option_code = option_it->first;
			std::string option_name = option_it->second;
			if(option_radio_button == NULL)
			{
				// If this is the first radio button in this field, create the new radio button in 
				// a new radio group (with the option name as its label):
				option_radio_button = gtk_radio_button_new_with_label(NULL, option_name.c_str());
			}
			else
			{
				// Otherwise, if this is not the first radio button, create a the new radio button in
				// the same radio group as the previous one (with the option name as its label):
				option_radio_button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(option_radio_button), option_name.c_str());
			}
			
			if(option_code == selected_option)
			{
				// If this is the selected option, set this as the active radio button:
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option_radio_button), TRUE);
			}
			
			gtk_box_pack_start(GTK_BOX(field_box), option_radio_button, FALSE, FALSE, 0);
			
			// Store the code for this option in the radio button widget so that it can be retreived by
			// cb_field_selection_changed
			gtk_widget_set_name(option_radio_button, option_code.c_str());
			g_signal_connect(option_radio_button, "toggled", G_CALLBACK(cb_field_selection_changed), NULL);
			
			gtk_widget_show(option_radio_button);
		}
		
		// This button opens the add option window:
		GtkWidget* add_option_button = gtk_button_new_with_label("Add...");
		gtk_widget_set_name(add_option_button, field_code.c_str());
		// TODO magic numbers
		gtk_widget_set_size_request(add_option_button, 100, -1);
		g_signal_connect(add_option_button, "clicked", G_CALLBACK(cb_add_new_option), NULL);
		
		// Use this alignment widget to prevent the button from expanding (so that all of
		// the add option buttons are the same size.
		// 0, 0, 0, 0 makes it top-left aligned with no option for expanding to fill all the
		// available space.
		GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
		gtk_container_add(GTK_CONTAINER(align), add_option_button);
		gtk_widget_show(add_option_button);
		
		gtk_box_pack_end(GTK_BOX(field_box), align, FALSE, FALSE, 0);
		gtk_widget_show(align);
		
		gtk_widget_show(field_box);
	}
	
	gtk_widget_show(fields_parent_box);
}

static void insert_capture_into_table(DataCapture capture)
{
	// Add a row to the table:
	gtk_table_resize(GTK_TABLE(capture_table), ++capture_table_current_rows, COLS_COUNT);
	// Put the capture into that row:
	insert_capture_into_table_row(capture, capture_table_current_rows - 1);
}

static void insert_capture_into_table_row(DataCapture capture, int row)
{
	// This label shows the name of the capture:
	GtkWidget *label = gtk_label_new(capture.name.c_str());
	// Top-left align:
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);
	
	// This label shows the date of the capture:
	label = gtk_label_new(capture.get_date_string().c_str());
	// Top-left align:
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);
	
	// This label shows the time of the capture:
	label = gtk_label_new(capture.get_time_string().c_str());
	// Top-left align:
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);
	
	std::stringstream ss;
	ss << std::setprecision(3) << capture.duration << " s";
	
	// This label shows the duration of the capture:
	label = gtk_label_new(ss.str().c_str());
	// Top-right align:
	gtk_misc_set_alignment(GTK_MISC(label), 1, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);
	
	ss.str("");
	ss << capture.size << " MB";
	
	// This label shows the size of the capture:
	label = gtk_label_new(ss.str().c_str());
	// Top-right align:
	gtk_misc_set_alignment(GTK_MISC(label), 1, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);
	
	// This label shows the user's notes on the capture:
	label = gtk_label_new(capture.notes.c_str());
	// Top-left align:
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);
	// The notes labels need to be saved so that they can be changed later:
	notes_labels.push_back(label);
	
	// This button opens the edit notes window:
	GtkWidget *edit_notes_button = gtk_button_new_with_label("Edit...");
	gtk_widget_show(edit_notes_button);
	g_signal_connect(edit_notes_button, "clicked", G_CALLBACK(cb_edit_notes), (gpointer)(row - 1));
	
	// This alignment widget prevents the edit notes button from expanding.
	// 0, 0, 0, 0 means top-left alignment and no expansion.
	GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(align), edit_notes_button);
	gtk_widget_show(align);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), align, COL_EDIT_NOTES, COL_EDIT_NOTES + 1, row, row + 1);
}



static gboolean cb_destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	// When the main window is closed by the user, quit the program:
	gtk_main_quit();
	// Return false to signify that the window destroy event should proceed.
	// (Returning true would prevent the window from actually being closed.)
	return FALSE;
}

static void cb_start_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	gtk_label_set_text(GTK_LABEL(current_capture_status), "capturing data... (click stop to finish)");
	start_acquire_in_child_process();
	
	// The only active button when acquire is running should be the stop button:
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, TRUE);
	gtk_widget_set_sensitive(new_button, FALSE);
	gtk_widget_set_sensitive(reset_button, FALSE);
}

static void cb_stop_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	gtk_widget_hide(current_capture_box);
	stop_acquiring();
	
	// The only active button when there is no current capture should be the new capture button:
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
}

static void cb_new_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	// Setting the new capture window as modal prevents other windows in the program
	// from receiving focus (so the only window that will accept input is the
	// new capture window).
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), TRUE);
	gtk_widget_show(new_capture_window);
}

static void cb_reset_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	current_capture.name = "";
	gtk_widget_hide(current_capture_box);
	
	// The only active button when there is no current capture should be the new capture button:
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
}

static void cb_create_new_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	// The input from the user in the radio group for each field in the naming convention
	// is already in the naming_convention object because it's stored there by
	// cb_field_selection_changed whenever the user clicks on a radio button.
	// So, here, all we have to do is have the naming convention build the name:
	current_capture.name = naming_convention.name();
	
	// Display to the user the name of the current capture and that it's ready to begin:
	gtk_label_set_text(GTK_LABEL(current_capture_name), current_capture.name.c_str());
	gtk_label_set_text(GTK_LABEL(current_capture_status), "ready to capture");
	gtk_widget_show(current_capture_box);
	
	// At this point, the user can either cancel the current capture or start the current
	// capture, so those are the buttons that are active right now:
	gtk_widget_set_sensitive(start_button, TRUE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, FALSE);
	gtk_widget_set_sensitive(reset_button, TRUE);
	
	gtk_widget_hide(new_capture_window);
	// Allow the main window to receive input again:
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), FALSE);
}

static void cb_cancel_new_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	// The only active button when there is no current capture should be the new capture button:
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
	
	gtk_widget_hide(new_capture_window);
	// Allow the main window to receive input again:
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), FALSE);
}

static void cb_add_new_option(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	// The field code was stored in the add option button by populate_fields_and_options:
	field_being_extended = gtk_widget_get_name(widget);
	
	// Show the user the name of the current field being extended and clear the entry fields:
	gtk_label_set_text(GTK_LABEL(add_option_field_label), ("  " + naming_convention.get_field_name(field_being_extended)).c_str());
	gtk_entry_set_text(GTK_ENTRY(add_option_entry_code), "");
	gtk_entry_set_text(GTK_ENTRY(add_option_entry_name), "");
	
	gtk_widget_show(add_option_window);
	// Prevent windows other from add_option_window from receiving input:
	gtk_window_set_modal(GTK_WINDOW(add_option_window), TRUE);
}

static void cb_field_selection_changed(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	// widget is the radio button that was clicked. Its name was assigned to the code of the
	// option in populate_fields_and_options. The parent of widget is the field box, and
	// its name was assigned to the code of the field in populate_fields_and_options.
	std::string field_code = gtk_widget_get_name(gtk_widget_get_parent(widget));
	std::string option_code = gtk_widget_get_name(widget);
	
	// Store the user's selection in naming_convention so that when we need to get the chosen
	// name, we don't have to crawl over the radio buttons to find the selected one.
	naming_convention.set_field_value(field_code, option_code);
}

static void cb_add_option_cancel(GtkWidget* widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	field_being_extended = "";
	gtk_widget_hide(add_option_window);
	// Allow windows other than add_option_window to receive input.
	gtk_window_set_modal(GTK_WINDOW(add_option_window), FALSE);
}

static void cb_add_option_add(GtkWidget* widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	std::string new_option_code = gtk_entry_get_text(GTK_ENTRY(add_option_entry_code));
	std::string new_option_name = gtk_entry_get_text(GTK_ENTRY(add_option_entry_name));
	
	if(!validate_new_option_and_trim(new_option_code, new_option_name))
	{
		// The new option is not valid, so we can't do anything here. The user was informed
		// by the validate_new_option_and_trim function, so we don't need to worry about that
		// here. Just return from this function and the user will have another chance to change
		// their input to be valid.
		return;	
	}
	
	naming_convention.add_option(field_being_extended, new_option_code, new_option_name);
	// Re-populate the fields and option radio buttons now that there's a new option:
	populate_fields_and_options();
	
	field_being_extended = "";
	gtk_widget_hide(add_option_window);
	// Allow other windows to receive input:
	gtk_window_set_modal(GTK_WINDOW(add_option_window), FALSE);
}

static void cb_edit_notes(GtkWidget *widget, gpointer data)
{
	long capture_index = (long)data;
	
	std::list<DataCapture>::iterator it = finished_captures.begin();
	std::advance(it, capture_index);
	
	DataCapture capture = *it;
	notes_being_edited = capture_index;
	
	// Fill the text view with the current notes on the capture:
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit_notes_text_view)), capture.notes.c_str(), -1);
	// Clear the error label:
	gtk_label_set_text(GTK_LABEL(edit_notes_label_error), "");
	
	gtk_widget_show(edit_notes_window);
	// Prevent other windows from receiving input:
	gtk_window_set_modal(GTK_WINDOW(edit_notes_window), TRUE);
}

static void cb_edit_notes_save(GtkWidget *widget, gpointer data)
{
	// This is the only way to read all of the text out of the text view:
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit_notes_text_view));
	GtkTextIter start_iter, end_iter;
	gtk_text_buffer_get_start_iter(buffer, &start_iter);
	gtk_text_buffer_get_end_iter(buffer, &end_iter);
	std::string new_notes = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
	
	if(new_notes.find('`') != std::string::npos)
	{
		// Backquote characters are not allowed in the notes text. Tell the user.
		gtk_label_set_text(GTK_LABEL(edit_notes_label_error), "Backquote (`) character not allowed");
		return;
	}
	
	std::list<DataCapture>::iterator it = finished_captures.begin();
	std::advance(it, notes_being_edited);
	
	(*it).notes = new_notes;
	// Save the notes to file so that they'll be persistent:
	(*it).write_to_file();
	
	// Display the new notes in the capture table:
	gtk_label_set_text(GTK_LABEL(notes_labels[notes_being_edited]), new_notes.c_str());
	
	notes_being_edited = -1;
	gtk_widget_hide(edit_notes_window);
	// Allow other windows to receive focus:
	gtk_window_set_modal(GTK_WINDOW(edit_notes_window), FALSE);
}
static void cb_edit_notes_cancel(GtkWidget *widget, gpointer data)
{
	notes_being_edited = -1;
	gtk_widget_hide(edit_notes_window);
	// Allow other windows to receive focus:
	gtk_window_set_modal(GTK_WINDOW(edit_notes_window), FALSE);
}

static bool validate_new_option_and_trim(std::string &new_option_code, std::string &new_option_name)
{
	// Empty strings and strings that are only space characters are not allowed for either the name
	// or the code:
	if(new_option_code == "" || new_option_name == "" || new_option_code.find_first_not_of(' ') == std::string::npos
			|| new_option_name.find_first_not_of(' ') == std::string::npos)
	{
		gtk_label_set_text(GTK_LABEL(add_option_label_error), "No empty paramaters allowed");
		return FALSE;
	}
	// At this point, neither string is empty, nor is either string only space characters
	
	// Trim the whitespace off of option code
	if(new_option_code.find(' ') != -1)
	{
		size_t first_not_space = new_option_code.find_first_not_of(' ');
		// This is safe because we know that the string is not only space characters.
		new_option_code = new_option_code.substr(first_not_space, new_option_code.find_last_not_of(' ') - first_not_space + 1);
	}
	
	// Trim the whitespace off of option name
	if(new_option_name.find(' ') != -1)
	{
		size_t first_not_space = new_option_name.find_first_not_of(' ');
		// This is safe because we know that the string is not only space characters.
		new_option_name = new_option_name.substr(first_not_space, new_option_name.find_last_not_of(' ') - first_not_space + 1);
	}
	
	if(new_option_code.find(' ') != -1)
	{
		// After stripping the whitespace off of the option code, if it still has spaces in it, it's invalid:
		gtk_label_set_text(GTK_LABEL(add_option_label_error), "No spaces allowed in code");
		return FALSE;
	}
	
	// Check each currently existing option in the field being extended to see if the new option shares a name
	// or a code with it. If it does, the new option is not valid.
	std::map<std::string, std::string> current_options = naming_convention.get_options(field_being_extended);
	for(std::map<std::string, std::string>::iterator option_it = current_options.begin();
			option_it != current_options.end(); ++option_it)
	{
		std::string option_code = option_it->first;
		std::string option_name = option_it->second;
		
		if(option_code == new_option_code)
		{
			gtk_label_set_text(GTK_LABEL(add_option_label_error), ("This code already in use for " + option_name).c_str());
			return FALSE;
		}
		else if(option_name == new_option_name)
		{
			gtk_label_set_text(GTK_LABEL(add_option_label_error), "This name already in use");
			return FALSE;
		}
	}
	// If it's valid, clear the error text:
	gtk_label_set_text(GTK_LABEL(add_option_label_error), "");
	return TRUE;
}

/**
 * This function returns true if the first DataCapture happened before the second one, false otherwise.
 */
static bool compare_captures(const DataCapture &first, const DataCapture &second)
{
	if(first.year < second.year)
	{
		return TRUE;	
	}
	else if(first.year > second.year)
	{
		return FALSE;
	}
	
	if(first.month < second.month)
	{
		return TRUE;	
	}
	else if(first.month > second.month)
	{
		return FALSE;
	}
	
	if(first.date < second.date)
	{
		return TRUE;	
	}
	else if(first.date > second.date)
	{
		return FALSE;
	}
	
	if(first.hour < second.hour)
	{
		return TRUE;	
	}
	else if(first.hour > second.hour)
	{
		return FALSE;
	}
	
	if(first.minute < second.minute)
	{
		return TRUE;	
	}
	else if(first.minute > second.minute)
	{
		return FALSE;
	}
	
	if(first.second < second.second)
	{
		return TRUE;	
	}
	else if(first.second > second.second)
	{
		return FALSE;
	}
	
	return FALSE;
}

static void load_all_captures_from_files()
{
	std::list<std::string> capture_files;
	
	{
		// Crawl the data folder to find all of the files that end with CAPTURE_META_ENDING
		DIR *directory_pointer = NULL;
		struct dirent *item = NULL;
	
		directory_pointer = opendir(DATA_FOLDER.c_str());
		if(directory_pointer)
		{
			while((item = readdir(directory_pointer)) != NULL)
			{
				std::string filename = item->d_name;
				int found_index = filename.find(CAPTURE_META_ENDING);
				int expected_index = filename.size() - CAPTURE_META_ENDING.size();
				if(found_index != std::string::npos && found_index == expected_index)
				{
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
	for(std::list<std::string>::iterator it = capture_files.begin(); it != capture_files.end(); ++it)
	{
		capture.read_from_file(*it);
		finished_captures.push_back(capture);
	}
	// Sort finished_captures by the time they happened, with older captures at the beginning:
	finished_captures.sort(compare_captures);
}
