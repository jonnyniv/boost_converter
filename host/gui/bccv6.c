#include <gtk/gtk.h>

gdouble scalev;
char buff[5];

void scale_mov(GtkWidget *widget, gpointer label)
{
	scalev = gtk_range_get_value (widget);
	
	sprintf(buff, "%3.1f", scalev);
	gtk_label_set_text(GTK_LABEL(label), buff);
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
	GtkWidget *label;
	GtkWidget *window;
	GtkWidget *frame;
	GtkWidget *plus;
	GtkWidget *minus;
	GtkWidget *table;
	GtkWidget *setlab;
	GtkWidget *setlab1;
	GtkWidget *setlab2;
	GtkWidget *setlab3;
	GtkWidget *setlab4;
	GtkWidget *setlab5;	
	GtkWidget *scale;
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 300,200);
	gtk_window_set_title(GTK_WINDOW(window), "Boost Converter Controller");
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("icon.png")); 
	//creates icon from file, has to be in the same directie as the exe also good if its a png
	
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
	
	table = gtk_table_new(5, 4, FALSE); //creates a 4x4 widget,  non homogenous 
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);//sets spcaing between rows and columns
	
	
	setlab = gtk_label_new("Voltage Setpoint");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab, 0, 1, 0, 1 );
	
	setlab1 = gtk_label_new("ADC Value");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab1, 0, 1, 1, 2 );
	setlab2 = gtk_label_new("Output Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab2, 0, 1, 2, 3 );
	setlab3 = gtk_label_new("Output Current");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab3, 0, 1, 3, 4 );
	setlab4 = gtk_label_new("Input Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab4, 0, 1, 4, 5 );
	
	scale = gtk_hscale_new_with_range( 0, 15, 0.1);
	gtk_table_attach_defaults(GTK_TABLE(table), scale, 1, 4, 0, 1 );
	
	setlab5 = gtk_label_new("0");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab5, 1, 4, 1, 2);
	
	gtk_container_add(GTK_CONTAINER(window), table);
	
	gtk_widget_show_all(window);
	
	g_signal_connect (G_OBJECT(scale), "value-changed", G_CALLBACK (scale_mov), setlab5);
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	
	gtk_main();
	
	return 0;
}

	