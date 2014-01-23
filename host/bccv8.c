#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "rs232_d1.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


#define BAUD_RATE 9600
#define SER_IN_BUFLEN 4096

unsigned char serbuf[SER_IN_BUFLEN];

gdouble setpoint, offset, v_value, v_out, i_out, adc, raw_v, load = 680;
gint com_no = 3,  read_v = 255, ready = 2;
char buff[10];

GtkWidget *label5, *label6, *label7;

void add(GtkWidget *widget, gpointer label)
{
	com_no ++;
	sprintf(buff, "%d", com_no);
	gtk_label_set_text(GTK_LABEL(label), buff);
}

void subtract(GtkWidget *widget, gpointer label)
{
	com_no --;

	sprintf(buff, "%d", com_no);
	gtk_label_set_text(GTK_LABEL(label), buff);
}

void init_com(GtkWidget *window)
{
	ready = OpenComport(com_no, BAUD_RATE);
}

void close_com(GtkWidget *window)
{
	CloseComport(com_no);
}


void scale_mov(GtkWidget *widget)
{
	v_value = gtk_range_get_value (widget);
}

void scale_mov2(GtkWidget *widget)
{
	offset = gtk_range_get_value (widget);
}

void send_setpoint(GtkWidget *widget)
{
	setpoint = v_value + offset;
	unsigned char set = (char)(setpoint*10);
	SendByte(com_no, set);	
}



static gboolean time_handler(GtkWidget *widget)
{
	//if (widget->window == NULL)  return FALSE;  //prevents the timer from running if the window has been closed. //removed because it causes compile error
	SendByte(com_no, read_v);
	
	PollComport(com_no, serbuf, SER_IN_BUFLEN);
	
	raw_v = (gdouble)serbuf[0];
	
	v_out = raw_v/10;
	i_out = v_out/load;
	adc = (v_out*0.1760299*1023)/3.3;
	
	sprintf(buff, "%3.0f", adc);
	gtk_label_set_text(GTK_LABEL(label5), buff);
	
	sprintf(buff, "%3.2f V", v_out);
	gtk_label_set_text(GTK_LABEL(label6), buff);
	
	sprintf(buff, "%3.3f A", i_out);
	gtk_label_set_text(GTK_LABEL(label7), buff);
	
	return TRUE; //alows the timer to run again
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
	GtkWidget *set, *close, *plus, *minus, *open, *exit;
	GtkWidget *table;
	GtkWidget *label, *label1, *label2, *label3, *label4, *label8, *label9, *label10, *label11;
	GtkWidget *scale, *scale2;
	
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 400,200);
	gtk_window_set_title(GTK_WINDOW(window), "Boost Converter Controller");
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("icon.png"));  //creates icon from file, has to be in the same directie as the exe also good if its a png
	
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
	
	table = gtk_table_new(7, 7, FALSE); //creates a 4x4 widget,  non homogenous 
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);//sets spcaing between rows and columns
	
	
	label = gtk_label_new("Voltage Setpoint");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2 );
	
	label1 = gtk_label_new("ADC Value");
	gtk_table_attach_defaults(GTK_TABLE(table), label1, 0, 1, 3, 4 );
	label2 = gtk_label_new("Output Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), label2, 0, 1, 4, 5 );
	label3 = gtk_label_new("Output Current");
	gtk_table_attach_defaults(GTK_TABLE(table), label3, 0, 1, 5, 6 );
	label4 = gtk_label_new("Input Voltage");
	gtk_table_attach_defaults(GTK_TABLE(table), label4, 0, 1, 6, 7 );
	label11 = gtk_label_new("Voltage Offset");
	gtk_table_attach_defaults(GTK_TABLE(table), label11, 0, 1, 2, 3 );
	
	scale = gtk_hscale_new_with_range( 2, 15, 0.1);	//Slider scale to set the voltage
	gtk_table_attach_defaults(GTK_TABLE(table), scale, 1, 5, 1, 2 );
	
	scale2 = gtk_hscale_new_with_range( -1.5, 1.5, 0.1);	//Slider scale to set the offset
	gtk_table_attach_defaults(GTK_TABLE(table), scale2, 1, 5, 2, 3 );
	
	label5 = gtk_label_new("0");	//ADC value
	gtk_table_attach_defaults(GTK_TABLE(table), label5, 1, 5, 3, 4);
	label6 = gtk_label_new("0");	//Output voltage
	gtk_table_attach_defaults(GTK_TABLE(table), label6, 1, 5, 4, 5);
	label7 = gtk_label_new("0");	//output current
	gtk_table_attach_defaults(GTK_TABLE(table), label7, 1, 5, 5, 6);
	label8 = gtk_label_new("1.5 v");	//input  voltage
	gtk_table_attach_defaults(GTK_TABLE(table), label8, 1, 5, 6, 7);
	
	set = gtk_button_new_with_label("Set"); 	//send voltage setpiont button
	gtk_widget_set_size_request(set, 40, 10);
	gtk_table_attach_defaults(GTK_TABLE(table), set, 5, 6, 1, 2);
	close = gtk_button_new_with_label("Exit"); 	//close windwow button
	gtk_widget_set_size_request(close, 40, 5);
	gtk_table_attach_defaults(GTK_TABLE(table), close, 5, 6, 6, 7);
	
	
	label9 = gtk_label_new("Comport Number");	//comport label
	gtk_table_attach_defaults(GTK_TABLE(table), label9, 0, 1, 0, 1);
	
	label10 = gtk_label_new("3");	//comport number
	gtk_table_attach_defaults(GTK_TABLE(table), label10, 2, 3, 0, 1);
	
	open = gtk_button_new_with_label("Open"); 	//open comport button
	gtk_table_attach_defaults(GTK_TABLE(table), open, 4, 5, 0, 1);
	exit = gtk_button_new_with_label("Close"); 	//close comport button
	gtk_table_attach_defaults(GTK_TABLE(table), exit, 5, 6, 0, 1);
	gtk_container_add(GTK_CONTAINER(window), table);
	
	plus = gtk_button_new_with_label("+"); 	//set comport
	gtk_table_attach_defaults(GTK_TABLE(table), plus, 3, 4, 0, 1);
	minus = gtk_button_new_with_label("-"); 	//set comport
	gtk_table_attach_defaults(GTK_TABLE(table), minus, 1, 2, 0, 1);
	gtk_container_add(GTK_CONTAINER(window), table);
	
	gtk_widget_show_all(window);
	
	g_timeout_add(500, (GSourceFunc) time_handler, (gpointer) window);
	
	g_signal_connect(G_OBJECT(plus), "clicked", G_CALLBACK(add), label10); //links the button to the function at the top
	
	g_signal_connect(G_OBJECT(minus), "clicked", G_CALLBACK(subtract), label10);
	
	g_signal_connect(G_OBJECT(open), "clicked", G_CALLBACK(init_com), NULL);
	
	g_signal_connect(G_OBJECT(exit), "clicked", G_CALLBACK(close_com), NULL);
	
	g_signal_connect (G_OBJECT(scale2), "value-changed", G_CALLBACK (scale_mov2), NULL);
	
	g_signal_connect (G_OBJECT(scale), "value-changed", G_CALLBACK (scale_mov), NULL);
	
	g_signal_connect (G_OBJECT(set), "clicked", G_CALLBACK (send_setpoint), NULL);
	
	g_signal_connect (G_OBJECT(close), "clicked", G_CALLBACK (gtk_main_quit), NULL);
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	gtk_main();
	
	return 0;
}