/***********************************************************************
 * fanet_mac.c
 * 
 * Copyright 2018  <christoph@betschart.ch>
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

#ifndef FANET_MAC_C
#define FANET_MAC_C

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "fanet_struct.c"
#include "fanet_radio.c"
#include "fanet_global.c"



/***********************************************************************
 * Fusion of payload data with FANET MAC data.
 *  
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |02.12.2018|  bet | Init Version
 **********************************************************************/
void fanet_mac_coder (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sRawMessage *_tx_message)
{
	sRawMessage	_tx_payload;
	byte		_i = 0;
	
	_tx_payload.m_length   = 0;
	_tx_payload.message[0] = 0;
	
	if (_fanet_mac->e_header)
		_tx_payload.message[0] |= 0x80;
	if (_fanet_mac->forward)
		_tx_payload.message[0] |= 0x40;
		
	_tx_payload.message[0] += (_fanet_mac->type&0x3F);
	
	_tx_payload.message[1] = _fanet_mac->s_manufactur_id;
	_tx_payload.message[2] = _fanet_mac->s_unique_id&0x00FF;
	_tx_payload.message[3] = (_fanet_mac->s_unique_id&0xFF00)>>8;
	
	_tx_payload.m_length = 4;
	
	if (_fanet_mac->e_header)
	{
		_tx_payload.message[_tx_payload.m_length] = 0;
		if (_fanet_mac->ack)			_tx_payload.message[_tx_payload.m_length] |= (_fanet_mac->ack<<6);
		if (_fanet_mac->cast) 			_tx_payload.message[_tx_payload.m_length] |= 0x20;
		if (_fanet_mac->signature_bit)	_tx_payload.message[_tx_payload.m_length] |= 0x10;
		_tx_payload.m_length += 1;
	}
	
	if (_fanet_mac->cast)
	{
		_tx_payload.message[_tx_payload.m_length]	= _fanet_mac->d_manufactur_id;
		_tx_payload.message[_tx_payload.m_length+1]	= _fanet_mac->d_unique_id&0x00FF;
		_tx_payload.message[_tx_payload.m_length+2]	= (_fanet_mac->d_unique_id&0xFF00)>>8;
		_tx_payload.m_length += 3;
	}
	
	if (_fanet_mac->signature_bit)
	{
		_tx_payload.message[_tx_payload.m_length]	= _fanet_mac->signature&0x000000FF;
		_tx_payload.message[_tx_payload.m_length+1]	= (_fanet_mac->signature&0x0000FF00)>>8;			
		_tx_payload.message[_tx_payload.m_length+2]	= (_fanet_mac->signature&0x00FF0000)>>16;
		_tx_payload.message[_tx_payload.m_length+3]	= (_fanet_mac->signature&0xFF000000)>>24;
		_tx_payload.m_length += 4;
	}
	
	
	while (_i < _tx_message->m_length)
	{
		_tx_payload.message[_tx_payload.m_length+_i] = _tx_message->message[_i];
		_i++;
	}
	_tx_payload.m_length += _tx_message->m_length;	
	
	write_tx_data(_radiodata, &_tx_payload);

}



void fanet_mac_decoder (sFanetMAC *_fanet_mac, sRawMessage *_rx_message, sRawMessage *_rx_payload)
{
	_fanet_mac->e_header 		= FALSE;
	_fanet_mac->forward  		= FALSE;
	_fanet_mac->type			= 0;
	_fanet_mac->cast          	= FALSE;
	_fanet_mac->signature_bit 	= FALSE;
	_fanet_mac->s_manufactur_id = 0;
	_fanet_mac->s_unique_id		= 0;
	_fanet_mac->d_manufactur_id = 0;
	_fanet_mac->d_unique_id		= 0;
	_fanet_mac->signature		= 0;
	_fanet_mac->valid_bit		= TRUE;				// If a MAC error is detected, set bit to false
	_rx_payload->m_length 		= 0;
	
	if (_rx_message->m_length >= 4)					// Check if minimum FANET data are received/available
	{
		if (_rx_message->message[0]&0x80) _fanet_mac->e_header = TRUE; else	_fanet_mac->e_header = FALSE;
		if (_rx_message->message[0]&0x40) _fanet_mac->forward  = TRUE; else _fanet_mac->forward  = FALSE;
	
		_fanet_mac->type			 = (_rx_message->message[0]&0x3F);	
		if (!fanet_type_check(_fanet_mac->type))	// Check if valid FANET type is received/available
			_fanet_mac->valid_bit = FALSE;
		
		_fanet_mac->s_manufactur_id  = _rx_message->message[1];
		if (!fanet_manufacturer_check(_fanet_mac->s_manufactur_id))	// Check if valid FANET manufacturer is received/available
			_fanet_mac->valid_bit = FALSE;
		
		_fanet_mac->s_unique_id		 = (_rx_message->message[3])<<8;
		_fanet_mac->s_unique_id 	+= _rx_message->message[2];
	
		_rx_message->m_pointer = 4;
	
		if (_fanet_mac->e_header)
		{
			_fanet_mac->ack = (_rx_message->message[_rx_message->m_pointer]&0xC0)>>6;
			if(_rx_message->message[_rx_message->m_pointer]&0x20) _fanet_mac->cast          = TRUE; else _fanet_mac->cast          = FALSE;
			if (_fanet_mac->ack && !_fanet_mac->cast)	// Check if ACK is set, CAST bit is set too
				_fanet_mac->valid_bit = FALSE;	
			
			if(_rx_message->message[_rx_message->m_pointer]&0x10) _fanet_mac->signature_bit = TRUE; else _fanet_mac->signature_bit = FALSE;
		
			_rx_message->m_pointer += 1;
		}
	
		if (_fanet_mac->cast & _fanet_mac->e_header)
		{
			_fanet_mac->d_manufactur_id  = _rx_message->message[_rx_message->m_pointer];
			if (!fanet_manufacturer_check(_fanet_mac->d_manufactur_id))	// Check if valid FANET manufacturer is received/available
				_fanet_mac->valid_bit = FALSE;
				
			_fanet_mac->d_unique_id		 = (_rx_message->message[_rx_message->m_pointer+2])<<8;
			_fanet_mac->d_unique_id 	+= _rx_message->message[_rx_message->m_pointer+1];		

			_rx_message->m_pointer += 3;
		}
	
		if (_fanet_mac->signature_bit & _fanet_mac->e_header)
		{
			_fanet_mac->signature		 = (_rx_message->message[_rx_message->m_pointer+3])<<8;
			_fanet_mac->signature		+= (_rx_message->message[_rx_message->m_pointer+2])<<8;
			_fanet_mac->signature		+= (_rx_message->message[_rx_message->m_pointer+1])<<8;		
			_fanet_mac->signature	 	+= _rx_message->message[_rx_message->m_pointer];			

			_rx_message->m_pointer += 4;
		}
	
		while (_rx_message->m_pointer < _rx_message->m_length)
		{
			_rx_payload->message[_rx_payload->m_length] = _rx_message->message[_rx_message->m_pointer];
			_rx_message->m_pointer++;
			_rx_payload->m_length++;
		}
	}
	else
		_fanet_mac->valid_bit = FALSE;		
}

#endif
