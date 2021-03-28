[TOC]

## 介绍

QuickLed 顾名思义，就是快速上手点亮手中的LED，闪烁频率、闪烁次数、点亮、熄灭，尽在掌握。

## 接口一览

```
int8_t led_create(led_obj_t *obj, void (*on)(void *arg), void (*off)(void *arg), void *arg);

int8_t led_on(led_obj_t *obj);
int8_t led_off(led_obj_t *obj);
int8_t led_toggle(led_obj_t *obj);

int8_t led_set_mode(led_obj_t *obj, int32_t loop, uint32_t on_period_ms, uint32_t off_period_ms, void (*endloop_cb)(led_obj_t *obj));
int8_t led_start(led_obj_t *obj);
int8_t led_stop(led_obj_t *obj);

void led_tick(uint16_t period_ms);
```



## 如何使用

快速使用说明：

1. led_create创建一个LED对象，并把点亮该LED、熄灭该LED的函数作为参数，注册到该LED对象里边。
2. 若只需要用到点亮、熄灭、反转LED灯话，使用led_on、led_off、led_toggle。
3. 若需要周期性的闪烁LED等，则可使用led_set_mode来设置LED闪烁，设置总共闪烁循环的次数、亮的时长、灭的时长，以及结束循环后的回调函数；当然你也可以设置为无限循环。
4. 另外你需要周期性的调用led_tick。



## 例子

参考 ./demo

