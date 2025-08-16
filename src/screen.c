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

/** @file screen.c
 ** @brief Implementación del modulo para la gestión de una pantalla de 7 segmentos.
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "poncho.h"
#include "bsp.h"
/* === Macros definitions ========================================================================================== */
#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

/* === Private data type declarations ============================================================================== */

struct screen_s {
    uint8_t digits;
    uint8_t dots; // Nuevo: número de puntos
    uint8_t currentDigit;
    uint8_t flashing_from;
    uint8_t flashing_to;
    uint8_t flashing_count;
    uint16_t flashing_frequency;
    screen_driver_t driver;
    uint8_t value[SCREEN_MAX_DIGITS];
    uint8_t value_dot[SCREEN_MAX_DIGITS]; // Nuevo: estado de los puntos
};

/* === Private function declarations =============================================================================== */
static const uint8_t IMAGES[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

screen_t ScreenCreate(uint8_t digits, uint8_t dots, screen_driver_t driver) {
    screen_t screen = malloc(sizeof(struct screen_s));
    if (digits > SCREEN_MAX_DIGITS) {
        digits = SCREEN_MAX_DIGITS;
    }
    if (screen != NULL) {
        screen->digits = digits;
        screen->dots = dots;
        screen->driver = driver;
        screen->currentDigit = 0;
        screen->flashing_count = 0;
        screen->flashing_frequency = 0;
    }

    return screen;
}

void ScreenWriteBCD(screen_t screen, uint8_t * value, uint8_t size) {
    memset(screen->value, 0, sizeof(screen->value));

    if (size > screen->digits) {
        size = screen->digits;
    }
    for (uint8_t i = 0; i < size; i++) {
        screen->value[i] = IMAGES[value[i]];
    }
}

void ScreenWriteDOT(screen_t screen, uint8_t * value_dot, uint8_t size) {
    memset(screen->value_dot, 0, sizeof(screen->value_dot));

    if (size > screen->dots) {
        size = screen->dots;
    }
    for (uint8_t i = 0; i < size; i++) {
        if (value_dot[i]) {
            screen->value_dot[i] = SEGMENT_P; // Asignar el segmento del punto
        }
    }
}

void ScreenRefresh(screen_t screen) {
    uint8_t segments;

    screen->driver->DigitsTurnOff();
    screen->currentDigit = (screen->currentDigit + 1) % screen->digits;

    // segments = screen->value[screen->currentDigit];
    segments = screen->value[screen->currentDigit] | screen->value_dot[screen->currentDigit];

    if (screen->flashing_frequency != 0) {
        if (screen->currentDigit == 0) {
            screen->flashing_count = (screen->flashing_count + 1) % (screen->flashing_frequency);
        }
        if (screen->flashing_count < (screen->flashing_frequency / 2)) {
            if (screen->currentDigit >= screen->flashing_from) {
                if (screen->currentDigit <= screen->flashing_to) {
                    segments = 0; // Flashing off
                }
            }
        }
    }
    screen->driver->SegmentsUpdate(segments);
    screen->driver->DigitTurnOn(screen->currentDigit);
}

int DisplayFlashDigits(screen_t screen, uint8_t from, uint8_t to, uint16_t divisor) {
    int result = 0;
    if ((from > to) || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)) {
        result = -1;
    } else if (!screen) {
        result = -1;
    } else {
        screen->flashing_from = from;
        screen->flashing_to = to;
        screen->flashing_frequency = 2 * divisor;
        screen->flashing_count = 0;
    }
    return result;
}

void ScreenToggleDot(screen_t screen, uint8_t position) {
    if (position < SCREEN_MAX_DIGITS) {
        screen->value_dot[position] ^= SEGMENT_P;
    }
}

/* === End of documentation ========================================================================================
 */
