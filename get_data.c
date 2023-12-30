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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "global.h"

#define	USER_NONE	0
#define	USER_DATA	1
#define	USER_BANDS	2
#define	USER_TYPES	3
#define	USER_COLOUR	4

void Read_City_Data(void)
{
	char Buffer[120], *t[2];
	char File_Name[81], *s;
	struct City_Struct Temp_Data, *City;
	FILE *fp;

	sprintf(File_Name, "%s/cityfile", MAPDIR);

	if ((fp = fopen(File_Name, "r")) == NULL) {
		Warning_Box("Cannot open the City file");
		return;
	}

	while(fgets(Buffer, 119, fp) != NULL) {
		if (Buffer[0] == '[') {
			if ((s = strchr(Buffer, ']')) == NULL)
				g_error("baken: malformed line %s in %s\n", Buffer, File_Name);
			*s = '\0';

			strcpy(Temp_Data.Name, Buffer + 1);
		} else {
			if ((t[0] = strtok(Buffer, "=")) == NULL)
				continue;
			if ((t[1] = strtok(NULL, "\r\n")) == NULL)
				continue;

			if (strcmp(t[0], "latitude") == 0) {
				Temp_Data.Latitude = atof(t[1]);
			} else if (strcmp(t[0], "longitude") == 0) {
				Temp_Data.Longitude = atof(t[1]);
			} else if (strcmp(t[0], "height") == 0) {
				if (Temp_Data.Latitude  >= MAP_BEGIN_LAT  && Temp_Data.Latitude   <  MAP_END_LAT &&
				    Temp_Data.Longitude >= MAP_BEGIN_LONG && Temp_Data.Longitude  <  MAP_END_LONG) {
					City  = g_malloc(sizeof(struct City_Struct));
					*City = Temp_Data;
					City_List = g_slist_append(City_List, City);
				}
			} else {
				fprintf(stderr, "baken: unknown keyword %s in %s\n", t[0], File_Name);
			}
		}
	}

	fclose(fp);
}

static gint Beacon_Comparison(gconstpointer d1, gconstpointer d2)
{
	const struct Beacon_Struct *First  = (struct Beacon_Struct *)d1;
	const struct Beacon_Struct *Second = (struct Beacon_Struct *)d2;

	return (gint)(First->Frequency * 1000.0 - Second->Frequency * 1000.0);
}

void Read_Beacon_Data(int type, char *filename)
{
	char Buffer[255], File_Name[81], *s, *p, *t[2];
	double Latitude, Longitude;
	struct Beacon_Struct Temp_Data, *Beacon = NULL;
	int valid = FALSE;
	FILE *fp;

	sprintf(File_Name, "%s/%s", VARDIR, filename);

	if ((fp = fopen(File_Name, "r")) == NULL) {
		fprintf(stderr, "baken: no file named %s found\n", File_Name);
		return;
	}

	while (fgets(Buffer, 254, fp) != NULL) {
		if (Buffer[0] == '#')
			continue;
	
		if (Buffer[0] == '[') {
			if (valid) {
				Beacon = g_malloc(sizeof(struct Beacon_Struct));
				*Beacon = Temp_Data;
				Beacon_List = g_slist_insert_sorted(Beacon_List, Beacon, Beacon_Comparison);
			}

			if ((s = strchr(Buffer, ']')) == NULL)
				g_error("baken: malformed line %s in %s\n", Buffer, File_Name);
			*s = '\0';

			memset(&Temp_Data, 0x00, sizeof(struct Beacon_Struct));
			strcpy(Temp_Data.Callsign, Buffer + 1);
			Temp_Data.Type = type;

			valid = FALSE;
		} else {
			if ((t[0] = strtok(Buffer, "=")) == NULL)
				continue;
			if ((t[1] = strtok(NULL, "\r\n")) == NULL)
				continue;

			if (strcmp(t[0], "frequency") == 0) {
				Temp_Data.Frequency = atof(t[1]);
			} else if (strcmp(t[0], "locator") == 0) {
				strcpy(Temp_Data.Locator, t[1]);

				if (Convert_Locator(Temp_Data.Locator, &Latitude, &Longitude)) {
					Temp_Data.Latitude  = Latitude;
					Temp_Data.Longitude = Longitude;

					if (Temp_Data.Latitude  >= MAP_BEGIN_LAT  && Temp_Data.Latitude   <  MAP_END_LAT &&
					    Temp_Data.Longitude >= MAP_BEGIN_LONG && Temp_Data.Longitude  <  MAP_END_LONG)
						valid = TRUE;
				}
			} else if (strcmp(t[0], "qth") == 0) {
				strcpy(Temp_Data.QTH, t[1]);
			} else if (strcmp(t[0], "antenna") == 0) {
				strcpy(Temp_Data.Antenna, t[1]);
			} else if (strcmp(t[0], "direction") == 0) {
				s = t[1];
				while ((p = strtok(s, ",")) != NULL) {
					Temp_Data.Angle[Temp_Data.Angle_Count] = atoi(p);
					Temp_Data.Angle_Count++;
					s = NULL;
				}
			} else if (strcmp(t[0], "height") == 0) {
				Temp_Data.Height = atoi(t[1]);
			} else if (strcmp(t[0], "power") == 0) {
				strcpy(Temp_Data.Power, t[1]);
			} else if (strcmp(t[0], "operator") == 0) {
				/* ignore */
			} else if (strcmp(t[0], "mode") == 0) {
				/* ignore */
			} else {
				fprintf(stderr, "baken: unknown keyword %s in %s\n", t[0], File_Name);
			}
		}
	}

	if (valid) {
		Beacon = g_malloc(sizeof(struct Beacon_Struct));
		*Beacon = Temp_Data;
		Beacon_List = g_slist_insert_sorted(Beacon_List, Beacon, Beacon_Comparison);
	}

	fclose(fp);
}

void Read_User_Data(void)
{
	char Buffer[120], *t[5];
	char File_Name[81], *s;
	int  Found = FALSE;
	FILE *fp = NULL;
	int  state = USER_NONE;
	int i;

	if ((s = getenv("HOME")) != NULL) {
		sprintf(File_Name, "%s/.bakenrc", s);

		if ((fp = fopen(File_Name, "r")) != NULL)
			Found = TRUE;
	}

	if (!Found) {
		sprintf(File_Name, "%s/bakenrc", RCDIR);

		if ((fp = fopen(File_Name, "r")) == NULL)
			return;
	}

	while (fgets(Buffer, 119, fp) != NULL) {
		if ((s = strchr(Buffer, '\n')) != NULL) *s = '\0';
		if ((s = strchr(Buffer, '\r')) != NULL) *s = '\0';

		if (strcmp(Buffer, "[User]") == 0) {
			state = USER_DATA;
			continue;
		}
		if (strcmp(Buffer, "[Bands]") == 0) {
			state = USER_BANDS;
			continue;
		}
		if (strcmp(Buffer, "[Types]") == 0) {
			state = USER_TYPES;
			continue;
		}
		if (strcmp(Buffer, "[Colours]") == 0) {
			state = USER_COLOUR;
			continue;
		}

		if ((t[0] = strtok(Buffer, "=")) == NULL)
			continue;
		if ((t[1] = strtok(NULL, "\r\n")) == NULL)
			continue;

		switch (state) {
			case USER_NONE:
				break;

			case USER_DATA:
				if (strcmp(t[0], "callsign") == 0) {
					strcpy(User_Data.Callsign, t[1]);
				} else if (strcmp(t[0], "locator") == 0) {
					if (!Convert_Locator(t[1], &User_Data.Latitude, &User_Data.Longitude))
						g_error("baken: invalid locator - %s\n", t[1]);
					else
						strcpy(User_Data.Locator, t[1]);
				} else if (strcmp(t[0], "band") == 0) {
					/* Old configuration file */
					for (i = 0; i < MAX_BANDS; i++)
						User_Data.Bands[i] = TRUE;
				} else
					fprintf(stderr, "baken: unknown keyword %s in [User] in %s\n", t[0], File_Name);
				break;

			case USER_BANDS:
				if (strcmp(t[0], "50mhz") == 0)
					User_Data.Bands[BAND_50MHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "70mhz") == 0)
					User_Data.Bands[BAND_70MHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "144mhz") == 0)
					User_Data.Bands[BAND_144MHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "432mhz") == 0)
					User_Data.Bands[BAND_432MHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "1296mhz") == 0)
					User_Data.Bands[BAND_1_3GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "2320mhz") == 0)
					User_Data.Bands[BAND_2_3GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "3456mhz") == 0)
					User_Data.Bands[BAND_3_4GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "5760mhz") == 0)
					User_Data.Bands[BAND_5_7GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "10000mhz") == 0)
					User_Data.Bands[BAND_10GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "24000mhz") == 0)
					User_Data.Bands[BAND_24GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "47000mhz") == 0)
					User_Data.Bands[BAND_47GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "76000mhz") == 0)
					User_Data.Bands[BAND_76GHZ] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else
					fprintf(stderr, "baken: unknown keyword %s in [Bands] in %s\n", t[0], File_Name);
				break;

			case USER_TYPES:
				if (strcmp(t[0], "iaru") == 0)
					User_Data.Types[TYPE_IARU] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "noniaru") == 0)
					User_Data.Types[TYPE_NONIARU] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "repeater") == 0)
					User_Data.Types[TYPE_REPEATER] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "linear") == 0)
					User_Data.Types[TYPE_LINEAR] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "tv") == 0)
					User_Data.Types[TYPE_TV] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "mailbox") == 0)
					User_Data.Types[TYPE_MAILBOX] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "dxcluster") == 0)
					User_Data.Types[TYPE_DXCLUSTER] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "node") == 0)
					User_Data.Types[TYPE_NODE] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "rs") == 0)
					User_Data.Types[TYPE_RS] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else if (strcmp(t[0], "misc") == 0)
					User_Data.Types[TYPE_MISC] = (atoi(t[1]) != 0) ? TRUE : FALSE;
				else
					fprintf(stderr, "baken: unknown keyword %s in [Types] in %s\n", t[0], File_Name);
				break;

			case USER_COLOUR:
				if (strcmp(t[0], "background") == 0) {
					t[2] = strtok(t[1], ",");
					t[3] = strtok(NULL, ",");
					t[4] = strtok(NULL, "");

					if (t[2] == NULL || t[3] == NULL || t[4] == NULL)
						g_error("baken: cannot parse background colour data\n");

					User_Data.Colours[COLOUR_BACKGROUND].pixel = 0;
					User_Data.Colours[COLOUR_BACKGROUND].red   = atoi(t[2]);
					User_Data.Colours[COLOUR_BACKGROUND].green = atoi(t[3]);
					User_Data.Colours[COLOUR_BACKGROUND].blue  = atoi(t[4]);
				} else if (strcmp(t[0], "beacons") == 0) {
					t[2] = strtok(t[1], ",");
					t[3] = strtok(NULL, ",");
					t[4] = strtok(NULL, "");

					if (t[2] == NULL || t[3] == NULL || t[4] == NULL)
						g_error("baken: cannot parse beacons colour data\n");

					User_Data.Colours[COLOUR_BEACONS].pixel = 0;
					User_Data.Colours[COLOUR_BEACONS].red   = atoi(t[2]);
					User_Data.Colours[COLOUR_BEACONS].green = atoi(t[3]);
					User_Data.Colours[COLOUR_BEACONS].blue  = atoi(t[4]);
				} else if (strcmp(t[0], "cities") == 0) {
					t[2] = strtok(t[1], ",");
					t[3] = strtok(NULL, ",");
					t[4] = strtok(NULL, "");

					if (t[2] == NULL || t[3] == NULL || t[4] == NULL)
						g_error("baken: cannot parse cities colour data\n");

					User_Data.Colours[COLOUR_CITIES].pixel = 0;
					User_Data.Colours[COLOUR_CITIES].red   = atoi(t[2]);
					User_Data.Colours[COLOUR_CITIES].green = atoi(t[3]);
					User_Data.Colours[COLOUR_CITIES].blue  = atoi(t[4]);
				} else if (strcmp(t[0], "land") == 0) {
					t[2] = strtok(t[1], ",");
					t[3] = strtok(NULL, ",");
					t[4] = strtok(NULL, "");

					if (t[2] == NULL || t[3] == NULL || t[4] == NULL)
						g_error("baken: cannot parse land colour data\n");

					User_Data.Colours[COLOUR_LAND].pixel = 0;
					User_Data.Colours[COLOUR_LAND].red   = atoi(t[2]);
					User_Data.Colours[COLOUR_LAND].green = atoi(t[3]);
					User_Data.Colours[COLOUR_LAND].blue  = atoi(t[4]);
				} else if (strcmp(t[0], "squares") == 0) {
					t[2] = strtok(t[1], ",");
					t[3] = strtok(NULL, ",");
					t[4] = strtok(NULL, "");

					if (t[2] == NULL || t[3] == NULL || t[4] == NULL)
						g_error("baken: cannot parse squares colour data\n");

					User_Data.Colours[COLOUR_SQUARES].pixel = 0;
					User_Data.Colours[COLOUR_SQUARES].red   = atoi(t[2]);
					User_Data.Colours[COLOUR_SQUARES].green = atoi(t[3]);
					User_Data.Colours[COLOUR_SQUARES].blue  = atoi(t[4]);
				} else if (strcmp(t[0], "user") == 0) {
					t[2] = strtok(t[1], ",");
					t[3] = strtok(NULL, ",");
					t[4] = strtok(NULL, "");

					if (t[2] == NULL || t[3] == NULL || t[4] == NULL)
						g_error("baken: cannot parse user colour data\n");

					User_Data.Colours[COLOUR_USER].pixel = 0;
					User_Data.Colours[COLOUR_USER].red   = atoi(t[2]);
					User_Data.Colours[COLOUR_USER].green = atoi(t[3]);
					User_Data.Colours[COLOUR_USER].blue  = atoi(t[4]);
				} else
					fprintf(stderr, "baken: unknown keyword %s in [Colours] in %s\n", t[0], File_Name);
				break;

			default:
				g_error("baken: unknown state in Read_User_Data - %d\n", state);
				break;
		}
	}

	fclose(fp);
}

int Write_User_Data(void)
{
	char File_Name[81], *s;
	FILE *fp;

	if (getuid() == 0) {
		sprintf(File_Name, "%s/bakenrc", RCDIR);
	} else {
		if ((s = getenv("HOME")) == NULL) {
			g_error("baken: cannot find home directory\n");
			return FALSE;
		}

		sprintf(File_Name, "%s/.bakenrc", s);
	}

	if ((fp = fopen(File_Name, "w")) == NULL) {
		g_error("baken: cannot open userfile for writing\n");
		return FALSE;
	}

	fprintf(fp, "[User]\n");
	fprintf(fp, "callsign=%s\n", User_Data.Callsign);
	fprintf(fp, "locator=%s\n", User_Data.Locator);
	
	fprintf(fp, "\n[Bands]\n");
	fprintf(fp, "50mhz=%d\n", (User_Data.Bands[BAND_50MHZ]) ? 1 : 0);
	fprintf(fp, "70mhz=%d\n", (User_Data.Bands[BAND_70MHZ]) ? 1 : 0);
	fprintf(fp, "144mhz=%d\n", (User_Data.Bands[BAND_144MHZ]) ? 1 : 0);
	fprintf(fp, "432mhz=%d\n", (User_Data.Bands[BAND_432MHZ]) ? 1 : 0);
	fprintf(fp, "1296mhz=%d\n", (User_Data.Bands[BAND_1_3GHZ]) ? 1 : 0);
	fprintf(fp, "2320mhz=%d\n", (User_Data.Bands[BAND_2_3GHZ]) ? 1 : 0);
	fprintf(fp, "3456mhz=%d\n", (User_Data.Bands[BAND_3_4GHZ]) ? 1 : 0);
	fprintf(fp, "5760mhz=%d\n", (User_Data.Bands[BAND_5_7GHZ]) ? 1 : 0);
	fprintf(fp, "10000mhz=%d\n", (User_Data.Bands[BAND_10GHZ]) ? 1 : 0);
	fprintf(fp, "24000mhz=%d\n", (User_Data.Bands[BAND_24GHZ]) ? 1 : 0);
	fprintf(fp, "47000mhz=%d\n", (User_Data.Bands[BAND_47GHZ]) ? 1 : 0);
	fprintf(fp, "76000mhz=%d\n", (User_Data.Bands[BAND_76GHZ]) ? 1 : 0);

	fprintf(fp, "\n[Types]\n");
	fprintf(fp, "iaru=%d\n", (User_Data.Types[TYPE_IARU]) ? 1 : 0);
	fprintf(fp, "noniaru=%d\n", (User_Data.Types[TYPE_NONIARU]) ? 1 : 0);
	fprintf(fp, "repeater=%d\n", (User_Data.Types[TYPE_REPEATER]) ? 1 : 0);
	fprintf(fp, "linear=%d\n", (User_Data.Types[TYPE_LINEAR]) ? 1 : 0);
	fprintf(fp, "tv=%d\n", (User_Data.Types[TYPE_TV]) ? 1 : 0);
	fprintf(fp, "mailbox=%d\n", (User_Data.Types[TYPE_MAILBOX]) ? 1 : 0);
	fprintf(fp, "dxcluster=%d\n", (User_Data.Types[TYPE_DXCLUSTER]) ? 1 : 0);
	fprintf(fp, "node=%d\n", (User_Data.Types[TYPE_NODE]) ? 1 : 0);
	fprintf(fp, "rs=%d\n", (User_Data.Types[TYPE_RS]) ? 1 : 0);
	fprintf(fp, "misc=%d\n", (User_Data.Types[TYPE_MISC]) ? 1 : 0);

	fprintf(fp, "\n[Colours]\n");
	fprintf(fp, "background=%d,%d,%d\n", User_Data.Colours[COLOUR_BACKGROUND].red, User_Data.Colours[COLOUR_BACKGROUND].green, User_Data.Colours[COLOUR_BACKGROUND].blue);
	fprintf(fp, "beacons=%d,%d,%d\n", User_Data.Colours[COLOUR_BEACONS].red, User_Data.Colours[COLOUR_BEACONS].green, User_Data.Colours[COLOUR_BEACONS].blue);
	fprintf(fp, "cities=%d,%d,%d\n", User_Data.Colours[COLOUR_CITIES].red, User_Data.Colours[COLOUR_CITIES].green, User_Data.Colours[COLOUR_CITIES].blue);
	fprintf(fp, "land=%d,%d,%d\n", User_Data.Colours[COLOUR_LAND].red, User_Data.Colours[COLOUR_LAND].green, User_Data.Colours[COLOUR_LAND].blue);
	fprintf(fp, "squares=%d,%d,%d\n", User_Data.Colours[COLOUR_SQUARES].red, User_Data.Colours[COLOUR_SQUARES].green, User_Data.Colours[COLOUR_SQUARES].blue);
	fprintf(fp, "user=%d,%d,%d\n", User_Data.Colours[COLOUR_USER].red, User_Data.Colours[COLOUR_USER].green, User_Data.Colours[COLOUR_USER].blue);

	fclose(fp);

	return TRUE;
}
