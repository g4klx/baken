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
#include <math.h>
#include <unistd.h>

#include <sys/stat.h>

#include <gtk/gtk.h>

#include "global.h"

GdkGC *MainBackgroundGC;
GdkGC *MainBeaconsGC;
GdkGC *MainLandGC;
GdkGC *MainSquaresGC;
GdkGC *MainUserGC;

static GdkPixmap *MapData;

static double X_Scale;
static double Y_Scale;

static void Calc_X_Scale(void)
{
	if (MAP_END_LONG > 0.0)
		X_Scale = (double)MAIN_MAP_WIDTH / (MAP_END_LONG - MAP_BEGIN_LONG);
	else
		X_Scale = (double)MAIN_MAP_WIDTH / fabs(MAP_BEGIN_LONG - MAP_END_LONG);
}

static void Calc_Y_Scale(void)
{
	if (MAP_END_LAT > 0.0)
		Y_Scale = (double)MAIN_MAP_HEIGHT / (MAP_END_LAT - MAP_BEGIN_LAT);
	else
		Y_Scale = (double)MAIN_MAP_HEIGHT / fabs(MAP_BEGIN_LAT - MAP_END_LAT);
}

static int Long_To_X(double Longitude)
{
	int x;

	if (Longitude > 180.0)  Longitude -= 360.0;
	if (Longitude < -180.0) Longitude += 360.0;

	x = (int)(X_Scale * (Longitude - MAP_BEGIN_LONG) + 0.5);

	return x;
}

static int Lat_To_Y(double Latitude)
{
	int y;

	y = MAIN_MAP_HEIGHT - (int)(Y_Scale * (Latitude - MAP_BEGIN_LAT) + 0.5);

	return y;
}

static double X_To_Long(int x)
{
	double longitude;

	longitude = MAP_BEGIN_LONG + (double)x / X_Scale;

	return longitude;
}

static double Y_To_Lat(int y)
{
	double latitude;

	latitude = MAP_BEGIN_LAT + (double)(MAIN_MAP_HEIGHT - y) / Y_Scale;

	return latitude;
}

static int X_In_Range(int x)
{
	return x >= 0 && x < MAIN_MAP_WIDTH;
}

static int Y_In_Range(int y)
{
	return y >= 0 && y < MAIN_MAP_HEIGHT;
}

static void Draw_Map(void)
{
	static int ignore = FALSE;
	unsigned char Buffer[101];
	double last_lat, last_long;
	double llat, llong;
	int last_x = 0, last_y = 0;
	int x = 0, y = 0;
	short *n, *slat, *slong;
	FILE *fp;

	sprintf(Buffer, "%s/mapfile", MAPDIR);

	if ((fp = fopen(Buffer, "r")) == NULL) {
		Error_Box("Cannot open Map file");
		return;
	}

	while (fread(Buffer, 6, 1, fp) == 1) {
		n     = (short *)(Buffer + 0);
		slat  = (short *)(Buffer + 2);
		slong = (short *)(Buffer + 4);

		llat  = (double)*slat  / 60.0;
		llong = (double)*slong / 60.0;

		if (*n >= 7000) {
			ignore = TRUE;
		} else if (*n >= 6000) {
			ignore = FALSE;
		} else if (*n >= 5000) {
			ignore = FALSE;
		} else if (*n >= 4000) {
			ignore = TRUE;
		} else if (*n >= 3000) {
			ignore = TRUE;
		} else if (*n >= 2000) {
			ignore = FALSE;
		} else if (*n >= 1000) {
			ignore = FALSE;
		}

		if (ignore)
			continue;

		x = Long_To_X(llong);
		y = Lat_To_Y(llat);

		if (*n >= 1000) {
			last_lat  = llat;
			last_long = llong;
			last_x = x;
			last_y = y;
			continue;
		}

		if (abs(last_x - x) > MAIN_MAP_WIDTH / 2 ||
		    abs(last_y - y) > 50) {
			last_lat  = llat;
			last_long = llong;
			last_x = x;
			last_y = y;
			continue;
		}

		if ((X_In_Range(last_x) && Y_In_Range(last_y)) || (X_In_Range(x) && Y_In_Range(y)))
			gdk_draw_line(MapData, MainLandGC, last_x, last_y, x, y);

		last_lat  = llat;
		last_long = llong;
		last_x = x;
		last_y = y;
	}

	fclose(fp);
}

static void Draw_Squares(void)
{
	int i, x, y;

	for (i = -180; i < 180; i += 20) {
		x = Long_To_X((double)i);

		if (X_In_Range(x)) {
			gdk_draw_line(MapData, MainSquaresGC, x, 0, x, MAIN_MAP_HEIGHT);
			gdk_draw_line(MapData, MainSquaresGC, x - 1, 0, x - 1, MAIN_MAP_HEIGHT);
		}
	}

	for (i = -90; i < 90; i +=10) {
		y = Lat_To_Y((double)i);

		if (Y_In_Range(y)) {
			gdk_draw_line(MapData, MainSquaresGC, 0, y, MAIN_MAP_WIDTH, y);
			gdk_draw_line(MapData, MainSquaresGC, 0, y - 1, MAIN_MAP_WIDTH, y - 1);
		}
	}

	for (i = -180; i < 180; i += 2) {
		x = Long_To_X((double)i);

		if (X_In_Range(x))
			gdk_draw_line(MapData, MainSquaresGC, x, 0, x, MAIN_MAP_HEIGHT);
	}

	for (i = -90; i < 90; i +=1) {
		y = Lat_To_Y((double)i);

		if (Y_In_Range(y))
			gdk_draw_line(MapData, MainSquaresGC, 0, y, MAIN_MAP_WIDTH, y);
	}
}

static void Draw_Home(void)
{
	int x, y;

	x = Long_To_X(User_Data.Longitude);
	y = Lat_To_Y(User_Data.Latitude);

	if (X_In_Range(x) && Y_In_Range(y))
		gdk_draw_rectangle(MapData, MainUserGC, TRUE, x - 1, y - 1, 3, 3);
}

static void Draw_Beacons(void)
{
	struct Beacon_Struct *Beacon_Data;
	GSList *Beacon;
	int x, y;

	Beacon = Beacon_List;

	while (Beacon != NULL) {
		Beacon_Data = (struct Beacon_Struct *)Beacon->data;

		if (Check_Beacon(Beacon_Data)) {
			x = Long_To_X(Beacon_Data->Longitude);
			y = Lat_To_Y(Beacon_Data->Latitude);

			if (X_In_Range(x) && Y_In_Range(y))
				gdk_draw_rectangle(MapData, MainBeaconsGC, TRUE, x, y, 2, 2);
		}

		Beacon = g_slist_next(Beacon);
	}
}

gint MainMapExposeCb(GtkWidget *w, GdkEventExpose *event)
{
	static int First_Time = TRUE;
	
	if (First_Time) {
		Allocate_Colours();
		Calc_X_Scale();
		Calc_Y_Scale();

		MainBackgroundGC = gdk_gc_new(MainMap->window);
		gdk_gc_set_foreground(MainBackgroundGC, &User_Data.Colours[COLOUR_BACKGROUND]);

		MainBeaconsGC = gdk_gc_new(MainMap->window);
		gdk_gc_set_foreground(MainBeaconsGC, &User_Data.Colours[COLOUR_BEACONS]);

		MainLandGC = gdk_gc_new(MainMap->window);
		gdk_gc_set_foreground(MainLandGC, &User_Data.Colours[COLOUR_LAND]);

		MainSquaresGC = gdk_gc_new(MainMap->window);
		gdk_gc_set_foreground(MainSquaresGC, &User_Data.Colours[COLOUR_SQUARES]);

		MainUserGC = gdk_gc_new(MainMap->window);
		gdk_gc_set_foreground(MainUserGC, &User_Data.Colours[COLOUR_USER]);

		MapData = gdk_pixmap_new(MainMap->window, MAIN_MAP_WIDTH, MAIN_MAP_HEIGHT, -1);

		Draw_Main_Map();

		First_Time = FALSE;
	} else {
		gdk_draw_pixmap(MainMap->window, MainBackgroundGC, MapData, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	}

	return FALSE;
}

void Draw_Main_Map(void)
{
	gdk_draw_rectangle(MapData, MainBackgroundGC, TRUE, 0, 0, MAIN_MAP_WIDTH, MAIN_MAP_HEIGHT);

	Draw_Map();

	Draw_Squares();

	Draw_Home();

	Draw_Beacons();

	gdk_draw_pixmap(MainMap->window, MainBackgroundGC, MapData, 0, 0, 0, 0, MAIN_MAP_WIDTH, MAIN_MAP_HEIGHT);
}

gint MainMapClickCb(GtkWidget *widget, GdkEventButton *event)
{
	double Latitude, Longitude;

	if (event->button != 1)
		return TRUE;

	Latitude  = Y_To_Lat((int)event->y);
	Longitude = X_To_Long((int)event->x);

	Draw_Detail_Map(Latitude, Longitude);

	return TRUE;
}
