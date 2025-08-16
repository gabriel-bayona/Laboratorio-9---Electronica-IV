/*********************************************************************************************************************
Copyright (c) 2025, Bayona Franco Gabriel <gabrielbayona19@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

#ifndef BSP_H_
#define BSP_H_

/** @file bsp.h
 ** @brief Declaraciones públicas para la placa de desarrollo.
 ** @details Este archivo contiene las definiciones y declaraciones necesarias para manejar la placa de desarrollo.
 **/
/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include "screen.h"
#include "config.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Estructura que representa la placa de desarrollo.
 * @details Esta estructura contiene los componentes digitales y la pantalla asociados a la placa.
 */
typedef struct Board_s {
    digital_output_t buzzer;
    digital_input_t set_time;
    digital_input_t set_alarm;
    digital_input_t decrement;
    digital_input_t increment;
    digital_input_t accept;
    digital_input_t cancel;
    digital_output_t led_red;
    digital_output_t led_green;
    digital_output_t led_blue;

    screen_t screen;
} const * Board_t;
/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Inicializa el temporizador del sistema.
 * @param ticks Cantidad de ticks por segundo.
 * @note Esta función configura el temporizador del sistema para generar interrupciones a intervalos regulares.
 */
void SysTickInit(uint16_t ticks);

/**
 * @brief Crea e instancia la estructura que representa la placa de desarrollo.
 * @return Un identificador para la placa de desarrollo.
 * @note Esta función instancia la estructura y retorna su descriptor.
 */
Board_t BoardCreate(void);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BSP_H_ */
