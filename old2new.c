/*
 *   Copyright (C) 1999-2002 by Jonathan Naylor G4KLX
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

#define	TRUE	1
#define	FALSE	0

char call[21];
double freq;
char loc[7];
char qth[51];
char antenna[51];
int angle_count;
int angle[10];
int height;
char power[51];
char operator[21];

void Write_Beacon_Data(void)
{
	int i;

	fprintf(stdout, "[%s]\n", call);
	fprintf(stdout, "frequency=%.3f\n", freq);
	fprintf(stdout, "locator=%s\n", loc);
	if (strlen(qth) > 0)
		fprintf(stdout, "qth=%s\n", qth);
	if (strlen(antenna) > 0)
		fprintf(stdout, "antenna=%s\n", antenna);
	if (angle_count > 0) {
		fprintf(stdout, "direction=");
		for (i = 0; i < angle_count; i++) {
			if (i > 0) fprintf(stdout, ",");
			fprintf(stdout, "%d", angle[i]);
		}
		fprintf(stdout, "\n");
	}
	if (height > 0)
		fprintf(stdout, "height=%d\n", height);
	if (strlen(power) > 0)
		fprintf(stdout, "power=%s\n", power);
	if (strlen(operator) > 0)
		fprintf(stdout, "operator=%s\n", operator);
	fprintf(stdout, "\n");
}

void Convert_Beacon_Data(FILE *fp)
{
	char Buffer[255], *s, *p, *t[2];
	int valid = FALSE;

	while (fgets(Buffer, 254, fp) != NULL) {
		if (Buffer[0] == '#')
			continue;
	
		if (Buffer[0] == '[') {
			if (valid)
				Write_Beacon_Data();

			if ((s = strchr(Buffer, ']')) == NULL)
				fprintf(stderr, "old2new: malformed line %s\n", Buffer);
			*s = '\0';

			freq = 0.0;
			loc[0] = '\0';
			qth[0] = '\0';
			antenna[0] = '\0';
			angle_count = 0;
			height = 0;
			power[0] = '\0';
			operator[0] = '\0';

			strcpy(call, Buffer + 1);

			valid = FALSE;
		} else {
			if ((t[0] = strtok(Buffer, "=")) == NULL)
				continue;
			if ((t[1] = strtok(NULL, "\r\n")) == NULL)
				continue;

			if (strcmp(t[0], "frequency") == 0) {
				freq = atof(t[1]);
				valid = TRUE;
			} else if (strcmp(t[0], "locator") == 0) {
				strcpy(loc, t[1]);
			} else if (strcmp(t[0], "qth") == 0) {
				strcpy(qth, t[1]);
			} else if (strcmp(t[0], "antenna") == 0) {
				strcpy(antenna, t[1]);
			} else if (strcmp(t[0], "direction") == 0) {
				if (strcmp(t[1], "Omni") != 0) {
					s = t[1];
					while ((p = strtok(s, "/")) != NULL) {
						angle[angle_count] = atoi(p);
						angle_count++;
						s = NULL;
					}
				}
			} else if (strcmp(t[0], "height") == 0) {
				height = atoi(t[1]);
			} else if (strcmp(t[0], "power") == 0) {
				strcpy(power, t[1]);
			} else if (strcmp(t[0], "operator") == 0) {
				strcpy(operator, t[1]);
			} else {
				fprintf(stderr, "old2new: unknown keyword %s\n", t[0]);
			}
		}
	}

	if (valid)
		Write_Beacon_Data();
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int i;

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if ((fp = fopen(argv[i], "r")) != NULL) {
				Convert_Beacon_Data(fp);
				fclose(fp);
			} else {
				fprintf(stderr, "old2new: cannot open %s\n", argv[i]);
			}
		}
	} else {
		Convert_Beacon_Data(stdin);
	}
	
	return 0;
}
