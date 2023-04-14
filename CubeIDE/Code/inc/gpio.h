/*
    SPOKE
    
    file: gpio.h
*/



#ifndef GPIO_HEADER
#define GPIO_HEADER



void gpio_init(void);
void ext_int_init(void);
void enable_buttons_interrupts(void);
void disable_buttons_interrupts(void);

void x6_high(void);
void x6_low(void);

void led_red_on(void);
void led_red_off(void);
void led_green_on(void);
void led_green_off(void);

void cs_rfm98_active(void);
void cs_rfm98_inactive(void);

void res_rfm98_active(void);
void res_rfm98_inactive(void);

void res_lcd_active(void);
void res_lcd_inactive(void);

void res_lcd_active(void);
void res_lcd_inactive(void);

void lcd_data_mode(void);
void lcd_command_mode(void);

void cs_lcd_active(void);
void cs_lcd_inactive(void);

void hold_power(void);
void release_power(void);

#endif /*GPIO_HEADER*/
