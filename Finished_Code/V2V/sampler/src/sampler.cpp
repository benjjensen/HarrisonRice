#include "NamingConvention.h"
#include <gtk/gtk.h>

static gboolean cb_destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	
	GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Sampler");
	g_signal_connect(main_window, "destroy", G_CALLBACK(cb_destroy), NULL);
	
	GtkWidget *layout_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), layout_box);
	
	GtkWidget *capture_table = gtk_table_new(1, 4, FALSE);
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
	
	gtk_widget_show(capture_table);
	
	GtkWidget *sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	GtkWidget *start_button = gtk_button_new_with_label("Start");
	gtk_box_pack_start(GTK_BOX(sub_box), start_button, TRUE, TRUE, 0);
	gtk_widget_show(start_button);
	
	GtkWidget *stop_button = gtk_button_new_with_label("Stop");
	gtk_box_pack_start(GTK_BOX(sub_box), stop_button, TRUE, TRUE, 0);
	gtk_widget_show(stop_button);
	
	gtk_widget_show(sub_box);
	
	sub_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(layout_box), sub_box, FALSE, FALSE, 0);
	
	GtkWidget *new_button = gtk_button_new_with_label("New Capture");
	gtk_box_pack_start(GTK_BOX(sub_box), new_button, TRUE, TRUE, 0);
	gtk_widget_show(new_button);
	
	GtkWidget *reset_button = gtk_button_new_with_label("Reset");
	gtk_box_pack_start(GTK_BOX(sub_box), reset_button, FALSE, FALSE, 0);
	gtk_widget_show(reset_button);
	
	gtk_widget_show(sub_box);
	
	gtk_widget_show(layout_box);
	gtk_widget_show(main_window);
	gtk_main();
	
	return 0;
}
