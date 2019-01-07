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
    printf ("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf ("| Listening at SF%i on %.6lf Mhz with a BW of %d kHz.\n", _sf,(double)_freq/1000000, _bandwith);
	printf ("|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");	
	printf ("| Time                  | RSSI   | P.RSSI | SNR    | CR | +-frq   | Lenght | Raw Data \n");
	printf ("|                                                                 | E F Typ| Ma:ID  | Latitude  | Longitude  |OnL| A-Type| Alt  | Speed    | Climb   | Heading | Distance|\n");
	printf ("|                                                                 | E F Typ| Ma:ID  | Latitude  | Longitude  | Temp   |Heading| Speed    | Gusts    | Humidity| Barometric|\n");
	printf ("|                                                                 | E F Typ| Ma:ID  | Name\n");
	printf ("|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

/***********************************************************************
 * Shows the RF information
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |01.10.2018|  bet | Init Version
 **********************************************************************/
void terminal_rf_info (boolean _rxtx, boolean _integrity, sRadioData *_radio_data)
{
	//time_t t;
	//struct tm *tm;
	struct timeval tv;
	struct tm* ptm;
	char _time_string[40];
	long _milliseconds;

	//t = time(NULL);
    //tm = localtime(&t);

	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	_milliseconds = tv.tv_usec / 1000;
    strftime (_time_string, sizeof (_time_string), "%Y-%m-%d %H:%M:%S", ptm);

	printf (KNRM); printf("|");
	// Timestamp
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf ("%s.%03ld",_time_string,_milliseconds);
	printf (KNRM); printf("|");
	
	// RSSI	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf (" %+4ddBm",_radio_data->rssi);
	printf (KNRM); printf("|");
	
	// P.RSSI	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf (" %+4ddBm",_radio_data->prssi);
	printf (KNRM); printf("|");	

	// P.RSSI	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_rxtx) printf("   -.-dB"); else printf (" %+5.1fdB",_radio_data->psnr);
	printf (KNRM); printf("|");

	// Coding rate (CR)	
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf (" %s",_radio_data->coding_rate);
	printf (KNRM); printf("|");

	// Frequency error (CR)	
	if (_rxtx) printf(KCYN); else printf(KGRN);
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
void terminal_mac_info (boolean _rxtx, boolean _integrity, sFanetMAC *_mac_data)
{
	// MAC Header
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf(" %d %d %2d", _mac_data->e_header, _mac_data->forward, _mac_data->type);
	printf (KNRM); printf("|");

	// MAC Source Address
	if (_rxtx) printf(KCYN); else printf(KGRN);	
	printf(" %x:%04x",_mac_data->s_manufactur_id, _mac_data->s_unique_id);
	printf (KNRM); printf("|");	

	// MAC Extended Header
	if (_rxtx) printf(KCYN); else printf(KGRN);	
	if (_mac_data->e_header) printf(" %d",_mac_data->ack); else printf (" -");
	if (_mac_data->e_header) printf(" %d",_mac_data->cast); else printf (" -");
	if (_mac_data->e_header) printf(" %d",_mac_data->signature_bit); else printf (" -");
	printf (KNRM); printf("|");

	// MAC Destination Address
	if (_rxtx) printf(KCYN); else printf(KGRN);	
	if (_mac_data->cast) printf(" %x:%04x",_mac_data->d_manufactur_id, _mac_data->d_unique_id); else printf (" --:----");
	printf (KNRM); printf("|");	

	// MAC Signature
	if (_rxtx) printf(KCYN); else printf(KGRN);	
	if (_mac_data->signature_bit) printf(" %08x",_mac_data->signature); else printf (" --------");
	printf (KNRM); printf("|");
	
}

/***********************************************************************
 * Shows the FANET message 2 (Name) on the terminal
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |25.12.2018|  bet | Init Version
 **********************************************************************/
void terminal_message_2 (boolean _rxtx, boolean _integrity, sFanetMAC *_mac_data, sName *_name)
{
					
	terminal_mac_info (_rxtx, _integrity, _mac_data);
	
	// Name
	if (_rxtx) printf(KCYN); else printf(KGRN);
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
void terminal_message_3 (boolean _rxtx, boolean _integrity, sFanetMAC *_mac_data, sMessage *_message)
{
	terminal_mac_info (_rxtx, _integrity, _mac_data);
	
	// Message
	if (_rxtx) printf(KCYN); else printf(KGRN);
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
void terminal_message_4 (boolean _rxtx, boolean _integrity, sFanetMAC *_mac_data, sWeather *_weather_data)
{
					
	terminal_mac_info (_rxtx, _integrity, _mac_data);
	
	// Latitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf(" %+9.5f°",_weather_data->latitude);
	printf (KNRM); printf("|");

	// Longitude
	if (_rxtx) printf(KCYN); else printf(KGRN);
	printf(" %+9.5f°",_weather_data->longitude);
	printf (KNRM); printf("|");

	// Temperature
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_weather_data->temp) printf(" %+5.1f°C",_weather_data->temperature); else printf("   -.-°C");
	printf (KNRM); printf("|");

	// Winde heading
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_weather_data->wind) printf(" %5.1f°",_weather_data->wind_heading); else printf("   -.-°");
	printf (KNRM); printf("|");

	// Winde speed
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_weather_data->wind) printf(" %5.1fkm/h",_weather_data->wind_speed); else printf("   -.-km/h");
	printf (KNRM); printf("|");

	// Winde gusts
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_weather_data->wind) printf(" %5.1fkm/h",_weather_data->wind_gusts); else printf("   -.-km/h");
	printf (KNRM); printf("|");

	// Humidity
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_weather_data->humid) printf(" %5.1f%%rh",_weather_data->humidity); else printf("   -.-%%rh");
	printf (KNRM); printf("|");

	// Barometric
	if (_rxtx) printf(KCYN); else printf(KGRN);
	if (_weather_data->barom) printf(" %7.2fhPa",_weather_data->barometric); else printf("     -.-hPa");
	printf (KNRM); printf("|");
	
	printf("\n");
	
}


#endif
