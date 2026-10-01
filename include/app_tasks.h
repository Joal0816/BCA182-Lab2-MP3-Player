#ifndef APP_TASKS_H
#define APP_TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Thread 1: Handles LCD and RGB LED updates with Mutex protection on LCD.
 */
void update_lcd_leds_thread(void *pvParameters);

/**
 * @brief Thread 2: Handles button 2-4 binary selection, button 1 select/confirm with 5s timeout, user button play/pause.
 */
void polling_buttons(void *pvParameters);

/**
 * @brief Thread 3: Reads ADC potentiometer and updates volume.
 */
void adjust_volume(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* APP_TASKS_H */
