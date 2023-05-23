/*
    SPOKE

    file: settings.c
*/

#include "stm32f10x.h"
#include "config.h"
#include "main.h"
#include "settings.h"
#include "memory.h"


#define UPDATE_INTERVAL_1S_VALUE		(1)
#define UPDATE_INTERVAL_5S_VALUE		(5)
#define UPDATE_INTERVAL_10S_VALUE		(10)
#define UPDATE_INTERVAL_30S_VALUE		(30)
#define UPDATE_INTERVAL_60S_VALUE		(60)

#define UPDATE_INTERVAL_VALUES_ARRAY 	{ 	UPDATE_INTERVAL_1S_VALUE, 	\
											UPDATE_INTERVAL_5S_VALUE, 	\
											UPDATE_INTERVAL_10S_VALUE, 	\
											UPDATE_INTERVAL_30S_VALUE, 	\
											UPDATE_INTERVAL_60S_VALUE	}



#define TX_POWER_0MILLIW_VALUE   	(0)
#define TX_POWER_1MILLIW_VALUE   	(1)
#define TX_POWER_10MILLIW_VALUE   	(10)
#define TX_POWER_100MILLIW_VALUE  	(100)

#define TX_POWER_VALUES_ARRAY 		{ 	TX_POWER_0MILLIW_VALUE, 	\
										TX_POWER_1MILLIW_VALUE, 	\
										TX_POWER_10MILLIW_VALUE, 	\
										TX_POWER_100MILLIW_VALUE	}



//positions:
#define SETTINGS_INIT_FLAG_POS          	(0)
#define SETTINGS_DEVICE_NUMBER_POS      	(1)
#define SETTINGS_DEVICES_ON_AIR_POS			(2)
#define SETTINGS_DEVICE_ID_POS          	(3)
#define SETTINGS_UPDATE_INTERVAL_POS        (4)
#define SETTINGS_FREQ_CHANNEL_POS       	(5)
#define SETTINGS_TX_POWER_POS           	(6)
#define SETTINGS_TIMEOUT_THRESHOLD_POS   	(7)
#define SETTINGS_FENCE_THRESHOLD_POS   		(8)
#define SETTINGS_TIME_ZONE_DIR_POS			(9)
#define SETTINGS_TIME_ZONE_HOUR_POS			(10)
#define SETTINGS_TIME_ZONE_MINUTE_POS		(11)

//default values:
#define SETTINGS_INIT_FLAG_DEFAULT      	(0xAA)
#define SETTINGS_DEVICE_NUMBER_DEFAULT  	(DEVICE_NUMBER_FIRST)
#define SETTINGS_DEVICES_ON_AIR_DEFAULT		(DEVICE_NUMBER_LAST)
#define SETTINGS_DEVICE_ID_DEFAULT 			(DEVICE_ID_FIRST_SYMBOL)
#define SETTINGS_UPDATE_INTERVAL_DEFAULT    (UPDATE_INTERVAL_1S_SETTING)
#define SETTINGS_FREQ_CHANNEL_DEFAULT   	(FREQ_CHANNEL_FIRST)             //base freq is 433.050 and freq step is 25kHz, so CH0 - 433.050 (not valid, not used); CH1 - 433.075 (first LPD channel)
#define SETTINGS_TX_POWER_DEFAULT       	(TX_POWER_10MILLIW_SETTING)
#define SETTINGS_TIMEOUT_THRESHOLD_DEFAULT  (60)
#define SETTINGS_FENCE_THRESHOLD_DEFAULT  	(100)
#define SETTINGS_TIME_ZONE_DIR_DEFAULT		(1)
#define SETTINGS_TIME_ZONE_HOUR_DEFAULT		(0)
#define SETTINGS_TIME_ZONE_MINUTE_DEFAULT	(0)

//settings size
#define SETTINGS_SIZE						(12) //half-words



uint16_t settings_array[SETTINGS_SIZE];
struct settings_struct settings;

uint8_t update_interval_values[] = UPDATE_INTERVAL_VALUES_ARRAY;
uint8_t tx_power_values[] = TX_POWER_VALUES_ARRAY;



uint8_t *get_update_interval_values(void)
{
	return &update_interval_values[0];
}



uint8_t *get_tx_power_values(void)
{
	return &tx_power_values[0];
}



struct settings_struct *get_settings(void)
{
	return &settings;
}



void settings_load(void)
{
	read_flash_page(FLASH_SETTINGS_PAGE, &settings_array[0], 1);

    if (settings_array[SETTINGS_INIT_FLAG_POS] != SETTINGS_INIT_FLAG_DEFAULT)     //if first power-up or FLASH had been erased
    {
        settings_save_default();
    }
    
    //read from flash
    read_flash_page(FLASH_SETTINGS_PAGE, &settings_array[0], SETTINGS_SIZE);
    
    //load settings to struct
    settings.device_number = 					settings_array[SETTINGS_DEVICE_NUMBER_POS];
    settings.devices_on_air = 					settings_array[SETTINGS_DEVICES_ON_AIR_POS];
    settings.device_id = 						settings_array[SETTINGS_DEVICE_ID_POS];
    settings.freq_channel = 					settings_array[SETTINGS_FREQ_CHANNEL_POS];
    settings.tx_power_opt = 					settings_array[SETTINGS_TX_POWER_POS];
    settings.update_interval_opt = 				settings_array[SETTINGS_UPDATE_INTERVAL_POS];
    settings.timeout_threshold = 				settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS];
    settings.fence_threshold = 					settings_array[SETTINGS_FENCE_THRESHOLD_POS];
    settings.time_zone_dir = 					settings_array[SETTINGS_TIME_ZONE_DIR_POS];
    settings.time_zone_hour = 					settings_array[SETTINGS_TIME_ZONE_HOUR_POS];
    settings.time_zone_minute = 				settings_array[SETTINGS_TIME_ZONE_MINUTE_POS];
}



void settings_save_default(void)
{
	erase_flash_page(FLASH_SETTINGS_PAGE);

    //assign default values
    settings_array[SETTINGS_INIT_FLAG_POS] = 			SETTINGS_INIT_FLAG_DEFAULT;
    settings_array[SETTINGS_DEVICE_NUMBER_POS] = 		SETTINGS_DEVICE_NUMBER_DEFAULT;
    settings_array[SETTINGS_DEVICES_ON_AIR_POS] = 		SETTINGS_DEVICES_ON_AIR_DEFAULT;
    settings_array[SETTINGS_DEVICE_ID_POS] = 			SETTINGS_DEVICE_ID_DEFAULT;
    settings_array[SETTINGS_FREQ_CHANNEL_POS] = 		SETTINGS_FREQ_CHANNEL_DEFAULT;
    settings_array[SETTINGS_TX_POWER_POS] = 			SETTINGS_TX_POWER_DEFAULT;
    settings_array[SETTINGS_UPDATE_INTERVAL_POS] = 		SETTINGS_UPDATE_INTERVAL_DEFAULT;
    settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS] = 	SETTINGS_TIMEOUT_THRESHOLD_DEFAULT;
    settings_array[SETTINGS_FENCE_THRESHOLD_POS] = 		SETTINGS_FENCE_THRESHOLD_DEFAULT;
    settings_array[SETTINGS_TIME_ZONE_DIR_POS] = 		SETTINGS_TIME_ZONE_DIR_DEFAULT;
    settings_array[SETTINGS_TIME_ZONE_HOUR_POS] = 		SETTINGS_TIME_ZONE_HOUR_DEFAULT;
    settings_array[SETTINGS_TIME_ZONE_MINUTE_POS] = 	SETTINGS_TIME_ZONE_MINUTE_DEFAULT;
    
    //write to flash
    write_flash_page(FLASH_SETTINGS_PAGE, &settings_array[0], SETTINGS_SIZE);
}



void settings_save(struct settings_struct *p_settings)
{
	erase_flash_page(FLASH_SETTINGS_PAGE);

    //assign values
    settings_array[SETTINGS_INIT_FLAG_POS] = 			SETTINGS_INIT_FLAG_DEFAULT;
    settings_array[SETTINGS_DEVICE_NUMBER_POS] = 		p_settings->device_number;
    settings_array[SETTINGS_DEVICES_ON_AIR_POS] =		p_settings->devices_on_air;
    settings_array[SETTINGS_DEVICE_ID_POS] = 			p_settings->device_id;
    settings_array[SETTINGS_FREQ_CHANNEL_POS] = 		p_settings->freq_channel;
    settings_array[SETTINGS_TX_POWER_POS] = 			p_settings->tx_power_opt;
    settings_array[SETTINGS_UPDATE_INTERVAL_POS] = 		p_settings->update_interval_opt;
    settings_array[SETTINGS_TIMEOUT_THRESHOLD_POS] = 	p_settings->timeout_threshold;
    settings_array[SETTINGS_FENCE_THRESHOLD_POS] = 		p_settings->fence_threshold;
    settings_array[SETTINGS_TIME_ZONE_DIR_POS] = 		p_settings->time_zone_dir;
    settings_array[SETTINGS_TIME_ZONE_HOUR_POS] = 		p_settings->time_zone_hour;
    settings_array[SETTINGS_TIME_ZONE_MINUTE_POS] = 	p_settings->time_zone_minute;
    
    //write to flash
    write_flash_page(FLASH_SETTINGS_PAGE, &settings_array[0], SETTINGS_SIZE);
}
