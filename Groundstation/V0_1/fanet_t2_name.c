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
//#include <math.h>
//#include <time.h>
#include "fanet_struct.c"
//#include "fanet_mysql.c"
//#include "fanet_global.c"
//#include "fanet_radio.c"

void type_2_name_decoder (sRawMessage *_rx_message, sName *_rx_name)
{
	int _i = 0;
	_rx_name->n_length = 0;
	
	// Check if lengt of name string isn't longer than 244 characters	
	if (_rx_message->m_length >= 244)
		_rx_message->m_length = 244;
	
	while (_i < _rx_message->m_length)
	{
		_rx_name->name[_i] = _rx_message->message[_i];
		_rx_name->n_length++;
		_i++;
	}
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

#endif
