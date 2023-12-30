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

static void UserEditCb(GtkWidget *, gpointer);

static GtkWidget *UserCall;
static GtkWidget *UserLocator;

static void UserEditCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window = (GtkWidget *)data;
	char *Callsign, *Locator;
	double Latitude, Longitude;
	
	Callsign = gtk_entry_get_text(GTK_ENTRY(UserCall));
	Locator  = gtk_entry_get_text(GTK_ENTRY(UserLocator));

	if (strlen(Locator) != 6 && strlen(Locator) != 8) {
		Message_Box("The Locator is not the correct length");
		return;
	}

	if (islower(Locator[0])) Locator[0] = toupper(Locator[0]);
	if (islower(Locator[1])) Locator[1] = toupper(Locator[1]);
	if (islower(Locator[4])) Locator[4] = toupper(Locator[4]);
	if (islower(Locator[5])) Locator[5] = toupper(Locator[5]);

	if (!Convert_Locator(Locator, &Latitude, &Longitude)) {
		Message_Box("The Locator entered was invalid");
		return;
	}

	strcpy(User_Data.Callsign, Callsign);
	strcpy(User_Data.Locator, Locator);
	User_Data.Latitude  = Latitude;
	User_Data.Longitude = Longitude;
	
	if (Write_User_Data())
		gtk_widget_destroy(window);

	Draw_Main_Map();
	Redraw_Detail_Map();
}

void EditCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *table;
	GtkWidget *label[2];
	GtkWidget *button[2];
	GtkWidget *separator;
	GtkWidget *bbox;
	
	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), "Preferences");
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	table    = gtk_table_new(2, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), table, TRUE, TRUE, 0);

	label[0] = gtk_label_new("Callsign");
	label[1] = gtk_label_new("Locator");

	UserCall    = gtk_entry_new_with_max_length(20);
	UserLocator = gtk_entry_new_with_max_length(20);

	gtk_table_attach(GTK_TABLE(table), label[0], 0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), label[1], 0, 1, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	gtk_table_attach(GTK_TABLE(table), UserCall,    1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), UserLocator, 1, 2, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), separator, TRUE, TRUE, 0);

	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_SPREAD);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
	gtk_box_pack_end(GTK_BOX(vbox), bbox, TRUE, TRUE, 0);

	button[0] = gtk_button_new_with_label("Save");
	button[1] = gtk_button_new_with_label("Cancel");

	gtk_container_add(GTK_CONTAINER(bbox), button[0]);
	gtk_container_add(GTK_CONTAINER(bbox), button[1]);

	gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(UserEditCb), window);
	gtk_signal_connect_object(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(button[1], GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button[0]);

	gtk_entry_set_text(GTK_ENTRY(UserCall),    User_Data.Callsign);
	gtk_entry_set_text(GTK_ENTRY(UserLocator), User_Data.Locator);

	gtk_widget_show(label[0]);
	gtk_widget_show(label[1]);
	gtk_widget_show(UserCall);
	gtk_widget_show(UserLocator);
	gtk_widget_show(button[0]);
	gtk_widget_show(button[1]);
	gtk_widget_show(bbox);
	gtk_widget_show(separator);
	gtk_widget_show(table);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}
