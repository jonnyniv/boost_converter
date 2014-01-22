#include <gtk/gtk.h>

GdkPixbuf *create_pixbuf(const gchar * filename)
{
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error); //Creates icon from file
	if (!pixbuf)
	{
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	
	return pixbuf;
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *slider;
	
	
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Icon");
	gtk_window_set_default_size(GTK_WINDOW(window), 230,150);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("icon.png")); 
	//creates icon from file, has to be in the same directie as the exe also good if its a png
	
	
	slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 15.0, 0.1 );
	gtk_scale_set_digits(GTK_SCALE(slider), 1);
	gtk_scale_set_draw_value(GTK_SCALE(slider), TRUE);
	
	gtk_scale_add_mark(GTK_SCALE(slider), 0, GTK_POS_TOP, "0");
	gtk_scale_add_mark(GTK_SCALE(slider), 3.3, GTK_POS_TOP, "3.3");
	gtk_scale_add_mark(GTK_SCALE(slider), 5, GTK_POS_TOP, "5");
	gtk_scale_add_mark(GTK_SCALE(slider), 9, GTK_POS_TOP, "9");
	gtk_scale_add_mark(GTK_SCALE(slider), 12, GTK_POS_TOP, "12");
	gtk_scale_add_mark(GTK_SCALE(slider), 15, GTK_POS_TOP, "15");
	
	gtk_container_add(GTK_CONTAINER(window), slider);
	
	gtk_widget_show(window);
	
	g_signal_connect_swapped(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_main();
	
	return 0;
}
