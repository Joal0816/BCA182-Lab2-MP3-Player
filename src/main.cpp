#include "main.h"
#include "hardware_config.h"
#include "app_tasks.h"
#include "lcd_driver.h"
#include "audio_engine.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

/* Global peripheral handles */
ADC_HandleTypeDef s_hadc1;
UART_HandleTypeDef huart1;

/* FreeRTOS SysTick handler integration */
extern "C" void xPortSysTickHandler(void);

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

/* FreeRTOS Hooks */
extern "C" void vApplicationIdleHook(void) {
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for (;;);
}

extern "C" void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    for (;;);
}

/* Retarget standard output to USART1 */
extern "C" int _write(int file, char *ptr, int len) {
    (void)file;
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, (uint16_t)len, HAL_MAX_DELAY);
    return len;
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    /* Buttons 1-4: PB0, PB1, PB2, PB3 (Active LOW with internal pull-up) */
    GPIO_InitStruct.Pin = BTN1_PIN | BTN2_PIN | BTN3_PIN | BTN4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* User Button: PA0 (Active HIGH with internal pull-down) */
    GPIO_InitStruct.Pin = USER_BTN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(USER_BTN_PORT, &GPIO_InitStruct);

    /* RGB LEDs: PF11 (Red), PF14 (Green), PF12 (Blue) */
    HAL_GPIO_WritePin(GPIOF, LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    s_hadc1.Instance = ADC1;
    s_hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    s_hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    s_hadc1.Init.ScanConvMode = DISABLE;
    s_hadc1.Init.ContinuousConvMode = DISABLE;
    s_hadc1.Init.DiscontinuousConvMode = DISABLE;
    s_hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    s_hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    s_hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    s_hadc1.Init.NbrOfConversion = 1;
    s_hadc1.Init.DMAContinuousRequests = DISABLE;
    s_hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&s_hadc1) != HAL_OK) {
        Error_Handler();
    }

    sConfig.Channel = POT_ADC_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
    if (HAL_ADC_ConfigChannel(&s_hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_USART1_UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = UART_BAUDRATE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Primary clock: HSE 8MHz crystal -> 168MHz */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        /* Fallback: HSI 16MHz -> 168MHz */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
        RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
        RCC_OscInitStruct.HSIState = RCC_HSI_ON;
        RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
        RCC_OscInitStruct.PLL.PLLM = 8;
        RCC_OscInitStruct.PLL.PLLN = 168;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
        RCC_OscInitStruct.PLL.PLLQ = 7;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
            Error_Handler();
        }
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler(void) {
    __disable_irq();
    for (;;);
}

static void print_instructions(void) {
    printf("\r\n========================================\r\n");
    printf(" BCA 182 - Laboratory Activity 2\r\n");
    printf(" STM32F407ZG Personal MP3 Player\r\n");
    printf("========================================\r\n");
    printf("Controls:\r\n");
    printf("  - User Button (PA0): Play / Pause\r\n");
    printf("  - Buttons 2-4 (PB1-PB3): Binary Song Select [0-7]\r\n");
    printf("  - Button 1 (PB0): Select Song / Confirm (5s timeout)\r\n");
    printf("  - Potentiometer (PA1): Volume Control [0-100%%]\r\n");
    printf("Status LEDs:\r\n");
    printf("  - RED   (PF11): Paused\r\n");
    printf("  - BLUE  (PF12): Playing\r\n");
    printf("  - GREEN (PF14): Song Confirming\r\n");
    printf("========================================\r\n");
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_ADC1_Init();
    lcd_init();
    audio_engine_init();

    print_instructions();

    /* Test drawing on screen immediately upon boot */
    lcd_clear();
    lcd_draw_string(56, 40, "== MP3 PLAYER ==", LCD_COLOR_GREEN, LCD_COLOR_BLACK);
    lcd_draw_line(20, 70, 220, 70, LCD_COLOR_CYAN);
    lcd_draw_string(32, 100, "Starting Player...", LCD_COLOR_WHITE, LCD_COLOR_BLACK);
    HAL_Delay(300);

    /* Create FreeRTOS application tasks */
    xTaskCreate(update_lcd_leds_thread, "LCD_LED", 512, NULL, 2, NULL);
    xTaskCreate(polling_buttons, "Buttons", 512, NULL, 3, NULL);
    xTaskCreate(adjust_volume, "Volume", 256, NULL, 1, NULL);

    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();

    while (1) {
    }
}
