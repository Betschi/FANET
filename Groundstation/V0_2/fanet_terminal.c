/*
 * fanet_terminal.c
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

#ifndef FANET_TERMINAL_C
#define FANET_TERMINAL_C

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "fanet_struct.c"

#define KNRM  "\x1B[0m"		// Color: Normal
#define KRED  "\x1B[31m"	// Color: Red
#define KGRN  "\x1B[32m"	// Color: Green
#define KYEL  "\x1B[33m"	// Color: Yellow
#define KBLU  "\x1B[34m"	// Color: Blue
#define KMAG  "\x1B[35m"	// Color: Mangenta
#define KCYN  "\x1B[36m"	// Color: Cyan
#define KWHT  "\x1B[37m"	// Color: White

#define BOLD		"\e[1m"		// Bold/Bright
#define DIM			"\e[2m"		// Dim
#define UNDERLINE	"\e[4m"		// Underline
#define BLINK		"\e[5m"		// Blink
#define INVERT		"\e[7m"		// Reverse (invert the foreground and background colors)

#define RESET_ALL	"\e[0m"		// Reset all attributs
#define R_BOLD		"\e[21m"	// Reset Bold/Bright
#define R_DIM		"\e[22m"	// Reset Dim
#define R_UNDERLINE	"\e[24m"	// Reset Underline
#define R_BLINK		"\e[25m"	// Reset Blink
#define R_INVERT	"\e[27m"	// Reset Reverse (invert the foreground and background colors)


#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

typedef unsigned char byte;


/***********************************************************************
 * Shows the FANET start screen
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |01.10.2018|  bet | Init Version
 **********************************************************************/
void terminal_start_screen (byte _sf, int _bandwith, int _freq)
{
	clear();
	printf (" FANET Monitor V0.1\n");
    printf ("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf ("| Listening at SF%i on %.6lf Mhz with a BW of %d kHz.\n", _sf,(double)_freq/1000000, _bandwith);
	printf ("|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");	
	printf ("| Time                  | RSSI   | P.RSSI | SNR    | CR | +-frq   | Lenght| Raw Data \n");
	printf ("|                                                                 | E F  T| Source | A C S| Destina| Signatur| Latitude  | Longitude |On|A-Typ| Alt  | Speed    | Climb   | Headin|   T-Rate |  Distance|\n");
	printf ("|                                                                 | E F  T| Source | A C S| Destina| Signatur| Latitude  | Longitude | Temp   |Heading| Speed    | Gusts    | Humidity| Barometric|\n");
	printf ("|                                                                 | E F  T| Source | A C S| Destina| Signatur| Name\n");
	printf ("|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

/***********************************************************************
 * Shows the RF information
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |01.10.2018|  bet | Init Version
 **********************************************************************/
void terminal_rf_info (boolean _rxtx, boolean _integrity, sRadioData *_radio_data, sFanetMAC *_mac_data)
{
	struct timeval tv;
	struct tm* ptm;
	char _time_string[40];
	long _milliseconds;

	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	_milliseconds = tv.tv_usec / 1000;
    strftime (_time_string, sizeof (_time_string), "%Y-%m-%d %H:%M:%S", ptm);

	printf (KNRM); printf("|");
	// Timestamp
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radio_data->crc_err || !_mac_data->valid_bit) printf(KYEL);
	printf ("%s.%03ld",_time_string,_milliseconds);
	printf (KNRM); printf("|");
	
	// RSSI	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radio_data->crc_err || !_mac_data->valid_bit) printf(KYEL);
	printf (" %+4ddBm",_radio_data->rssi);
	printf (KNRM); printf("|");
	
	// P.RSSI	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radio_data->crc_err || !_mac_data->valid_bit) printf(KYEL);
	printf (" %+4ddBm",_radio_data->prssi);
	printf (KNRM); printf("|");	

	// P.RSSI	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radio_data->crc_err || !_mac_data->valid_bit) printf(KYEL);
	if (_rxtx) printf("   -.-dB"); else printf (" %+5.1fdB",_radio_data->psnr);
	printf (KNRM); printf("|");

	// Coding rate (CR)	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radio_data->crc_err || !_mac_data->valid_bit) printf(KYEL);
	printf (" %s",_radio_data->coding_rate);
	printf (KNRM); printf("|");

	// Frequency error	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radio_data->crc_err || !_mac_data->valid_bit) printf(KYEL);
	if (_rxtx) printf("      -Hz"); else printf (" %+6dHz", _radio_data->freq_err);
	printf (KNRM); printf("|");	

}

/***********************************************************************
 * Shows the MAC information
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |25.12.2018|  bet | Init Version
 **********************************************************************/
void terminal_mac_info (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data)
{
	// MAC Header
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);
	printf(" %d %d %2d", _mac_data->e_header, _mac_data->forward, _mac_data->type);
	printf (KNRM); printf("|");

	// MAC Source Address
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);	
	printf(" %02x:%04x",_mac_data->s_manufactur_id, _mac_data->s_unique_id);
	printf (KNRM); printf("|");	

	// MAC Extended Header
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);	
	if (_mac_data->e_header) printf(" %d",_mac_data->ack); else printf (" -");
	if (_mac_data->e_header) printf(" %d",_mac_data->cast); else printf (" -");
	if (_mac_data->e_header) printf(" %d",_mac_data->signature_bit); else printf (" -");
	printf (KNRM); printf("|");

	// MAC Destination Address
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);
	if (_mac_data->cast) printf(" %02x:%04x",_mac_data->d_manufactur_id, _mac_data->d_unique_id); else printf (" --:----");
	printf (KNRM); printf("|");	

	// MAC Signature
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);	
	if (_mac_data->signature_bit) printf(" %08x",_mac_data->signature); else printf (" --------");
	printf (KNRM); printf("|");
	
}


/***********************************************************************
 * Shows the FANET message as HEX on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |13.01.2019|  bet | Init Version
 **********************************************************************/
void terminal_message_raw (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data, sRawMessage *_raw_message)
{
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);
	//terminal_mac_info (_rxtx, _integrity, _mac_data);
	
	// Raw Data
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);
	//printf(BOLD);
	printf ("    %3d",_raw_message->m_length);
	printf (KNRM); printf("|");

	if (_rxtx) printf(KCYN); else printf(KGRN);	
	if (_radiodata->crc_err || !_mac_data->valid_bit) printf(KYEL);
	//printf(BOLD);
	_raw_message->m_pointer=0;
	while (_raw_message->m_pointer < (_raw_message->m_length))
	{	
		printf (" %02x",_raw_message->message[_raw_message->m_pointer]);
		_raw_message->m_pointer++;
	}
	//printf(R_BOLD);
	printf (KNRM); printf("|");
	printf("\n");
}


/***********************************************************************
 * Shows the CRC error on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.01.2019|  bet | Init Version
 **********************************************************************/
void terminal_message_crc_err (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data)
{				
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	// Name
	printf(KYEL);
	printf(INVERT);
	printf(" CRC error");
	printf(R_INVERT);
	printf (KNRM); printf("|");
	printf("\n");	
}

/***********************************************************************
 * Shows the MAC error on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |27.01.2019|  bet | Init Version
 **********************************************************************/
void terminal_message_mac_err (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data)
{				
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	printf(KYEL);
	printf(INVERT);
	printf(" MAC error");
	printf(R_INVERT);
	printf (KNRM); printf("|");
	printf("\n");	
}


/***********************************************************************
 * Shows the FANET message 0 (ACK) on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |13.01.2019|  bet | Init Version
 **********************************************************************/
void terminal_message_0 (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data, sACK *_ack)
{				
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);
	terminal_mac_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	// Name
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(INVERT);
	printf(" ACK");
	printf(R_INVERT);
	printf (KNRM); printf("|");
	printf("\n");
	
}


/***********************************************************************
 * Shows the FANET message 1 (Tracking) on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.01.2019|  bet | Init Version
 **********************************************************************/
void terminal_message_1 (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data, sTRACKING *_tracking)
{
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);					
	terminal_mac_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	// Latitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %+9.5f°",_tracking->latitude);
	printf (KNRM); printf("|");

	// Longitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %+9.5f°",_tracking->longitude);
	printf (KNRM); printf("|");

	// Online Tracking
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %d",_tracking->tracking);
	printf (KNRM); printf("|");	

	// Aircraft type
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf("    %d",_tracking->aircraft_type);
	printf (KNRM); printf("|");	

	// Altitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %4dm",_tracking->altitude);
	printf (KNRM); printf("|");	

	// Speed
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %5.1fkm/h",_tracking->speed);
	printf (KNRM); printf("|");	

	// Climb
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %+5.1fm/s",_tracking->climb);
	printf (KNRM); printf("|");	

	// Heading
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %5.1f°",_tracking->heading);
	printf (KNRM); printf("|");	

	// Turm rate
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_tracking->turn_rate_on) printf(" %+6.2f°/s",_tracking->turn_rate); else printf("   -.--°/s");
	printf (KNRM); printf("|");
	
	// Distance
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf("  %6.2fkm",_tracking->distance);
	printf (KNRM); printf("|");	
	printf("\n");		
}


/***********************************************************************
 * Shows the FANET message 2 (Name) on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |25.12.2018|  bet | Init Version
 **********************************************************************/
void terminal_message_2 (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data, sName *_name)
{
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);					
	terminal_mac_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	// Name
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	_name->name[_name->n_length]=0;			// Add EOF at last
	printf(" %s",_name->name);
	printf (KNRM); printf("|");
	printf("\n");
	
}


/***********************************************************************
 * Shows the FANET message 3 (Message data) on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |29.12.2018|  bet | Init Version
 **********************************************************************/
void terminal_message_3 (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data, sMessage *_message)
{
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);
	terminal_mac_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	// Message
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	_message->message[_message->m_length]=0;			// Add EOF at last
	printf(" %s",_message->message);
	printf (KNRM); printf("|");
	printf("\n");
}


/***********************************************************************
 * Shows the FANET message 4 (Weather data) on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |01.10.2018|  bet | Init Version
 **********************************************************************/
void terminal_message_4 (boolean _rxtx, boolean _integrity, sRadioData *_radiodata, sFanetMAC *_mac_data, sWeather *_weather_data)
{
	terminal_rf_info (_rxtx, _integrity, _radiodata, _mac_data);				
	terminal_mac_info (_rxtx, _integrity, _radiodata, _mac_data);
	
	// Latitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %+9.5f°",_weather_data->latitude);
	printf (KNRM); printf("|");

	// Longitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	printf(" %+9.5f°",_weather_data->longitude);
	printf (KNRM); printf("|");

	// Temperature
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_weather_data->temp) printf(" %+5.1f°C",_weather_data->temperature); else printf("   -.-°C");
	printf (KNRM); printf("|");

	// Winde heading
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_weather_data->wind) printf(" %5.1f°",_weather_data->wind_heading); else printf("   -.-°");
	printf (KNRM); printf("|");

	// Winde speed
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_weather_data->wind) printf(" %5.1fkm/h",_weather_data->wind_speed); else printf("   -.-km/h");
	printf (KNRM); printf("|");

	// Winde gusts
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_weather_data->wind) printf(" %5.1fkm/h",_weather_data->wind_gusts); else printf("   -.-km/h");
	printf (KNRM); printf("|");

	// Humidity
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_weather_data->humid) printf(" %5.1f%%rh",_weather_data->humidity); else printf("   -.-%%rh");
	printf (KNRM); printf("|");

	// Barometric
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_radiodata->crc_err) printf(KYEL);
	if (_weather_data->barom) printf(" %7.2fhPa",_weather_data->barometric); else printf("    -.--hPa");
	printf (KNRM); printf("|");
	
	printf("\n");
	
}


#endif
