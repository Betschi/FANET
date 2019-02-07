/*
 * fanet_t2_name.c
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

#ifndef FANET_T2_NAME_C
#define FANET_T2_NAME_C

#include <stdio.h>
#include "fanet_struct.c"
#include "fanet_terminal.c"
#include "fanet_mysql.c"


void type_2_name_decoder (sRawMessage *_rx_payload, sName *_rx_name)
{
	int _i = 0;
	_rx_name->n_length = 0;
	
	// Check if lengt of name string isn't longer than 244 characters	
	if (_rx_payload->m_length >= 244)
		_rx_payload->m_length = 244;
	
	while (_i < _rx_payload->m_length)
	{
		_rx_name->name[_i] = _rx_payload->message[_i];
		if (_rx_name->name[_i] == 0x27)	// Check if Apostrophe " ' " ASCII = 0x27 is present -> " ' " can crash SQL routine
			_rx_name->name[_i] = 0x20;	// Replace " ' " with space " "		
		_rx_name->n_length++;
		_i++;
	}
	_rx_name->name[_i] = 0;				// Add EOF at last
}


void type_2_name_coder (sRawMessage *_tx_message, sName *_name)
{
	int _i = 0;
    _tx_message->m_length = 0;
    
	// Check if lengt of name string isn't longer than 244 characters	
	if (_name->n_length >= 244)
		_name->n_length = 244;
	
	while (_i < _name->n_length)
	{
		_tx_message->message[_tx_message->m_length]	= _name->name[_i];
		_tx_message->m_length++;
		_i++;
	}
}

void type_2_name_receiver (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sRawMessage *_rx_payload)
{
	sName	_rx_name;
	
	type_2_name_decoder (_rx_payload, &_rx_name);
	
	terminal_message_2 (0,0, _radiodata, _fanet_mac, &_rx_name);

	write_object_name (_radiodata, _fanet_mac, &_rx_name);	
}


#endif
