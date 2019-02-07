/*
 * fanet_routing.c
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

#ifndef FANET_ROUTING_C
#define FANET_ROUTING_C

#include <stdio.h>
#include <string.h>
#include "fanet_struct.c"
#include "fanet_mysql.c"

void update_routing (sRadioData *_radiodata, sFanetMAC *_mac_data)
{
	sRouting	_routing_data;
	
	if (_mac_data->valid_bit && !_radiodata->crc_err)
	{
		if (_radiodata->psnr >= -8)					// Rejecting weak signals from the routing table. SNR has to be better than -8 dB.
		{
			_routing_data.address_manufactur_id = _mac_data->s_manufactur_id;
			_routing_data.address_unique_id = _mac_data->s_unique_id;
			_routing_data.last_seen = _radiodata->timestamp;
			_routing_data.snr = _radiodata->psnr;

			update_routing_table (&_routing_data);
		}
	}
}


#endif

