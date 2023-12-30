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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

char *mstrtok(char *buf, char *delim)
{
	static char *buffer = NULL;
	char *p;
	int len;

	if (buf != NULL)
		buffer = buf;

	len = strcspn(buffer, delim);

	if (len != strlen(buffer)) {
		p = buffer;
		buffer[len] = '\0';
		buffer += len + 1;
		return p;
	} else {
		return NULL;
	}
}

void Convert_Beacon_Data(FILE *fp)
{
	char buffer[255];

	while (fgets(buffer, 254, fp) != NULL) {
		char *freq, *call, *qth, *loc, *height, *antenna, *direction, *power, *operator;
		char *sav = strdup(buffer);

		if ((freq = mstrtok(buffer, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on frequency: %s", sav);
			free(sav);
			continue;
		}

		if ((call = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on callsign: %s", sav);
			free(sav);
			continue;
		}

		if ((qth = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on qth: %s", sav);
			free(sav);
			continue;
		}

		if ((loc = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on locator: %s", sav);
			free(sav);
			continue;
		}

		if ((height = mstrtok(NULL, ",")) == NULL) {
   			fprintf(stderr, "iaru: parse error on height: %s", sav);
			free(sav);
			continue;
		}

		if ((antenna = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on height: %s", sav);
			free(sav);
			continue;
		}

		if ((direction = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on direction: %s", sav);
			free(sav);
			continue;
		}

		if ((power = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on power: %s", sav);
			free(sav);
			continue;
		}

		if ((operator = mstrtok(NULL, ",")) == NULL) {
			fprintf(stderr, "iaru: parse error on operator: %s", sav);
			free(sav);
			continue;
		}

		if (strlen(call) == 0 || strlen(freq) == 0 || strlen(loc) == 0) {
			fprintf(stderr, "iaru: nonsense data: %s", sav);
			free(sav);
			continue;
		}

		fprintf(stdout, "[%s]\n", call);
		fprintf(stdout, "frequency=%.3f\n", atof(freq));
		fprintf(stdout, "locator=%s\n", loc);
		if (strlen(qth) > 0)
			fprintf(stdout, "qth=%s\n", qth);
		if (strlen(antenna) > 0)
			fprintf(stdout, "antenna=%s\n", antenna);
		if (strlen(direction) > 0)
			fprintf(stdout, "direction=%s\n", direction);
		if (strlen(height) > 0)
			fprintf(stdout, "height=%d\n", atoi(height));
		if (strlen(power) > 0)
			fprintf(stdout, "power=%s\n", power);
		if (strlen(operator) > 0)
			fprintf(stdout, "operator=%s\n", operator);
		fprintf(stdout, "\n");

		free(sav);
	}
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
				fprintf(stderr, "iaru: cannot open %s\n", argv[i]);
			}
		}
	} else {
		Convert_Beacon_Data(stdin);
	}
	
	return 0;
}
