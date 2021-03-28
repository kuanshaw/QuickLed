/**
* @file     mod_led.c
* @brief    LED Control Module
* @author   kuanshaw
* @date     2021-3-11
* @version  v1.0.0
*/

#ifndef __MOD_LED_H
#define __MOD_LED_H

#include <stdint.h>
#include <stddef.h>

#define LED_TICK_INTERVAL           100     //unit [ms]

typedef struct
{
    uint32_t open           : 1;    ///< 1 - led object had created, 0 - not create
    uint32_t on             : 1;    ///< 1 - led is on
    uint32_t off            : 1;    ///< 1 - led is off 
    uint32_t start          : 1;    ///< 1 - led object is in working
    uint32_t stop           : 1;    ///< 1 - led object is not in working
    uint32_t always         : 1;    ///< 1 - led object is always working
    uint32_t setmode        : 1;    ///< 1 - led object had set mode
    uint32_t leftloop       : 16;   ///< the count of left loop
    uint32_t remain         : 9;    ///< no use
} led_state_t;

typedef struct 
{
    uint16_t cnt;       ///< counter value
    uint16_t on_num;    ///< the value is equal (led in on time / period time)
    uint16_t off_num;   ///<
} led_tick_t;

typedef struct led_obj_struct led_obj_t;

struct led_obj_struct
{
    led_obj_t *next;
    
    led_state_t state;
    led_tick_t  tick;
    void (*on)(void *arg);
    void (*off)(void *arg);
    void (*endloop_cb)(led_obj_t *obj);
    void *arg;
};


int8_t led_create(led_obj_t *obj, void (*on)(void *arg), void (*off)(void *arg), void *arg);

int8_t led_on(led_obj_t *obj);
int8_t led_off(led_obj_t *obj);
int8_t led_toggle(led_obj_t *obj);

int8_t led_set_mode(led_obj_t *obj, int32_t loop, uint32_t on_period_ms, uint32_t off_period_ms, void (*endloop_cb)(led_obj_t *obj));
int8_t led_start(led_obj_t *obj);
int8_t led_stop(led_obj_t *obj);

void led_tick(uint16_t period_ms);

#endif 

