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
 ** @brief Declaraciones públicas para la gestión de entradas y salidas digitales.
 ** @details Este archivo contiene las definiciones y declaraciones necesarias para manejar entradas y salidas
 *digitales.
 **/

#ifndef DIGITAL_H_
#define DIGITAL_H_

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Estados posibles de una entrada digital.
 *
 */
typedef enum digital_states_e {
    DIGITAL_INPUT_WAS_DEACTIVATED = -1,
    DIGITAL_INPUT_NO_CHANGE = 0,
    DIGITAL_INPUT_WAS_ACTIVATED = 1,
} digital_states_t;

/** @brief Estructura que representa una salida digital.
 * @details Esta estructura contiene el número de GPIO, el bit específico del GPIO y un indicador de inversión.
 */
typedef struct digital_output_s * digital_output_t;

/** @brief Estructura que representa una entrada digital.
 * @details Esta estructura contiene el número de GPIO, el bit específico del GPIO y un indicador de inversión.
 */
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea una salida digital.
 * @param gpio El número del GPIO asociado a la salida.
 * @param bit El bit específico del GPIO que se utilizará.
 * @param inverted Indica si la salida está invertida (true) o no (false).
 * @return Un identificador para la salida digital creada.
 */
digital_output_t DigitalOutputCreate(uint8_t gpio, uint8_t bit, bool inverted);

/**
 * @brief Libera los recursos asociados a una salida digital.
 * @param output El identificador de la salida digital a liberar.
 * @note Esta función libera la memoria utilizada por la salida digital.
 */
void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Activa una salida digital.
 * @param output El identificador de la salida digital a activar.
 * @note Esta función configura el estado de la salida digital a activo (1).
 * @note Si la salida está invertida, se activa con un valor lógico bajo (0).
 */
void DigitalOutputDeactivate(digital_output_t output);

/**
 * @brief Obtiene el estado actual de una salida digital.
 * @param output El identificador de la salida digital.
 * @return true si la salida está activa, false si está inactiva.
 */
void DigitalOutputToggle(digital_output_t output);

/**
 * @brief Crea una entrada digital.
 * @param gpio El número del GPIO asociado a la entrada.
 * @param bit El bit específico del GPIO que se utilizará.
 * @param inverted Indica si la entrada está invertida (true) o no (false).
 * @return Un identificador para la entrada digital creada.
 */
digital_input_t DigitalInputCreate(uint8_t gpio, uint8_t bit, bool inverted);

/**
 * @brief Permite saber si la entrada digital está activa.
 *
 * @param input Identificador de la entrada digital.
 * @return true si la entrada está activa, false si está inactiva.
 * @note La función devuelve true si la entrada está activa o false si está inactiva, sin importar si es de lógica
 * invertida o no.
 */
bool DigitalInputGetIsActive(digital_input_t input);

/**
 * @brief Permite saber si la entrada digital está inactiva.
 *
 * @param input Identificador de la entrada digital.
 * @return true si la entrada está inactiva, false si está activa.
 *
 * @note La función devuelve tun booleano.
 */
bool DigitalInputWasActivated(digital_input_t input);

/**
 * @brief Permite saber si la entrada digital fue desactivada.
 *
 * @param input Identificador de la entrada digital.
 * @return true si la entrada fue desactivada, false si no.
 * @note La función devuelve un booleano.
 */
bool DigitalInputWasDeactivated(digital_input_t input);

/**
 * @brief Permite saber si el estado de la entrada digital cambió.
 *
 * @param input Identificador de la entrada digital.
 * @return Un valor de tipo digital_states_t que indica el estado de la entrada:
 *         - DIGITAL_INPUT_WAS_DEACTIVATED: si la entrada fue desactivada.
 *         - DIGITAL_INPUT_NO_CHANGE: si no hubo cambio en el estado.
 *         - DIGITAL_INPUT_WAS_ACTIVATED: si la entrada fue activada.
 */
digital_states_t DigitalInputWasChanged(digital_input_t input);

/* === End of conditional blocks =================================================================================== */
#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H_ */
