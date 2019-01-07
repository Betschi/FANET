/*
 * fanet_global.c
 * 
 * Copyright 2018  <pi@RPi3B_FANET_2>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifndef FANET_GLOBAL_C
#define FANET_GLOBAL_C

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fanet_struct.c"

#define pi 3.14159265358979323846

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  Function prototypes                                           :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double);
double rad2deg(double);

double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dist = dist * 60 * 1.1515;
  switch(unit) {
    case 'M':
      break;
    case 'K':
      dist = dist * 1.609344;
      break;
    case 'N':
      dist = dist * 0.8684;
      break;
  }
  return (dist);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
  return (deg * pi / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
  return (rad * 180 / pi);
}



/***********************************************************************
	Absolute:
	[Byte 0-2]	Position	(Little Endian, 2-Complement)
	bit 0-23	Latitude
	[Byte 3-5]	Position	(Little Endian, 2-Complement)
	bit 0-23	Longitude

	Details:
	Latitude = value_lat/93206	\in [-90, +90]
	Longitude = value_lon/46603 \in [-180, +180]
	(Note: 32bit floating point is required for direct conversion)
 
 ***********************************************************************/
void decode_abs_coordination (char _input[], float *_latitude, float *_longitude)
{
	signed int	latitude_int;
	signed int	longitude_int;

	latitude_int =  _input[2];
	latitude_int <<= 8;
	latitude_int += _input[1];
	latitude_int <<=8;
	latitude_int += _input[0];
	*_latitude = latitude_int/(float)93206;
		
	longitude_int =  _input[5];
	longitude_int <<=8;
	longitude_int += _input[4];
	longitude_int <<=8;
	longitude_int += _input[3];
	*_longitude = longitude_int/(float)46603;	
}

void code_abs_coordination (sRawMessage *_tx_message, sWeather *_weather_data)
{
	signed int	_latitude_int;
	signed int	_longitude_int;
	
	if (_weather_data->latitude > 90)
		_weather_data->latitude = 0;
	if (_weather_data->latitude < -90)
		_weather_data->latitude = 0;	

	if (_weather_data->longitude > 180)
		_weather_data->longitude = 0;
	if (_weather_data->longitude < -180)
		_weather_data->longitude = 0;
	
	_latitude_int  = round(_weather_data->latitude*93206);
	_longitude_int = round(_weather_data->longitude*46603);
	
	_tx_message->message[_tx_message->m_length]   = _latitude_int&0x000000FF;
	_tx_message->message[_tx_message->m_length+1] = (_latitude_int&0x0000FF00)>>8;
	_tx_message->message[_tx_message->m_length+2] = (_latitude_int&0x00FF0000)>>16;

	_tx_message->message[_tx_message->m_length+3] = _longitude_int&0x000000FF;
	_tx_message->message[_tx_message->m_length+4] = (_longitude_int&0x0000FF00)>>8;
	_tx_message->message[_tx_message->m_length+5] = (_longitude_int&0x00FF0000)>>16;
	
	_tx_message->m_length += 6;
}



void address_int (char *_address_string, byte *_manuf, uint16_t *_id)
{
	char _addr_manuf[4];
	char _addr_id[6];
	
	strncpy (_addr_manuf, _address_string, 2);
	_addr_manuf[2] ='\n';
	*_manuf = atoi(_addr_manuf);

	strncpy (_addr_id, _address_string+3, 4);	
	_addr_id[4] ='\n';
	*_id = atoi(_addr_id);
	
}

#endif

