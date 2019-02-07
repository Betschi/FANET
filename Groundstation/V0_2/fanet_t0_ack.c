/*
 * fanet_t0_ack.c
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

#ifndef FANET_T0_ACK_C
#define FANET_T0_ACK_C

#include <stdio.h>
//#include <math.h>
//#include <time.h>
#include "fanet_mac.c"
#include "fanet_struct.c"
#include "fanet_terminal.c"
#include "fanet_global.c"
//#include "fanet_mysql.c"
//#include "fanet_global.c"
//#include "fanet_radio.c"

void type_0_ack_decoder (sRawMessage *_rx_payload, sACK *_rx_ack)
{

}


void type_0_ack_coder (sRawMessage *_tx_message, sACK *_name)
{


}


void send_ack (sFanetMAC *_rx_fanet_mac)
{
	sRadioData		_radiodata;
	sFanetMAC		_tx_fanet_mac;
	sRawMessage		_tx_raw_message;
	sACK			_rx_ack;
	
	if (fanet_own_id_checker(_rx_fanet_mac))
	{
		_tx_fanet_mac.type	= 0;
		_tx_fanet_mac.s_manufactur_id	= _rx_fanet_mac->d_manufactur_id;
		_tx_fanet_mac.s_unique_id		= _rx_fanet_mac->d_unique_id;
		_tx_fanet_mac.d_manufactur_id	= _rx_fanet_mac->s_manufactur_id;
		_tx_fanet_mac.d_unique_id		= _rx_fanet_mac->s_unique_id;
		_tx_fanet_mac.e_header			= 1;
		_tx_fanet_mac.forward			= 0;
		_tx_fanet_mac.ack				= 0;
		_tx_fanet_mac.cast				= 1;
		_tx_fanet_mac.signature_bit 	= 0;
		
		_tx_raw_message.m_length		= 0;
		
		//type_0_ack_coder (&_tx_raw_message, _message);
		fanet_mac_coder (&_radiodata, &_tx_fanet_mac, &_tx_raw_message);
	
		// Monitor task
		//terminal_message_raw (1,0, &_radiodata, &_tx_fanet_mac, &_tx_raw_message);
		type_0_ack_decoder (&_tx_raw_message, &_rx_ack);
		terminal_message_0 (true, false, &_radiodata, &_tx_fanet_mac, &_rx_ack);	
	}	
}

void type_0_ack_receiver (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sRawMessage *_rx_payload)
{
	sACK	_rx_ack;
	
	type_0_ack_decoder (_rx_payload, &_rx_ack);
	
	terminal_message_0 (0,0, _radiodata, _fanet_mac, &_rx_ack);

	//write_object_name_2 (_radiodata, _fanet_mac, &_rx_name);	
}


#endif
