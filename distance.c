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

#include <gtk/gtk.h>

#include "global.h"

static void DistanceCb(GtkWidget *, gpointer);

static GtkWidget *SrcLocator;
static GtkWidget *DestLocator;
static GtkWidget *SrcDestLabel;
static GtkWidget *DestSrcLabel;

static void DistanceCb(GtkWidget *w, gpointer data)
{
	char *SLocator, *DLocator, Buffer[21];
	double SrcLatitude, SrcLongitude;
	double DestLatitude, DestLongitude;
	double Bearing1, Distance1;
	double Bearing2, Distance2;
	
	SLocator  = gtk_entry_get_text(GTK_ENTRY(SrcLocator));
	DLocator  = gtk_entry_get_text(GTK_ENTRY(DestLocator));

	if (strlen(SLocator) != 6 && strlen(SLocator) != 8) {
		gtk_label_set_text(GTK_LABEL(SrcDestLabel), "<none>");
		gtk_label_set_text(GTK_LABEL(DestSrcLabel), "<none>");
		Message_Box("The Source Locator is not the correct length");
		return;
	}

	if (strlen(DLocator) != 6 && strlen(DLocator) != 8) {
		gtk_label_set_text(GTK_LABEL(SrcDestLabel), "<none>");
		gtk_label_set_text(GTK_LABEL(DestSrcLabel), "<none>");
		Message_Box("The Destination Locator is not the correct length");
		return;
	}

	if (islower(SLocator[0])) SLocator[0] = toupper(SLocator[0]);
	if (islower(SLocator[1])) SLocator[1] = toupper(SLocator[1]);
	if (islower(SLocator[4])) SLocator[4] = toupper(SLocator[4]);
	if (islower(SLocator[5])) SLocator[5] = toupper(SLocator[5]);

	if (islower(DLocator[0])) DLocator[0] = toupper(DLocator[0]);
	if (islower(DLocator[1])) DLocator[1] = toupper(DLocator[1]);
	if (islower(DLocator[4])) DLocator[4] = toupper(DLocator[4]);
	if (islower(DLocator[5])) DLocator[5] = toupper(DLocator[5]);

	if (!Convert_Locator(SLocator, &SrcLatitude, &SrcLongitude)) {
		gtk_label_set_text(GTK_LABEL(SrcDestLabel), "<none>");
		gtk_label_set_text(GTK_LABEL(DestSrcLabel), "<none>");
		Message_Box("The Source Locator entered was invalid");
		return;
	}

	if (!Convert_Locator(DLocator, &DestLatitude, &DestLongitude)) {
		gtk_label_set_text(GTK_LABEL(SrcDestLabel), "<none>");
		gtk_label_set_text(GTK_LABEL(DestSrcLabel), "<none>");
		Message_Box("The Destination Locator entered was invalid");
		return;
	}

	Calc_Distance_Bearing(SrcLatitude, SrcLongitude, DestLatitude, DestLongitude, &Bearing1, &Distance1);
	Calc_Distance_Bearing(DestLatitude, DestLongitude, SrcLatitude, SrcLongitude, &Bearing2, &Distance2);

	sprintf(Buffer, "%.0f deg  %.0f kms", Bearing1, Distance1);
	gtk_label_set_text(GTK_LABEL(SrcDestLabel), Buffer);

	sprintf(Buffer, "%.0f deg  %.0f kms", Bearing2, Distance2);
	gtk_label_set_text(GTK_LABEL(DestSrcLabel), Buffer);
}

void DistCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *table;
	GtkWidget *label[6];
	GtkWidget *button[2];
	GtkWidget *separator;
	GtkWidget *bbox;
	
	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), "Distance Calculation");
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	table    = gtk_table_new(2, 4, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), table, TRUE, TRUE, 0);

	label[0]      = gtk_label_new("Source");
	label[1]      = gtk_label_new("Destination");
	label[2]      = gtk_label_new("Src to Dest");
	label[3]      = gtk_label_new("Dest to Src");

	SrcLocator   = gtk_entry_new_with_max_length(20);
	DestLocator  = gtk_entry_new_with_max_length(20);
	SrcDestLabel = gtk_label_new("<none>");
	DestSrcLabel = gtk_label_new("<none>");

	gtk_entry_set_text(GTK_ENTRY(SrcLocator), User_Data.Locator);

	gtk_table_attach(GTK_TABLE(table), label[0],      0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[1],      0, 1, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[2],      0, 1, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[3],      0, 1, 3, 4,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	gtk_table_attach(GTK_TABLE(table), SrcLocator,   1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), DestLocator,  1, 2, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), SrcDestLabel, 1, 2, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), DestSrcLabel, 1, 2, 3, 4,
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

	gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(DistanceCb), window);
	gtk_signal_connect_object(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(button[1], GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button[0]);

	gtk_widget_show(label[0]);
	gtk_widget_show(label[1]);
	gtk_widget_show(label[2]);
	gtk_widget_show(label[3]);
	gtk_widget_show(SrcLocator);
	gtk_widget_show(DestLocator);
	gtk_widget_show(SrcDestLabel);
	gtk_widget_show(DestSrcLabel);
	gtk_widget_show(button[0]);
	gtk_widget_show(button[1]);
	gtk_widget_show(bbox);
	gtk_widget_show(separator);
	gtk_widget_show(table);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}
