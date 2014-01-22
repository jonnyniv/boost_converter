#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *slider;
	
	
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "BCC");
	gtk_window_set_default_size(GTK_WINDOW(window), 300,300);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	
	
	slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 15.0, 0.1 );
	gtk_scale_set_digits(GTK_SCALE(slider), 1);
	gtk_scale_set_draw_value(GTK_SCALE(slider), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(slider), GTK_POS_TOP);
	

	
	gtk_container_add(GTK_CONTAINER(window), slider);
	
	gtk_widget_show(window);
	
	g_signal_connect_swapped(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_main();
	
	return 0;
}
