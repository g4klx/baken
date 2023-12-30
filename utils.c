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
#include <math.h>

#include <gtk/gtk.h>

#include "global.h"

double RAD(double angle)
{
	return (angle / 180.0) * M_PI;
}

double DEG(double angle)
{
	return (angle / M_PI) * 180.0;
}

double mod(double n, double m)
{
	return n - floor(n / fabs(m)) * m;
}

int Convert_Locator(char *buffer, double *latitude, double *longitude)
{
	if (strlen(buffer) != 6 && strlen(buffer) != 8)
		return FALSE;

	if (buffer[0] < 'A' || buffer[0] > 'R' ||
	    buffer[1] < 'A' || buffer[1] > 'R' ||
	    buffer[2] < '0' || buffer[2] > '9' ||
	    buffer[3] < '0' || buffer[3] > '9' ||
	    buffer[4] < 'A' || buffer[4] > 'X' ||
	    buffer[5] < 'A' || buffer[5] > 'X')
		return FALSE;

	if (strlen(buffer) == 8 &&
	   (buffer[6] < '0' || buffer[6] > '9' ||
	    buffer[7] < '0' || buffer[7] > '9'))
		return FALSE;

	*longitude = -180.0 + FIELD_WIDTH   * (buffer[0] - 'A') +
			      SQUARE_WIDTH  * (buffer[2] - '0') +
			      SUB_WIDTH     * (buffer[4] - 'A');

	*latitude  = -90.0  + FIELD_HEIGHT  * (buffer[1] - 'A') +
			      SQUARE_HEIGHT * (buffer[3] - '0') +
			      SUB_HEIGHT    * (buffer[5] - 'A');

	if (strlen(buffer) == 8) {
		*longitude += SUB_SUB_WIDTH  * (buffer[6] - '0') + SUB_SUB_WIDTH / 2.0;
		*latitude  += SUB_SUB_HEIGHT * (buffer[7] - '0') + SUB_SUB_HEIGHT / 2.0;
	} else {
		*longitude += SUB_SUB_WIDTH * 5.0;
		*latitude  += SUB_SUB_HEIGHT * 5.0;
	}

	return TRUE;
}

int Convert_Angles(double Latitude, double Longitude, char *buffer)
{
	int ILatitude, ILongitude;

	if (Latitude > 90.0 || Latitude < -90.0)
		return FALSE;

	if (Longitude > 180.0 || Longitude < -180.0)
		return FALSE;

	Latitude  += 90.0;
	Longitude += 180.0;

	ILongitude = (int)(Longitude / FIELD_WIDTH);
	buffer[0]  = ILongitude + 'A';
	Longitude -= (double)ILongitude * FIELD_WIDTH;

	ILongitude = (int)(Longitude / SQUARE_WIDTH);
	buffer[2]  = ILongitude + '0';
	Longitude -= (double)ILongitude * SQUARE_WIDTH;

	ILongitude = (int)(Longitude / SUB_WIDTH);
	buffer[4]  = ILongitude + 'A';

	ILatitude = (int)(Latitude / FIELD_HEIGHT);
	buffer[1] = ILatitude + 'A';
	Latitude -= (double)ILatitude * FIELD_HEIGHT;

	ILatitude = (int)(Latitude / SQUARE_HEIGHT);
	buffer[3] = ILatitude + '0';
	Latitude -= (double)ILatitude * SQUARE_HEIGHT;

	ILatitude = (int)(Latitude / SUB_HEIGHT);
	buffer[5] = ILatitude + 'A';

	buffer[6] = '\0';

	return TRUE;
}

/*	Full distance calculation on elliptical earth (non-optimised).
	John Morris, GM4ANB, 1994.
*/
	
void Calc_Distance_Bearing(double lat1, double lon1, double lat2, double lon2, double *bearing, double *distance)
{
	double e, si, co, th, ca, a1, a2, l1, l2, ep;	/* Locals */
	double hangle;
	int rev = FALSE;

	lat1 = RAD(lat1);
	lat2 = RAD(lat2);
	lon1 = RAD(lon1);
	lon2 = RAD(lon2);

	e = 1.0 - (RB * RB) / (RA * RA);		/* Eccentricity squared */

	/* Convert latitudes to geocentric */
	lat1 = atan((RB * RB) / (RA * RA) * tan(lat1));
	lat2 = atan((RB * RB) / (RA * RA) * tan(lat2));

	/* Calculate central angle */
	ca = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1));

	/* Calculate angles along great ellipse from equator */
	if (lat1 > lat2) {
		double t;

		t = lat2; 
		lat2 = lat1;
		lat1 = t;

		rev = TRUE;
	}

	si = sin(lat1) * sin(ca);
	co = sin(lat2) - sin(lat1) * cos(ca);
	a1 = atan2(si, co);
	a2 = a1 + ca;

	/* Calculate ellipticity (squared) of great circle */
	th = sin(lat2) / sin(a2);
	ep = e * th * th;

	/* Calculate great circle distance from equator for each */
	l1 = a1 - ep * a1 / 4.0 + ep * sin(2.0 * a1) / 8.0;
	l2 = a2 - ep * a2 / 4.0 + ep * sin(2.0 * a2) / 8.0;
	hangle = l2 - l1;

	*distance = RA * hangle;	/* Station distance is difference */ 
	*bearing  = DEG(acos((sin(lat2) - sin(lat1) * cos(hangle)) / (cos(lat1) * sin(hangle))));

	if (lon1 > lon2 && !rev)
		*bearing = 360.0 - *bearing;
	else if (lon1 > lon2 && rev)
		*bearing = 180.0 + *bearing;
	else if (lon1 < lon2 && rev)
		*bearing = 180.0 - *bearing;
}

int Calc_Scatter_Point(double lat1, double lon1, double bear1, double lat2, double lon2, double bear2, double *slat, double *slon)
{
	double dst12, dst13;
	double crs12, crs21;
	double crs13, crs23;
	double ang1, ang2, ang3;
	double lat3, lon3;
	double temp1, temp2;

	double TOL = 1.0e-15;

	lat1  = RAD(lat1);
	lon1  = RAD(lon1);
	lat2  = RAD(lat2);
	lon2  = RAD(lon2);
	crs13 = 2.0 * M_PI - RAD(bear1);
	crs23 = 2.0 * M_PI - RAD(bear2);

	temp1 = sin((lat1 - lat2) / 2.0);
	temp2 = sin((lon1 - lon2) / 2.0);

	dst12 = 2.0 * asin(sqrt(temp1 * temp1 + cos(lat1) * cos(lat2) * temp2 * temp2));

	crs12 = acos((sin(lat2) - sin(lat1) * cos(dst12)) / (sin(dst12) * cos(lat1)));

	if (sin(lon2 - lon1) >= 0.0)
		crs12 = 2.0 * M_PI - crs12;

	crs21 = acos((sin(lat1) - sin(lat2) * cos(dst12)) / (sin(dst12) * cos(lat2)));

	if (sin(lon1 - lon2) >= 0.0)
		crs21 = 2.0 * M_PI - crs21;

	ang1 = mod(crs13 - crs12 + M_PI, 2.0 * M_PI) - M_PI;
	ang2 = mod(crs21 - crs23 + M_PI, 2.0 * M_PI) - M_PI;

	if ((sin(ang1) * sin(ang2)) <= sqrt(TOL)) {
		return FALSE; 
	} else {
		ang1 = fabs(ang1);
		ang2 = fabs(ang2);

		ang3  = acos(-cos(ang1) * cos(ang2) + sin(ang1) * sin(ang2) * cos(dst12));
		dst13 = asin(sin(ang2) * sin(dst12) / sin(ang3)); 

		lat3 = asin(sin(lat1) * cos(dst13) + cos(lat1) * sin(dst13) * cos(crs13));
		lon3 = mod(lon1 - asin(sin(crs13) * sin(dst13) / cos(lat3)) + M_PI, 2.0 * M_PI) - M_PI;

		*slat = DEG(lat3);
		*slon = DEG(lon3);

		return TRUE;
	}
}

/*
int main(int argc, char **argv)
{
	double lat1, lon1, qtf1;
	double lat2, lon2, qtf2;
	double bear1, bear2;
	double dist1, dist2;
	double slat, slon;
	char loc[10];

	if (argc < 5) {
		fprintf(stderr, "Usage: scp loc1 qtf1 loc2 qtf2\n");
		return 1;
	}

	if (!Convert_Locator(argv[1], &lat1, &lon1)) {
		fprintf(stderr, "scp: loc1 is invalid\n");
		return 1;
	}

	qtf1 = atof(argv[2]);
	
	if (qtf1 < 0.0 || qtf1 >= 360.0) {
		fprintf(stderr, "scp: qtf1 is invalid\n");
		return 1;
	}

	if (!Convert_Locator(argv[3], &lat2, &lon2)) {
		fprintf(stderr, "scp: loc2 is invalid\n");
		return 1;
	}

	qtf2 = atof(argv[4]);
	
	if (qtf2 < 0.0 || qtf2 >= 360.0) {
		fprintf(stderr, "scp: qtf2 is invalid\n");
		return 1;
	}

	if (!Calc_Scatter_Point(lat1, lon1, qtf1, lat2, lon2, qtf2, &slat, &slon)) {
		printf("No SCP\n");
		return 0;
	}

	Calc_Distance_Bearing(lat1, lon1, slat, slon, &bear1, &dist1);
	Calc_Distance_Bearing(lat2, lon2, slat, slon, &bear2, &dist2);

	if (!Convert_Angles(slat, slon, loc)) {
		fprintf(stderr, "scp: something wrong\n");
		return 1;
	}

	if (dist1 > 500.0 || dist2 > 500.0) {
		printf("Unreasonable SCP = %s, dists=%.0fkms and %.0fkms\n", loc, dist1, dist2);
		return 0;
	}

	printf("%s\n", loc);

	return 0;
}
*/
