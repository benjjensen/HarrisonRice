#include "NamingConvention.h"
#include <gtk/gtk.h>
#include <iostream>

const unsigned int START_BUTTON_INDEX = 0;
const unsigned int STOP_BUTTON_INDEX = 1;
const unsigned int NEW_BUTTON_INDEX = 2;
const unsigned int RESET_BUTTON_INDEX = 3;
const unsigned int BUTTON_COUNT = 4;

NamingConvention *naming_convention;

GtkWidget *main_window;
GtkWidget *capture_table;

GtkWidget *start_button, *stop_button, *new_button, *reset_button;

GtkWidget *new_capture_window;

GtkWidget *add_option_window;
std::string field_being_extended;

static gboolean cb_destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

static void cb_start_capture(GtkWidget *widget, gpointer data)
{
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, TRUE);
	gtk_widget_set_sensitive(new_button, FALSE);
	gtk_widget_set_sensitive(reset_button, FALSE);
}

static void cb_stop_capture(GtkWidget *widget, gpointer data)
{
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
}

static void cb_new_capture(GtkWidget *widget, gpointer data)
{
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), TRUE);
	gtk_widget_show(new_capture_window);
	// TODO Make main_window unfocusable
}

static void cb_reset_capture(GtkWidget *widget, gpointer data)
{
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
}

static void cb_create_new_capture(GtkWidget *widget, gpointer data)
{
	gtk_widget_set_sensitive(start_button, TRUE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, FALSE);
	gtk_widget_set_sensitive(reset_button, TRUE);
	gtk_widget_hide(new_capture_window);
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), FALSE);
}

static void cb_cancel_new_capture(GtkWidget *widget, gpointer data)
{
	gtk_widget_set_sensitive(start_button, FALSE);
	gtk_widget_set_sensitive(stop_button, FALSE);
	gtk_widget_set_sensitive(new_button, TRUE);
	gtk_widget_set_sensitive(reset_button, FALSE);
	gtk_widget_hide(new_capture_window);
	gtk_window_set_modal(GTK_WINDOW(new_capture_window), FALSE);
}

static void cb_add_new_option(GtkWidget* widget, gpointer data)
{
	std::string field_code = gtk_widget_get_name(widget);
	std::cout << "Adding new option for field " << field_code << std::endl;
	field_being_extended = field_code;
	
	// gtk_widget_show(add_option_window);
	// TODO Make new_capture_window unfocusable
}

void init_main_window()
{	
	std::cout << "init_main_window\n";
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Sampler");
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy), NULL);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), layout_box);
	
	capture_table = gtk_table_new(1, 4, FALSE);
	gtk_box_pack_start (GTK_BOX(layout_box), capture_table, TRUE, TRUE, 0);
	
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

void init_new_capture_window()
{	
	std::cout << "init_new_capture_window\n";
	new_capture_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(new_capture_window), GTK_WINDOW(main_window));
	gtk_window_set_title(GTK_WINDOW(new_capture_window), "New Capture");
	gtk_window_set_deletable(GTK_WINDOW(new_capture_window), FALSE);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(new_capture_window), layout_box);
	
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, TRUE, TRUE, 0);
	
	std::list<std::string> field_order = naming_convention->get_field_order();
	for(std::list<std::string>::iterator field_it = field_order.begin();
			field_it != field_order.end(); ++field_it)
	{
		GtkWidget* field_box = gtk_vbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(sub_box), field_box, TRUE, TRUE, 0);
		
		std::string field_code = *field_it;
		gtk_widget_set_name(field_box, field_code.c_str());
		
		GtkWidget* field_label = gtk_label_new(naming_convention->get_field_name(field_code).c_str());
		gtk_box_pack_start(GTK_BOX(field_box), field_label, FALSE, FALSE, 0);
		gtk_widget_show(field_label);
		
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
			
			gtk_box_pack_start(GTK_BOX(field_box), option_radio_button, FALSE, FALSE, 0);
			
			gtk_widget_set_name(option_radio_button, option_code.c_str());
			
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
	
	gtk_widget_show(sub_box);
	
	sub_box = gtk_hbox_new(FALSE, 0);
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

int main(int argc, char **argv)
{
	std::cout << "main\n";
	
	naming_convention = new NamingConvention;
	
	field_being_extended = "";
	gtk_init(&argc, &argv);
	
	init_main_window();
	
	init_new_capture_window();
	
	gtk_widget_show(main_window);
	gtk_main();
	
	delete naming_convention;
	
	return 0;
}
