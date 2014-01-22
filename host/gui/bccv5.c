#include <gtk/gtk.h>

gfloat count = 0;
char buff[5];

void add(GtkWidget *widget, gpointer label)
{
	if (count < 15)
	{
		count += 0.1;
	}
	
	sprintf(buff, "%3.1f", count);
	gtk_label_set_text(GTK_LABEL(label), buff);
}

void subtract(GtkWidget *widget, gpointer label)
{
	if(count >= 0.1)
	{
		count -= 0.1;
	}
	
	sprintf(buff, "%3.1f", count);
	gtk_label_set_text(GTK_LABEL(label), buff);
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
	
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 250,200);
	gtk_window_set_title(GTK_WINDOW(window), "Boost Converter Controller");
	
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
	
	table = gtk_table_new(5, 4, FALSE); //creates a 4x4 widget,  non homogenous 
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);//sets spcaing between rows and columns
	
	plus = gtk_button_new_with_label("+");
	gtk_widget_set_size_request(plus, 30, 30);
	gtk_table_attach_defaults(GTK_TABLE(table), plus, 3, 4, 0, 1 );
	
	minus = gtk_button_new_with_label("-");
	gtk_widget_set_size_request(minus, 30, 30);
	gtk_table_attach_defaults(GTK_TABLE(table), minus, 1, 2, 0, 1 );
	
	label = gtk_label_new("0");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 0, 1 );
	
	setlab = gtk_label_new("Voltage Setpoint");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab, 0, 1, 0, 1 );
	
	setlab1 = gtk_label_new("Output Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab1, 0, 1, 1, 2 );
	setlab2 = gtk_label_new("Diode Current");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab2, 0, 1, 2, 3 );
	setlab3 = gtk_label_new("Input Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab3, 0, 1, 3, 4 );
	setlab4 = gtk_label_new("Inductor Current");
	gtk_table_attach_defaults(GTK_TABLE(table), setlab4, 0, 1, 4, 5 );
	
	gtk_container_add(GTK_CONTAINER(window), table);
	
	gtk_widget_show_all(window);
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	g_signal_connect(G_OBJECT(plus), "clicked", G_CALLBACK(add), label); //links the button to the function at the top
	
	g_signal_connect(G_OBJECT(minus), "clicked", G_CALLBACK(subtract), label);
	
	gtk_main();
	
	return 0;
}

	