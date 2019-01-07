
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>

#include "fanet_struct.c"
#include "fanet_radio.c"
#include "fanet_mysql.c"
#include "fanet_terminal.c"
#include "fanet_t3_messenger.c"
#include "fanet_t4_service.c"

#include <sys/ioctl.h> 

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define KNRM  "\x1B[0m"		// Color: Normal
#define KRED  "\x1B[31m"	// Color: Red
#define KGRN  "\x1B[32m"	// Color: Green
#define KYEL  "\x1B[33m"	// Color: Yellow
#define KBLU  "\x1B[34m"	// Color: Blue
#define KMAG  "\x1B[35m"	// Color: Mangenta
#define KCYN  "\x1B[36m"	// Color: Cyan
#define KWHT  "\x1B[37m"	// Color: White

#define	debug	0x01

#define STATION_MANUF	0xFC
#define STATION_ID		0x9001
unsigned char    STATION_NAME[64] = "Groundstation Interlaken";


#define pi 3.14159265358979323846
#define STATION_LAT  46.684681
#define STATION_LON  7.867658
#define STATION_ALT  580				// Meter over sea
#define STATION_SPEED	0				// km/h
#define STATION_CLIMB	0				// m/s
#define STATION_HEADING 0				// deg
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

// #############################################
// #############################################

#define REG_FIFO                    0x00
#define REG_OPMODE                  0x01
#define REG_FIFO_ADDR_PTR           0x0D
#define REG_FIFO_TX_BASE_AD         0x0E
#define REG_FIFO_RX_BASE_AD         0x0F
#define REG_RX_NB_BYTES             0x13
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_MODEM_CONFIG            0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_SYMB_TIMEOUT_LSB  		0x1F
#define REG_PKT_SNR_VALUE			0x19
#define REG_PAYLOAD_LENGTH          0x22
#define REG_IRQ_FLAGS_MASK          0x11
#define REG_MAX_PAYLOAD_LENGTH 		0x23
#define REG_HOP_PERIOD              0x24
#define REG_SYNC_WORD				0x39
#define REG_VERSION	  				0x42

#define PAYLOAD_LENGTH              0x40

// LOW NOISE AMPLIFIER
#define REG_LNA                     0x0C
#define LNA_MAX_GAIN                0x23
#define LNA_OFF_GAIN                0x00
#define LNA_LOW_GAIN		    	0x20

#define RegDioMapping1                             0x40 // common
#define RegDioMapping2                             0x41 // common

#define RegPaConfig                                0x09 // common
#define RegPaRamp                                  0x0A // common
#define RegPaDac                                   0x5A // common

#define SX72_MC2_FSK                0x00
#define SX72_MC2_SF7                0x70
#define SX72_MC2_SF8                0x80
#define SX72_MC2_SF9                0x90
#define SX72_MC2_SF10               0xA0
#define SX72_MC2_SF11               0xB0
#define SX72_MC2_SF12               0xC0

#define SX72_MC1_LOW_DATA_RATE_OPTIMIZE  0x01 // mandated for SF11 and SF12

// sx1276 RegModemConfig1
#define SX1276_MC1_BW_125                0x70
#define SX1276_MC1_BW_250                0x80
#define SX1276_MC1_BW_500                0x90
#define SX1276_MC1_CR_4_5            0x02
#define SX1276_MC1_CR_4_6            0x04
#define SX1276_MC1_CR_4_7            0x06
#define SX1276_MC1_CR_4_8            0x08

#define SX1276_MC1_IMPLICIT_HEADER_MODE_ON    0x01

// sx1276 RegModemConfig2
#define SX1276_MC2_RX_PAYLOAD_CRCON        0x04

// sx1276 RegModemConfig3
#define SX1276_MC3_LOW_DATA_RATE_OPTIMIZE  0x08
#define SX1276_MC3_AGCAUTO                 0x04

// preamble for lora networks (nibbles swapped)
#define LORA_MAC_PREAMBLE                  0x34

#define RXLORA_RXMODE_RSSI_REG_MODEM_CONFIG1 0x0A
#ifdef LMIC_SX1276
#define RXLORA_RXMODE_RSSI_REG_MODEM_CONFIG2 0x70
#elif LMIC_SX1272
#define RXLORA_RXMODE_RSSI_REG_MODEM_CONFIG2 0x74
#endif

// FRF
#define        REG_FRF_MSB              0x06
#define        REG_FRF_MID              0x07
#define        REG_FRF_LSB              0x08

#define        FRF_MSB                  0xD9 // 868.1 Mhz
#define        FRF_MID                  0x06
#define        FRF_LSB                  0x66

// ----------------------------------------
// Constants for radio registers
#define OPMODE_LORA      0x80
#define OPMODE_MASK      0x07
#define OPMODE_SLEEP     0x00
#define OPMODE_STANDBY   0x01
#define OPMODE_FSTX      0x02
#define OPMODE_TX        0x03
#define OPMODE_FSRX      0x04
#define OPMODE_RX        0x05
#define OPMODE_RX_SINGLE 0x06
#define OPMODE_CAD       0x07

// ----------------------------------------
// Bits masking the corresponding IRQs from the radio
#define IRQ_LORA_RXTOUT_MASK 0x80
#define IRQ_LORA_RXDONE_MASK 0x40
#define IRQ_LORA_CRCERR_MASK 0x20
#define IRQ_LORA_HEADER_MASK 0x10
#define IRQ_LORA_TXDONE_MASK 0x08
#define IRQ_LORA_CDDONE_MASK 0x04
#define IRQ_LORA_FHSSCH_MASK 0x02
#define IRQ_LORA_CDDETD_MASK 0x01

// DIO function mappings                D0D1D2D3
#define MAP_DIO0_LORA_RXDONE   0x00  // 00------
#define MAP_DIO0_LORA_TXDONE   0x40  // 01------
#define MAP_DIO1_LORA_RXTOUT   0x00  // --00----
#define MAP_DIO1_LORA_NOP      0x30  // --11----
#define MAP_DIO2_LORA_NOP      0xC0  // ----11--

// #############################################
// #############################################
//


//time_t t = time(NULL);
//struct tm tm = *localtime(&t);

time_t t;
struct tm *tm;


struct timeval tv;
struct tm* ptm;
char time_string[40];
long milliseconds;

//static const int CHANNEL = 0;

char message[256];
byte message_tx[256]; 

sRawMessage _tx_message;

bool sx1272 = true;

byte receivedbytes;
byte sf;

enum { SF7=7, SF8, SF9, SF10, SF11, SF12 };

// Set spreading factor (SF7 - SF12)
byte sf = SF7;


void die(const char *s)
{
    perror(s);
    exit(1);
}



int type_1_tracking (void)
{

	signed int	latitude_int;
	signed int	longitude_int;
	signed int	altitude;
	
	message_tx[0] = 0x01;
	message_tx[1] = STATION_MANUF;
	message_tx[2] = STATION_ID&0x00FF;
	message_tx[3] = (STATION_ID&0xFF00)>>8;
	//message_tx[4] = 0x00;
	
	latitude_int  = STATION_LAT*93206;
	longitude_int = STATION_LON*46603;
	
	printf("%d  %d\n",latitude_int, longitude_int);
	
	message_tx[4] = latitude_int&0x000000FF;
	message_tx[5] = (latitude_int&0x0000FF00)>>8;
	message_tx[6] = (latitude_int&0x00FF0000)>>16;

	message_tx[7] = longitude_int&0x000000FF;
	message_tx[8] = (longitude_int&0x0000FF00)>>8;
	message_tx[9] = (longitude_int&0x00FF0000)>>16;

	if (STATION_ALT < 2048)
		altitude = STATION_ALT;
	else
	{
		altitude = STATION_ALT/4;
		printf("Alt: %d\n", altitude);
	}	
	message_tx[10] = STATION_ALT&0x00FF;
	message_tx[11] = (STATION_ALT&0x0700)>>8;
		
	if (STATION_ALT > 2048)
		message_tx[11] |= 0x08;			// Set Altitude Scaling Bit: 1->4x
		
		
	message_tx[12] = 0x00;				// Speed
	
	message_tx[13] = 0x00;				// Climb
	
	message_tx[14] = 0x00;				// Heading
	
	return 15;
}


/*
int type_3_message_coder (int _data_length, char _message[])
{
	int _i = 0;
	
	message_tx[_data_length] = 0;		// Subheader (TBD)
	_data_length++;
	
	while (_message[_i] && _i < 240)
	{
		message_tx[_data_length] = _message[_i];
		_data_length++;
		_i++;
	}
	
	return _data_length;
}

*/

//void FANET_decoder(int message_length, int rssi, float prssi, float _psnr, char _coding_rate, int32_t _freq_err)
//void FANET_decoder(int message_length, sRadioData *_radiodata)
void FANET_decoder(sRawMessage *_rx_radio, sRadioData *_radiodata)
{
	//char _rx_messages[255];
	//byte _rx_length;
	sRouting _routing_data;
	
	char ext_header;
	char forward;
	char message_type;
	char online_track;	 
	
	int _i;
	int _z = 0;
	int id;
	int alt;
	int distance_int;
	int offset = 0;
	int last_welcome_message;

	char 	manufacturer;

	char    idname[10];
	char	name[255];
	char	a_type;
	
	signed int  latitude_int;
	signed int   longitude_int;
	signed char climb_char;
	
	float latitude;
	float longitude;
	float speed;
	float climb;
	float heading;
	float turn_rate = 0;
	
	/*float temperature;
	float humidity;
	float barometric;
	float wind_heading;
	float wind_speed;
	float wind_gusts;
	*/
	
	float _distance;

	t = time(NULL);
    tm = localtime(&t);

	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);
	milliseconds = tv.tv_usec / 1000;
    strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
    //printf ("%s.%03ld\n", time_string, milliseconds);	
	
	//_rx_length = _rx_radio->m_length;
	while (_z < _rx_radio->m_length)
	{
		message[_z] = _rx_radio->message[_z];
		_z++;
	}
	
	if (message[0]&0x80)
		ext_header = 1;
	else
		ext_header = 0;
		
	if (message[0]&0x40)
		forward = 1;
	else
		forward = 0;
	
	message_type = (message[0]&0x3F);
	manufacturer = message[1];
	id = (message[3])<<8;
	id += message[2];
	
	// Check Message Type
	if (message_type == 1)		// 1 = Tracking
	{
		latitude_int =  message[6];
		latitude_int <<= 8;
		latitude_int += message[5];
		latitude_int <<=8;
		latitude_int += message[4];
		latitude = latitude_int/(float)93206;
		
		longitude_int =  message[9];
		longitude_int <<=8;
		longitude_int += message[8];
		longitude_int <<=8;
		longitude_int += message[7];
		longitude = longitude_int/(float)46603;
		
		_distance = distance (STATION_LAT, STATION_LON, latitude, longitude, 'K');
		
		if (message[11]&0x80)
			online_track = 1;
		else
			online_track = 0;
		
		a_type = (message[11]&0x70);
		a_type >>=4;
		
		
		alt = (message[11]&0x07);
		alt <<= 8;
		alt |= message[10];
		if ((message[11] & 0x08))
			alt <<= 2;
			
		speed = (message[12]&0x3F)*0.5;
		if ((message[12]&0x80))
			speed = speed * 5;
			
		climb_char = (message[13]&0x7F);
		if ((message[13] & 0x40))
			climb_char |= 0x80;
		climb = (float)climb_char;

		if ((message[13] & 0x80))
			climb *= 0.5;
		else
			climb *= 0.1;

			
		heading = 360.0/256*(message[14]);
			
		
	}
	
	if (message_type == 2)		// 2 = Name
	{
		_i = 0;
		while (_i <= (_rx_radio->m_length-5))
		{
			name[_i] = message[_i+4];
			if (name[_i] == 0x27)		// Check if Apostrophe " ' " ASCII = 0x27 is present
				name[_i] = 0x20;		// Replace " ' " with space " "
			_i++;
		}
		name[_i] = 0;
	}
	

	
	if (!_radiodata->crc_err)
		printf(ANSI_COLOR_GREEN);
	else
		printf(ANSI_COLOR_YELLOW);

	printf ("|%s.%03ld| %+4ddBm| %+4.0ddBm| %+5.1fdB| %s| %+6dHz|",
		time_string,
		milliseconds,
		_radiodata->rssi, 
		_radiodata->prssi, 
		_radiodata->psnr, 
		(*_radiodata).coding_rate, 
		_radiodata->freq_err);


	if (!_radiodata->crc_err)
	{
		printf ("    %3d | ",_rx_radio->m_length);
		
		_i=0;
		while (_i < (_rx_radio->m_length))
		{	
			printf ("%02x ",message[_i]);
			_i++;
		}
		printf ("\n");
	}
		
	if (_radiodata->crc_err)
			printf (" CRC error \n");
		
	
	if (message_type == 1 && !_radiodata->crc_err)
	{
		printf ("|                                                                 | %d %d %2d | %x:%04x| %+9.5f°| %+10.5f°| %i |    %i  | %4dm| %5.1fkm/h| %+5.1fm/s|   %5.1f°| %6.2fkm|  \n", 
			ext_header, 
			forward, 
			message_type, 
			manufacturer, 
			id, 
			latitude, 
			longitude, 
			online_track, 
			a_type, 
			alt, 
			speed, 
			climb, 
			heading, 
			_distance);

		sprintf(idname, "%x:%04x", manufacturer,id);
		_distance *= 1000;
		distance_int = (int)_distance;
		write_object_tracking (t, idname, _radiodata, latitude, longitude, online_track, a_type, alt, speed, climb, heading, turn_rate, distance_int);

	}
	if (message_type == 2 && !_radiodata->crc_err)
	{
		printf ("|                                                                 | %d %d %2d | %x:%04x| %s\n",
			ext_header,
			forward,
			message_type,
			manufacturer,
			id,
			name);

		sprintf(idname, "%x:%04x", manufacturer,id);
		write_object_name (idname , name, t, &last_welcome_message);
		/*
		if ((t - last_welcome_message) > 86400)
			welcome_object (idname);
		*/
		
	}

	if (message_type == 4 && !_radiodata->crc_err)
	{
	
		
		
		decode_abs_coordination (&message[5], &latitude, &longitude);

		if (message[4]&0x40)
		{	
			//decode_temperature (&message[11], &temperature);
			offset += 1;
		}
		

		if (message[4]&0x20)
		{		
			//decode_wind (&message[11+offset], &wind_heading, &wind_speed, &wind_gusts);
			offset += 3;
		}
		else
		{
			//wind_heading = -1;
			//wind_speed   = -1;
			//wind_gusts   = -1;
		}

	
		if (message[4]&0x10)
		{		
		//decode_humidity (&message[11+offset], &humidity);
			offset += 1;
		}

		if (message[4]&0x08)
		{		
		//decode_barometric (&message[11+offset], &barometric);
			offset += 2;
		}		

		/*printf ("|                                                                 | %d %d %2d | %x:%04x| %+9.5f°| %+10.5f°| %+5.1f°C| %5.1f°| %5.1fkm/h| %5.1fkm/h| %5.1f%%rh| %7.2fhPa|\n", 
					ext_header, 
					forward, 
					message_type, 
					manufacturer, 
					id, 
					latitude, 
					longitude,
					temperature,
					wind_heading,
					wind_speed,
					wind_gusts,
					humidity,
					barometric); */
	}
		

	if ((message_type < 1 || message_type > 4) && !_radiodata->crc_err)
		printf ("|                                                                 | %d %d %2d | %x:%04x|\n",
			ext_header, forward, message_type, manufacturer, id);
			
	printf(ANSI_COLOR_RESET);
	
	_routing_data.address_manufactur_id = manufacturer;
	_routing_data.address_unique_id = id;
	_routing_data.last_seen = t;
	_routing_data.snr = _radiodata->psnr;
	if (_routing_data.snr > -9)					// Rejecting weak signals from the routing table. SNR has to better than -9 dB.
		update_routing_table (&_routing_data);
			
}

void system_data ()
{
	sSystem		_system_data;
	sRadioData	_radiodata;
		
	static int 		_yy;
	static int		_rssi_sum_1min;
	static float	_rssi_sum_15min;
	//static float	_rssi_1min_avg;
	//static float	_rssi_15min_avg;
	
	static int		_rssi_max_1min = -157;
	static int		_rssi_max_15min = -157;
	static int  	_rssi_avg_1min_counter;
	static int  	_rssi_avg_15min_counter;
	


	if (_yy == 500)			// 500 ms update rate
	{
		t = time(NULL);
		tm = localtime(&t);

		gettimeofday (&tv, NULL);
		ptm = localtime (&tv.tv_sec);
		milliseconds = tv.tv_usec / 1000;
		strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);	

		read_radio_data(&_radiodata);
		
		if ((_radiodata.rssi > -130) & (_radiodata.rssi < 0))		// Ignore RSSI values <-130dBm and > 0dBm
		{
			_rssi_sum_1min += _radiodata.rssi;
			_rssi_avg_1min_counter++;
			if (_radiodata.rssi > _rssi_max_1min)
				_rssi_max_1min = _radiodata.rssi;
		}



        printf("|%s.%03ld| RSSI:%+4ddBm  RX Headers: %d \b\r", time_string, milliseconds, _radiodata.rssi, _radiodata.rx_headers);	
		fflush(stdout);
	
		if (!(t % 60) && _rssi_avg_1min_counter > 2)		// Modulo Divison: Volle Minute = kein Rest bei Disision durch 60 (1 min)
		{
			_system_data.rssi_avg_1min = (float)_rssi_sum_1min / (float)_rssi_avg_1min_counter;
			_system_data.rssi_max_1min = _rssi_max_1min;
			
			printf ("\nSystem  1 min Data: %ld RSSI avg: %+7.2fdBm  RSSI max: %ddBm \n", t, _system_data.rssi_avg_1min, _system_data.rssi_max_1min);

			write_system_data_1min (t, _system_data.rssi_avg_1min, _system_data.rssi_max_1min);
			//delete_old_data();	
			
			_rssi_sum_15min += _system_data.rssi_avg_1min;
			_rssi_avg_15min_counter++;
			if (_rssi_max_1min > _rssi_max_15min)
				_rssi_max_15min = _rssi_max_1min;

			_rssi_sum_1min = 0;
			_rssi_avg_1min_counter = 0;
			_rssi_max_1min = -157;
			
			if (!(t % 900) && _rssi_avg_15min_counter > 2)		// Modulo Divison: Volle Minute = kein Rest bei Disision durch 900 (15 min)
			{
				_system_data.rssi_avg_15min = _rssi_sum_15min /(float) _rssi_avg_15min_counter;
				_system_data.rssi_max_15min = _rssi_max_15min;
			
				printf ("\nSystem 15 min Data: %ld RSSI avg: %+7.2fdBm  RSSI max: %ddBm \n", t, _system_data.rssi_avg_15min, _system_data.rssi_max_15min);

				write_system_data_15min (t, _system_data.rssi_avg_15min, _system_data.rssi_max_15min);				
				
				_rssi_sum_15min = 0;
				_rssi_avg_15min_counter = 0;
				_rssi_max_15min = -157;
				
			}

			
		}
		_yy = 0;
	}
	_yy++;

}


void receivepacket()
{
	sRawMessage   _rx_radio;
	sRadioData _radiodata;

    //if(read_rx_data(&_rx_radio, message,&receivedbytes,&_radiodata))
	if(read_rx_data(&_rx_radio, &_radiodata))
    {
		//FANET_decoder((int)receivedbytes, &_radiodata);
		FANET_decoder(&_rx_radio, &_radiodata);
    }
}






int main (int argc, char *argv[])
{

    time_t now;
    struct tm *tm;

	char _second_new;
	char _second_old;

    sql_login();

    
    /*if (argc < 2) {
        printf ("Usage: argv[0] sender|rec [message]\n");
        exit(1);
    }*/

    init_fanet_radio();

    delay(2000);

    //start_screen();
    terminal_start_screen(sf, 250, freq);

    //opmodeLora();
    opmode(OPMODE_STANDBY);
    writeReg(RegPaRamp, (readReg(RegPaRamp) & 0xF0) | 0x08); // set PA ramp-up time 50 uSec
    configPower(15);
    opmode(OPMODE_RX);    

    while(1)
    {
		now = time(0);
		tm = localtime (&now);
		_second_new = tm->tm_sec;
		
		system_data();
		receivepacket();
		
		if (_second_new!=_second_old) 
		{
		  _second_old = _second_new;
		  
		  fanet_t4_service_scheduler();
		  //fanet_t3_messenger_scheduler();
		
		}
        delay(1);
	}
 
    return (0);
}
