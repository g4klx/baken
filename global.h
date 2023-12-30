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

#define	MAP_BEGIN_LONG	-10.0
#define	MAP_BEGIN_LAT	42.0

#define	MAP_END_LONG	26.0
#define	MAP_END_LAT	60.0

#define	MAIN_MAP_WIDTH		700
#define	MAIN_MAP_HEIGHT		500

#define	DETAIL_MAP_WIDTH	600
#define	DETAIL_MAP_HEIGHT	400
#define	DETAIL_MAP_TEXT_HEIGHT	53

#define	TEXT_LINE1	15
#define	TEXT_LINE2	30
#define	TEXT_LINE3	45

#define	TEXT_CALL	5
#define	TEXT_FREQUENCY	150
#define	TEXT_TYPE	200
#define	TEXT_LOCATOR	300
#define	TEXT_HEIGHT	425

#define	TEXT_QTH	5
#define	TEXT_ANTENNA	150
#define	TEXT_POWER	425

#define	TEXT_BEARING	5
#define	TEXT_DISTANCE	150
#define	TEXT_OPERATOR	300

#define	BORDER_WIDTH	10
#define	BORDER_ITEM	5

#define	LIST_WIDTH	450
#define	LIST_HEIGHT	400

#define	BAND_50MHZ	0
#define	BAND_70MHZ	1
#define	BAND_144MHZ	2
#define	BAND_432MHZ	3
#define	BAND_1_3GHZ	4
#define	BAND_2_3GHZ	5
#define	BAND_3_4GHZ	6
#define	BAND_5_7GHZ	7
#define	BAND_10GHZ	8
#define	BAND_24GHZ	9
#define	BAND_47GHZ	10
#define	BAND_76GHZ	11
#define	MAX_BANDS	12

#define	TYPE_IARU	0
#define	TYPE_NONIARU	1
#define	TYPE_REPEATER	2
#define	TYPE_LINEAR	3
#define	TYPE_MAILBOX	4
#define	TYPE_TV		5
#define	TYPE_DXCLUSTER	6
#define	TYPE_NODE	7
#define	TYPE_RS		8
#define	TYPE_MISC	9
#define	MAX_TYPES	10

#define	LIST_FREQUENCY		0
#define	LIST_DISTANCE		1
#define	LIST_BEARING		2

#define	COLOUR_BACKGROUND	0
#define	COLOUR_BEACONS		1
#define	COLOUR_CITIES		2
#define	COLOUR_LAND		3
#define	COLOUR_SQUARES		4
#define	COLOUR_USER		5

#ifndef TRUE
#define	TRUE		1
#endif

#ifndef FALSE
#define	FALSE		0
#endif

#define	FIELD_WIDTH	20.0			/* A - R */
#define	FIELD_HEIGHT	10.0			/* A - R */
#define	SQUARE_WIDTH	(FIELD_WIDTH / 10.0)	/* 0 - 9 */
#define	SQUARE_HEIGHT	(FIELD_HEIGHT / 10.0)	/* 0 - 9 */
#define	SUB_WIDTH	(SQUARE_WIDTH / 24.0)	/* A - X */
#define	SUB_HEIGHT	(SQUARE_HEIGHT / 24.0)	/* A - X */
#define	SUB_SUB_WIDTH	(SUB_WIDTH / 10.0)	/* 0 - 9 */
#define	SUB_SUB_HEIGHT	(SUB_HEIGHT / 10.0)	/* 0 - 9 */

#ifndef M_PI
#define	M_PI		3.141592654
#endif

/* International ellipsoid of 1924 */
#define	RA		6378.388		/* Semi-major axis */
#define	RB		6356.912		/* Semi-minor axis */

struct Beacon_Struct {
	int    Type;
	char   Callsign[21];
	double Frequency;
	char   Locator[10];
	double Latitude;
	double Longitude;
	char   QTH[51];
	char   Antenna[51];
	int    Angle[10];
	int    Angle_Count;
	int    Height;
	char   Power[51];
};

struct User_Struct {
	char   Callsign[21];
	char   Locator[10];
	double Latitude;
	double Longitude;
	int    Bands[MAX_BANDS];
	int    Types[MAX_TYPES];
	GdkColor Colours[6];
};

struct City_Struct {
	char   Name[51];
	double Latitude;
	double Longitude;
};

/************************************************************************/
/* In Baken.C								*/
/************************************************************************/

extern GSList *Beacon_List;

extern struct User_Struct User_Data;

extern GSList *City_List;

extern GtkWidget *MainMap;

extern void Error_Box(char *);

extern void Warning_Box(char *);

extern void Message_Box(char *);

extern void Info_Box(char *);

extern void YN_Box(char *, void (*)(void), void (*)(void));

extern int  Check_Beacon(struct Beacon_Struct *);

/************************************************************************/
/* In Choose.C								*/
/************************************************************************/

extern void ChooseCb(GtkWidget *, gpointer);

/************************************************************************/
/* In Colour.C								*/
/************************************************************************/

extern GdkColor Text_Bg_Colour;
extern GdkColor Text_Fg_Colour;

extern void ColourCb(GtkWidget *, gpointer);

extern void Allocate_Colours(void);

/************************************************************************/
/* In Data_Ed.C								*/
/************************************************************************/

extern void EditCb(GtkWidget *, gpointer);

/************************************************************************/
/* In Distance.C							*/
/************************************************************************/

extern void DistCb(GtkWidget *, gpointer);

/************************************************************************/
/* In Reflection.C							*/
/************************************************************************/

extern void ReflCb(GtkWidget *, gpointer);

/************************************************************************/
/* In Get_Data.C							*/
/************************************************************************/

extern void Read_User_Data(void);

extern void Read_Beacon_Data(int, char *);

extern void Read_City_Data(void);

extern int  Write_User_Data(void);

/************************************************************************/
/* In List.C								*/
/************************************************************************/

extern void ListCb(GtkWidget *, gpointer);

/************************************************************************/
/* In Main_Map.C							*/
/************************************************************************/

extern GdkGC *MainBackgroundGC;
extern GdkGC *MainBeaconsGC;
extern GdkGC *MainLandGC;
extern GdkGC *MainSquaresGC;
extern GdkGC *MainUserGC;

extern void Draw_Main_Map(void);

extern gint MainMapExposeCb(GtkWidget *, GdkEventExpose *);

extern gint MainMapClickCb(GtkWidget *, GdkEventButton *);

/************************************************************************/
/* In Utils.C								*/
/************************************************************************/

extern double RAD(double);

extern double DEG(double);

extern double mod(double, double);

extern int Convert_Locator(char *, double *, double *);

extern int  Convert_Angles(double, double, char *);

extern void Calc_Distance_Bearing(double, double, double, double, double *, double *);

extern int  Calc_Scatter_Point(double, double, double, double, double, double, double *, double *);

/************************************************************************/
/* In Detail_Map.C							*/
/************************************************************************/

extern GdkGC *DetailBackgroundGC;
extern GdkGC *DetailBeaconsGC;
extern GdkGC *DetailCitiesGC;
extern GdkGC *DetailLandGC;
extern GdkGC *DetailUserGC;

extern GdkGC *DetailTextFgGC;
extern GdkGC *DetailTextBgGC;

extern void Draw_Detail_Map(double, double);

extern void Redraw_Detail_Map(void);
