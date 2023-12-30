/*
 *   Copyright (C) 1999-2001 by Jonathan Naylor G4KLX
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

#include <gtk/gtk.h>

#include "global.h"

struct Beacon_Plus {
	struct Beacon_Struct *Data;
	double                Bearing;
	double                Distance;
};

static gint Frequency_Comparison(gconstpointer a, gconstpointer b)
{
	struct Beacon_Plus *beacon1 = (struct Beacon_Plus *)a;
	struct Beacon_Plus *beacon2 = (struct Beacon_Plus *)b;

	 if (beacon1->Data->Frequency > beacon2->Data->Frequency)
	 	return 1;
	 else if (beacon1->Data->Frequency < beacon2->Data->Frequency)
	 	return -1;
	 else
	 	return 0;
}

static gint Bearing_Comparison(gconstpointer a, gconstpointer b)
{
	struct Beacon_Plus *beacon1 = (struct Beacon_Plus *)a;
	struct Beacon_Plus *beacon2 = (struct Beacon_Plus *)b;

	 if (beacon1->Bearing > beacon2->Bearing)
	 	return 1;
	 else if (beacon1->Bearing < beacon2->Bearing)
	 	return -1;
	 else
	 	return 0;
}

static gint Distance_Comparison(gconstpointer a, gconstpointer b)
{
	struct Beacon_Plus *beacon1 = (struct Beacon_Plus *)a;
	struct Beacon_Plus *beacon2 = (struct Beacon_Plus *)b;

	 if (beacon1->Distance > beacon2->Distance)
	 	return 1;
	 else if (beacon1->Distance < beacon2->Distance)
	 	return -1;
	 else
	 	return 0;
}

void ListCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *listlist, *listwin;
	GtkWidget *separator;
	GtkWidget *bbox;
	GtkWidget *button;
	double Bearing, Distance;
	char FreqBuffer[21];
	char BearBuffer[21];
	char DistBuffer[21];
	char *title[7];
	char *items[7];
	GSList *Beacon, *Sorted_Beacon_List = NULL;
	struct Beacon_Struct *Data;
	int sort_order;
	struct Beacon_Plus *Data_Plus;

	if (Beacon_List == NULL) {
		Warning_Box("No Data loaded !");
		return;
	}

	sort_order = (int)data;

	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), "Frequencies, Bearings and Distances");
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	title[0] = "Frequency";
	title[1] = "Type              ";
	title[2] = "Callsign         ";
	title[3] = "Locator      ";
	title[4] = "Bearing   ";
	title[5] = "Distance";
	title[6] = NULL;
	listlist = gtk_clist_new_with_titles(6, (gchar **)title);
	gtk_widget_set_usize(listlist, LIST_WIDTH, LIST_HEIGHT);
	gtk_clist_column_titles_passive(GTK_CLIST(listlist));

	listwin = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(listwin), listlist);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(listwin), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_set_border_width(GTK_CONTAINER(listwin), BORDER_ITEM);
	gtk_box_pack_start(GTK_BOX(vbox), listwin, TRUE, TRUE, 0);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), separator, TRUE, TRUE, 0);

	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
	gtk_box_pack_end(GTK_BOX(vbox), bbox, TRUE, TRUE, 0);

	button = gtk_button_new_with_label("Cancel");

	gtk_container_add(GTK_CONTAINER(bbox), button);

	gtk_signal_connect_object(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button);

	gtk_clist_freeze(GTK_CLIST(listlist));

	Beacon = Beacon_List;

	while (Beacon != NULL) {
		Data = (struct Beacon_Struct *)Beacon->data;

		if (Check_Beacon(Data)) {
			Calc_Distance_Bearing(User_Data.Latitude, User_Data.Longitude, Data->Latitude, Data->Longitude, &Bearing, &Distance);

			Data_Plus = g_malloc(sizeof(struct Beacon_Plus));

			Data_Plus->Data     = Data;
			Data_Plus->Bearing  = Bearing;
			Data_Plus->Distance = Distance;

			switch (sort_order) {
				case LIST_FREQUENCY:
					Sorted_Beacon_List = g_slist_insert_sorted(Sorted_Beacon_List, Data_Plus, Frequency_Comparison);
					break;
				case LIST_BEARING:
					Sorted_Beacon_List = g_slist_insert_sorted(Sorted_Beacon_List, Data_Plus, Bearing_Comparison);
					break;
				case LIST_DISTANCE:
					Sorted_Beacon_List = g_slist_insert_sorted(Sorted_Beacon_List, Data_Plus, Distance_Comparison);
					break;
				default:
					g_error("Unknown value for list sorting");
					break;
			}
		}

		Beacon = g_slist_next(Beacon);
	}

	Beacon = Sorted_Beacon_List;

	while (Beacon != NULL) {
		Data_Plus = (struct Beacon_Plus *)Beacon->data;

		sprintf(FreqBuffer, "%.3f", Data_Plus->Data->Frequency);
		items[0] = FreqBuffer;

		switch (Data_Plus->Data->Type) {
			case TYPE_IARU:
				items[1] = "IARU";
				break;
			case TYPE_NONIARU:
				items[1] = "Non-IARU";
				break;
			case TYPE_REPEATER:
				items[1] = "Repeater";
				break;
			case TYPE_LINEAR:
				items[1] = "Linear";
				break;
			case TYPE_MAILBOX:
				items[1] = "Mailbox";
				break;
			case TYPE_DXCLUSTER:
				items[1] = "DX Cluster";
				break;
			case TYPE_NODE:
				items[1] = "Node";
				break;
			case TYPE_RS:
				items[1] = "Rain-Scatter";
				break;
			case TYPE_MISC:
				items[1] = "Misc";
				break;
			default:
				items[1] = "Unknown";
				break;
		}

		items[2] = Data_Plus->Data->Callsign;

		items[3] = Data_Plus->Data->Locator;

		sprintf(BearBuffer, "%.0f", Data_Plus->Bearing);
		items[4] = BearBuffer;

		sprintf(DistBuffer, "%.0f", Data_Plus->Distance);
		items[5] = DistBuffer;

		gtk_clist_append(GTK_CLIST(listlist), items);

		g_free(Data_Plus);

		Beacon = g_slist_next(Beacon);
	}

	g_slist_free(Sorted_Beacon_List);

	gtk_clist_thaw(GTK_CLIST(listlist));

	gtk_widget_show(button);
	gtk_widget_show(bbox);
	gtk_widget_show(separator);
	gtk_widget_show(listlist);
	gtk_widget_show(listwin);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}
