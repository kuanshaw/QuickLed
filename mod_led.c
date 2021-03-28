/**
* @file     mod_led.c
* @brief    LED Control Module
* @author   kuanshaw
* @date     2021-3-11
* @version  v1.0.0
*/

#include "mod_led.h"

//#define LED_DEBUG_ENABLE
#ifdef  LED_DEBUG_ENABLE
#include <rtthread.h>
#define LED_DEBUG(fmt, ...)  rt_kprintf("[LedLog]: "fmt, ##__VA_ARGS__)
#else
#define LED_DEBUG(fmt, ...)
#endif

static led_obj_t *_head = NULL;

static void _led_obj_reset(led_obj_t *obj)
{
    obj->next = NULL;
    
    obj->state.open = 0;
    obj->state.on = 0;
    obj->state.off = 0;
    obj->state.start = 0;
    obj->state.stop = 0;
    obj->state.always = 0;
    obj->state.setmode = 0;
    obj->state.leftloop = 0;
    
    obj->tick.cnt = 0;
    obj->tick.on_num = 0;
    obj->tick.off_num = 0;
    
    obj->on = NULL;
    obj->off = NULL;
    obj->endloop_cb = NULL;
    obj->arg = NULL;
}


/** 
 * @brief      Create LED control target
 * @param[in]  obj Led object handler
 * @param[in]  on  The function that turn on led
 * @param[in]  off The function that turn on led
 * @param[in]  arg User arguments
 * @retval     0 Create Success -1 Create Fail
 * @note     
 *@code
 * 
 *@endcode
 */ 
int8_t led_create(led_obj_t *obj, void (*on)(void *arg), void (*off)(void *arg), void *arg)
{
    led_obj_t *_obj = _head;
    
    if((obj == NULL) || (on == NULL) || (off == NULL))
        return -1;
    
    _led_obj_reset(obj);
    
    // TODO: insert the new Object in the head of list
    while(_obj)
    {
        if(_obj == obj)
            return -2;
        _obj = _obj->next;
    }
    obj->next = _head;
    _head = obj;
    
    // TODO: 
    obj->on  = on;
    obj->off = off;
    obj->arg = arg;
    obj->state.open = 1;
    
    led_stop(obj);
    led_off(obj);   //default state is led off
    
    return 0;
}


/** 
 * @brief      Turn on LED
 * @param[in]  obj Led object handler
 * @retval     0 Success, -1 Fail
 * @note       
 */ 
int8_t led_on(led_obj_t *obj)
{
    if(obj == NULL)
        return -1;
    
    if(!obj->state.open)
        return -2;
    
    LED_DEBUG("Led on\r\n");
    obj->on(obj->arg);
    obj->state.on = 1;
    obj->state.off = 0;
    
    return 0;
}


/** 
 * @brief      Turn off LED
 * @param[in]  obj Led object handler
 * @retval     0 Success, -1 Fail
 * @note       
 */ 
int8_t led_off(led_obj_t *obj)
{
    if(obj == NULL)
        return -1;
    
    if(!obj->state.open)
        return -2;
    
    LED_DEBUG("led off\r\n");
    obj->off(obj->arg);
    obj->state.off = 1;
    obj->state.on = 0;
    
    return 0;
}


/** 
 * @brief      Toggle LED
 * @param[in]  obj Led object handler
 * @retval     0 Success, -1 Fail
 * @note       
 */ 
int8_t led_toggle(led_obj_t *obj)
{
    if(obj->state.off)
        return led_on(obj);
    else
        return led_off(obj);
}


/** 
 * @brief      Set the LED change period
 * @param[in]  obj Led object handler
 * @param[in]  loop Number of LED changes
 * @param[in]  period_ms Time base of period, unit [ms]
 * @param[in]  on_period_ms Time of LED on, unit [ms]
 * @param[in]  off_period_ms Time of LED off, unit [ms]
 * @param[in]  endloop_cb The function when end of the loop
 * @retval     0 Success, -1 Fail
 * @note       period_ms must be the same as the period that call led_tick()
 */ 
int8_t led_set_mode(led_obj_t *obj, int32_t loop, uint32_t on_period_ms, uint32_t off_period_ms, void (*endloop_cb)(led_obj_t *obj))
{
    if(obj == NULL)
        return -1;
    
    if(!obj->state.open)
        return -2;
    
    if(obj->state.setmode)
    {
        led_stop(obj);
        obj->state.setmode = 0;
    }
    
    if(loop > 0)
    {
        obj->state.leftloop = (uint16_t)loop;
        obj->state.always = 0;
    }
    else
    {
        obj->state.leftloop = 0;
        obj->state.always = 1;
    }
    
    obj->tick.on_num  = on_period_ms  / LED_TICK_INTERVAL;
    obj->tick.off_num = off_period_ms / LED_TICK_INTERVAL;
    
    obj->endloop_cb = NULL;
    if(endloop_cb != NULL)
        obj->endloop_cb  = endloop_cb;
    
    obj->state.setmode = 1;
    
    LED_DEBUG("set mode: Cnt: On %d, Off %d\r\n", obj->tick.on_num, obj->tick.off_num);
    
    return 0;
}


/** 
 * @brief      LED start to change periodically
 * @param[in]  obj Led object handler
 * @retval     0 Success, -1 Fail
 * @note       
 */ 
int8_t led_start(led_obj_t *obj)
{
    if(obj == NULL)
        return -1;
    
    if(!obj->state.open)
        return -2;
    
    LED_DEBUG("led start\r\n");
    obj->state.start = 1;
    obj->state.stop = 0;
    
    return 0;
}


/** 
 * @brief      LED stop to change periodically
 * @param[in]  obj Led object handler
 * @retval     0 Success, -1 Fail
 * @note       
 */ 
int8_t led_stop(led_obj_t *obj)
{
    if(obj == NULL)
        return -1;
    
    if(!obj->state.open)
        return -2;
    
    LED_DEBUG("led stop\r\n");
    obj->state.start = 0;
    obj->state.stop = 1;
    
    return 0;
}


static void _handler(led_obj_t *obj)
{
    if((!obj->state.open) || (!obj->state.setmode))
    {
//        LED_DEBUG("led object is NOT open, or NOT set mode!\r\n");
        return;
    }
    
    if(obj->state.stop)
        return;
    
    if((obj->state.leftloop > 0) || (obj->state.always))
    {
        obj->tick.cnt++;
        
        if(obj->state.on)
        {
            if(obj->tick.cnt >= obj->tick.on_num)
            {
                obj->tick.cnt = 0;
                led_off(obj);
                
                if(obj->state.leftloop > 0)
                {
                    obj->state.leftloop --;
                    LED_DEBUG("remain loop: %d\r\n", obj->state.leftloop);
                    if((!obj->state.leftloop) && (obj->endloop_cb != NULL))
                    {
                        led_stop(obj);  //stop led
                        obj->endloop_cb(obj);   //call back
                    }
                }
            }
        }
        else
        {
            if(obj->tick.cnt >= obj->tick.off_num)
            {
                obj->tick.cnt = 0;
                led_on(obj);
            }
        }
    }
}


/** 
 * @brief      LED Tick Handler
 * @param[in]  period_ms The period of calling
 * @note       
 */ 
void led_tick(uint16_t period_ms)
{
    led_obj_t *obj = NULL;

    for(obj = _head; obj; obj = obj->next)
    {
        _handler(obj);
    }
}


