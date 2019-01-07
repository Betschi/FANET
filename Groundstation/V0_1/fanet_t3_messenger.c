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
#include "fanet_struct.c"
#include "fanet_mysql.c"
#include "fanet_global.c"
#include "fanet_radio.c"

#define STATION_MANUF	0xFC
#define STATION_ID		0x9001

#define	ONLINE_AGE		60			// Online if object is not older than 60 sec.

#define	START_OFFSET_MESSENGER	4;
#define INTERVAL_MESSENGER		10;

unsigned char    WELCOME_MESSAGE[] = "Willkommen im Fluggebiet Interlaken. Testbetrieb der ersten FANET-Bodenstation. Kontakt: christoph@betschart.ch";



void type_3_message_decoder (sRawMessage *_rx_raw_message, sMessage *_rx_message)
{
	int _i = 0;
	_rx_message->m_length = 0;
	
	// Check if lengt of name string isn't longer than 244 characters	
	if (_rx_raw_message->m_length >= 244)
		_rx_raw_message->m_length = 244;
	
	while (_i < _rx_raw_message->m_length)
	{
		_rx_message->message[_i] = _rx_raw_message->message[_i];
		_rx_message->m_length++;
		_i++;
	}
}


void type_3_message_coder (sRawMessage *_tx_raw_message, sMessage *_message)
{
	int _i = 0;
    _tx_raw_message->m_length = 0;
    
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
	sFanetMAC		_fanet_mac;
	sRawMessage		_tx_raw_message;
	sMessage		_rx_message;

	_tx_raw_message.m_length = 0;
	_fanet_mac.type	= 3;
	_fanet_mac.s_manufactur_id	= _message->s_address_manufactur_id;
	_fanet_mac.s_unique_id		= _message->s_address_unique_id;
	_fanet_mac.e_header			= 0;
	_fanet_mac.ack				= 0;
	_fanet_mac.cast				= 0;
	_fanet_mac.signature_bit 	= 0;
	

	if (_message->ack_req)
	{
		_fanet_mac.e_header			= 1;
		_fanet_mac.ack				= _message->ack_req;
	}
	
	if ((_message->d_address_manufactur_id > 0x00) & (_message->d_address_unique_id > 0x0000))
	{
		_fanet_mac.e_header			= 1;
		_fanet_mac.cast				= 1;
		_fanet_mac.d_manufactur_id	= _message->d_address_manufactur_id; 
		_fanet_mac.d_unique_id		= _message->d_address_unique_id;
	}
	
	
	type_3_message_coder (&_tx_raw_message, _message);
	fanet_mac_coder (&_fanet_mac, &_tx_raw_message);
	
	// Monitor task
	type_3_message_decoder (&_tx_raw_message, &_rx_message);
	terminal_message_3 (true, false, &_fanet_mac, &_rx_message);	
	
	
}


void welcome_object (void)
{
	sMessage	_message;
	
	_message.time = time(NULL);
	_message.s_address_manufactur_id = STATION_MANUF;
	_message.s_address_unique_id	 = STATION_ID;
	
	_message.d_address_manufactur_id = 0x11;
	_message.d_address_unique_id	 = 0x0002;
	
	_message.ack_req				 = 1;
	_message.message_type			 = 4;
	
	_message.m_length = 0;
	while ((WELCOME_MESSAGE[_message.m_length]) && (_message.m_length < 244))
	{
		_message.message[_message.m_length] = WELCOME_MESSAGE[_message.m_length];
		_message.m_length++;
	}
	_message.message[_message.m_length] = 0;
	
	send_message (&_message);
	write_message_send(&_message);
	
}

void fanet_t3_messenger_scheduler (void)
{
	time_t t;

	sOnline				_online;

	static uint16_t		_data_timer = START_OFFSET_MESSENGER;

	if (!_data_timer)
	{

		t = time(NULL);
		
		_online.timestamp = t - ONLINE_AGE;
		

		search_online_address (&_online);
		welcome_object();
		
		

		_data_timer = INTERVAL_MESSENGER;
	}
	_data_timer--;
		
	
}
