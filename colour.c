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

#include <gtk/gtk.h>

#include "global.h"

GdkColor Text_Bg_Colour = {0, 48830, 48830, 48830};     /* Grey   */
GdkColor Text_Fg_Colour = {0, 0,     0,     0};         /* Black  */

static int          Colour_Element;

static GdkColormap *Colour_Map;

static void ColourSelCb(GtkWidget *, GtkWidget *);

void ColourCb(GtkWidget *w, gpointer data)
{
	GtkWidget         *colourseldlg;
	GtkColorSelection *coloursel;
	char              *name = NULL;
	gdouble           colour[3];

	Colour_Element = (int)data;

	switch (Colour_Element) {
		case COLOUR_BACKGROUND:
			name = "Select Map Background Colour";
			break;
		case COLOUR_BEACONS:
			name = "Select Beacons Colour";
			break;
		case COLOUR_CITIES:
			name = "Select Cities Colour";
			break;
		case COLOUR_LAND:
			name = "Select Map Land Colour";
			break;
		case COLOUR_SQUARES:
			name = "Select Locator Lines Colour";
			break;
		case COLOUR_USER:
			name = "Select User Position Colour";
			break;
		default:
			g_error("baken: unknown colour item %d\n", Colour_Element);
			break;
	}

	colourseldlg = gtk_color_selection_dialog_new(name);
	coloursel    = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(colourseldlg)->colorsel);

	colour[0] = (double)User_Data.Colours[Colour_Element].red   / 65535.0;
	colour[1] = (double)User_Data.Colours[Colour_Element].green / 65535.0;
	colour[2] = (double)User_Data.Colours[Colour_Element].blue  / 65535.0;

	gtk_color_selection_set_color(GTK_COLOR_SELECTION(coloursel), colour);

        gtk_signal_connect_object(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(colourseldlg)->cancel_button), "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(colourseldlg));
	gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(colourseldlg)->ok_button), "clicked", (GtkSignalFunc)ColourSelCb, colourseldlg);

	gtk_widget_show(colourseldlg);
}

static void ColourSelCb(GtkWidget *widget, GtkWidget *colourseldlg)
{
	gdouble            colour[3];
	GdkColor           gdkcolour;
	GtkColorSelection *coloursel;

	coloursel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(colourseldlg)->colorsel);

	gtk_color_selection_get_color(coloursel, colour);

	gdkcolour.pixel = 0;
	gdkcolour.red   = (guint)(colour[0] * 65535.0 + 0.5);
	gdkcolour.green = (guint)(colour[1] * 65535.0 + 0.5);
	gdkcolour.blue  = (guint)(colour[2] * 65535.0 + 0.5);

	gdk_color_alloc(Colour_Map, &gdkcolour);

	switch (Colour_Element) {
		case COLOUR_BACKGROUND:
			User_Data.Colours[COLOUR_BACKGROUND] = gdkcolour;
			gdk_gc_set_foreground(MainBackgroundGC, &gdkcolour);
			gdk_gc_set_foreground(DetailBackgroundGC, &gdkcolour);
			break;
		case COLOUR_BEACONS:
			User_Data.Colours[COLOUR_BEACONS] = gdkcolour;
			gdk_gc_set_foreground(MainBeaconsGC, &gdkcolour);
			gdk_gc_set_foreground(DetailBeaconsGC, &gdkcolour);
			break;
		case COLOUR_CITIES:
			User_Data.Colours[COLOUR_CITIES] = gdkcolour;
			gdk_gc_set_foreground(DetailCitiesGC, &gdkcolour);
			break;
		case COLOUR_LAND:
			User_Data.Colours[COLOUR_LAND] = gdkcolour;
			gdk_gc_set_foreground(MainLandGC, &gdkcolour);
			gdk_gc_set_foreground(DetailLandGC, &gdkcolour);
			break;
		case COLOUR_SQUARES:
			User_Data.Colours[COLOUR_SQUARES] = gdkcolour;
			gdk_gc_set_foreground(MainSquaresGC, &gdkcolour);
			break;
		case COLOUR_USER:
			User_Data.Colours[COLOUR_USER] = gdkcolour;
			gdk_gc_set_foreground(MainUserGC, &gdkcolour);
			gdk_gc_set_foreground(DetailUserGC, &gdkcolour);
			break;
		default:
			g_error("baken: unknown colour item %d\n", Colour_Element);
			break;
	}

	gtk_widget_destroy(colourseldlg);

	Draw_Main_Map();
	Redraw_Detail_Map();
}

void Allocate_Colours(void)
{
	static int First_Time = TRUE;

	if (First_Time) {
		Colour_Map = gdk_colormap_get_system();

		gdk_color_alloc(Colour_Map, &User_Data.Colours[COLOUR_BACKGROUND]);
		gdk_color_alloc(Colour_Map, &User_Data.Colours[COLOUR_BEACONS]);
		gdk_color_alloc(Colour_Map, &User_Data.Colours[COLOUR_CITIES]);
		gdk_color_alloc(Colour_Map, &User_Data.Colours[COLOUR_LAND]);
		gdk_color_alloc(Colour_Map, &User_Data.Colours[COLOUR_SQUARES]);
		gdk_color_alloc(Colour_Map, &User_Data.Colours[COLOUR_USER]);
		gdk_color_alloc(Colour_Map, &Text_Fg_Colour);
		gdk_color_alloc(Colour_Map, &Text_Bg_Colour);

		First_Time = FALSE;
	}
}
