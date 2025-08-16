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

/** @file digital.h
 ** @brief Declaraciones públicas para la gestión de una pantalla de 7 segmentos.
 **/

#ifndef SCREEN_H_
#define SCREEN_H_

/* === Headers files inclusions ==================================================================================== */

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define SEGMENT_A (1 << 0)
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)

/* === Public data type declarations =============================================================================== */

/*
 * @brief Estructura que representa el estado de un dígito en la pantalla de 7 segmentos.
 * @details Esta estructura contiene un valor BCD y un indicador de punto decimal.
 */
typedef struct screen_s * screen_t; //

/*
 * @brief Tipo de función para apagar todos los dígitos de la pantalla.
 * @details Esta función se utiliza para apagar todos los dígitos de la pantalla de 7 segmentos.
 * @note Se debe implementar en el controlador de pantalla.
 */
typedef void (*digits_turn_off_t)(void);

/*
 * @brief Tipo de función para actualizar los segmentos de un dígito en la pantalla.
 * @details Esta función se utiliza para actualizar los segmentos de un dígito específico en la pantalla de 7 segmentos.
 * @param digit El número del dígito a actualizar (0 a N-1, donde N es el número total de dígitos).
 * @note Se debe implementar en el controlador de pantalla.
 */
typedef void (*segments_update_t)(uint8_t);

/*
 * @brief Tipo de función para encender un dígito específico en la pantalla.
 * @details Esta función se utiliza para encender un dígito específico en la pantalla de 7 segmentos.
 * @param digit El número del dígito a encender (0 a N-1, donde N es el número total de dígitos).
 * @note Se debe implementar en el controlador de pantalla.
 */
typedef void (*digit_turn_on_t)(uint8_t);

/**
 * @brief Estructura que representa un controlador de pantalla de 7 segmentos.
 * @details Esta estructura contiene punteros a funciones que permiten interactuar con la pantalla.
 * @note Se debe implementar en el controlador de pantalla.
 */
typedef struct screen_driver_s {
    digits_turn_off_t DigitsTurnOff;
    segments_update_t SegmentsUpdate;
    digit_turn_on_t DigitTurnOn;
} const * screen_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea e instancia una pantalla de 7 segmentos.
 * @param digits Número de dígitos en la pantalla.
 * @param dots Número de puntos en la pantalla.
 * @param driver Controlador de pantalla.
 * @return Un identificador para la pantalla creada.
 */
screen_t ScreenCreate(uint8_t digits, uint8_t dots, screen_driver_t driver);

/**
 * @brief Escribe un valor en formato BCD en la pantalla de 7 segmentos.
 * @param screen Identificador de la pantalla.
 * @param value Valor a escribir en formato BCD.
 * @param size Número de dígitos a escribir.
 */
void ScreenWriteBCD(screen_t screen, uint8_t * value, uint8_t size);

/**
 * @brief Escribe un punto decimal en la pantalla de 7 segmentos.
 * @details Esta función escribe un valor en formato decimal en la pantalla de 7 segmentos, permitiendo mostrar números
 * con puntos decimales.
 * @param screen Identificador de la pantalla.
 * @param value_dot Puntero al valor a escribir en formato decimal.
 * @param size Número de dígitos a escribir.
 */
void ScreenWriteDOT(screen_t screen, uint8_t * value_dot, uint8_t size);

/**
 * @brief refresca la pantalla de 7 segmentos.
 * @param screen Identificador de la pantalla.
 */
void ScreenRefresh(screen_t screen); //

/**
 * @brief Apaga todos los dígitos de la pantalla.
 * @param screen Identificador de la pantalla.
 */
int DisplayFlashDigits(screen_t screen, uint8_t from, uint8_t to, uint16_t divisor);

/**
 * @brief  Parpadeo del punto indicado en la pantalla
 *
 * @param screen Pantalla
 * @param position  Posicion del punto al que se quiere parpadear [0,1,2,3].
 */
void ScreenToggleDot(screen_t screen, uint8_t position);
/* === End of conditional blocks =================================================================================== */
#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
