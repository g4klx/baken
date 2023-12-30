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
#include <ctype.h>
#include <unistd.h>

#define	STATE_NONE	0
#define	STATE_FIRST	1
#define	STATE_LAST	2
#define	STATE_DATA	3

char *strupr(char *s)
{
	char *p = s;

	while (*s != '\0') {
		*s = toupper(*s);
		s++;
	}

	return p;
}

void Convert_RS_Line(int band, char *buffer)
{
	char *call,*name,  *loc, *power, *antenna, *nf, *height;
	char *sav = strdup(buffer);

	if ((call = strtok(buffer, " \t")) == NULL) {
		fprintf(stderr, "rslist: parse error on callsign: %s", sav);
		free(sav);
		return;
	}

	if ((name = strtok(NULL, " \t")) == NULL) {
		fprintf(stderr, "rslist: parse error on name: %s", sav);
		free(sav);
		return;
	}

	if ((loc = strtok(NULL, " \t")) == NULL) {
		fprintf(stderr, "rslist: parse error on locator: %s", sav);
		free(sav);
		return;
	}

	if ((power = strtok(NULL, " \t")) == NULL) {
		fprintf(stderr, "rslist: parse error on locator: %s", sav);
		free(sav);
		return;
	}

	if ((antenna = strtok(NULL, " \t")) == NULL) {
		fprintf(stderr, "rslist: parse error on antenna: %s", sav);
		free(sav);
		return;
	}

	if ((nf = strtok(NULL, " \t")) == NULL) {
   		fprintf(stderr, "rslist: parse error on noise figure: %s", sav);
		free(sav);
		return;
	}

	if ((height = strtok(NULL, " \t")) == NULL) {
		fprintf(stderr, "rslist: parse error on height: %s", sav);
		free(sav);
		return;
	}

	if (strlen(call) == 0 || strlen(loc) == 0) {
		fprintf(stderr, "rslist: nonsense data: %s", sav);
		free(sav);
		return;
	}

	fprintf(stdout, "[%s]\n", call);
	fprintf(stdout, "frequency=%d.000\n", band);
	fprintf(stdout, "locator=%s\n", strupr(loc));
	if (strlen(antenna) > 0)
		fprintf(stdout, "antenna=%s Dish\n", antenna);
	if (strlen(height) > 0)
		fprintf(stdout, "height=%d\n", atoi(height));
	if (strlen(power) > 0)
		fprintf(stdout, "power=%s TX\n", power);
	fprintf(stdout, "\n");

	free(sav);
}

void Convert_RS_Data(FILE *fp)
{
	char buffer[255];
	int state = STATE_NONE;
	int band = 0;
	
	while (fgets(buffer, 254, fp) != NULL) {
		if (strlen(buffer) < 3)
			continue;

		if (strncmp(buffer, "====", 4) == 0) {
			if (state == STATE_FIRST) {
				state = STATE_DATA;
			} else {
				state = STATE_FIRST;
			}
			continue;
		}

		if (state == STATE_FIRST) {
			if (strncmp(buffer, "3.4 GHz", 7) == 0)
				band = 3400;
			if (strncmp(buffer, "5.7 GHz", 7) == 0)
				band = 5760;
			if (strncmp(buffer, "     10 GHz", 11) == 0)
				band = 10368;
			if (strncmp(buffer, "     24GHZ", 10) == 0)
				band = 24192;
			if (strncmp(buffer, "   47GHZ", 10) == 0)
				band = 47088;
			continue;
		}

		if (state == STATE_DATA)
			Convert_RS_Line(band, buffer);
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int i;

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if ((fp = fopen(argv[i], "r")) != NULL) {
				Convert_RS_Data(fp);
				fclose(fp);
			} else {
				fprintf(stderr, "rslist: cannot open %s\n", argv[i]);
			}
		}
	} else {
		Convert_RS_Data(stdin);
	}
	
	return 0;
}
