/*!
 * @file        DAC_lib.c
 *
 * @brief       Функции для работы с ЦАП
 *
 * @version     V1.0.0
 *
 * @date        02-10-2023
 */

#include "DAC_lib.h"

/*!
 * @brief     Инициализация модуля ЦАП
 *
 * Выставленные параметры:
 * - канал 1, пин PA4
 */

void DAC_Init()
{
    GPIO_Config_T   gpioConfig;
    DAC_Config_T    dacConfig;

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NOPULL;
    gpioConfig.pin     = GPIO_PIN_4;
    GPIO_Config(GPIOA, &gpioConfig);

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_DAC);

    DAC_ConfigStructInit(&dacConfig);
    dacConfig.trigger             = DAC_TRIGGER_NONE;
    dacConfig.waveGeneration      = DAC_WAVE_GENERATION_NONE;
    dacConfig.maskAmplitudeSelect = DAC_LFSR_MASK_BIT11_1;
    dacConfig.outputBuffer        = DAC_OUTPUT_BUFFER_ENBALE;
    DAC_Config(DAC_CHANNEL_1, &dacConfig);

    DAC_Enable(DAC_CHANNEL_1);
}

/*!
* @brief     Установка High (3.3 V) уровня на ЦАП
 */

void DAC_SetHigh() {
	DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0FFF);
}

/*!
* @brief     Установка Low (0 V) уровня на ЦАП
 */
	
void DAC_SetLow() {
	DAC_ConfigChannel1Data(DAC_ALIGN_12BIT_R, 0x0);
}
