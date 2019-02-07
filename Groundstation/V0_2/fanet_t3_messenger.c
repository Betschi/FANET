/*
 * fanet_t3_messenger.c
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


#include <stdio.h>
#include <math.h>
#include <time.h>
#include "fanet_mac.c"
#include "fanet_struct.c"
#include "fanet_mysql.c"
#include "fanet_global.c"
#include "fanet_radio.c"

#define STATION_MANUF	0xFC
#define STATION_ID		0x9002

#define	WELCOME_MESSAGE_LIFETIME	43200 //86400 	// 86400 sec = 24:00:00 h
#define	ONLINE_AGE		60			// Online if object is not older than 60 sec.

#define	START_OFFSET_MESSENGER	6;
#define INTERVAL_MESSENGER		20;


unsigned char    WELCOME_MESSAGE[] = "Willkommen im Fluggebiet Interlaken. Testbetrieb der ersten FANET-Bodenstation. Kontakt: christoph@betschart.ch";



void type_3_message_decoder (sRawMessage *_rx_raw_message, sMessage *_rx_message)
{
	int _i = 0;
	_rx_message->m_length = 0;
		
	_rx_message->subheader = _rx_raw_message->message[0];
	_rx_message->m_length++;
	
	// Check if lengt of name string isn't longer than 244 characters	
	if (_rx_raw_message->m_length >= 244)
		_rx_raw_message->m_length = 244;
	
	while (_i < _rx_raw_message->m_length-1)
	{
		_rx_message->message[_i] = _rx_raw_message->message[_i+1];
		_rx_message->m_length++;
		_i++;
	}
	_rx_message->message[_i] = 0;				// Add EOF at last
}


void type_3_message_coder (sRawMessage *_tx_raw_message, sMessage *_message)
{
	int _i = 0;
    _tx_raw_message->m_length = 0;
    
	// Subheader Type 3
	_tx_raw_message->message[_tx_raw_message->m_length] = _message->subheader;
	_tx_raw_message->m_length++;
	
	// Check if lengt of name string isn't longer than 244 characters	
	if (_message->m_length >= 244)
		_message->m_length = 244;
	
	while (_i < _message->m_length)
	{
		_tx_raw_message->message[_tx_raw_message->m_length]	= _message->message[_i];
		_tx_raw_message->m_length++;
		_i++;
	}
}

void send_message (sMessage	*_message)
{
	sRadioData		_radiodata;
	sFanetMAC		_fanet_mac;
	sRawMessage		_tx_raw_message;
	sMessage		_rx_message;

	_tx_raw_message.m_length = 0;
	_fanet_mac.type	= 3;
	_fanet_mac.s_manufactur_id	= _message->s_address_manufactur_id;
	_fanet_mac.s_unique_id		= _message->s_address_unique_id;
	_fanet_mac.e_header			= 0;
	_fanet_mac.forward			= 0;
	_fanet_mac.ack				= 0;
	_fanet_mac.cast				= 0;
	_fanet_mac.signature_bit 	= 0;
	

	if (_message->ack_req)
	{
		_fanet_mac.e_header			= 1;
		_fanet_mac.ack				= _message->ack_req;
	}
	_message->send_events++;
	
	if ((_message->d_address_manufactur_id > 0x00) & (_message->d_address_unique_id > 0x0000))
	{
		_fanet_mac.e_header			= 1;
		_fanet_mac.cast				= 1;
		_fanet_mac.d_manufactur_id	= _message->d_address_manufactur_id; 
		_fanet_mac.d_unique_id		= _message->d_address_unique_id;
	}
	
	
	type_3_message_coder (&_tx_raw_message, _message);
	fanet_mac_coder (&_radiodata, &_fanet_mac, &_tx_raw_message);
	
	// Monitor task
	//terminal_message_raw (1,0, &_radiodata, &_fanet_mac, &_tx_raw_message);
	type_3_message_decoder (&_tx_raw_message, &_rx_message);
	terminal_message_3 (true, false, &_radiodata,  &_fanet_mac, &_rx_message);	
	
	
}


void welcome_object (void)
{
	sOnline		_online;
	sMessage	_message;
	int			_i = 0;
	time_t t = time(NULL);

	_online.timestamp = t - ONLINE_AGE;
	search_online_address (&_online);
	
	while (_i < _online.online)
	{
		_message.d_address_manufactur_id	= _online.address_manufactur_id[_i];
		_message.d_address_unique_id		= _online.address_unique_id[_i];
	
		search_last_welcome_message (&_message);
		
		if (_message.time < (t - WELCOME_MESSAGE_LIFETIME))
		{
			_message.time = t;
			_message.subheader = 0;
			_message.s_address_manufactur_id = STATION_MANUF;
			_message.s_address_unique_id	 = STATION_ID;
			_message.ack_req				 = 1;
			_message.send_events			 = 0;
			_message.message_type			 = 4;

			_message.m_length = 0;
			while ((WELCOME_MESSAGE[_message.m_length]) && (_message.m_length < 244))
			{
				_message.message[_message.m_length] = WELCOME_MESSAGE[_message.m_length];
				_message.m_length++;
			}
			_message.message[_message.m_length] = 0;		// Terminate string with \0 
	
			send_message (&_message);
			write_message_send(&_message);
		}
		_i++;
	}
}

void fanet_t3_messenger_scheduler (void)
{


	static uint16_t		_data_timer = START_OFFSET_MESSENGER;

	if (!_data_timer)
	{
		welcome_object();

		_data_timer = INTERVAL_MESSENGER;
	}
	_data_timer--;
		
	
}

void type_3_message_receiver (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sRawMessage *_rx_payload)
{
	sMessage	_rx_message;
	
	type_3_message_decoder (_rx_payload, &_rx_message);
	
	terminal_message_3 (0,0, _radiodata, _fanet_mac, &_rx_message);

	//write_object_name_2 (_radiodata, _fanet_mac, &_rx_name);	
}
