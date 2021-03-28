#include "app_led.h"

#define DBG_TAG "app/led"
#define DBG_LVL DBG_LOG 
#include <rtdbg.h>

led_obj_t led_obj[_LED_ID_MAX];
static uint8_t led_arg[_LED_ID_MAX] = 
{
    LED_YELLOW_ID,
    LED_GREEN_ID,
    LED_BLUE_ID,
};



static void off(void *arg)
{
    uint8_t index = *(uint8_t *)arg;
    
    LOG_D("led%d off", index);
    switch(index)
    {
        case LED_YELLOW_ID:
            rt_pin_write(LED_YELLOW_PIN, PIN_HIGH); break;
        case LED_GREEN_ID:
            rt_pin_write(LED_GREEN_PIN, PIN_HIGH); break;
        case LED_BLUE_ID:
            rt_pin_write(LED_BLUE_PIN, PIN_HIGH); break;
        default:
            break;
    }
}


static void on(void *arg)
{
    uint8_t index = *(uint8_t *)arg; 
    
    LOG_D("led%d on", index);
    switch(index)
    {
        case LED_YELLOW_ID:
            rt_pin_write(LED_YELLOW_PIN, PIN_LOW); break;
        case LED_GREEN_ID:
            rt_pin_write(LED_GREEN_PIN, PIN_LOW); break;
        case LED_BLUE_ID:
            rt_pin_write(LED_BLUE_PIN, PIN_LOW); break;
        default:
            break;
    }
}


void led_init(void)
{
	rt_pin_mode(LED_YELLOW_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(LED_GREEN_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(LED_BLUE_PIN, PIN_MODE_OUTPUT);
    
    int index = 0;
    for(; index < _LED_ID_MAX; index++)
    {
        led_create(&led_obj[index], on, off, (void *)&led_arg[index]);
        led_set_mode(&led_obj[index], -1, 1000, 1000, NULL);
        led_start(&led_obj[index]);
    }
}


void led_process_100ms(void)
{
    led_tick(LED_TICK_INTERVAL);
}


