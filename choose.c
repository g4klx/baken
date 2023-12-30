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
#include <ctype.h>

#include <gtk/gtk.h>

#include "global.h"

static GtkWidget *Bands[MAX_BANDS];
static GtkWidget *Types[MAX_TYPES];

static void ChooseOkCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window = (GtkWidget *)data;
	int i;

	for (i = 0; i < MAX_BANDS; i++) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Bands[i])))
			User_Data.Bands[i] = TRUE;
		else
			User_Data.Bands[i] = FALSE;
	}
	
	for (i = 0; i < MAX_TYPES; i++) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Types[i])))
			User_Data.Types[i] = TRUE;
		else
			User_Data.Types[i] = FALSE;
	}
	
	if (Write_User_Data())
		if (window != NULL)
			gtk_widget_destroy(window);

	Draw_Main_Map();
	Redraw_Detail_Map();
}

void ChooseCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *notebook;
	GtkWidget *notelabel[2];
	GtkWidget *typebox;
	GtkWidget *frame[4];
	GtkWidget *framebox[4];
	GtkWidget *table;
	GtkWidget *button[3];
	GtkWidget *separator;
	GtkWidget *bbox;
	int i;
	
	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), "Selection Criteria");
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

	notelabel[0] = gtk_label_new("Band");
	table = gtk_table_new(3, 4, FALSE);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, notelabel[0]);

	Bands[BAND_50MHZ]  = gtk_check_button_new_with_label("50 MHz");
	Bands[BAND_70MHZ]  = gtk_check_button_new_with_label("70 MHz");
	Bands[BAND_144MHZ] = gtk_check_button_new_with_label("144 MHz");
	Bands[BAND_432MHZ] = gtk_check_button_new_with_label("432 MHz");
	Bands[BAND_1_3GHZ] = gtk_check_button_new_with_label("1296 MHz");
	Bands[BAND_2_3GHZ] = gtk_check_button_new_with_label("2320 MHz");
	Bands[BAND_3_4GHZ] = gtk_check_button_new_with_label("3456 MHz");
	Bands[BAND_5_7GHZ] = gtk_check_button_new_with_label("5760 MHz");
	Bands[BAND_10GHZ]  = gtk_check_button_new_with_label("10 GHz");
	Bands[BAND_24GHZ]  = gtk_check_button_new_with_label("24 GHz");
	Bands[BAND_47GHZ]  = gtk_check_button_new_with_label("47 GHz");
	Bands[BAND_76GHZ]  = gtk_check_button_new_with_label("76 GHz");

	gtk_table_attach(GTK_TABLE(table), Bands[BAND_50MHZ],  0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_70MHZ],  1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_144MHZ], 2, 3, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	gtk_table_attach(GTK_TABLE(table), Bands[BAND_432MHZ], 0, 1, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_1_3GHZ], 1, 2, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_2_3GHZ], 2, 3, 1, 2,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	gtk_table_attach(GTK_TABLE(table), Bands[BAND_3_4GHZ], 0, 1, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_5_7GHZ], 1, 2, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_10GHZ],  2, 3, 2, 3,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	gtk_table_attach(GTK_TABLE(table), Bands[BAND_24GHZ],  0, 1, 3, 4,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_47GHZ],  1, 2, 3, 4,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(table), Bands[BAND_76GHZ],  2, 3, 3, 4,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	Types[TYPE_IARU]      = gtk_check_button_new_with_label("IARU");
	Types[TYPE_NONIARU]   = gtk_check_button_new_with_label("Non-IARU");
	Types[TYPE_REPEATER]  = gtk_check_button_new_with_label("Voice");
	Types[TYPE_LINEAR]    = gtk_check_button_new_with_label("Linear");
	Types[TYPE_TV]        = gtk_check_button_new_with_label("TV");
	Types[TYPE_MAILBOX]   = gtk_check_button_new_with_label("Mailbox");
	Types[TYPE_DXCLUSTER] = gtk_check_button_new_with_label("DX Cluster");
	Types[TYPE_NODE]      = gtk_check_button_new_with_label("Node");
	Types[TYPE_RS]        = gtk_check_button_new_with_label("RS Stations");
	Types[TYPE_MISC]      = gtk_check_button_new_with_label("Misc");

	typebox = gtk_vbox_new(FALSE, BORDER_WIDTH);

	notelabel[1] = gtk_label_new("Type");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), typebox, notelabel[1]);

	frame[0] = gtk_frame_new("Beacons");
	gtk_box_pack_start(GTK_BOX(typebox), frame[0], TRUE, TRUE, 0);
	framebox[0] = gtk_table_new(1, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame[0]), framebox[0]);

	gtk_table_attach(GTK_TABLE(framebox[0]), Types[TYPE_IARU],    0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(framebox[0]), Types[TYPE_NONIARU], 1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

 	frame[1] = gtk_frame_new("Repeaters");
	gtk_box_pack_start(GTK_BOX(typebox), frame[1], TRUE, TRUE, 0);
	framebox[1] = gtk_table_new(1, 3, FALSE);
	gtk_container_add(GTK_CONTAINER(frame[1]), framebox[1]);
	
	gtk_table_attach(GTK_TABLE(framebox[1]), Types[TYPE_REPEATER], 0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(framebox[1]), Types[TYPE_LINEAR],   1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(framebox[1]), Types[TYPE_TV],       2, 3, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	frame[2] = gtk_frame_new("Packet Radio");
	gtk_box_pack_start(GTK_BOX(typebox), frame[2], TRUE, TRUE, 0);
	framebox[2] = gtk_table_new(1, 3, FALSE);
	gtk_container_add(GTK_CONTAINER(frame[2]), framebox[2]);
	
	gtk_table_attach(GTK_TABLE(framebox[2]), Types[TYPE_MAILBOX],   0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(framebox[2]), Types[TYPE_DXCLUSTER], 1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(framebox[2]), Types[TYPE_NODE],      2, 3, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	frame[3] = gtk_frame_new("Miscellaneous");
	gtk_box_pack_start(GTK_BOX(typebox), frame[3], TRUE, TRUE, 0);
	framebox[3] = gtk_table_new(1, 1, FALSE);
	gtk_container_add(GTK_CONTAINER(frame[3]), framebox[3]);

	gtk_table_attach(GTK_TABLE(framebox[3]), Types[TYPE_RS],   0, 1, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);
	gtk_table_attach(GTK_TABLE(framebox[3]), Types[TYPE_MISC], 1, 2, 0, 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, BORDER_ITEM, BORDER_ITEM);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), separator, TRUE, TRUE, 0);

	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_SPREAD);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
	gtk_box_pack_end(GTK_BOX(vbox), bbox, TRUE, TRUE, 0);

	button[0] = gtk_button_new_with_label("OK");
	button[1] = gtk_button_new_with_label("Apply");
	button[2] = gtk_button_new_with_label("Cancel");

	gtk_container_add(GTK_CONTAINER(bbox), button[0]);
	gtk_container_add(GTK_CONTAINER(bbox), button[1]);
	gtk_container_add(GTK_CONTAINER(bbox), button[2]);

	gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(ChooseOkCb), window);
	gtk_signal_connect(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(ChooseOkCb), NULL);
	gtk_signal_connect_object(GTK_OBJECT(button[2]), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));

	GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(button[1], GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(button[2], GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button[0]);

	for (i = 0; i < MAX_BANDS; i++) {
		if (User_Data.Bands[i])
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Bands[i]), TRUE);
		gtk_widget_show(Bands[i]);
	}
	for (i = 0; i < MAX_TYPES; i++)	{
		if (User_Data.Types[i])
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Types[i]), TRUE);
		gtk_widget_show(Types[i]);
	}
	gtk_widget_show(button[0]);
	gtk_widget_show(button[1]);
	gtk_widget_show(button[2]);
	gtk_widget_show(bbox);
	gtk_widget_show(separator);
	gtk_widget_show(notelabel[0]);
	gtk_widget_show(notelabel[1]);
	gtk_widget_show(notebook);
	gtk_widget_show(frame[0]);
	gtk_widget_show(frame[1]);
	gtk_widget_show(frame[2]);
	gtk_widget_show(frame[3]);
	gtk_widget_show(framebox[0]);
	gtk_widget_show(framebox[1]);
	gtk_widget_show(framebox[2]);
	gtk_widget_show(framebox[3]);
	gtk_widget_show(typebox);
	gtk_widget_show(table);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}
