/*
 *   Copyright (C) 1999-2005 by Jonathan Naylor G4KLX
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
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>

#include <gtk/gtk.h>

#include "global.h"

GSList *Beacon_List = NULL;
GSList *City_List   = NULL;

struct User_Struct User_Data = {
	"G4KLX",
	"IO93FB",
	53.05,
	1.55,
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
	{ 1, 1, 0, 0, 0, 0, 0, 0 },
	{
	{0, 0, 0, 65535},
	{0, 65535, 65535, 65535},
	{0, 65535, 0, 0},
	{0, 0, 0, 0},
	{0, 65535, 0, 0},
	{0, 65535, 65535, 0}
	}
};

static char About_Text[] = "baken v0.5.4\n"
	"A GTK based Beacon Display Program\n"
	"27 March 2005\n"
	"Written by Jonathan Naylor  ON/G4KLX\n";

GtkWidget *MainMap;

static GtkWidget *MainWindow;

static void ExitCb(GtkWidget *, gpointer);
static void AboutCb(GtkWidget *, gpointer);

#define	ERROR_BOX	0
#define	WARNING_BOX	1
#define	MESSAGE_BOX	2
#define	INFO_BOX	3
#define	YN_BOX		4
#define	EXIT_BOX	5

static void YesCb(GtkWidget *, GtkWidget *);
static void NoCb(GtkWidget *, GtkWidget *);
static void (*Question_Yes)(void);
static void (*Question_No)(void);

static GtkItemFactoryEntry Menu_Items[] = {
	{"/_File",                      NULL,      NULL,     0, "<Branch>"},
	{"/File/New...",		NULL,	   ChooseCb, 0, NULL},
	{"/File/sep",			NULL,	   NULL,     0, "<Separator>"},
	{"/File/List",                  NULL,      NULL,     0, "<Branch>"},
	{"/File/List/By Frequency",     NULL,      ListCb,   LIST_FREQUENCY, NULL},
	{"/File/List/By Distance",      NULL,      ListCb,   LIST_DISTANCE,  NULL},
	{"/File/List/By Bearing",       NULL,      ListCb,   LIST_BEARING,   NULL},
	{"/File/sep",			NULL,	   NULL,     0, "<Separator>"},
	{"/File/Distance...",		NULL,	   DistCb,   0, NULL},
	{"/File/Reflection...",		NULL,	   ReflCb,   0, NULL},
	{"/File/sep",                   NULL,      NULL,     0, "<Separator>"},
	{"/File/Exit",                  NULL,      ExitCb,   0, NULL},
	{"/_Edit",                      NULL,      NULL,     0, "<Branch>"},
	{"/Edit/User...",               NULL,      EditCb,   0, NULL},
	{"/_Colours",                   NULL,      NULL,     0, "<Branch>"},
	{"/Colours/Background",         NULL,      ColourCb, COLOUR_BACKGROUND, NULL},
	{"/Colours/Beacons",            NULL,      ColourCb, COLOUR_BEACONS, NULL},
	{"/Colours/Cities",             NULL,      ColourCb, COLOUR_CITIES, NULL},
	{"/Colours/Land",               NULL,      ColourCb, COLOUR_LAND, NULL},
	{"/Colours/Squares",            NULL,      ColourCb, COLOUR_SQUARES, NULL},
	{"/Colours/User",               NULL,      ColourCb, COLOUR_USER, NULL},
	{"/_Help",                      NULL,      NULL,     0, "<Branch>"},
	{"/Help/About Baken",           NULL,      AboutCb,  0, NULL}
};

int main(int argc, char **argv)
{
	GtkWidget *menubar;
	GtkWidget *vbox;
	GtkItemFactory *item_factory;
	GtkAccelGroup *accel_group;
	int nmenu_items = sizeof(Menu_Items) / sizeof(Menu_Items[0]);

	gtk_init(&argc, &argv);

	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_signal_connect(GTK_OBJECT(MainWindow), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), &MainWindow);

	gtk_container_set_border_width(GTK_CONTAINER(MainWindow), 0);

	vbox = gtk_vbox_new(FALSE, 1);
	gtk_container_border_width(GTK_CONTAINER(vbox), 1);
	gtk_container_add(GTK_CONTAINER(MainWindow), vbox);

	accel_group = gtk_accel_group_new();
	item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(item_factory, nmenu_items, Menu_Items, NULL);
	gtk_accel_group_attach(accel_group, GTK_OBJECT(MainWindow));

	menubar = gtk_item_factory_get_widget(item_factory, "<main>");
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, TRUE, 0);
	gtk_widget_show(menubar);

	MainMap = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(MainMap), MAIN_MAP_WIDTH, MAIN_MAP_HEIGHT);
	gtk_widget_set_events(MainMap, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
	gtk_signal_connect(GTK_OBJECT(MainMap), "expose_event", (GtkSignalFunc)MainMapExposeCb, NULL);
	gtk_signal_connect(GTK_OBJECT(MainMap), "button_press_event", (GtkSignalFunc)MainMapClickCb, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), MainMap, FALSE, TRUE, 0);
	gtk_widget_show(MainMap);

	gtk_widget_show(vbox);

	Read_City_Data();

	Read_Beacon_Data(TYPE_IARU,      "iarufile");
	Read_Beacon_Data(TYPE_NONIARU,   "noniarufile");
	Read_Beacon_Data(TYPE_MAILBOX,   "mailboxfile");
	Read_Beacon_Data(TYPE_DXCLUSTER, "dxclusterfile");
	Read_Beacon_Data(TYPE_NODE,      "nodefile");
	Read_Beacon_Data(TYPE_REPEATER,  "repeaterfile");
	Read_Beacon_Data(TYPE_LINEAR,    "linearfile");
	Read_Beacon_Data(TYPE_TV,        "tvfile");
	Read_Beacon_Data(TYPE_RS,        "rsfile");
	Read_Beacon_Data(TYPE_MISC,      "miscfile");

	Read_User_Data();

	gtk_widget_show(MainWindow);

	gtk_main();

	return 0;
}

static void AboutCb(GtkWidget *w, gpointer data)
{
	GtkWidget *window;
	GtkWidget *hbox;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *bbox;
	GtkWidget *label;
	GtkWidget *button;

	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), "About Baken");
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	hbox = gtk_hbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), hbox);

	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new(About_Text);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	bbox = gtk_vbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_START);
	gtk_box_pack_end(GTK_BOX(hbox), bbox, TRUE, TRUE, 0);

	button = gtk_button_new_with_label("OK");
	gtk_container_add(GTK_CONTAINER(bbox), button);

	gtk_signal_connect_object(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button);

	gtk_widget_show(button);
	gtk_widget_show(label);
	gtk_widget_show(bbox);
	gtk_widget_show(frame);
	gtk_widget_show(vbox);
	gtk_widget_show(hbox);
	gtk_widget_show(window);
}

static void General_Box(char *title, char *message, int type)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *bbox;
	GtkWidget *label;
	GtkWidget *separator;
	GtkWidget *button[2];

	window = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_window_set_title(GTK_WINDOW(window), title);
	gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);

	vbox = gtk_vbox_new(FALSE, BORDER_WIDTH);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	label = gtk_label_new(message);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), separator, TRUE, TRUE, 0);

	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_SPREAD);
	gtk_box_pack_end(GTK_BOX(vbox), bbox, TRUE, TRUE, 0);

	switch (type) {
		case ERROR_BOX:
			button[0] = gtk_button_new_with_label("OK");
			gtk_container_add(GTK_CONTAINER(bbox), button[0]);
			gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", ExitCb, window);
			GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
			gtk_widget_grab_default(button[0]);
			gtk_widget_show(button[0]);
			break;
		case WARNING_BOX:
		case MESSAGE_BOX:
		case INFO_BOX:
			button[0] = gtk_button_new_with_label("OK");
			gtk_container_add(GTK_CONTAINER(bbox), button[0]);
			gtk_signal_connect_object(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));
			GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
			gtk_widget_grab_default(button[0]);
			gtk_widget_show(button[0]);
			break;
		case YN_BOX:
			button[0] = gtk_button_new_with_label("Yes");
			button[1] = gtk_button_new_with_label("No");
			gtk_container_add(GTK_CONTAINER(bbox), button[0]);
			gtk_container_add(GTK_CONTAINER(bbox), button[1]);
			gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(YesCb), window);
			gtk_signal_connect(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(NoCb), window);
			GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
			GTK_WIDGET_SET_FLAGS(button[1], GTK_CAN_DEFAULT);
			gtk_widget_grab_default(button[0]);
			gtk_widget_show(button[0]);
			gtk_widget_show(button[1]);
			break;
		case EXIT_BOX:
			button[0] = gtk_button_new_with_label("OK");
			button[1] = gtk_button_new_with_label("Cancel");
			gtk_container_add(GTK_CONTAINER(bbox), button[0]);
			gtk_container_add(GTK_CONTAINER(bbox), button[1]);
			gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", ExitCb, window);
			gtk_signal_connect_object(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(window));
			GTK_WIDGET_SET_FLAGS(button[0], GTK_CAN_DEFAULT);
			GTK_WIDGET_SET_FLAGS(button[1], GTK_CAN_DEFAULT);
			gtk_widget_grab_default(button[0]);
			gtk_widget_show(button[0]);
			gtk_widget_show(button[1]);
			break;
		default:
			g_error("baken: unknown box type %d\n", type);
			break;
	}

	gtk_widget_show(label);
	gtk_widget_show(separator);
	gtk_widget_show(bbox);
	gtk_widget_show(vbox);
	gtk_widget_show(window);
}

void Error_Box(char *message)
{
	General_Box("Error from Baken", message, ERROR_BOX);
}

void Warning_Box(char *message)
{
	General_Box("Warning from Baken", message, WARNING_BOX);
}

void Message_Box(char *message)
{
	General_Box("Message from Baken", message, MESSAGE_BOX);
}

void Info_Box(char *message)
{
	General_Box("Information about Baken", message, INFO_BOX);
}

void YN_Box(char *message, void (*Yes)(void), void (*No)(void))
{
	Question_Yes = Yes;
	Question_No  = No;

	General_Box("Question from Baken", message, YN_BOX);
}

static void YesCb(GtkWidget *widget, GtkWidget *window)
{
	gtk_widget_destroy(window);
	
	if (Question_Yes != (void (*)(void))NULL)
		(*Question_Yes)();
}

static void NoCb(GtkWidget *widget, GtkWidget *window)
{
	gtk_widget_destroy(window);
	
	if (Question_No != (void (*)(void))NULL)
		(*Question_No)();
}

static void ExitCb(GtkWidget *widget, gpointer data)
{
	Write_User_Data();

	gtk_main_quit();
}

int Check_Beacon(struct Beacon_Struct *Beacon)
{
	int found = FALSE;
	int type;

	if (User_Data.Bands[BAND_50MHZ])
		if (Beacon->Frequency >= 50.000 && Beacon->Frequency <= 54.000)
			found = TRUE;

	if (User_Data.Bands[BAND_70MHZ])
		if (Beacon->Frequency >= 70.000 && Beacon->Frequency <= 70.500)
			found = TRUE;

	if (User_Data.Bands[BAND_144MHZ])
		if (Beacon->Frequency >= 144.000 && Beacon->Frequency <= 148.000)
			found = TRUE;

	if (User_Data.Bands[BAND_432MHZ])
		if (Beacon->Frequency >= 420.000 && Beacon->Frequency <= 450.000)
			found = TRUE;

	if (User_Data.Bands[BAND_1_3GHZ])
		if (Beacon->Frequency >= 1240.000 && Beacon->Frequency <= 1325.000)
			found = TRUE;

	if (User_Data.Bands[BAND_2_3GHZ])
		if (Beacon->Frequency >= 2310.000 && Beacon->Frequency <= 2450.500)
			found = TRUE;

	if (User_Data.Bands[BAND_3_4GHZ])
		if (Beacon->Frequency >= 3400.000 && Beacon->Frequency <= 3475.000)
			found = TRUE;

	if (User_Data.Bands[BAND_5_7GHZ])
		if (Beacon->Frequency >= 5650.000 && Beacon->Frequency <= 5850.000)
			found = TRUE;

	if (User_Data.Bands[BAND_10GHZ])
		if (Beacon->Frequency >= 10000.000 && Beacon->Frequency <= 10500.000)
			found = TRUE;

	if (User_Data.Bands[BAND_24GHZ])
		if (Beacon->Frequency >= 24000.000 && Beacon->Frequency <= 24250.000)
			found = TRUE;

	if (User_Data.Bands[BAND_47GHZ])
		if (Beacon->Frequency >= 47000.000 && Beacon->Frequency <= 47200.000)
			found = TRUE;

	if (User_Data.Bands[BAND_76GHZ])
		if (Beacon->Frequency >= 76000.000 && Beacon->Frequency <= 76500.000)
			found = TRUE;

	if (!found)
		return FALSE;

	for (type = 0; type < MAX_TYPES; type++) {
		if (User_Data.Types[type])
			if (Beacon->Type == type)
				return TRUE;
	}

	return FALSE;
}
