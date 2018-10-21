#!/usr/bin/env python
# -*- coding: utf-8 -*-

# File:				weatherdata_winds_x.py
#
# Function:			Takes from https://winds.mobi live weather informations from the selected weather stations.
#					Informations are delivered in JSON format.
#					Informations are stored in the MariaDB (SQL)
#					More information about the winds.mobi API under https://winds.mobi/doc/#!/stations
#
# Requirements:		- Installed MariaDB with installed "FANET" databench
#					- Internet connection
#
# Version:			0.1 (Beta)
# Date:				21.10.2018
# Author:			Christoph Betschart (christoph[at]betschart.ch)

import time
import json
import requests
import mysql.connector
import datetime

# Configuration
max_stations   = 8				# Limits the number of weather stations Recommendation: 8... max 16 stations
repetiton_time = 30 			# Time for the next read from https://winds.mobi Recommendation: 30...60 seconds
	
#	- Weather stations:
station	   = [None] * max_stations
station[0] = 'windline-4117'	# Landeplatz Höhematte, Interlaken, BE
station[1] = 'windline-4116'	# Landeplatz Lehn, Interlaken, BE
station[2] = 'windline-5200'	# Startplatz Amisbühl, Beatenberg, BE
station[3] = 'windline-4115'	# Hohwald, Beatenberg, BE
station[4] = 'windline-4109'	# Niederhorn, Beatenberg, BE
station[5] = 'meteoswiss-INT'	# Interlaken
station[6] = None				# Reserve
station[7] = None				# Reserve


# Creat URL for https request
# DEBUG: url = requests.get('https://winds.mobi/api/2/stations/?limit=5&provider=jdc')
# DEBUG: url = requests.get('https://winds.mobi/api/2/stations/?ids=windline-4117&ids=windline-4116&ids=windline-5200&ids=windline-4115&ids=windline-4109&ids=meteoswiss-INT')
url_text = "https://winds.mobi/api/2/stations/"
station_nr = 0
while station[station_nr]:
	if station_nr == 0:
		url_text += "?ids="+station[station_nr]
	else:
		url_text += "&ids="+station[station_nr]			
	station_nr += 1

time_stamp	= [0] * max_stations

_id 		= [None] * max_stations
name 		= [None] * max_stations
short 		= [None] * max_stations
latitude 	= [None] * max_stations
longitude 	= [None] * max_stations
alt 		= [None] * max_stations
temp 		= [None] * max_stations
hum 		= [None] * max_stations
pres 		= [None] * max_stations
w_avg 		= [None] * max_stations
w_max 		= [None] * max_stations
w_dir 		= [None] * max_stations

# Connects with the local MariaDB.
# Install "FANET" databench previously
# Caution: passwd="raspberry" is the standard password of the Rasperry Pi. Chnage it if necessary.
mydb = mysql.connector.connect(
  host="localhost",
  user="pi",
  passwd="raspberry",
  database="FANET"
)
    
while True:
	url = requests.get(url_text)
	url.encoding = 'utf-8'
	weather = json.loads(url.text)
	print "Request data..."

	# Read out each weather station
	for x in range(station_nr):
		print "------------------------------------------------------"
		if time_stamp[x] ==  weather[x]['last']['time']:
			print "No data changed for Station %d"%(x),weather[x]['short']
		elif time_stamp[x] <>  weather[x]['last']['time']:
			time_stamp[x] = weather[x]['last']['time']
			
			print "ID  : ",weather[x]['_id']
			print "Name: ",weather[x]['name']
			print "Short:",weather[x]['short']
			print "Longitute     : ",weather[x]['loc']['coordinates'][0]
			print "Latitude      : ",weather[x]['loc']['coordinates'][1]
			print "Altitude      : ",weather[x]['alt'],"m"
			print "Time          : ",weather[x]['last']['time'],"sec = ", datetime.datetime.fromtimestamp(weather[x]['last']['time']).strftime('%Y-%m-%d %H:%M:%S')

			# Check, if JSON keys are existing. Not all weather stations provide the full spectrum of data (keys)
			if 'temp' in weather[x]['last']:
				temp[x] = weather[x]['last']['temp']
				print "Temperature   : ",temp[x],"C"
			else:
				temp[x] = None
				print "Temperature   : * (Key doesn't exist)"

			if 'hum' in weather[x]['last']:
				hum[x] = weather[x]['last']['hum']
				print "Humidity      : ",hum[x],"%rh"
			else:
				hum[x] = None
				print "Humidity      : * (Key doesn't exist)"
			
			if 'pres' in weather[x]['last']:
				pres[x] = weather[x]['last']['pres']['qfe']
				print "Pressure      : ",pres[x],"hPa"
			else:
				pres[x] = None
				print "Pressure      :  * (Key doesn't exist)"
				
			if 'w-avg' in weather[x]['last']:
				w_avg[x] = weather[x]['last']['w-avg']
				print "Wind average  : ",w_avg[x],"km/h"
			else:
				w_avg[x] = None
				print "Wind average  :  * (Key doesn't exist)"
				
			if 'w-max' in weather[x]['last']:
				w_max[x] = weather[x]['last']['w-max']
				print "Wind peak     : ",w_max[x],"km/h"
			else:
				w_avg[x] = None
				print "Wind peak     :  * (Key doesn't exist)"				
			
			if 'w-dir' in weather[x]['last']:
				w_dir[x] = weather[x]['last']['w-dir']
				print "Wind direction: ",w_dir[x],"Grad"
			else:
				w_dir[x] = None
				print "Wind direction:  * (Key doesn't exist)"				
			
			mycursor = mydb.cursor()
			
			# Update weather station information in the SQL only if waether station data has changed
			sql = "INSERT INTO weather_stations (_id, name, short, longitude, latitude, alt) VALUES (%s, %s, %s, %s, %s, %s) ON DUPLICATE KEY UPDATE name=%s, short=%s, longitude=%s, latitude=%s, alt=%s"
			
			val = (weather[x]['_id'],
				weather[x]['name'],
				weather[x]['short'],
				weather[x]['loc']['coordinates'][0],
				weather[x]['loc']['coordinates'][1],
				weather[x]['alt'],
				weather[x]['name'],			# variables after "... ON DUPLICATE KEY UPDATE ..."
				weather[x]['short'],
				weather[x]['loc']['coordinates'][0],
				weather[x]['loc']['coordinates'][1],
				weather[x]['alt']
				)			
			
			mycursor.execute(sql,val)
			
			mydb.commit()
			
			print("MySQL weather_stations: ",mycursor.rowcount, "record inserted.")
			
			# Insert new weather data into the SQL 
			sql = "INSERT INTO weather_values (idweather_values, _id_stations, time, temp, hum, pres, w_avg, w_max, w_dir) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)"
			
			val = ("0",
				weather[x]['_id'],
				weather[x]['last']['time'],
				temp[x],
				hum[x],
				pres[x],
				w_avg[x],
				w_max[x],
				w_dir[x]
				)
			
			mycursor.execute(sql,val)
			
			mydb.commit()
			
			print("MySQL weather_values: ",mycursor.rowcount, "record inserted.")

	
	time.sleep(repetiton_time)
