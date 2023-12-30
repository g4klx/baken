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
#include <math.h>
#include <unistd.h>

#include <sys/stat.h>

#include <gtk/gtk.h>

#include "global.h"

GdkGC *DetailBackgroundGC;
GdkGC *DetailBeaconsGC;
GdkGC *DetailCitiesGC;
GdkGC *DetailLandGC;
GdkGC *DetailUserGC;
GdkGC *DetailTextBackGC;
GdkGC *DetailTextForeGC;

static int       Visible       = FALSE;

static GtkWidget *DetailWindow = NULL;
static GtkWidget *DetailMap    = NULL;
static GtkWidget *DetailText   = NULL;

static GdkFont   *MapFont;
static GdkFont   *TextFont;

static GdkPixmap *MapData;
static GdkPixmap *TextData;

static double Begin_Latitude;
static double Begin_Longitude;

static double X_Scale;
static double Y_Scale;

static void Draw_Map(void);
static void Draw_Arrow(int, int, int);

static int Normalise_X_Text(int x, char *text)
{
	int width;

	width = gdk_string_width(MapFont, text);

	if ((x + width / 2) > DETAIL_MAP_WIDTH)
		return DETAIL_MAP_WIDTH - width;

	if ((x - width / 2) < 0)
		return 0;

	return x - width / 2;
}

static int Normalise_Y_Text(int y, char *text)
{
	int height;

	height = gdk_string_height(MapFont, text) + 3;

	if ((y + height + 3) > DETAIL_MAP_HEIGHT)
		return y - 6;

	return y + height + 3;
}

static void Calc_X_Scale(void)
{
	X_Scale = (double)DETAIL_MAP_WIDTH / SQUARE_WIDTH;
}

static void Calc_Y_Scale(void)
{
	Y_Scale = (double)DETAIL_MAP_HEIGHT / SQUARE_HEIGHT;
}

static int Long_To_X(double Longitude)
{
	int x;

	if (Longitude > 180.0)  Longitude -= 360.0;
	if (Longitude < -180.0) Longitude += 360.0;

	x = (int)(X_Scale * (Longitude - Begin_Longitude) + 0.5);

	return x;
}

static int Lat_To_Y(double Latitude)
{
	int y;

	y = DETAIL_MAP_HEIGHT - (int)(Y_Scale * (Latitude - Begin_Latitude) + 0.5);

	return y;
}

static double X_To_Long(int x)
{
	double longitude;

	longitude = Begin_Longitude + (double)x / X_Scale;

	return longitude;
}

static double Y_To_Lat(int y)
{
	double latitude;

	latitude = Begin_Latitude + (double)(DETAIL_MAP_HEIGHT - y) / Y_Scale;

	return latitude;
}

static int X_In_Range(int x)
{
	return x >= 0 && x < DETAIL_MAP_WIDTH;
}

static int Y_In_Range(int y)
{
	return y >= 0 && y < DETAIL_MAP_HEIGHT;
}

static void Draw_Map(void)
{
	static int ignore = FALSE;
	char Buffer[101];
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

		if (abs(last_x - x) > DETAIL_MAP_WIDTH / 2) {
			last_lat  = llat;
			last_long = llong;
			last_x = x;
			last_y = y;
			continue;
		}

		if ((X_In_Range(last_x) && Y_In_Range(last_y)) || (X_In_Range(x) && Y_In_Range(y)))
			gdk_draw_line(MapData, DetailLandGC, last_x, last_y, x, y);

		last_lat  = llat;
		last_long = llong;
		last_x = x;
		last_y = y;
	}

	fclose(fp);
}

static void Draw_Home(void)
{
	int x, y;

	x = Long_To_X(User_Data.Longitude);
	y = Lat_To_Y(User_Data.Latitude);

	if (X_In_Range(x) && Y_In_Range(y)) {
		gdk_draw_rectangle(MapData, DetailUserGC, TRUE, x - 2, y - 2, 4, 4);

		x = Normalise_X_Text(x, User_Data.Callsign);
		y = Normalise_Y_Text(y, User_Data.Callsign);

		gdk_draw_string(MapData, MapFont, DetailUserGC, x, y, User_Data.Callsign);
	}
}

static void Draw_Cities(void)
{
	struct City_Struct *City_Data;
	GSList *City;
	int x, y;

	City = City_List;

	while (City != NULL) {
		City_Data = (struct City_Struct *)City->data;

		x = Long_To_X(City_Data->Longitude);
		y = Lat_To_Y(City_Data->Latitude);

		if (X_In_Range(x) && Y_In_Range(y)) {
			gdk_draw_rectangle(MapData, DetailCitiesGC, TRUE, x - 2, y - 2, 4, 4);

			x = Normalise_X_Text(x, City_Data->Name);
			y = Normalise_Y_Text(y, City_Data->Name);

			gdk_draw_string(MapData, MapFont, DetailCitiesGC, x, y, City_Data->Name);
		}

		City = g_slist_next(City);
	}
}

static void Draw_Beacons(void)
{
	struct Beacon_Struct *Beacon_Data;
	GSList *Beacon;
	int i, x, y;

	Beacon = Beacon_List;

	while (Beacon != NULL) {
		Beacon_Data = (struct Beacon_Struct *)Beacon->data;

		if (Check_Beacon(Beacon_Data)) {
			x = Long_To_X(Beacon_Data->Longitude);
			y = Lat_To_Y(Beacon_Data->Latitude);

			if (X_In_Range(x) && Y_In_Range(y)) {
				gdk_draw_rectangle(MapData, DetailBeaconsGC, TRUE, x - 3, y - 3, 6, 6);

				for (i = 0; i < Beacon_Data->Angle_Count; i++)
					Draw_Arrow(x, y, Beacon_Data->Angle[i]);

				x = Normalise_X_Text(x, Beacon_Data->Callsign);
				y = Normalise_Y_Text(y, Beacon_Data->Callsign);

				gdk_draw_string(MapData, MapFont, DetailBeaconsGC, x, y, Beacon_Data->Callsign);
			}
		}

		Beacon = g_slist_next(Beacon);
	}
}

static void Draw_Arrow(int x, int y, int angle)
{
	int line_end_x, line_end_y;
	int back_arrow_x, back_arrow_y;
	double sin_angle, cos_angle;
	int x_len, y_len;
	GdkPoint points[3];

	sin_angle = sin(RAD((double)angle));
	cos_angle = cos(RAD((double)angle));

	line_end_x = x + (int)(14.0 * sin_angle + 0.5);
	line_end_y = y - (int)(14.0 * cos_angle + 0.5);

	gdk_draw_line(MapData, DetailBeaconsGC, x, y, line_end_x, line_end_y);

	back_arrow_x = x + (int)(7.0 * sin_angle + 0.5);
	back_arrow_y = y - (int)(7.0 * cos_angle + 0.5);

	x_len = (int)(3.0 * cos_angle + 0.5);
	y_len = (int)(3.0 * sin_angle + 0.5);

	points[0].x = back_arrow_x - x_len;
	points[0].y = back_arrow_y - y_len;

	points[1].x = line_end_x;
	points[1].y = line_end_y;

	points[2].x = back_arrow_x + x_len;
	points[2].y = back_arrow_y + y_len;

	gdk_draw_polygon(MapData, DetailBeaconsGC, TRUE, points, 3);
}

static gint DetailMapDestroyCb(GtkWidget *w, gpointer data)
{
	gtk_widget_destroy(DetailWindow);

	gdk_pixmap_unref(MapData);
	gdk_pixmap_unref(TextData);

	MapData  = NULL;
	TextData = NULL;

	Visible = FALSE;

	return TRUE;
}

static gint DetailTextExposeCb(GtkWidget *w, GdkEventExpose *event)
{
	if (TextData == NULL) {
		TextFont = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");

		DetailTextBackGC = gdk_gc_new(DetailText->window);
		gdk_gc_set_foreground(DetailTextBackGC, &Text_Bg_Colour);
	        gdk_gc_set_font(DetailTextBackGC, TextFont);

		DetailTextForeGC = gdk_gc_new(DetailText->window);
		gdk_gc_set_foreground(DetailTextForeGC, &Text_Fg_Colour);
	        gdk_gc_set_font(DetailTextForeGC, TextFont);

		TextData = gdk_pixmap_new(DetailText->window, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT, -1);
		gdk_draw_rectangle(TextData, DetailTextBackGC, TRUE, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);
		gdk_draw_pixmap(DetailText->window, DetailTextBackGC, TextData, 0, 0, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);
	} else {
		gdk_draw_pixmap(DetailText->window, DetailTextBackGC, TextData, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	}

	return FALSE;
}

static gint DetailMapMotionCb(GtkWidget *widget, GdkEventMotion *event)
{
	static struct Beacon_Struct *Last_Beacon;
	struct Beacon_Struct *Beacon_Data, *Best_Beacon;
	GSList *Beacon;
	double Latitude, Longitude;
	double Bearing, Distance;
	double Best_Distance;
	char buf[51], buffer[101];
	int i;

	Latitude  = Y_To_Lat(event->y);
	Longitude = X_To_Long(event->x);

	Beacon = Beacon_List;
	Best_Beacon   = NULL;
	Best_Distance = 9999.0;

	while (Beacon != NULL) {
		Beacon_Data = (struct Beacon_Struct *)Beacon->data;

		if (Check_Beacon(Beacon_Data) &&
		    Beacon_Data->Latitude  >= Begin_Latitude &&
		    Beacon_Data->Latitude  <= Begin_Latitude + SQUARE_HEIGHT &&
		    Beacon_Data->Longitude >= Begin_Longitude &&
		    Beacon_Data->Longitude <= Begin_Longitude + SQUARE_WIDTH) {
			Calc_Distance_Bearing(Latitude, Longitude, Beacon_Data->Latitude, Beacon_Data->Longitude, &Bearing, &Distance);

			if (Distance < Best_Distance && Distance < 20.0) {
				Best_Beacon   = Beacon_Data;
				Best_Distance = Distance;
			}
		}

		Beacon = g_slist_next(Beacon);
	}

	if (Best_Beacon != Last_Beacon) {
		if (Best_Beacon != NULL) {
			Calc_Distance_Bearing(User_Data.Latitude, User_Data.Longitude, Best_Beacon->Latitude, Best_Beacon->Longitude, &Bearing, &Distance);

			gdk_draw_rectangle(TextData, DetailTextBackGC, TRUE, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);

			sprintf(buffer, "Call: %-10s", Best_Beacon->Callsign);
			gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_CALL, TEXT_LINE1, buffer);

			sprintf(buffer, "Freq: %.3f MHz", Best_Beacon->Frequency);
			gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_FREQUENCY, TEXT_LINE1, buffer);

			sprintf(buffer, "Loc: %-6s", Best_Beacon->Locator);
			gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_LOCATOR, TEXT_LINE1, buffer);

			if (Best_Beacon->Height > 0) {
				sprintf(buffer, "Height: %dm ASL", Best_Beacon->Height);
				gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_HEIGHT, TEXT_LINE1, buffer);
			}

			if (strlen(Best_Beacon->QTH) > 0) {
				sprintf(buffer, "QTH: %-20s", Best_Beacon->QTH);
				gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_QTH, TEXT_LINE2, buffer);
			}

			buffer[0] = '\0';

			if (strlen(Best_Beacon->Antenna) > 0) {
				sprintf(buf, "Ant: %s", Best_Beacon->Antenna);
				strcat(buffer, buf);
			}

			if (Best_Beacon->Angle_Count == 0) {
				strcat(buffer, " - Omni");
			} else {
				strcat(buffer, " - ");

				for (i = 0; i < Best_Beacon->Angle_Count; i++) {
					sprintf(buf, "%d", Best_Beacon->Angle[i]);
					if (i > 0) strcat(buffer, "/");
					strcat(buffer, buf);
				}

				strcat(buffer, " deg");
			}

			if (strlen(buffer) > 0)
				gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_ANTENNA, TEXT_LINE2, buffer);

			if (strlen(Best_Beacon->Power) > 0) {
				sprintf(buffer, "Power: %s Watts", Best_Beacon->Power);
				gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_POWER, TEXT_LINE2, buffer);
			}

			sprintf(buffer, "Bearing: %.0f deg", Bearing);
			gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_BEARING, TEXT_LINE3, buffer);

			sprintf(buffer, "Distance: %.0f kms", Distance);
			gdk_draw_string(TextData, TextFont, DetailTextForeGC, TEXT_DISTANCE, TEXT_LINE3, buffer);

			gdk_draw_pixmap(DetailText->window, DetailTextBackGC, TextData, 0, 0, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);
		} else {
			gdk_draw_rectangle(TextData, DetailTextBackGC, TRUE, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);
			gdk_draw_pixmap(DetailText->window, DetailTextBackGC, TextData, 0, 0, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);
		}

		Last_Beacon = Best_Beacon;
	}

	return TRUE;
}

static gint DetailMapExposeCb(GtkWidget *w, GdkEventExpose *event)
{
	if (MapData == NULL) {
		Calc_X_Scale();
		Calc_Y_Scale();

		MapFont = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");

		DetailBackgroundGC = gdk_gc_new(DetailMap->window);
		gdk_gc_set_foreground(DetailBackgroundGC, &User_Data.Colours[COLOUR_BACKGROUND]);

		DetailBeaconsGC = gdk_gc_new(DetailMap->window);
		gdk_gc_set_foreground(DetailBeaconsGC, &User_Data.Colours[COLOUR_BEACONS]);
	        gdk_gc_set_font(DetailBeaconsGC, MapFont);

		DetailCitiesGC = gdk_gc_new(DetailMap->window);
		gdk_gc_set_foreground(DetailCitiesGC, &User_Data.Colours[COLOUR_CITIES]);
	        gdk_gc_set_font(DetailCitiesGC, MapFont);

		DetailLandGC = gdk_gc_new(DetailMap->window);
		gdk_gc_set_foreground(DetailLandGC, &User_Data.Colours[COLOUR_LAND]);

		DetailUserGC = gdk_gc_new(DetailMap->window);
		gdk_gc_set_foreground(DetailUserGC, &User_Data.Colours[COLOUR_USER]);
	        gdk_gc_set_font(DetailUserGC, MapFont);

		MapData = gdk_pixmap_new(DetailMap->window, DETAIL_MAP_WIDTH, DETAIL_MAP_HEIGHT, -1);

		Redraw_Detail_Map();
	} else {
		gdk_draw_pixmap(DetailMap->window, DetailBackgroundGC, MapData, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	}

	return FALSE;
}

void Draw_Detail_Map(double Latitude, double Longitude)
{
	GtkWidget *vbox;
	GtkWidget *separator;
	char Locator[9];
	char buffer[51];

	if (!Convert_Angles(Latitude, Longitude, Locator)) {
		g_error("baken: nonsense Lat & Long, %f & %f", Latitude, Longitude);
		return;
	}

	Locator[4] = 'A';
	Locator[5] = 'A';

	Convert_Locator(Locator, &Begin_Latitude, &Begin_Longitude);

	Locator[4] = '\0';

	Begin_Latitude  -= SUB_HEIGHT / 2.0;
	Begin_Longitude -= SUB_WIDTH / 2.0;

	if (Visible) {
		gtk_window_activate_focus(GTK_WINDOW(DetailWindow));
		Redraw_Detail_Map();
	} else {
		DetailWindow = gtk_window_new(GTK_WINDOW_DIALOG);

		gtk_signal_connect(GTK_OBJECT(DetailWindow), "destroy", (GtkSignalFunc)DetailMapDestroyCb, NULL);

		gtk_container_set_border_width(GTK_CONTAINER(DetailWindow), 0);

		vbox = gtk_vbox_new(FALSE, 1);
		gtk_container_border_width(GTK_CONTAINER(vbox), 1);
		gtk_container_add(GTK_CONTAINER(DetailWindow), vbox);

		DetailMap = gtk_drawing_area_new();
		gtk_drawing_area_size(GTK_DRAWING_AREA(DetailMap), DETAIL_MAP_WIDTH, DETAIL_MAP_HEIGHT);
		gtk_widget_set_events(DetailMap, GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK);
		gtk_signal_connect(GTK_OBJECT(DetailMap), "expose_event", (GtkSignalFunc)DetailMapExposeCb, NULL);
		gtk_signal_connect(GTK_OBJECT(DetailMap), "motion_notify_event", (GtkSignalFunc)DetailMapMotionCb, NULL);
		gtk_box_pack_start(GTK_BOX(vbox), DetailMap, FALSE, TRUE, 0);
		gtk_widget_show(DetailMap);

		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, TRUE, 0);
		gtk_widget_show(separator);

		DetailText = gtk_drawing_area_new();
		gtk_drawing_area_size(GTK_DRAWING_AREA(DetailText), DETAIL_MAP_WIDTH, DETAIL_MAP_TEXT_HEIGHT);
		gtk_signal_connect(GTK_OBJECT(DetailText), "expose_event", (GtkSignalFunc)DetailTextExposeCb, NULL);
		gtk_box_pack_start(GTK_BOX(vbox), DetailText, FALSE, TRUE, 0);
		gtk_widget_show(DetailText);

		gtk_widget_show(vbox);
		gtk_widget_show(DetailWindow);
	}

	sprintf(buffer, "Detail of Locator %s", Locator);
	gtk_window_set_title(GTK_WINDOW(DetailWindow), buffer);

	Visible = TRUE;
}

void Redraw_Detail_Map(void)
{
	if (Visible) {
		gdk_draw_rectangle(MapData, DetailBackgroundGC, TRUE, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_HEIGHT);

		Draw_Map();
		Draw_Cities();
		Draw_Home();
		Draw_Beacons();

		gdk_draw_pixmap(DetailMap->window, DetailBackgroundGC, MapData, 0, 0, 0, 0, DETAIL_MAP_WIDTH, DETAIL_MAP_HEIGHT);
	}
}
