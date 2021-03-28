#ifndef __APP_LED_H
#define __APP_LED_H

#include "board.h"
#include "mod_led.h"

typedef enum
{
    LED_YELLOW_ID = 0,
    LED_GREEN_ID,
    LED_BLUE_ID,
    _LED_ID_MAX,
} app_led_id_t;


extern led_obj_t led_obj[];

void led_init(void);
void led_process_100ms(void);


#endif 
