#include <gtk/gtk.h>

gdouble scalev, adc = 0, v_out;
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
	GtkWidget *window;
	GtkWidget *frame;
	GtkWidget *plus;
	GtkWidget *minus;
	GtkWidget *table;
	GtkWidget *label, *label1, *label2, *label3, *label4, *label5, *label6, *label7, *label8;	
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
	
	
	label = gtk_label_new("Voltage Setpoint");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1 );
	
	label1 = gtk_label_new("ADC Value");
	gtk_table_attach_defaults(GTK_TABLE(table), label1, 0, 1, 1, 2 );
	label2 = gtk_label_new("Output Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), label2, 0, 1, 2, 3 );
	label3 = gtk_label_new("Output Current");
	gtk_table_attach_defaults(GTK_TABLE(table), label3, 0, 1, 3, 4 );
	label4 = gtk_label_new("Input Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), label4, 0, 1, 4, 5 );
	
	scale = gtk_hscale_new_with_range( 0, 15, 0.1);
	gtk_table_attach_defaults(GTK_TABLE(table), scale, 1, 4, 0, 1 );
	
	sprintf(buff, "%3.1f", adc);
	gtk_label_set_text(GTK_LABEL(label), buff);
	label5 = gtk_label_new(buff);
	gtk_table_attach_defaults(GTK_TABLE(table), label5, 1, 4, 1, 2);
	v_out = adc *
	label6 = gtk_label_new(buff);
	gtk_table_attach_defaults(GTK_TABLE(table), label6, 1, 4, 1, 2);
	label7 = gtk_label_new(buff);
	gtk_table_attach_defaults(GTK_TABLE(table), label7, 1, 4, 1, 2);
	label8 = gtk_label_new(buff);
	gtk_table_attach_defaults(GTK_TABLE(table), label8, 1, 4, 1, 2);
	
	gtk_container_add(GTK_CONTAINER(window), table);
	
	gtk_widget_show_all(window);
	
	g_signal_connect (G_OBJECT(scale), "value-changed", G_CALLBACK (scale_mov), label5);
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	
	gtk_main();
	
	return 0;
}

	