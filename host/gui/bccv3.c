#include <gtk/gtk.h>
#include <string.h>

gdouble count = 0;
char counts[5];

void plus(GtkWidget *widget,  GtkWidget *label) 
{
	if (count < 15)
	 {count+=0.1; }
	 
	sprintf(counts, "%d", count);
	gtk_label_set_text(GTK_LABEL(label), counts);
}

void minus(GtkWidget *widget,  GtkWidget *label) 
{
	if(count < 0)
	{ count-=0.1; }
	
	sprintf(counts, "%d", count);
	gtk_label_set_text(GTK_LABEL(label), counts);
}
void plus_inactive(GtkWidget *widget,  gpointer item) 
{
	if (count >= 15)
	{
		gtk_widget_set_sensitive(widget, FALSE);
		gtk_widget_set_sensitive(item, TRUE);
	}
}

void minus_inactive(GtkWidget *widget,  gpointer item) 
{
	if (count <= 0)
	{
		gtk_widget_set_sensitive(widget, FALSE);
		gtk_widget_set_sensitive(item, TRUE);
	}
}

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

int main(int argc, char** argv)
{
	GtkWidget *window;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *plus;
	GtkWidget *minus;
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Icon");
	gtk_window_set_default_size(GTK_WINDOW(window), 500,500);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("icon.png")); 
	//creates icon from file, has to be in the same directie as the exe also good if its a png
	
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);

	table = gtk_table_new(5, 4, TRUE); //creates a 4x4 widget
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);//sets spcaing between rows and columns
	
	plus = gtk_button_new_with_label("+");
	minus = gtk_button_new_with_label("-");
	label = gtk_label_new("0");
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3 );
	gtk_table_attach_defaults(GTK_TABLE(table), plus, 0, 1, 3, 4 );
	gtk_table_attach_defaults(GTK_TABLE(table), minus, 0, 1, 1, 2 );
	
	gtk_widget_show(window);
	
	g_signal_connect(G_OBJECT(plus), "clicked", G_CALLBACK(plus), &label);

	g_signal_connect(G_OBJECT(minus), "clicked", G_CALLBACK(minus), &label);
	
	g_signal_connect(G_OBJECT(plus), "clicked", G_CALLBACK(plus_inactive), minus);

	g_signal_connect(G_OBJECT(minus), "clicked", G_CALLBACK(minus_inactive), plus);
	
	g_signal_connect_swapped(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_main();
	
	return 0;
}
