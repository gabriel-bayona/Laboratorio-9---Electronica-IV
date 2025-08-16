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

/** @file digital.c
 ** @brief Código fuente del modulo para la gestión de entradas y salidas digitales.
 **/

/* === Headers files inclusions ==================================================================================== */
#include <string.h>
#include "digital.h"
#include <stdlib.h>
#include "config.h"
#include <stdbool.h>
#include "chip.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct digital_output_s {
    uint8_t gpio;  /*!< Puerto al que pertenece la salida */
    uint8_t bit;   /*!< Bit al que pertenece la salida */
    bool inverted; /*! Indica si la entrada está invertida o no */
}; /*! Estructura que representa una salida digital */

struct digital_input_s {
    uint8_t gpio;   /*!< Puerto al que pertenece la entrada */
    uint8_t bit;    /*! Bit al que pertenece la entrada */
    bool inverted;  /*! Indica si la entrada está invertida o no */
    bool lastState; /*! Último estado conocido de la entrada */
}; /*!< Estructura que representa una entrada digital */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

digital_output_t DigitalOutputCreate(uint8_t gpio, uint8_t bit, bool inverted) {
    digital_output_t self = malloc(sizeof(struct digital_output_s));
    if (self != NULL) {
        self->gpio = gpio;
        self->bit = bit;
        self->inverted = inverted;

        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio, self->bit, self->inverted);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->gpio, self->bit, true);
    }
    return self;
}

void DigitalOutputActivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio, self->bit, self->inverted);
}

void DigitalOutputDeactivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio, self->bit, !self->inverted);
}

void DigitalOutputToggle(digital_output_t self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->gpio, self->bit);
}

digital_input_t DigitalInputCreate(uint8_t gpio, uint8_t bit, bool inverted) {
    digital_input_t self = malloc(sizeof(struct digital_input_s));
    if (self != NULL) {
        self->gpio = gpio;
        self->bit = bit;
        self->inverted = inverted;

        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->gpio, self->bit, false);

        self->lastState = DigitalInputGetIsActive(self);
    }
    return self;
}

bool DigitalInputGetIsActive(digital_input_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->gpio, self->bit) != 0;
    if (self->inverted) {
        state = !state;
    }

    return state;
}

digital_states_t DigitalInputWasChanged(digital_input_t self) {
    digital_states_t result = DIGITAL_INPUT_NO_CHANGE;

    bool state = DigitalInputGetIsActive(self);

    if (state && !self->lastState) {
        result = DIGITAL_INPUT_WAS_ACTIVATED;
    } else if (!state && self->lastState) {
        result = DIGITAL_INPUT_WAS_DEACTIVATED;
    }
    self->lastState = state;
    return result;
}

bool DigitalInputWasActivated(digital_input_t self) {
    return DIGITAL_INPUT_WAS_ACTIVATED == DigitalInputWasChanged(self);
}

bool DigitalInputWasDeactivated(digital_input_t self) {
    return DIGITAL_INPUT_WAS_DEACTIVATED == DigitalInputWasChanged(self);
}

/* === End of documentation ======================================================================================== */
