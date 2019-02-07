/*
 * fanet_mysql.c
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

#ifndef FANET_MYSQL_C
#define FANET_MYSQL_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "fanet_struct.c"
#include "fanet_global.c"

#define KNRM  "\x1B[0m"		// Color: Normal
#define KRED  "\x1B[31m"	// Color: Red
#define KGRN  "\x1B[32m"	// Color: Green
#define KYEL  "\x1B[33m"	// Color: Yellow
#define KBLU  "\x1B[34m"	// Color: Blue
#define KMAG  "\x1B[35m"	// Color: Mangenta
#define KCYN  "\x1B[36m"	// Color: Cyan
#define KWHT  "\x1B[37m"	// Color: White

char SQL_query_debug = 0;
char function_debug  = 0;
char message_debug   = 0;

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

/***********************************************************************
 * SQL error hanlder
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |24.09.2018|  bet | Init Version
 **********************************************************************/
void sql_finish_with_error(MYSQL *conn)
{
  fprintf(stderr, "[%sERR%s]   MySQL error: %s\n",KRED, KNRM, mysql_error(conn));
  mysql_close(conn);
  exit(1);        
}

/***********************************************************************
 * Login into SQL databench
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |24.09.2018|  bet | Init Version
 **********************************************************************/
void sql_login (void)
{
	char *_sql_server = "localhost";
	char *_sql_user = "pi";
	char *_sql_password = "raspberry"; /* set me first */
	char *_sql_database = "FANET";

	if (function_debug) printf("[%sFUNC%s]  sql_login()\n",KCYN, KNRM);

	if (message_debug) printf("[%sOK%s]    MySQL client version: %s\n",KGRN, KNRM, mysql_get_client_info());
	
	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, _sql_server, _sql_user, _sql_password, _sql_database, 0, NULL, 0))
	{
		sql_finish_with_error(conn);
	}
	else
		
	if (message_debug) printf("[%sOK%s]    MySQL server version: %s\n",KGRN, KNRM, mysql_get_server_info(conn));
	if (message_debug) printf("[%sOK%s]    MySQL host info     : %s\n",KGRN, KNRM, mysql_get_host_info(conn));

}

/***********************************************************************
 * Enters a new system data into table "FANET.system_data_1min"
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.09.2018|  bet | Init Version
 **********************************************************************/
void sql_write_system_data_1min (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sSystem *_system_data)
{
	char _sql_query[1000];
	
	if (function_debug) printf("[%sFUNC%s]  sql_write_system_data_1min()\n",KCYN, KNRM);

	sprintf(_sql_query, "INSERT INTO FANET.system_data_1min (timestamp, rx_rssi_avg, rx_rssi_max, rx_packet, tx_packet, tx_time) VALUES ('%ld','%+7.2f','%d','%d','%d','%d')",
		_system_data->timestamp_1min,
		_system_data->rssi_avg_1min,
		_system_data->rssi_max_1min,
		_system_data->rx_packets_1min,
		_system_data->tx_packets_1min,
		_system_data->tx_time_1min);

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
}

/***********************************************************************
 * Enters a new system data into table "FANET.system_data_15min"
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |29.12.2018|  bet | Init Version
 **********************************************************************/
void sql_write_system_data_15min (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sSystem *_system_data)
{
	char _sql_query[1000];
	
	if (function_debug) printf("[%sFUNC%s]  sql_write_system_data_15min()\n",KCYN, KNRM);
		
	sprintf(_sql_query, "INSERT INTO FANET.system_data_15min (timestamp, rx_rssi_avg, rx_rssi_max, rx_packet, tx_packet, tx_time) VALUES ('%ld','%+7.2f','%d','%d','%d','%d')",
		_system_data->timestamp_15min,
		_system_data->rssi_avg_15min,
		_system_data->rssi_max_15min,
		_system_data->rx_packets_15min,
		_system_data->tx_packets_15min,
		_system_data->tx_time_15min);		

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
}



/***********************************************************************
 * Enters a new tracking data into table "FANET.object_tracking"
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.09.2018|  bet | Init Version
 * 0.2 |10.11.2018|  bet | Modified: radiodata as struct
 * 0.3 |26.01.2019|  bet | Modified: tracking data as struct
 **********************************************************************/
void sql_write_tracking_data (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row,
	sRadioData	*_radiodata,
	sFanetMAC	*_fanet_mac,
	sTRACKING *_tracking)
{
	char _sql_query[1000];
	float _distance_meter;
	
	_distance_meter = _tracking->distance*1000;	// Convert from km to meter
	
	if (function_debug) printf("[%sFUNC%s]  sql_write_tracking_data()\n",KCYN, KNRM);
	
	// SQL Query: Creat a new "idtime" with the current time
	sprintf(_sql_query, "INSERT INTO FANET.object_tracking (time_stamp, idobject_name, P_RSSI, P_SNR, coding_rate, freq_dev, latitude, longitude, online_tracking, aircraft_typ, alt, speed, climb, heading, turn_rate, distance) VALUES ('%d', '%02x:%04x', '%d','%f', '%s', '%d', '%f', '%f', '%i', '%i', '%d', '%f', '%f', '%f', '%f', '%9.0f')",
		_radiodata->timestamp,
		_fanet_mac->s_manufactur_id,
		_fanet_mac->s_unique_id,
		_radiodata->prssi, //_P_RSSI,
		_radiodata->psnr,  //_P_SNR,
		_radiodata->coding_rate,
		_radiodata->freq_err, //_freq_dev,
		_tracking->latitude,
		_tracking->longitude,
		_tracking->tracking,
		_tracking->aircraft_type,
		_tracking->altitude,
		_tracking->speed,
		_tracking->climb,
		_tracking->heading,
		_tracking->turn_rate,
		_distance_meter);

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
}

/***********************************************************************
 * Update data in table FANET.object_name
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.09.2018|  bet | Init Version
 * 0.2 |01.01.2019|  bet | Modified: name data as struct
 **********************************************************************/
void sql_write_object_name (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row,
	sRadioData	*_radiodata,
	sFanetMAC	*_fanet_mac,
	sName		*_name)
{
	int  _idobject_found = 0;
	char _sql_query[1000];
	
	if (function_debug) printf("[%sFUNC%s]  sql_update_object_name_2()\n",KCYN, KNRM);

	// SQL Query: Search "idobject_name" if object_name already exists
	sprintf(_sql_query, "SELECT idobject_name FROM FANET.object_name WHERE idobject_name = '%02x:%04x'",
		_fanet_mac->s_manufactur_id, _fanet_mac->s_unique_id);
	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);
	
	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}

	_res = mysql_use_result(_conn);

	// SQL readout
	while ((_row = mysql_fetch_row(_res)) != NULL)
	{
		if (message_debug) printf("[%sOK%s]    SQL idobject_name for %02x:%04x found\n",KGRN , KNRM, _fanet_mac->s_manufactur_id, _fanet_mac->s_unique_id);

		_idobject_found = 1;

		//if (_row[5]!=NULL) { *_last_welcome_message    = atoi(_row[5]);} else *_last_welcome_message = 0;

		sprintf(_sql_query, "UPDATE FANET.object_name SET name = '%s', last_seen = '%d' WHERE idobject_name = '%02x:%04x'",
			_name->name, _radiodata->timestamp, _fanet_mac->s_manufactur_id, _fanet_mac->s_unique_id);
		if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);
	}

	
	if (!_idobject_found)			// idobject_name does not exists
	{
		//*_last_welcome_message = 0;
		
		sprintf(_sql_query, "INSERT INTO FANET.object_name (idobject_name, name, first_seen) VALUES ('%02x:%04x','%s','%d')",
			_fanet_mac->s_manufactur_id, _fanet_mac->s_unique_id, _name->name, _radiodata->timestamp);
		if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);
	}

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
}

/***********************************************************************
 * Searchs in the SQL table "FANET.weather_station" the station information 
 * of a specific weather station.
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |24.09.2018|  bet | Init Version
 * 0.2 |01.01.2019|  bet | Modified: weather station name as struct
 **********************************************************************/
void sql_get_weather_station (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sWeather *_weather_data)
{
	char _data_found = 0;
	char _sql_query[1000];
	
	// Clears variable
	strcpy (_weather_data->name, "");
	strcpy (_weather_data->short_name, "");
	_weather_data->longitude = 0;
	_weather_data->latitude = 0;
	_weather_data->altitude = 0;
	
	if (function_debug) printf("[%sFUNC%s]  sql_get_weather_values()\n",KCYN, KNRM);

	sprintf(_sql_query, "SELECT * FROM FANET.weather_stations WHERE _id = '%s'", _weather_data->id_station);
	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}

	_res = mysql_use_result(_conn);

	// SQL readout
	while ((_row = mysql_fetch_row(_res)) != NULL)
	{
		if (_row[0]) _data_found = 1;
		if (message_debug) printf("[%sOK%s]    SQL Data for %s is: %s %s %s %s %s %s \n",
			KGRN , KNRM, _weather_data->id_station,
			_row[0], _row[1], _row[2], _row[3], _row[4], _row[5]);

		if (_row[1]!=NULL) { strcpy(_weather_data->name,  _row[1]);}
		if (_row[2]!=NULL) { strcpy(_weather_data->short_name, _row[2]);}
		if (_row[3]!=NULL) { _weather_data->longitude	= strtof(_row[3], NULL);}	
		if (_row[4]!=NULL) { _weather_data->latitude   = strtof(_row[4], NULL);}
		if (_row[5]!=NULL) { _weather_data->altitude	= atoi(_row[5]);}
	}
	if (!_data_found && message_debug)
	{
		printf("[%sWARN%s]  SQL Data for %s does not exist\n",KYEL , KNRM, _weather_data->id_station);
	}
}

/***********************************************************************
 * Searchs in the SQL table "FANET.weather_values" latest weather values
 * of a specific weather station.
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |24.09.2018|  bet | Init Version
 * 0.2 |01.01.2019|  bet | Modified: weather data as struct
 **********************************************************************/
void sql_get_weather_values (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sWeather *_weather_data)
{
	int  _idtime_found = 0;
	char _sql_query[1000];

	// Clears all weather variables to zero
	_weather_data->temp  = false;
	_weather_data->humid = false;
	_weather_data->barom = false;
	_weather_data->wind  = false;
	_weather_data->time = 0;
	_weather_data->temperature = 0;
	_weather_data->humidity = 0;
	_weather_data->barometric = 0;
	_weather_data->wind_speed = 0;
	_weather_data->wind_gusts = 0;
	_weather_data->wind_heading = 0;
	
	if (function_debug) printf("[%sFUNC%s]  sql_get_weather_values()\n",KCYN, KNRM);

	sprintf(_sql_query, "SELECT * FROM FANET.weather_values WHERE (_id_stations = '%s') AND time = (SELECT MAX(time) FROM FANET.weather_values WHERE (_id_stations = '%s'))", _weather_data->id_station, _weather_data->id_station);
	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}

	_res = mysql_use_result(_conn);

	// SQL readout
	while ((_row = mysql_fetch_row(_res)) != NULL)
	{
		_idtime_found = atoi (_row[0]);
		if (message_debug) printf("[%sOK%s]    SQL Data for %s is: %s %s %s %s %s %s %s %s %s\n",
			KGRN , KNRM, _weather_data->id_station,
			_row[0], _row[1], _row[2], _row[3], _row[4], _row[5], _row[6], _row[7], _row[8]);

		if (_row[2]!=NULL) { _weather_data->time		= atoi(_row[2]);}
		if (_row[3]!=NULL) { _weather_data->temperature	= strtof(_row[3], NULL); _weather_data->temp  = true;}
		if (_row[4]!=NULL) { _weather_data->humidity	= strtof(_row[4], NULL); _weather_data->humid = true;}	
		if (_row[5]!=NULL) { _weather_data->barometric  = strtof(_row[5], NULL); _weather_data->barom = true;}
		if (_row[6]!=NULL) { _weather_data->wind_speed	= strtof(_row[6], NULL); _weather_data->wind  = true;}
		if (_row[7]!=NULL) { _weather_data->wind_gusts 	= strtof(_row[7], NULL); _weather_data->wind  = true;}
		if (_row[8]!=NULL) { _weather_data->wind_heading= strtof(_row[8], NULL); _weather_data->wind  = true;}

	}
	if (!_idtime_found && message_debug)
	{
		printf("[%sWARN%s]  SQL Data for %s does not exist\n",KYEL , KNRM, _weather_data->id_station);
	}
}

/***********************************************************************
 * Delete old date in table "FANET.system_data_1min"
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.12.2018|  bet | Init Version
 **********************************************************************/
void sql_delete_old_data (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row)
{
	char _sql_query[1000];
	
	if (function_debug) printf("[%sFUNC%s]  sql_delete_old_data()\n",KCYN, KNRM);
	
	// SQL Query: Creat a new "idtime" with the current time

	sprintf(_sql_query, "DELETE FROM FANET.system_data_1min WHERE (UNIX_TIMESTAMP()-timestamp) >= 864000");

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
}


/***********************************************************************
 * Update routing table FANET.routing_table
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.09.2018|  bet | Init Version
 **********************************************************************/
void sql_update_routing_table (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sRouting *_routing_data)
{
	int  _idobject_found = 0;
	char _sql_query[1000];
	
	if (function_debug) printf("[%sFUNC%s]  sql_update_routing_table()\n",KCYN, KNRM);

	// SQL Query: Search "idobject_name" if object_name already exists
	sprintf(_sql_query, "SELECT address FROM FANET.routing_table WHERE address = '%02x:%04x'",
		_routing_data->address_manufactur_id, _routing_data->address_unique_id);
	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);
	
	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}

	_res = mysql_use_result(_conn);

	// SQL readout
	while ((_row = mysql_fetch_row(_res)) != NULL)
	{
		if (message_debug) printf("[%sOK%s]    SQL address for %02x:%04x found\n",KGRN , KNRM, _routing_data->address_manufactur_id, _routing_data->address_unique_id);

		_idobject_found = 1;

		sprintf(_sql_query, "UPDATE FANET.routing_table SET last_seen = '%ld', snr = '%+6.2f' WHERE address = '%02x:%04x'",
			_routing_data->last_seen, _routing_data->snr, _routing_data->address_manufactur_id, _routing_data->address_unique_id);
		if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);
	}

	
	if (!_idobject_found)			// address does not exists
	{
		sprintf(_sql_query, "INSERT INTO FANET.routing_table (address, last_seen, snr) VALUES ('%02x:%04x','%ld','%+6.2f')",
			_routing_data->address_manufactur_id, _routing_data->address_unique_id, _routing_data->last_seen, _routing_data->snr);
		if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);
	}

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
}


/***********************************************************************
 * Search for online 
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.09.2018|  bet | Init Version
 **********************************************************************/
void sql_search_online_address (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sOnline *_online)
{
	char _sql_query[1000];
		
	_online->online = 0;
	
	if (function_debug) printf("[%sFUNC%s]  sql_search_online_address()\n",KCYN, KNRM);
	
	sprintf(_sql_query, "SELECT * FROM FANET.routing_table WHERE `last_seen` > %ld",
		_online->timestamp);

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}
	
	_res = mysql_use_result(_conn);

	_online->online = 0;
	// SQL readout
	while ((_row = mysql_fetch_row(_res)) != NULL)
	{
		if (message_debug) printf("[%sOK%s]    Online: %s\n",
			KGRN , KNRM, _row[1]);

		if (_row[1]!=NULL) {address_int (_row[1], &_online->address_manufactur_id[_online->online], &_online->address_unique_id[_online->online]);}
		_online->online++;
		if (_online->online == 255) break;
	}
	if (!_online->online && message_debug)
	{
		printf("[%sWARN%s]  Nobody online\n",KYEL , KNRM);
	}
}


/***********************************************************************
 * Write message to FANET.message_send
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |26.09.2018|  bet | Init Version
 **********************************************************************/
void sql_write_message_send (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sMessage *_message)
{
	char _sql_query[1000];
	
	sprintf(_sql_query, "INSERT INTO FANET.messages_send (timestamp, subheader, s_address, d_address, ack_set, send_events, message_type, message) VALUES ('%d','%d', '%02x:%04x','%02x:%04x','%d','%d','%d','%s')",
		_message->time,
		_message->subheader,
		_message->s_address_manufactur_id, _message->s_address_unique_id,
		_message->d_address_manufactur_id, _message->d_address_unique_id,
		_message->ack_req,
		_message->send_events,
		_message->message_type,
		_message->message);

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}	
}

/***********************************************************************
 * Search Welcome message in FANET.message_send
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |30.12.2018|  bet | Init Version
 **********************************************************************/
void sql_search_last_welcome_message (MYSQL *_conn, MYSQL_RES *_res, MYSQL_ROW _row, sMessage *_message)
{
	char _sql_query[1000];
	
	_message->time = 0;
	
	sprintf(_sql_query, "SELECT * FROM FANET.messages_send WHERE d_address = '%02x:%04x' AND message_type = 4 ORDER by timestamp DESC LIMIT 1",
		_message->d_address_manufactur_id, _message->d_address_unique_id);

	if (SQL_query_debug) printf("[%sSQL%s]   %s\n",KBLU, KNRM, _sql_query);

	mysql_free_result(_res);

	if (mysql_query(_conn, _sql_query))
	{
		sql_finish_with_error(_conn);
	}

	_res = mysql_use_result(_conn);

	// SQL readout
	while ((_row = mysql_fetch_row(_res)) != NULL)
	{
		if (message_debug) printf("[%sOK%s]    Welcome message found for %s with timestamp: %s\n",
			KGRN , KNRM, _row[4], _row[1]);

		if (_row[1]!=NULL) {(_message->time = atoi(_row[1]));}
	}		
}



void get_weather_data(sWeather *_weather_data)
{
	sql_get_weather_values (conn, res, row, _weather_data);
}


void get_weather_station(sWeather *_weather_data)
{
	sql_get_weather_station (conn, res, row, _weather_data);
}


void write_system_data_1min (sSystem *_system_data)
{
	sql_write_system_data_1min (conn, res, row, _system_data);
}


void write_system_data_15min (sSystem *_system_data)
{
	sql_write_system_data_15min (conn, res, row, _system_data);
}


void write_object_tracking (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sTRACKING *_tracking)
{
	sql_write_tracking_data (conn, res, row, _radiodata, _fanet_mac, _tracking);
}


void write_object_name (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sName *_name)
{
	sql_write_object_name (conn, res, row, _radiodata, _fanet_mac, _name);
}

void update_routing_table (sRouting *_routing_data)
{
	sql_update_routing_table (conn, res, row, _routing_data);
}

void write_message_send (sMessage *_message)
{
	sql_write_message_send (conn, res, row, _message);
}

void search_last_welcome_message (sMessage *_message)
{
	sql_search_last_welcome_message (conn, res, row, _message);
}

void search_online_address (sOnline *_online)
{
	sql_search_online_address (conn, res, row, _online);
}

void delete_old_data (void)
{
	sql_delete_old_data (conn, res, row);
}

#endif
