#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <cstdlib>
#include <dirent.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <ext/stdio_filebuf.h>

#include <gtk/gtk.h>

#include "NamingConvention.h"
#include "DataCapture.h"

const unsigned int COLS_COUNT = 6;
const unsigned int COL_NAME = 0;
const unsigned int COL_DATE = 1;
const unsigned int COL_TIME = 2;
const unsigned int COL_DURATION = 3;
const unsigned int COL_SIZE = 4;
const unsigned int COL_NOTES = 5;

const std::string DATA_FOLDER = "../acquire/data";
const std::string CAPTURE_META_ENDING = "-meta.txt";

NamingConvention *naming_convention = NULL;

GtkWidget *main_window = NULL;
GtkWidget *capture_table = NULL;
size_t capture_table_current_rows = 0;

GtkWidget *current_capture_box = NULL;
GtkWidget *current_capture_name = NULL;
GtkWidget *current_capture_status = NULL;

DataCapture current_capture;
std::list<DataCapture> finished_captures;

GtkWidget *start_button = NULL;
GtkWidget *stop_button = NULL;
GtkWidget *new_button = NULL;
GtkWidget *reset_button = NULL;

GtkWidget *new_capture_window = NULL;
GtkWidget *fields_parent_box = NULL;

GtkWidget *add_option_window = NULL;
GtkWidget *add_option_field_label = NULL;
GtkWidget *add_option_entry_code = NULL;
GtkWidget *add_option_entry_name = NULL;
GtkWidget *add_option_label_error = NULL;

bool all_initialized = false;

std::string field_being_extended = "";

int acquire_process_id = -1;
int acquire_output_fd = -1;

static gboolean cb_destroy(GtkWidget *widget, GdkEvent *event, gpointer data);
static void cb_start_capture(GtkWidget *widget, gpointer data);
static void cb_stop_capture(GtkWidget *widget, gpointer data);
static void cb_new_capture(GtkWidget *widget, gpointer data);
static void cb_reset_capture(GtkWidget *widget, gpointer data);
static void cb_create_new_capture(GtkWidget *widget, gpointer data);
static void cb_cancel_new_capture(GtkWidget *widget, gpointer data);
static void cb_add_new_option(GtkWidget* widget, gpointer data);
static void cb_add_option_cancel(GtkWidget* widget, gpointer data);
static void cb_add_option_add(GtkWidget* widget, gpointer data);
static void cb_field_selection_changed(GtkWidget *widget, gpointer data);

static bool validate_new_option_and_trim(std::string &new_option_code, std::string &new_option_name);

static void init_main_window();
static void init_new_capture_window();
static void init_add_option_window();

static void populate_fields_and_options();
static void insert_capture_into_table_row(DataCapture capture, int row);
static void insert_capture_into_table(DataCapture capture);
static void load_all_captures_from_files();

static bool start_acquire_in_child_process();
static bool stop_acquiring();



int main(int argc, char **argv)
{
	std::cout << "main\n";
	
	naming_convention = new NamingConvention;
	field_being_extended = "";
	
	gtk_init(&argc, &argv);
	
	load_all_captures_from_files();
	
	init_main_window();
	init_new_capture_window();
	init_add_option_window();
	
	all_initialized = TRUE;
	
	gtk_widget_show(main_window);
	gtk_main();
	
	delete naming_convention;
	
	return 0;
}



static bool start_acquire_in_child_process()
{
	const int READ_FD = 0;
	const int WRITE_FD = 1;
	
	//const in
	
	int       parentToChild[2];
	int       childToParent[2];
	pid_t     pid;
	//string    dataReadFromChild;
	//char      buffer[ BUFFER_SIZE + 1 ];
	//ssize_t   readResult;
	int       status;
	
	//ASSERT_IS(0, 
	pipe(parentToChild);//);
	//ASSERT_IS(0, 
	pipe(childToParent);//);
	
	switch ( pid = fork() )
	{
	case -1:
		std::cerr << "ERROR: failure to fork for acquire" << std::endl;
		return FALSE;

	case 0: // Child
		//ASSERT_NOT(-1, 
		dup2( parentToChild[ READ_FD  ], STDIN_FILENO  );// );
		//ASSERT_NOT(-1, 
		dup2( childToParent[ WRITE_FD ], STDOUT_FILENO );// );
		//ASSERT_NOT(-1, 
		//dup2( childToParent[ WRITE_FD ], STDERR_FILENO );// );
		//ASSERT_IS(  0, 
		close( parentToChild[ WRITE_FD ] );// );
		//ASSERT_IS(  0, 
		close( childToParent[ READ_FD  ] );// );
		
		// Not necessary to change directories because acquire is in the same directory as this
		// chdir("../acquire");
		execlp("acquire", "acquire", "20", "-scm", "-scs", "0", NULL);

		std::cerr << "ERROR: this line should never be reached!" << std::endl;
		std::exit(-1);


	default: // Parent
		break;
	}
	std::cout << "Child " << pid << " process running..." << std::endl;
	
	if(close(parentToChild[READ_FD]) != 0)
	{
		std::cerr << "ERROR: failed to close parentToChild read" << std::endl;
	}
	if(close(childToParent[WRITE_FD]) != 0)
	{
		std::cerr << "ERROR: failed to close childToParent write" << std::endl;
	}
	//ASSERT_IS(  0, close( parentToChild [ READ_FD  ] ) );
	//ASSERT_IS(  0, close( childToParent [ WRITE_FD ] ) );
	
	acquire_process_id = pid;
	acquire_output_fd = childToParent[READ_FD];
	
	return true;
}

static bool stop_acquiring()
{
	__gnu_cxx::stdio_filebuf<char> *sb = new __gnu_cxx::stdio_filebuf<char>(acquire_output_fd, std::ios::in);
	std::istream input_from_acquire(sb);
	
	kill(acquire_process_id, SIGINT);
	
	current_capture.read_from_stream(input_from_acquire);
	finished_captures.push_back(current_capture);
	insert_capture_into_table(current_capture);
	
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
	// TODO put the table inside something that scrolls for when it gets too large
	std::cout << "init_main_window\n";
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Sampler");
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy), NULL);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), layout_box);
	
	capture_table_current_rows = 1;
	capture_table = gtk_table_new(capture_table_current_rows, COLS_COUNT, FALSE);
	gtk_box_pack_start (GTK_BOX(layout_box), capture_table, TRUE, TRUE, 0);
	
	int row = 0;
	
	GtkWidget *label = gtk_label_new("Name");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);
	label = gtk_label_new("Date");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);
	label = gtk_label_new("Time");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);
	label = gtk_label_new("Duration");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);
	label = gtk_label_new("Size");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);
	label = gtk_label_new("Notes");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);
	
	for(std::list<DataCapture>::iterator it = finished_captures.begin(); it != finished_captures.end(); ++it)
	{
		insert_capture_into_table(*it);
	}
	
	gtk_widget_show(capture_table);
	
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
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	start_button = gtk_button_new_with_label("Start");
	stop_button = gtk_button_new_with_label("Stop");
	reset_button = gtk_button_new_with_label("Reset");
	new_button = gtk_button_new_with_label("New Capture");

	gtk_box_pack_start(GTK_BOX(sub_box), start_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(start_button, FALSE);
	g_signal_connect(start_button, "clicked", G_CALLBACK(cb_start_capture), NULL);
	gtk_widget_show(start_button);
	
	gtk_box_pack_start(GTK_BOX(sub_box), stop_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(stop_button, FALSE);
	g_signal_connect (stop_button, "clicked", G_CALLBACK(cb_stop_capture), NULL);
	gtk_widget_show(stop_button);
	
	gtk_widget_show(sub_box);
	
	sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(sub_box), new_button, TRUE, TRUE, 0);
	g_signal_connect (new_button, "clicked", G_CALLBACK(cb_new_capture), NULL);
	gtk_widget_show(new_button);
	
	gtk_box_pack_start(GTK_BOX(sub_box), reset_button, FALSE, FALSE, 0);
	gtk_widget_set_sensitive(reset_button, FALSE);
	g_signal_connect (reset_button, "clicked", G_CALLBACK(cb_reset_capture), NULL);
	gtk_widget_show(reset_button);
	
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
	gtk_window_set_deletable(GTK_WINDOW(new_capture_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(new_capture_window), layout_box);
	
	fields_parent_box = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(layout_box), fields_parent_box, TRUE, TRUE, 0);
	populate_fields_and_options();
	
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	GtkWidget* create_new_capture_button = gtk_button_new_with_label("Prepare");
	GtkWidget* cancel_new_capture_button = gtk_button_new_with_label("Cancel");
	
	gtk_box_pack_start(GTK_BOX(sub_box), create_new_capture_button, TRUE, TRUE, 0);
	g_signal_connect(create_new_capture_button, "clicked", G_CALLBACK(cb_create_new_capture), NULL);
	gtk_widget_show(create_new_capture_button);
	
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
	g_signal_connect(add_option_cancel_button, "clicked", G_CALLBACK(cb_add_option_cancel), NULL);
	gtk_widget_show(add_option_cancel_button);
	
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
	
	std::list<std::string> field_order = naming_convention->get_field_order();
	for(std::list<std::string>::iterator field_it = field_order.begin();
			field_it != field_order.end(); ++field_it)
	{
		GtkWidget* field_box = gtk_vbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(fields_parent_box), field_box, TRUE, TRUE, 0);
		
		std::string field_code = *field_it;
		gtk_widget_set_name(field_box, field_code.c_str());
		
		GtkWidget* field_label = gtk_label_new(naming_convention->get_field_name(field_code).c_str());
		gtk_box_pack_start(GTK_BOX(field_box), field_label, FALSE, FALSE, 0);
		gtk_widget_show(field_label);
		
		std::string selected_option = naming_convention->get_field_value(field_code);
		
		GtkWidget* option_radio_button = NULL;
		std::map<std::string, std::string> options = naming_convention->get_options(field_code);
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
		g_signal_connect(add_option_button, "clicked", G_CALLBACK(cb_add_new_option), NULL);
		gtk_box_pack_end(GTK_BOX(field_box), add_option_button, FALSE, FALSE, 0);
		gtk_widget_show(add_option_button);
		// TODO Stop add_option_button from expanding horizontally.
		
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
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NAME, COL_NAME + 1, row, row + 1);
	
	label = gtk_label_new(capture.get_date_string().c_str());
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DATE, COL_DATE + 1, row, row + 1);
	
	label = gtk_label_new(capture.get_time_string().c_str());
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_TIME, COL_TIME + 1, row, row + 1);
	
	std::stringstream ss;
	ss << capture.duration << " s";
	
	label = gtk_label_new(ss.str().c_str());
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_DURATION, COL_DURATION + 1, row, row + 1);
	
	ss.str("");
	ss << capture.size << " MB";
	
	label = gtk_label_new(ss.str().c_str());
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_SIZE, COL_SIZE + 1, row, row + 1);
	
	label = gtk_label_new(capture.notes.c_str());
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, COL_NOTES, COL_NOTES + 1, row, row + 1);
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
	
	current_capture.name = naming_convention->name();
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
	gtk_label_set_text(GTK_LABEL(add_option_field_label), naming_convention->get_field_name(field_being_extended).c_str());
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
	
	naming_convention->set_field_value(field_code, option_code);
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
	
	naming_convention->add_option(field_being_extended, new_option_code, new_option_name);
	populate_fields_and_options();
	
	field_being_extended = "";
	gtk_widget_hide(add_option_window);
	gtk_window_set_modal(GTK_WINDOW(add_option_window), FALSE);
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
	
	std::map<std::string, std::string> current_options = naming_convention->get_options(field_being_extended);
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
}
