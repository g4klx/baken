/*
 *   Copyright (C) 1999-2000 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <gtk/gtk.h>

#include "global.h"

static void ReflectionCb(GtkWidget *, gpointer);

static GtkWidget *Locator1;
static GtkWidget *Bearing1;
static GtkWidget *Locator2;
static GtkWidget *Bearing2;
static GtkWidget *ScpLabel;
static GtkWidget *DistLabel1;
static GtkWidget *DistLabel2;

static void ReflectionCb(GtkWidget *w, gpointer data)
{
	char *LocatorText1, *LocatorText2, Buffer[21];
	char *BearingText1, *BearingText2;
	double Latitude1, Longitude1;
	double Latitude2, Longitude2;
	double SLatitude, SLongitude;
	double Bearing1S, Bearing2S;
	double Distance1S, Distance2S;
	
	LocatorText1  = gtk_entry_get_text(GTK_ENTRY(Locator1));
	BearingText1  = gtk_entry_get_text(GTK_ENTRY(Bearing1));
	LocatorText2  = gtk_entry_get_text(GTK_ENTRY(Locator2));
	BearingText2  = gtk_entry_get_text(GTK_ENTRY(Bearing2));

	if (strlen(LocatorText1) != 6) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The first Locator is not the correct length");
		return;
	}

	if (strspn(BearingText1, "0123456789.-") != strlen(BearingText1)) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The first Bearing is not valid");
		return;
	}

	Bearing1S = atof(BearingText1);

	if (Bearing1S < 0.0 || Bearing1S > 360.0) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The first Bearing is not valid");
		return;
	}

	if (strlen(LocatorText2) != 6) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The second Locator is not the correct length");
		return;
	}

	if (strspn(BearingText2, "0123456789.-") != strlen(BearingText2)) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The second Bearing is not valid");
		return;
	}

	Bearing2S = atof(BearingText2);

	if (Bearing2S < 0.0 || Bearing2S > 360.0) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The second Bearing is not valid");
		return;
	}

	if (islower(LocatorText1[0])) LocatorText1[0] = toupper(LocatorText1[0]);
	if (islower(LocatorText1[1])) LocatorText1[1] = toupper(LocatorText1[1]);
	if (islower(LocatorText1[4])) LocatorText1[4] = toupper(LocatorText1[4]);
	if (islower(LocatorText1[5])) LocatorText1[5] = toupper(LocatorText1[5]);

	if (islower(LocatorText2[0])) LocatorText2[0] = toupper(LocatorText2[0]);
	if (islower(LocatorText2[1])) LocatorText2[1] = toupper(LocatorText2[1]);
	if (islower(LocatorText2[4])) LocatorText2[4] = toupper(LocatorText2[4]);
	if (islower(LocatorText2[5])) LocatorText2[5] = toupper(LocatorText2[5]);

	if (!Convert_Locator(LocatorText1, &Latitude1, &Longitude1)) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The first Locator entered was invalid");
		return;
	}

	if (!Convert_Locator(LocatorText2, &Latitude2, &Longitude2)) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("The second Locator entered was invalid");
		return;
	}

	if (!Calc_Scatter_Point(Latitude1, Longitude1, Bearing1S, Latitude2, Longitude2, Bearing2S, &SLatitude, &SLongitude)) {
		gtk_label_set_text(GTK_LABEL(ScpLabel),   "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel1), "<none>");
		gtk_label_set_text(GTK_LABEL(DistLabel2), "<none>");
		Message_Box("Impossible to calculate the scatter point");
		return;
	}

	Calc_Distance_Bearing(Latitude1, Longitude1, SLatitude, SLongitude, &Bearing1S, &Distance1S);
	Calc_Distance_Bearing(Latitude2, Longitude2, SLatitude, SLongitude, &Bearing2S, &Distance2S);

	Convert_Angles(SLatitude, SLongitude, Buffer);
	gtk_label_set_text(GTK_LABEL(ScpLabel), Buffer);

	sprintf(Buffer, "%.0f kms", Distance1S);
	gtk_label_set_text(GTK_LABEL(DistLabel1), Buffer);

	sprintf(Buffer, "%.0f kms", Distance2S);
	gtk_label_set_text(GTK_LABEL(DistLabel2), Buffer);

	if (Distance1S >= 450.0 || Distance2S >= 450.0)
		Message_Box("Unreasonable distance to the scatter point");
}

void ReflCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *table;
	GtkWidget *label[7];
	GtkWidget *button[2];
	GtkWidget *separator;
	GtkWidget *bbox;
	int i;
	
	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), "Scatter Point Calculation");
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	table    = gtk_table_new(2, 7, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), table, TRUE, TRUE, 0);

	label[0]   = gtk_label_new("Locator 1");
	label[1]   = gtk_label_new("Bearing 1");
	label[2]   = gtk_label_new("Locator 2");
	label[3]   = gtk_label_new("Bearing 2");
	label[4]   = gtk_label_new("Scatter Point");
	label[5]   = gtk_label_new("Distance 1");
	label[6]   = gtk_label_new("Distance 2");

	Locator1   = gtk_entry_new_with_max_length(20);
	Bearing1   = gtk_entry_new_with_max_length(20);
	Locator2   = gtk_entry_new_with_max_length(20);
	Bearing2   = gtk_entry_new_with_max_length(20);
	ScpLabel   = gtk_label_new("<none>");
	DistLabel1 = gtk_label_new("<none>");
	DistLabel2 = gtk_label_new("<none>");

	gtk_entry_set_text(GTK_ENTRY(Locator1), User_Data.Locator);

	gtk_table_attach(GTK_TABLE(table), label[0],      0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[1],      0, 1, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[2],      0, 1, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[3],      0, 1, 3, 4,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[4],      0, 1, 4, 5,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[5],      0, 1, 5, 6,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[6],      0, 1, 6, 7,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	gtk_table_attach(GTK_TABLE(table), Locator1,      1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bearing1,      1, 2, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Locator2,      1, 2, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bearing2,      1, 2, 3, 4,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), ScpLabel,      1, 2, 4, 5,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), DistLabel1,    1, 2, 5, 6,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), DistLabel2,    1, 2, 6, 7,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), separator, TRUE, TRUE, 0);

	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_SPREAD);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
	gtk_box_pack_end(GTK_BOX(vbox), bbox, TRUE, TRUE, 0);

	button[0] = gtk_button_new_with_label("OK");
	button[1] = gtk_button_new_with_label("Cancel");

	gtk_container_add(GTK_CONTAINER(bbox), button[0]);
	gtk_container_add(GTK_CONTAINER(bbox), button[1]);

	gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(ReflectionCb), window);
	gtk_signal_connect_object(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(button[1], GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button[0]);

	for (i = 0; i < 7; i++)
		gtk_widget_show(label[i]);
	gtk_widget_show(Locator1);
	gtk_widget_show(Bearing1);
	gtk_widget_show(Locator2);
	gtk_widget_show(Bearing2);
	gtk_widget_show(ScpLabel);
	gtk_widget_show(DistLabel1);
	gtk_widget_show(DistLabel2);
	gtk_widget_show(button[0]);
	gtk_widget_show(button[1]);
	gtk_widget_show(bbox);
	gtk_widget_show(separator);
	gtk_widget_show(table);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}
