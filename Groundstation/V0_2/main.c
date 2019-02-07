/*
 * main.c
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


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>

#include "fanet_struct.c"
#include "fanet_radio.c"
#include "fanet_mac.c"
#include "fanet_mysql.c"
#include "fanet_terminal.c"
#include "fanet_t0_ack.c"
#include "fanet_t1_tracking.c"
#include "fanet_t3_messenger.c"
#include "fanet_t4_service.c"
#include "fanet_routing.c"

#include <sys/ioctl.h> 

#include <wiringPi.h>
#include <wiringPiSPI.h>

time_t t;
struct tm *tm;


struct timeval tv;
struct tm* ptm;
char time_string[40];
long milliseconds;

byte sf;

enum { SF7=7, SF8, SF9, SF10, SF11, SF12 };

// Set spreading factor (SF7 - SF12)
byte sf = SF7;


void die(const char *s)
{
    perror(s);
    exit(1);
}


void system_data ()
{
	static sSystem	_system_data;
	sRadioData		_radiodata;
		
	static int 		_yy;
	static int		_rssi_sum_1min;
	static float	_rssi_sum_15min;
	static int		_rssi_max_1min = -157;
	static int		_rssi_max_15min = -157;
	static int  	_rssi_avg_1min_counter;
	static int  	_rssi_avg_15min_counter;
	
	if (_yy == 500)			// 500 ms update rate
	{
		t = time(NULL);
		tm = localtime(&t);

		gettimeofday (&tv, NULL);
		ptm = localtime (&tv.tv_sec);
		milliseconds = tv.tv_usec / 1000;
		strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);	

		read_radio_data(&_radiodata);
		
		if ((_radiodata.rssi > -130) & (_radiodata.rssi < 0))		// Ignore RSSI values <-130dBm and > 0dBm
		{
			_rssi_sum_1min += _radiodata.rssi;
			_rssi_avg_1min_counter++;
			if (_radiodata.rssi > _rssi_max_1min)
				_rssi_max_1min = _radiodata.rssi;
		}

        printf("|%s.%03ld| RSSI:%+4ddBm  RX Headers: %d \b\r", time_string, milliseconds, _radiodata.rssi, _radiodata.rx_headers);	
		fflush(stdout);
	
		if (!(t % 60) && _rssi_avg_1min_counter > 2)		// Modulo Divison: Volle Minute = kein Rest bei Disision durch 60 (1 min)
		{
			radio_info(&_system_data);						// Reads the tx/rx packets and tx time. After reading, values will be reset to 0.
			
			_system_data.timestamp_1min = t;
			_system_data.rssi_avg_1min = (float)_rssi_sum_1min / (float)_rssi_avg_1min_counter;
			_system_data.rssi_max_1min = _rssi_max_1min;
			
			printf ("\nSystem  1 min Data: %ld RSSI avg: %+7.2fdBm  RSSI max: %ddBm  RX packets: %d  TX packets: %d  TX time: %dms\n",
				_system_data.timestamp_1min,
				_system_data.rssi_avg_1min,
				_system_data.rssi_max_1min,
				_system_data.rx_packets_1min,
				_system_data.tx_packets_1min,
				_system_data.tx_time_1min);

			write_system_data_1min (&_system_data);
			//delete_old_data();	
			
			_rssi_sum_15min += _system_data.rssi_avg_1min;
			_rssi_avg_15min_counter++;
			if (_rssi_max_1min > _rssi_max_15min)
				_rssi_max_15min = _rssi_max_1min;

			_system_data.rx_packets_15min += _system_data.rx_packets_1min;
			_system_data.tx_packets_15min += _system_data.tx_packets_1min;
			_system_data.tx_time_15min += _system_data.tx_time_1min;

			_rssi_sum_1min = 0;
			_rssi_avg_1min_counter = 0;
			_rssi_max_1min = -157;
			
			_system_data.rx_packets_1min = 0;
			_system_data.tx_packets_1min = 0;
			_system_data.tx_time_1min = 0;
			
			if (!(t % 900) && _rssi_avg_15min_counter > 2)		// Modulo Divison: Volle Minute = kein Rest bei Disision durch 900 (15 min)
			{
				_system_data.timestamp_15min = t;
				
				_system_data.rssi_avg_15min = _rssi_sum_15min /(float) _rssi_avg_15min_counter;
				_system_data.rssi_max_15min = _rssi_max_15min;

				printf ("\nSystem 15 min Data: %ld RSSI avg: %+7.2fdBm  RSSI max: %ddBm  RX packets: %d  TX packets: %d  TX time: %dms\n",
					_system_data.timestamp_15min,
					_system_data.rssi_avg_15min,
					_system_data.rssi_max_15min,
					_system_data.rx_packets_15min,
					_system_data.tx_packets_15min,
					_system_data.tx_time_15min);

				write_system_data_15min (&_system_data);				
				
				_rssi_sum_15min = 0;
				_rssi_avg_15min_counter = 0;
				_rssi_max_15min = -157;
				
				_system_data.rx_packets_15min = 0;
				_system_data.tx_packets_15min = 0;
				_system_data.tx_time_15min = 0;
			}
		}
		_yy = 0;
	}
	_yy++;
}


void receivepacket()
{
	sFanetMAC	  _fanet_mac;
	sRawMessage   _rx_radio;
	sRawMessage	  _rx_payload;
	sRadioData 	  _radiodata;

	if(read_rx_data(&_rx_radio, &_radiodata))
    {
		_fanet_mac.valid_bit = 1; 
		
		if (_radiodata.crc_err)
			terminal_message_crc_err (0,0,&_radiodata, &_fanet_mac);

		fanet_mac_decoder (&_fanet_mac, &_rx_radio, &_rx_payload);
		if (!_fanet_mac.valid_bit)
			terminal_message_mac_err (0,0,&_radiodata, &_fanet_mac);

		terminal_message_raw (0,0, &_radiodata, &_fanet_mac, &_rx_radio);

					
		if (_fanet_mac.valid_bit && !_radiodata.crc_err)
		{
			switch (_fanet_mac.type)
			{
				case 0:	type_0_ack_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 1:	type_1_tracking_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 2:	type_2_name_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 3: type_3_message_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 4: type_4_service_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				//default: terminal_mac_info (0,0,&_fanet_mac); printf ("\n");
			}
			if (_fanet_mac.ack)
				send_ack (&_fanet_mac);			
		}
		else
		{
			terminal_rf_info (0,0,&_radiodata, &_fanet_mac);
			terminal_mac_info (0,0,&_radiodata, &_fanet_mac); printf ("\n");
		}
		
		update_routing (&_radiodata, &_fanet_mac);
    }
}


int main (int argc, char *argv[])
{

    time_t now;
    struct tm *tm;

	char _second_new;
	char _second_old;
	//char _minute_new;
	//char _minute_old;

    sql_login();
    
    /*if (argc < 2) {
        printf ("Usage: argv[0] sender|rec [message]\n");
        exit(1);
    }*/

    init_fanet_radio();

    delay(2000);

    //start_screen();
    terminal_start_screen(sf, 250, freq);

    

    while(1)
    {
		now = time(0);
		tm = localtime (&now);
		_second_new = tm->tm_sec;
		//_minute_new = tm->tm_min;
		
		system_data();
		receivepacket();
		
		if (_second_new!=_second_old) 
		{
		  _second_old = _second_new;
		  
		  fanet_t4_service_scheduler();
		  fanet_t3_messenger_scheduler();
		
		}
		
		/*if (_minute_new!=_minute_old)
		{
			_minute_old = _minute_new;
			show_register();
		}*/
		
        delay(1);
	}
    return (0);
}
