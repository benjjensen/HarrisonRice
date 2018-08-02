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
// TODO sideways scrolling

// TODO finish commenting the code

int main(int argc, char **argv)
{	
	gtk_init(&argc, &argv);
	
	load_all_captures_from_files();
	
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
	
	int parent_to_child[2];
	if(pipe(parent_to_child) != 0)
	{
		std::cerr << "ERROR: unable to open pipe for parent to child" << std::endl;
	}
	
	int child_to_parent[2];
	if(pipe(child_to_parent) != 0)
	{
		std::cerr << "ERROR: unable to open pipe for child to parent" << std::endl;
	}
	
	pid_t pid;
	switch ( pid = fork() )
	{
	case -1:
		std::cerr << "ERROR: failure to fork for acquire" << std::endl;
		return FALSE;
	case 0: // Child
		if(dup2(parent_to_child[READ_FD], STDIN_FILENO) == -1)
		{
			std::cerr << "ERROR: unable to attach std::cin" << std::endl;
		}
		if(dup2(child_to_parent[WRITE_FD], STDOUT_FILENO) == -1)
		{
			std::cerr << "ERROR: unable to attach std::cout" << std::endl;
		}
		if(close(parent_to_child[WRITE_FD]) != 0)
		{
			std::cerr << "ERROR: unable to close other side of parent_to_child from child process" << std::endl;
		}
		if(close(child_to_parent[READ_FD]) != 0)
		{
			std::cerr << "ERROR: unable to close other side of child_to_parent from child process" << std::endl;
		}
		
		// Not necessary to change directories because acquire is in the same directory as this
		execlp("acquire", "acquire", "20", "-f", current_capture.name.c_str(), "-scm", "-scs", "0", NULL);

		std::cerr << "ERROR: this line should never be reached!" << std::endl;
		std::exit(-1);
	default: // Parent
		break;
	}
	std::cout << "Child " << pid << " process running..." << std::endl;
	
	if(close(parent_to_child[READ_FD]) != 0)
	{
		std::cerr << "ERROR: failed to close parent_to_child read" << std::endl;
	}
	if(close(child_to_parent[WRITE_FD]) != 0)
	{
		std::cerr << "ERROR: failed to close child_to_parent write" << std::endl;
	}
	
	acquire_process_id = pid;
	acquire_output_fd = child_to_parent[READ_FD];
	
	return true;
}

static bool stop_acquiring()
{
	__gnu_cxx::stdio_filebuf<char> *sb = new __gnu_cxx::stdio_filebuf<char>(acquire_output_fd, std::ios::in);
	std::istream input_from_acquire(sb);
	
	kill(acquire_process_id, SIGINT);
	int err_code = 0;
	wait(&err_code);
	std::cout << "Error code: " << err_code << std::endl;
	
	std::string line_str = "";
	while(std::getline(input_from_acquire, line_str))
	{
		std::stringstream line(line_str);
		std::string first_word = "";
		line >> first_word;
		std::cout << "Processing first word: " << first_word << std::endl;
		if(first_word == "DataCapture.meta_filename")
		{
			line >> current_capture.meta_filename;
			std::string notes = current_capture.notes;
			current_capture.read_from_file();
			current_capture.notes = notes;
			finished_captures.push_back(current_capture);
			insert_capture_into_table(current_capture);
		}
	}
	
	if(close(acquire_output_fd) != 0)
	{
		std::cerr << "ERROR: failed to close acquire_output_fd" << std::endl;
	}
	delete sb;
	acquire_output_fd = 0;
	acquire_process_id = 0;
}



static void init_main_window()
{
	std::cout << "init_main_window\n";
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Sampler");
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_default_size(GTK_WINDOW(main_window), 300, 400);
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy), NULL);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), layout_box);
	
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX(layout_box), scrolled_window, TRUE, TRUE, 0);
	
	GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), align);
	gtk_widget_show(align);
	
	gtk_viewport_set_shadow_type(GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(scrolled_window))), GTK_SHADOW_NONE);
	
	capture_table_current_rows = 1;
	capture_table = gtk_table_new(capture_table_current_rows, COLS_COUNT, FALSE);
	gtk_container_add(GTK_CONTAINER(align), capture_table);
	gtk_table_set_col_spacings(GTK_TABLE(capture_table), 20);
	gtk_table_set_row_spacings(GTK_TABLE(capture_table), 10);
	
	int row = 0;
	
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
	
	for(std::list<DataCapture>::iterator it = finished_captures.begin(); it != finished_captures.end(); ++it)
	{
		insert_capture_into_table(*it);
	}
	
	gtk_widget_show(capture_table);
	gtk_widget_show(scrolled_window);
	
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
	
	new_button = gtk_button_new_with_label("New Capture...");
	gtk_box_pack_start(GTK_BOX(sub_box), new_button, TRUE, TRUE, 0);
	g_signal_connect (new_button, "clicked", G_CALLBACK(cb_new_capture), NULL);
	gtk_widget_show(new_button);
	
	reset_button = gtk_button_new_with_label("Reset");
	gtk_widget_set_size_request(reset_button, 100, -1);
	gtk_box_pack_start(GTK_BOX(sub_box), reset_button, FALSE, FALSE, 0);
	gtk_widget_set_sensitive(reset_button, FALSE);
	g_signal_connect (reset_button, "clicked", G_CALLBACK(cb_reset_capture), NULL);
	gtk_widget_show(reset_button);
	
	gtk_widget_show(sub_box);	
		
	sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	start_button = gtk_button_new_with_label("Start");
	gtk_box_pack_start(GTK_BOX(sub_box), start_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(start_button, FALSE);
	g_signal_connect(start_button, "clicked", G_CALLBACK(cb_start_capture), NULL);
	gtk_widget_show(start_button);
	
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
	std::cout << "init_new_capture_window\n";
	new_capture_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(new_capture_window), GTK_WINDOW(main_window));
	gtk_window_set_title(GTK_WINDOW(new_capture_window), "New Capture");
	gtk_container_set_border_width(GTK_CONTAINER(new_capture_window), 10);
	gtk_window_set_position(GTK_WINDOW(new_capture_window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_deletable(GTK_WINDOW(new_capture_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(new_capture_window), layout_box);
	
	fields_parent_box = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(layout_box), fields_parent_box, FALSE, FALSE, 0);
	populate_fields_and_options();
	
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	GtkWidget* create_new_capture_button = gtk_button_new_with_label("Prepare");
	GtkWidget* cancel_new_capture_button = gtk_button_new_with_label("Cancel");
	
	gtk_box_pack_start(GTK_BOX(sub_box), create_new_capture_button, TRUE, TRUE, 0);
	g_signal_connect(create_new_capture_button, "clicked", G_CALLBACK(cb_create_new_capture), NULL);
	gtk_widget_show(create_new_capture_button);
	
	
	gtk_widget_set_size_request(cancel_new_capture_button, 100, -1);
	gtk_box_pack_start(GTK_BOX(sub_box), cancel_new_capture_button, FALSE, FALSE, 0);
	g_signal_connect(cancel_new_capture_button, "clicked", G_CALLBACK(cb_cancel_new_capture), NULL);
	gtk_widget_show(cancel_new_capture_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
}

static void init_add_option_window()
{
	std::cout << "init_add_option_window\n";
	add_option_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(add_option_window), GTK_WINDOW(new_capture_window));
	gtk_window_set_title(GTK_WINDOW(add_option_window), "Add Option");
	gtk_container_set_border_width(GTK_CONTAINER(add_option_window), 10);
	gtk_window_set_position(GTK_WINDOW(add_option_window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_deletable(GTK_WINDOW(add_option_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(add_option_window), layout_box);
	
	GtkWidget *frame = gtk_frame_new("Field:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	//gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	add_option_field_label = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(frame), add_option_field_label);
	gtk_misc_set_alignment(GTK_MISC(add_option_field_label), 0, 0);
	gtk_widget_show(add_option_field_label);
	
	gtk_widget_show(frame);
	
	frame = gtk_frame_new("Code:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	//gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	add_option_entry_code = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(frame), add_option_entry_code);
	gtk_widget_show(add_option_entry_code);
	
	gtk_widget_show(frame);
	
	frame = gtk_frame_new("Name:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	//gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	add_option_entry_name = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(frame), add_option_entry_name);
	gtk_widget_show(add_option_entry_name);
	
	gtk_widget_show(frame);
	
	add_option_label_error = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(layout_box), add_option_label_error, FALSE, FALSE, 0);
	
	GdkColor color;
  	gdk_color_parse ("red", &color);
  	gtk_widget_modify_fg (add_option_label_error, GTK_STATE_NORMAL, &color);
  	gtk_widget_show(add_option_label_error);
	
	GtkWidget* sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	GtkWidget* add_option_add_button = gtk_button_new_with_label("Add");
	gtk_box_pack_start(GTK_BOX(sub_box), add_option_add_button, TRUE, TRUE, 0);
	g_signal_connect(add_option_add_button, "clicked", G_CALLBACK(cb_add_option_add), NULL);
	gtk_widget_show(add_option_add_button);
	
	GtkWidget* add_option_cancel_button = gtk_button_new_with_label("Cancel");
	gtk_box_pack_start(GTK_BOX(sub_box), add_option_cancel_button, FALSE, FALSE, 0);
	gtk_widget_set_size_request(add_option_cancel_button, 100, -1);
	g_signal_connect(add_option_cancel_button, "clicked", G_CALLBACK(cb_add_option_cancel), NULL);
	gtk_widget_show(add_option_cancel_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
}

static void init_edit_notes_window()
{
	std::cout << "init_edit_notes_window\n";
	edit_notes_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(edit_notes_window), GTK_WINDOW(main_window));
	gtk_window_set_title(GTK_WINDOW(edit_notes_window), "Edit notes");
	gtk_container_set_border_width(GTK_CONTAINER(edit_notes_window), 10);
	gtk_window_set_position(GTK_WINDOW(edit_notes_window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_deletable(GTK_WINDOW(edit_notes_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(edit_notes_window), layout_box);
	
	GtkWidget *frame = gtk_frame_new("Notes:");
	gtk_box_pack_start(GTK_BOX(layout_box), frame, FALSE, FALSE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
	
	edit_notes_text_view = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(frame), edit_notes_text_view);
	gtk_widget_show(edit_notes_text_view);
	
	gtk_widget_show(frame);
	
	edit_notes_label_error = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(layout_box), edit_notes_label_error, FALSE, FALSE, 0);
	
	GdkColor color;
  	gdk_color_parse ("red", &color);
  	gtk_widget_modify_fg(edit_notes_label_error, GTK_STATE_NORMAL, &color);
	gtk_widget_show(edit_notes_label_error);
	
	GtkWidget* sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	GtkWidget* save_button = gtk_button_new_with_label("Save");
	gtk_box_pack_start(GTK_BOX(sub_box), save_button, TRUE, TRUE, 0);
	g_signal_connect(save_button, "clicked", G_CALLBACK(cb_edit_notes_save), NULL);
	gtk_widget_show(save_button);
	
	GtkWidget* cancel_button = gtk_button_new_with_label("Cancel");
	gtk_box_pack_start(GTK_BOX(sub_box), cancel_button, FALSE, FALSE, 0);
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
		// Remove all of the old fields
		GList *children, *iter;

		children = gtk_container_get_children(GTK_CONTAINER(fields_parent_box));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
			gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);
		children = NULL;
		iter = NULL;
	}
	
	std::list<std::string> field_order = naming_convention.get_field_order();
	for(std::list<std::string>::iterator field_it = field_order.begin();
			field_it != field_order.end(); ++field_it)
	{
		GtkWidget* field_box = gtk_vbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(fields_parent_box), field_box, TRUE, TRUE, 0);
		
		std::string field_code = *field_it;
		gtk_widget_set_name(field_box, field_code.c_str());
		
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
				option_radio_button = gtk_radio_button_new_with_label(NULL, option_name.c_str());
			}
			else
			{
				option_radio_button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(option_radio_button), option_name.c_str());
			}
			
			if(option_code == selected_option)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option_radio_button), TRUE);
			}
			
			gtk_box_pack_start(GTK_BOX(field_box), option_radio_button, FALSE, FALSE, 0);
			
			gtk_widget_set_name(option_radio_button, option_code.c_str());
			g_signal_connect(option_radio_button, "toggled", G_CALLBACK(cb_field_selection_changed), NULL);
			
			gtk_widget_show(option_radio_button);
		}
		
		GtkWidget* add_option_button = gtk_button_new_with_label("Add...");
		gtk_widget_set_name(add_option_button, field_code.c_str());
		gtk_widget_set_size_request(add_option_button, 100, -1);
		g_signal_connect(add_option_button, "clicked", G_CALLBACK(cb_add_new_option), NULL);
		
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
	gtk_table_resize(GTK_TABLE(capture_table), ++capture_table_current_rows, COLS_COUNT);
	insert_capture_into_table_row(capture, capture_table_current_rows - 1);
}

static void insert_capture_into_table_row(DataCapture capture, int row)
{
	GtkWidget *label = gtk_label_new(capture.name.c_str());
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);
	
	label = gtk_label_new(capture.get_date_string().c_str());
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);
	
	label = gtk_label_new(capture.get_time_string().c_str());
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);
	
	std::stringstream ss;
	ss << std::setprecision(3) << capture.duration << " s";
	
	label = gtk_label_new(ss.str().c_str());
	gtk_misc_set_alignment(GTK_MISC(label), 1, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);
	
	ss.str("");
	ss << capture.size << " MB";
	
	label = gtk_label_new(ss.str().c_str());
	gtk_misc_set_alignment(GTK_MISC(label), 1, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);
	
	label = gtk_label_new(capture.notes.c_str());
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);
	notes_labels.push_back(label);
	
	GtkWidget *edit_notes_button = gtk_button_new_with_label("Edit...");
	gtk_widget_show(edit_notes_button);
	g_signal_connect(edit_notes_button, "clicked", G_CALLBACK(cb_edit_notes), (gpointer)(row - 1));
	
	GtkWidget *align = gtk_alignment_new(0, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(align), edit_notes_button);
	gtk_widget_show(align);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), align, COL_EDIT_NOTES, COL_EDIT_NOTES + 1, row, row + 1);
}



static gboolean cb_destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
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
	
	current_capture.name = naming_convention.name();
	gtk_label_set_text(GTK_LABEL(current_capture_name), current_capture.name.c_str());
	gtk_label_set_text(GTK_LABEL(current_capture_status), "ready to capture");
	gtk_widget_show(current_capture_box);
	
	gtk_widget_set_sensitive(start_button, TRUE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, FALSE);
	gtk_widget_set_sensitive(reset_button, TRUE);
	gtk_widget_hide(new_capture_window);
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), FALSE);
}

static void cb_cancel_new_capture(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
	gtk_widget_hide(new_capture_window);
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), FALSE);
}

static void cb_add_new_option(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	std::string field_code = gtk_widget_get_name(widget);
	field_being_extended = field_code;
	gtk_label_set_text(GTK_LABEL(add_option_field_label), ("  " + naming_convention.get_field_name(field_being_extended)).c_str());
	gtk_entry_set_text(GTK_ENTRY(add_option_entry_code), "");
	gtk_entry_set_text(GTK_ENTRY(add_option_entry_name), "");
	
	gtk_widget_show(add_option_window);
	gtk_window_set_modal(GTK_WINDOW(add_option_window), TRUE);
}

static void cb_field_selection_changed(GtkWidget *widget, gpointer data)
{
	if(!all_initialized)
	{
		return;
	}
	
	std::string field_code = gtk_widget_get_name(gtk_widget_get_parent(widget));
	std::string option_code = gtk_widget_get_name(widget);
	
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
		// here.
		return;	
	}
	
	naming_convention.add_option(field_being_extended, new_option_code, new_option_name);
	populate_fields_and_options();
	
	field_being_extended = "";
	gtk_widget_hide(add_option_window);
	gtk_window_set_modal(GTK_WINDOW(add_option_window), FALSE);
}

static void cb_edit_notes(GtkWidget *widget, gpointer data)
{
	long capture_index = (long)data;
	
	std::list<DataCapture>::iterator it = finished_captures.begin();
	std::advance(it, capture_index);
	
	DataCapture capture = *it;
	notes_being_edited = capture_index;
	
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit_notes_text_view)), capture.notes.c_str(), -1);
	gtk_label_set_text(GTK_LABEL(edit_notes_label_error), "");
	gtk_widget_show(edit_notes_window);
	gtk_window_set_modal(GTK_WINDOW(edit_notes_window), TRUE);
}

static void cb_edit_notes_save(GtkWidget *widget, gpointer data)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(edit_notes_text_view));
	GtkTextIter start_iter, end_iter;
	gtk_text_buffer_get_start_iter(buffer, &start_iter);
	gtk_text_buffer_get_end_iter(buffer, &end_iter);
	std::string new_notes = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
	
	if(new_notes.find('`') != std::string::npos)
	{
		gtk_label_set_text(GTK_LABEL(edit_notes_label_error), "Backquote (`) character not allowed");
		return;
	}
	
	std::list<DataCapture>::iterator it = finished_captures.begin();
	std::advance(it, notes_being_edited);
	
	(*it).notes = new_notes;
	(*it).write_to_file();
	
	gtk_label_set_text(GTK_LABEL(notes_labels[notes_being_edited]), new_notes.c_str());
	
	notes_being_edited = -1;
	gtk_widget_hide(edit_notes_window);
	gtk_window_set_modal(GTK_WINDOW(edit_notes_window), FALSE);
}
static void cb_edit_notes_cancel(GtkWidget *widget, gpointer data)
{
	notes_being_edited = -1;
	gtk_widget_hide(edit_notes_window);
	gtk_window_set_modal(GTK_WINDOW(edit_notes_window), FALSE);
}

static bool validate_new_option_and_trim(std::string &new_option_code, std::string &new_option_name)
{
	if(new_option_code == "" || new_option_name == "" || new_option_code.find_first_not_of(' ') == -1
			|| new_option_name.find_first_not_of(' ') == -1)
	{
		std::cout << "empty error\n";
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
		std::cout << "space in code error\n";
		gtk_label_set_text(GTK_LABEL(add_option_label_error), "No spaces allowed in code");
		return FALSE;
	}
	
	std::map<std::string, std::string> current_options = naming_convention.get_options(field_being_extended);
	for(std::map<std::string, std::string>::iterator option_it = current_options.begin();
			option_it != current_options.end(); ++option_it)
	{
		std::string option_code = option_it->first;
		std::string option_name = option_it->second;
		
		if(option_code == new_option_code)
		{
			std::cout << "duplicate code error\n";
			gtk_label_set_text(GTK_LABEL(add_option_label_error), ("This code already in use for " + option_name).c_str());
			return FALSE;
		}
		else if(option_name == new_option_name)
		{
			std::cout << "duplicate name error\n";
			gtk_label_set_text(GTK_LABEL(add_option_label_error), "This name already in use");
			return FALSE;
		}
	}
	gtk_label_set_text(GTK_LABEL(add_option_label_error), "");
	return TRUE;
}

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
		DIR *directory_pointer = NULL;
		struct dirent *item = NULL;
	
		directory_pointer = opendir(DATA_FOLDER.c_str());
		if (directory_pointer)
		{
			while ((item = readdir(directory_pointer)) != NULL)
			{
				std::string filename = item->d_name;
				int found_index = filename.find(CAPTURE_META_ENDING);
				int expected_index = filename.size() - CAPTURE_META_ENDING.size();
				if(found_index != -1 && found_index == expected_index)
				{
					capture_files.push_back(DATA_FOLDER + "/" + filename);
				}
			}
			closedir(directory_pointer);
		}
		directory_pointer = NULL;
	}
	
	DataCapture capture;
	for(std::list<std::string>::iterator it = capture_files.begin(); it != capture_files.end(); ++it)
	{
		capture.read_from_file(*it);
		finished_captures.push_back(capture);
	}
	finished_captures.sort(compare_captures);
	
}
