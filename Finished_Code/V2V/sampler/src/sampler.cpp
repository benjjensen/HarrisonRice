#include "NamingConvention.h"
#include <gtk/gtk.h>
#include <iostream>

const unsigned int START_BUTTON_INDEX = 0;
const unsigned int STOP_BUTTON_INDEX = 1;
const unsigned int NEW_BUTTON_INDEX = 2;
const unsigned int RESET_BUTTON_INDEX = 3;
const unsigned int BUTTON_COUNT = 4;

NamingConvention *naming_convention = NULL;

GtkWidget *main_window = NULL;
GtkWidget *capture_table = NULL;
GtkWidget *start_button = NULL;
GtkWidget *stop_button = NULL;
GtkWidget *new_button = NULL;
GtkWidget *reset_button = NULL;

GtkWidget *new_capture_window = NULL;
GtkWidget *fields_parent_box = NULL;
//std::list<GtkWidget*> *option_buttons;

GtkWidget *add_option_window = NULL;
GtkWidget *add_option_field_label = NULL;
GtkWidget *add_option_entry_code = NULL;
GtkWidget *add_option_entry_name = NULL;
GtkWidget *add_option_label_error = NULL;

bool all_initialized = false;

std::string field_being_extended = "";

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



int main(int argc, char **argv)
{
	std::cout << "main\n";
	
	naming_convention = new NamingConvention;
	field_being_extended = "";
	
	gtk_init(&argc, &argv);
	
	init_main_window();
	init_new_capture_window();
	init_add_option_window();
	
	all_initialized = TRUE;
	
	gtk_widget_show(main_window);
	gtk_main();
	
	delete naming_convention;
	
	return 0;
}



static void init_main_window()
{	
	std::cout << "init_main_window\n";
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Sampler");
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy), NULL);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), layout_box);
	
	capture_table = gtk_table_new(1, 4, FALSE);
	gtk_box_pack_start (GTK_BOX(layout_box), capture_table, TRUE, TRUE, 0);
	
	// TODO fix magic numbers here
	GtkWidget *label = gtk_label_new("Name");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, 0, 1, 0, 1);
	label = gtk_label_new("Time");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, 1, 2, 0, 1);
	label = gtk_label_new("Duration");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, 2, 3, 0, 1);
	label = gtk_label_new("Size");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(capture_table), label, 3, 4, 0, 1);
	
	// TODO captures...
	
	gtk_widget_show(capture_table);
	
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	start_button = gtk_button_new_with_label("Start");
	stop_button = gtk_button_new_with_label("Stop");
	reset_button = gtk_button_new_with_label("Reset");
	new_button = gtk_button_new_with_label("New Capture");

	gtk_box_pack_start(GTK_BOX(sub_box), start_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(start_button, FALSE);
	g_signal_connect (start_button, "clicked",
                      G_CALLBACK (cb_start_capture), NULL);
	gtk_widget_show(start_button);
	
	gtk_box_pack_start(GTK_BOX(sub_box), stop_button, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(stop_button, FALSE);
	g_signal_connect (stop_button, "clicked",
                      G_CALLBACK (cb_stop_capture), NULL);
	gtk_widget_show(stop_button);
	
	gtk_widget_show(sub_box);
	
	sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(sub_box), new_button, TRUE, TRUE, 0);
	g_signal_connect (new_button, "clicked",
                      G_CALLBACK (cb_new_capture), NULL);
	gtk_widget_show(new_button);
	
	gtk_box_pack_start(GTK_BOX(sub_box), reset_button, FALSE, FALSE, 0);
	gtk_widget_set_sensitive(reset_button, FALSE);
	g_signal_connect (reset_button, "clicked",
                      G_CALLBACK (cb_reset_capture), NULL);
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
	g_signal_connect(create_new_capture_button, "clicked", G_CALLBACK (cb_create_new_capture), NULL);
	gtk_widget_show(create_new_capture_button);
	
	gtk_box_pack_start(GTK_BOX(sub_box), cancel_new_capture_button, FALSE, FALSE, 0);
	g_signal_connect(cancel_new_capture_button, "clicked", G_CALLBACK (cb_cancel_new_capture), NULL);
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
		std::cerr << "ERROR: populate_fields_and_options() called before fields_parent_box was set.\n";
	}
	
	{
		// Remove all of the old fields
		GList *children, *iter;

		children = gtk_container_get_children(GTK_CONTAINER(fields_parent_box));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
			gtk_container_remove(GTK_CONTAINER(fields_parent_box), (GtkWidget*)iter->data);
			//gtk_widget_destroy(GTK_WIDGET(iter->data));
		//g_list_free(children);
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
				//std::cout << "Before set_active: " << option_code << std::endl;
				//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option_radio_button), TRUE);
				//std::cout << "After set_active: " << option_code << std::endl;
			}
			
			gtk_box_pack_start(GTK_BOX(field_box), option_radio_button, FALSE, FALSE, 0);
			
			gtk_widget_set_name(option_radio_button, option_code.c_str());
			//g_signal_connect(option_radio_button, "toggled", G_CALLBACK(cb_field_selection_changed), NULL);
			
			gtk_widget_show(option_radio_button);
		}
		
		for(int i = 0; i < 1000; ++i)
		{
			std::cout << i << std::endl;
			std::string code("l"), name("P");
			// For some reason, this always fails on the 130th execution of this loop:
			naming_convention->set_field_value(code, name);
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
	// TODO actually start capturing
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
	// TODO stop capturing
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
	// TODO cancel capture (reset current capture variable?)
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
	// TODO actually create the new capture
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
	
	std::cout << "cb_field_selection_changed\n";
	
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
