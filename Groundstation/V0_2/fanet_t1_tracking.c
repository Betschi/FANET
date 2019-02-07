/*
 * fanet_t1_tracking.c
 * 
 * Copyright 2019  <pi@RPi3B_FANET_2>
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

#ifndef FANET_T1_TRACKING_C
#define FANET_T1_TRACKING_C

#include <stdio.h>
#include "fanet_mac.c"
#include "fanet_struct.c"
#include "fanet_terminal.c"
#include "fanet_global.c"
#include "fanet_mysql.c"

#define STATION_LAT  46.684681			// Position of Ground Statation -> Is needed to calculate the distance to the tracked object 
#define STATION_LON  7.867658			// Modfied if necessary


void decode_tracking (sRawMessage *_rx_message, sTRACKING *_tracking)
{
	if (_rx_message->message[_rx_message->m_pointer+1]&0x80)
		_tracking->tracking = 1;
	else
		_tracking->tracking = 0;			
}


void decode_aircraft_type (sRawMessage *_rx_message, sTRACKING *_tracking)
{
	_tracking->aircraft_type = (_rx_message->message[_rx_message->m_pointer+1]&0x70);
	_tracking->aircraft_type >>=4;
	
	switch (_tracking->aircraft_type){
		case 0:  strcpy (_tracking->aircraft_type_char, "Other"); break;
		case 1:  strcpy (_tracking->aircraft_type_char, "Paraglider"); break;		
		case 2:  strcpy (_tracking->aircraft_type_char, "Hangglider"); break;
		case 3:  strcpy (_tracking->aircraft_type_char, "Balloon"); break;
		case 4:  strcpy (_tracking->aircraft_type_char, "Glider"); break;
		case 5:  strcpy (_tracking->aircraft_type_char, "Aircraft"); break;
		case 6:  strcpy (_tracking->aircraft_type_char, "Helicopter"); break;
		case 7:  strcpy (_tracking->aircraft_type_char, "UAV"); break;
		default: strcpy (_tracking->aircraft_type_char, "---"); break;							
	}
}


void decode_alitude (sRawMessage *_rx_message, sTRACKING *_tracking)
{
	_tracking->altitude = (_rx_message->message[_rx_message->m_pointer+1]&0x07);
	_tracking->altitude <<= 8;
	_tracking->altitude |= _rx_message->message[_rx_message->m_pointer];	// Return range will be 0...2'047 m in 1 m steps
	if ((_rx_message->message[_rx_message->m_pointer+1] & 0x08))
		_tracking->altitude <<= 2;											// Return range will be 2'047...8'188 m in 4 m steps

	_rx_message->m_pointer += 2;			
}

void decode_speed (sRawMessage *_rx_message, sTRACKING *_tracking)
{
	_tracking->speed = (_rx_message->message[_rx_message->m_pointer]&0x7F)*0.5;		// Return range will be 0...63.5 km/h m in 0.5 km/h steps
	if ((_rx_message->message[_rx_message->m_pointer] & 0x80))
		_tracking->speed *= 5;														// Return range will be 63.5...317.5 km/h in 2.5 km/h steps

	_rx_message->m_pointer += 1;
}


void decode_climb (sRawMessage *_rx_message, sTRACKING *_tracking)
{
	signed char _climb_char;
	
	_climb_char = (_rx_message->message[_rx_message->m_pointer]&0x7F);	
	if (_climb_char & 0x40)												// Check, if minus (-) bit is set
		_climb_char |= 0x80;
		
	_tracking->climb = (float)_climb_char;

	if ((_rx_message->message[_rx_message->m_pointer] & 0x80))
		_tracking->climb  *= 0.5;										// Return range will be -32.5...-6.5 / +6.5...+31.5 m/s in +-0.5m/s steps
	else
		_tracking->climb  *= 0.1;										// Return range will be -6.4...+6.3 m/s in +-0.1m/s steps
										
	_rx_message->m_pointer += 1;
}

void decode_heading (sRawMessage *_rx_message, sTRACKING *_tracking)
{
	_tracking->heading = 360.0/256*(_rx_message->message[_rx_message->m_pointer]);	// Return range will be 0...360° in 1.4° steps

	_rx_message->m_pointer += 1;
}


void type_1_tracking_decoder (sRawMessage *_rx_payload, sTRACKING *_rx_tracking)
{
	float _latitude;
	float _longitude;
	
	_rx_payload->m_pointer = 0;
	
	decode_abs_coordination (&_rx_payload->message[_rx_payload->m_pointer], &_latitude, &_longitude);
	_rx_tracking->latitude  = _latitude;
	_rx_tracking->longitude = _longitude;
	_rx_payload->m_pointer += 6;

	_rx_tracking->distance = distance (STATION_LAT, STATION_LON, _rx_tracking->latitude, _rx_tracking->longitude, 'K');

	decode_tracking (_rx_payload, _rx_tracking);
	decode_aircraft_type (_rx_payload, _rx_tracking);
	decode_alitude (_rx_payload, _rx_tracking);
	decode_speed (_rx_payload, _rx_tracking);
	decode_climb (_rx_payload, _rx_tracking);		
	decode_heading (_rx_payload, _rx_tracking);
	
	_rx_tracking->turn_rate_on = 0;		// No decode routine for turn rate
	_rx_tracking->turn_rate = 0;
}


void type_1_tracking_coder (sRawMessage *_tx_message, sTRACKING *_tx_tracking)
{
/* Not implemented yet
 * 
int type_1_tracking (void)
{

	signed int	latitude_int;
	signed int	longitude_int;
	signed int	altitude;
	
	message_tx[0] = 0x01;
	message_tx[1] = STATION_MANUF;
	message_tx[2] = STATION_ID&0x00FF;
	message_tx[3] = (STATION_ID&0xFF00)>>8;
	//message_tx[4] = 0x00;
	
	latitude_int  = STATION_LAT*93206;
	longitude_int = STATION_LON*46603;
	
	printf("%d  %d\n",latitude_int, longitude_int);
	
	message_tx[4] = latitude_int&0x000000FF;
	message_tx[5] = (latitude_int&0x0000FF00)>>8;
	message_tx[6] = (latitude_int&0x00FF0000)>>16;

	message_tx[7] = longitude_int&0x000000FF;
	message_tx[8] = (longitude_int&0x0000FF00)>>8;
	message_tx[9] = (longitude_int&0x00FF0000)>>16;

	if (STATION_ALT < 2048)
		altitude = STATION_ALT;
	else
	{
		altitude = STATION_ALT/4;
		printf("Alt: %d\n", altitude);
	}	
	message_tx[10] = STATION_ALT&0x00FF;
	message_tx[11] = (STATION_ALT&0x0700)>>8;
		
	if (STATION_ALT > 2048)
		message_tx[11] |= 0x08;			// Set Altitude Scaling Bit: 1->4x
		
		
	message_tx[12] = 0x00;				// Speed
	
	message_tx[13] = 0x00;				// Climb
	
	message_tx[14] = 0x00;				// Heading
	
	return 15;
}
*/
}


void type_1_tracking_receiver (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sRawMessage *_rx_payload)
{
	sTRACKING	_rx_tracking;
	
	type_1_tracking_decoder (_rx_payload, &_rx_tracking);
	
	terminal_message_1 (0,0, _radiodata, _fanet_mac, &_rx_tracking);

	write_object_tracking (_radiodata, _fanet_mac, &_rx_tracking);	
}


#endif
