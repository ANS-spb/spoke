/*
    SPOKE
    
    file: gps.c
*/



#include <string.h>
#include "stm32f10x.h"
#include "main.h"
#include "gps.h"
#include "uart.h"
#include "service.h"
#include "settings.h"
#include "points.h"
#include "lrns.h"
#include "calendar.h"



void gps_raw_convert_to_numerical(void);
void configure_gps_receiver(void);
void update_my_coordinates(void);
void convert_time(void);



const float knots_to_kph = 1.852;                   //knots to kilometers per hour multiplyer



uint8_t nmea_checksum(uint16_t pos);
uint8_t parse_RMC(void);
uint8_t parse_GGA(void);
uint8_t parse_GSA(void);
uint8_t parse_GSV(void);



char nmea_data[UART_BUF_LEN];
struct gps_raw_struct gps_raw;
struct gps_num_struct gps_num;
struct settings_struct *p_settings;
struct devices_struct **pp_devices;


uint8_t this_device;
char tmp_char[15];
uint8_t tmp_uint8;
float tmp_float;



void gps_init(void)
{
	//Load all devices
	pp_devices = get_devices();

	p_settings = get_settings();
	this_device = p_settings->device_number;		//todo overall make this device nub get as a function or as a more beutiful way

	configure_gps_receiver();
}



void configure_gps_receiver(void)
{
	//todo: send UBX config here
}



//Parse all gps fields all together
uint8_t parse_gps(void)
{
	memset(&gps_raw, 0, sizeof(gps_raw));
	if (parse_RMC() && parse_GGA() && parse_GSA() && parse_GSV())
	{
		memset(&gps_num, 0, sizeof(gps_num));
		gps_raw_convert_to_numerical();
		update_my_coordinates();
		return 1;
	}
	else
	{
		return 0;
	}
}



//Converts gps_raw data (symbols) to gps_num data (numbers) with conversions if needed
void gps_raw_convert_to_numerical(void)
{
    //Time
    tmp_char[0] = gps_raw.time[0];
    tmp_char[1] = gps_raw.time[1];
    tmp_char[2] = 0;
    gps_num.hour = (uint8_t)atoi32(&tmp_char[0]);

    tmp_char[0] = gps_raw.time[2];
    tmp_char[1] = gps_raw.time[3];
    tmp_char[2] = 0;
    gps_num.minute = (uint8_t)atoi32(&tmp_char[0]);

    tmp_char[0] = gps_raw.time[4];
    tmp_char[1] = gps_raw.time[5];
    tmp_char[2] = 0;
    gps_num.second = (uint8_t)atoi32(&tmp_char[0]);

    //Date
    tmp_char[0] = gps_raw.date[0];
    tmp_char[1] = gps_raw.date[1];
    tmp_char[2] = 0;
    gps_num.day = (uint8_t)atoi32(&tmp_char[0]);

    tmp_char[0] = gps_raw.date[2];
    tmp_char[1] = gps_raw.date[3];
    tmp_char[2] = 0;
    gps_num.month = (uint8_t)atoi32(&tmp_char[0]);

    tmp_char[0] = gps_raw.date[4];
    tmp_char[1] = gps_raw.date[5];
    tmp_char[2] = 0;
    gps_num.year = (uint8_t)atoi32(&tmp_char[0]);

    convert_time();	//convert UTC time to local

    //Latitude
    tmp_char[0] = gps_raw.latitude[0];
    tmp_char[1] = gps_raw.latitude[1];
    tmp_char[2] = 0;
    tmp_uint8 = (uint8_t)atoi32(&tmp_char[0]);          //int part of lat
    tmp_float = atof32(&(gps_raw.latitude[2]));        //frac part of lat

    tmp_float /= 60.0;                                  //convert ddmm.mmmm to dd.dddddd
    tmp_float += tmp_uint8;

    if (gps_raw.ns[0] == 'S')
    {
        gps_num.latitude = tmp_float * -1.0;    //negative (southern) latitudes
    }
    else
    {
        gps_num.latitude = tmp_float;
    }

    //Longitude
    tmp_char[0] = gps_raw.longitude[0];
    tmp_char[1] = gps_raw.longitude[1];
    tmp_char[2] = gps_raw.longitude[2];
    tmp_char[3] = 0;
    tmp_uint8 = (uint8_t)atoi32(&tmp_char[0]);          //int part of lon
    tmp_float = atof32(&(gps_raw.longitude[3]));       //frac part of lon

    tmp_float /= 60.0;                                  //convert dddmm.mmmm to ddd.dddddd
    tmp_float += tmp_uint8;

    if (gps_raw.ew[0] == 'W')
    {
        gps_num.longitude = tmp_float * -1.0;   //negative (western) longitudes
    }
    else
    {
        gps_num.longitude = tmp_float;
    }

    //Speed
    gps_num.speed = atof32(&(gps_raw.speed[0])) * knots_to_kph;

    //Course
    gps_num.course = atof32(&(gps_raw.course[0]));

    //Altitude
    gps_num.altitude = atof32(&(gps_raw.altitude[0]));

    //Satellites
    gps_num.sat_view = (uint8_t)atoi32(&(gps_raw.sat_view[0]));
    gps_num.sat_used = (uint8_t)atoi32(&(gps_raw.sat_used[0]));

    //Status, Mode, PDOP
    if (gps_raw.status[0] == 'A')
    {
        gps_num.status = GPS_DATA_VALID;
    }
    else
    {
        gps_num.status = GPS_DATA_INVALID;
    }

    gps_num.mode = (uint8_t)atoi32(&(gps_raw.mode[0]));

    gps_num.pdop = atof32(&(gps_raw.pdop[0]));
}



void convert_time(void)
{
	int8_t minute = gps_num.minute;
	int8_t hour = gps_num.hour;
	int8_t day = gps_num.day;
	int8_t month = gps_num.month;
	int8_t year = gps_num.year;

	if (day == 0)	//invalid data
	{
		return;
	}

	if (p_settings->time_zone_dir != 0)
	{
		if (p_settings->time_zone_dir > 0)
		{
			minute += p_settings->time_zone_minute;

			if (minute > 59)
			{
				minute -= 60;
				hour += 1;
			}

			hour += p_settings->time_zone_hour;

			if (hour > 23)
			{
				hour -= 24;
				day += 1;

				if (day > calendar[year - CALENDAR_START_YEAR][month - 1])
				{
					day = 1;
					month += 1;

					if (month > 12)
					{
						month = 1;
						year += 1;
					}
				}
			}
		}
		else
		{
			minute -= p_settings->time_zone_minute;

			if (minute < 0)
			{
				minute += 60;
				hour -= 1;
			}

			hour -= p_settings->time_zone_hour;

			if (hour < 0)
			{
				hour += 24;
				day -= 1;

				if (day < 1)
				{
					month -= 1;

					if (month < 1)
					{
						month = 12;
						year -= 1;
					}

					day = calendar[year - CALENDAR_START_YEAR][month - 1];
				}
			}
		}
	}

	gps_num.minute_tz = minute;
	gps_num.hour_tz = hour;

	gps_num.day_tz = day;
	gps_num.month_tz = month;
	gps_num.year_tz = year;

}






//Update devices structure with coordinates of this device in order to transmit them further
void update_my_coordinates(void)
{
    //Latitude & Longitude
	pp_devices[this_device]->latitude.as_float = gps_num.latitude;
	pp_devices[this_device]->longitude.as_float = gps_num.longitude;

	//Altitude
	pp_devices[this_device]->altitude.as_integer = (int16_t)gps_num.altitude;
}



//Parse RMC sentence
uint8_t parse_RMC(void)
{
    uint8_t comma = 0;
    uint8_t sym = 0;
    uint16_t pos = 0;
    
    while (!((nmea_data[pos] == '$') &&
            (nmea_data[pos + 3] == 'R') &&
            (nmea_data[pos + 4] == 'M') &&
            (nmea_data[pos + 5] == 'C')) && pos < UART_BUF_LEN)      //search for start pos
    {
    pos++;
    }
    
    if(nmea_checksum(pos) == 0)
    {
        return 0;           //checksum error
    }
    
    for (uint16_t i = pos + 6; i < UART_BUF_LEN ; i++)          //i starts from the symbol right after "$GPRMC" string
    {
        if (nmea_data[i] == '*') return 1;       //end of the sentence
        
        if (nmea_data[i] == ',')
        {
            comma++;        //increase comma counter
            sym = 0;
        }
        else
        {
            switch (comma)  //surfing through RMC data fields
            {
                case 1:
                    gps_raw.time[sym++] = nmea_data[i];
                    break;
                case 2:
                    gps_raw.status[sym++] = nmea_data[i];
                    break;
                case 3:
                    gps_raw.latitude[sym++] = nmea_data[i];
                    break;
                case 4:
                    gps_raw.ns[sym++] = nmea_data[i];
                    break;
                case 5:
                    gps_raw.longitude[sym++] = nmea_data[i];
                    break;
                case 6:
                    gps_raw.ew[sym++] = nmea_data[i];
                    break;
                case 7:
                    gps_raw.speed[sym++] = nmea_data[i];
                    break;
                case 8:
                    gps_raw.course[sym++] = nmea_data[i];
                    break;
                case 9:
                    gps_raw.date[sym++] = nmea_data[i];
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}



//Parse GGA sentence
uint8_t parse_GGA(void)
{
    uint8_t comma = 0;
    uint8_t sym = 0;
    uint16_t pos = 0;
    
    while (!((nmea_data[pos] == '$') &&
            (nmea_data[pos + 3] == 'G') &&
            (nmea_data[pos + 4] == 'G') &&
            (nmea_data[pos + 5] == 'A')) && pos < UART_BUF_LEN)      //search for start pos
    {
    pos++;
    }
    
    if(nmea_checksum(pos) == 0)
    {
        return 0;           //checksum error
    }
    
    for (uint16_t i = pos + 6; i < UART_BUF_LEN ; i++)          //i starts from the symbol right after "$GPRMC" string
    {
        if (nmea_data[i] == '*') return 1;       //end of the sentence
        
        if (nmea_data[i] == ',')
        {
            comma++;        //increase comma counter
            sym = 0;
        }
        else
        {
            switch (comma)  //surfing through GGA data fields
            {
                case 7:
                    gps_raw.sat_used[sym++] = nmea_data[i];
                    break;
                case 9:
                    gps_raw.altitude[sym++] = nmea_data[i];
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}



//Parse GSA sentence
uint8_t parse_GSA(void)
{
    uint8_t comma = 0;
    uint8_t sym = 0;
    uint16_t pos = 0;
    
    while (!((nmea_data[pos] == '$') &&
            (nmea_data[pos + 3] == 'G') &&
            (nmea_data[pos + 4] == 'S') &&
            (nmea_data[pos + 5] == 'A')) && pos < UART_BUF_LEN)      //search for start pos
    {
    pos++;
    }
    
    if(nmea_checksum(pos) == 0)
    {
        return 0;           //checksum error
    }
    
    for (uint16_t i = pos + 6; i < UART_BUF_LEN ; i++)          //i starts from the symbol right after "$GPRMC" string
    {
        if (nmea_data[i] == '*') return 1;       //end of the sentence
        
        if (nmea_data[i] == ',')
        {
            comma++;        //increase comma counter
            sym = 0;
        }
        else
        {
            switch (comma)  //surfing through GSA data fields
            {
                case 2:
                    gps_raw.mode[sym++] = nmea_data[i];
                    break;
                case 15:
                    gps_raw.pdop[sym++] = nmea_data[i];
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}



//Parse GSV sentence
uint8_t parse_GSV(void)
{
    uint8_t comma = 0;
    uint8_t sym = 0;
    uint16_t pos = 0;
    
    while (!((nmea_data[pos] == '$') &&
            (nmea_data[pos + 3] == 'G') &&
            (nmea_data[pos + 4] == 'S') &&
            (nmea_data[pos + 5] == 'V')) && pos < UART_BUF_LEN)      //search for start pos
    {
    pos++;
    }
    
    if(nmea_checksum(pos) == 0)
    {
        return 0;           //checksum error
    }
    
    for (uint16_t i = pos + 6; i < UART_BUF_LEN ; i++)          //i starts from the symbol right after "$GPRMC" string
    {
        if (nmea_data[i] == '*') return 1;       //end of the sentence
        
        if (nmea_data[i] == ',')
        {
            comma++;        //increase comma counter
            sym = 0;
        }
        else
        {
            switch (comma)  //surfing through GSV data fields
            {
                case 3:
                    gps_raw.sat_view[sym++] = nmea_data[i];
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}



//NMEA checksum
uint8_t nmea_checksum(uint16_t pos)
{
    uint8_t CheckSum = 0;
    uint8_t MessageCheckSum = 0;
    
    pos++;                                      //pick up symbol right after $
    while (nmea_data[pos] != '*')
    {
        CheckSum ^= nmea_data[pos];
        if (++pos >= UART_BUF_LEN) return 0;    //check sum not found
    }
    
    if (nmea_data[++pos] > 0x40)
    {
        MessageCheckSum = (nmea_data[pos] - 0x37) << 4;   //ascii hex to number
    }
    else
    {
        MessageCheckSum = (nmea_data[pos] - 0x30) << 4;
    }
    
    if (nmea_data[++pos] > 0x40)
    {
        MessageCheckSum += (nmea_data[pos] - 0x37);       //ascii hex to number
    }
    else
    {
        MessageCheckSum += (nmea_data[pos] - 0x30);
    }
    
    if (MessageCheckSum == CheckSum)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



char *get_nmea_buf(void)
{
	return &nmea_data[0];
}



struct gps_raw_struct *get_gps_raw(void)
{
	return &gps_raw;
}



struct gps_num_struct *get_gps_num(void)
{
	return &gps_num;
}
