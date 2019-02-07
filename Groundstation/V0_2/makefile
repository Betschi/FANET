# FANET setting

#CC=g++
CFLAGS=-c -Wall `mysql_config --cflags` `mysql_config --libs`

CC=gcc
#CFLAGS=-c -Wall -o `mysql_config --libs` `mysql_config --cflags`
LIBS=-lwiringPi -lmariadb -lm

all: fanet_station

fanet_station: main.o fanet_mysql.o
	$(CC) main.o  $(LIBS) -o fanet_station
	
main.o: main.c
	$(CC) $(CFLAGS) main.c
	
fanet_mysql.o: fanet_mysql.c
	$(CC) $(CFLAGS) fanet_mysql.c 

clean:
	rm *.o fanet_station rm *.o fanet_mysql
	
	

