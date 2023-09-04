/*!
 * @file        bsp_key.h
 *
 * @brief       Header for bsp_key.c module
 *
 * @version     V1.0.0
 *
 * @date        2022-02-25
 *
 * @attention
 *
 *  Copyright (C) 2021-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include "Board.h"
#include "bsp_delay.h"

/** @addtogroup Board
  @{
*/

/** @addtogroup Board_APM32F407_MINI
  @{
*/

/** @defgroup APM32F407_MINI_Macros
  @{
  */

/** GPIO define*/
#define KEY1_PIN            KEY1_BUTTON_PIN
#define KEY2_PIN            KEY2_BUTTON_PIN

#define KEY1                GPIO_ReadInputBit(KEY1_BUTTON_GPIO_PORT,KEY1_BUTTON_PIN)
#define KEY2                GPIO_ReadInputBit(KEY2_BUTTON_GPIO_PORT,KEY2_BUTTON_PIN)

/**
  * @}
  */

/** @defgroup APM32F407_MINI_Enumerations
  @{
  */

/*!
 * @brief    Key value
 */
typedef enum
{
    KEY_ALL_REALEASED,      /*!< All key is realeased */
    KEY1_PRESSED,           /*!< KEY1 is pressed */
    KEY2_PRESSED,           /*!< KEY2 is pressed */
} KEY_VALUE_T;

/*!
 * @brief    Key mode
 */
typedef enum
{
    KEY_MODE_SINGLE,            /*!< Single detect mode */
    KEY_MODE_CONTINUOUS,        /*!< Continuous detect mode */
} KEY_MODE_T;

/**
  * @}
  */

/** @defgroup APM32F407_MINI_Functions
  @{
  */

/** function declaration*/
uint8_t Key_Scan(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif

/**@} end of group APM32F407_MINI_Functions */
/**@} end of group Board_APM32F407_MINI */
/**@} end of group Board */
