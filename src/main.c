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

/* === Headers files inclusions ==================================================================================== */

#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "digital.h"
#include "config.h"
#include "bsp.h"
#include "screen.h"
#include "poncho.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */
#define LONG_PRESS_TIME_MS    3000
#define DEBOUNCE_TOLERANCE_MS 100

/* === Private data type declarations ============================================================================== */
system_mode_t mode = MODE_UNSET;
Board_t board;
clock_t clock;
clock_time_t current_time;
clock_time_t alarm_time;

uint8_t digits[4] = {0, 0, 0, 0};
uint8_t dots[4] = {0, 1, 0, 0};
uint8_t last_state;

volatile bool timeout = false;
volatile uint8_t timeout_counter = 0;

typedef struct {
    TickType_t press_time;
    TickType_t release_time;
    bool active;
} long_press_t;

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Private function declarations =============================================================================== */
void LongPressInit(long_press_t * lp) {
    lp->press_time = 0;
    lp->release_time = 0;
    lp->active = false;
}

bool LongPressUpdate(long_press_t * lp, bool is_pressed, TickType_t current_time, TickType_t press_threshold,
                     TickType_t release_tolerance) {
    if (is_pressed) {
        lp->release_time = 0;
        if (lp->press_time == 0) {
            lp->press_time = current_time;
        } else if (!lp->active && (current_time - lp->press_time >= press_threshold)) {
            lp->active = true;
            return true; // Se activó por presión larga
        }
    } else {
        if (lp->press_time > 0) {
            if (lp->release_time == 0) {
                lp->release_time = current_time;
            } else if (current_time - lp->release_time >= release_tolerance) {
                lp->press_time = 0;
                lp->release_time = 0;
                lp->active = false;
            }
        }
    }

    return false;
}

void digitsToTime(uint8_t * digits, clock_time_t * time) {
    time->bcd[5] = digits[0];
    time->bcd[4] = digits[1];
    time->bcd[3] = digits[2];
    time->bcd[2] = digits[3];
    time->bcd[1] = 0;
    time->bcd[0] = 0;
}

void timeToDigits(uint8_t * digits, clock_time_t * time) {
    digits[0] = time->bcd[5];
    digits[1] = time->bcd[4];
    digits[2] = time->bcd[3];
    digits[3] = time->bcd[2];
}

uint32_t ClockGetTicks(void) {
    return xTaskGetTickCount();
}

void DisplayTask(void * pvParameters) {

    while (true) {
        ScreenWriteBCD(board->screen, digits, sizeof(digits));
        ScreenWriteDOT(board->screen, dots, sizeof(dots));
        ScreenRefresh(board->screen); // Multiplexa solo
        vTaskDelay(pdMS_TO_TICKS(5)); // Refresca a 5 ms
    }
}

void ClockTask(void * pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (true) {
        ClockNewTick(clock);
        ClockGetTime(clock, &current_time);
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1));
    }
}

void ButtonTask(void * pvParameters) {
    static long_press_t set_alarm_lp;
    LongPressInit(&set_alarm_lp);

    static long_press_t set_time_lp;
    LongPressInit(&set_time_lp);

    while (true) {
        switch (mode) {
        case MODE_UNSET:
            if (LongPressUpdate(&set_alarm_lp, !DigitalInputGetIsActive(board->set_alarm), xTaskGetTickCount(),
                                pdMS_TO_TICKS(LONG_PRESS_TIME_MS), pdMS_TO_TICKS(DEBOUNCE_TOLERANCE_MS))) {

                if (ClockGetAlarmTime(clock, &alarm_time)) {
                    timeToDigits(digits, &alarm_time);
                }
                mode = MODE_SET_ALARM_MINUTES;
                last_state = MODE_UNSET;
                DisplayFlashDigits(board->screen, 2, 3, 10);

            } else if (LongPressUpdate(&set_time_lp, !DigitalInputGetIsActive(board->set_time), xTaskGetTickCount(),
                                       pdMS_TO_TICKS(LONG_PRESS_TIME_MS), pdMS_TO_TICKS(DEBOUNCE_TOLERANCE_MS))) {
                mode = MODE_SET_TIME_MINUTES;
                last_state = MODE_UNSET;
                DisplayFlashDigits(board->screen, 2, 3, 10);
            }
            break;

        case MODE_HOME:

            if (ClockGetTime(clock, &current_time)) {
                timeToDigits(digits, &current_time); // Actualizar los dígitos con la hora actual
            }

            ScreenWriteBCD(board->screen, digits, sizeof(digits));

            if (LongPressUpdate(&set_time_lp, !DigitalInputGetIsActive(board->set_time), xTaskGetTickCount(),
                                pdMS_TO_TICKS(LONG_PRESS_TIME_MS), pdMS_TO_TICKS(DEBOUNCE_TOLERANCE_MS))) {
                mode = MODE_SET_TIME_MINUTES;
                last_state = MODE_HOME;
                DisplayFlashDigits(board->screen, 2, 3, 10);
            } else if (LongPressUpdate(&set_alarm_lp, !DigitalInputGetIsActive(board->set_alarm), xTaskGetTickCount(),
                                       pdMS_TO_TICKS(LONG_PRESS_TIME_MS), pdMS_TO_TICKS(DEBOUNCE_TOLERANCE_MS))) {

                if (ClockGetAlarmTime(clock, &alarm_time)) {
                    timeToDigits(digits, &alarm_time); // Convierte la hora de la alarma a dígitos
                }
                mode = MODE_SET_ALARM_MINUTES;
                last_state = MODE_HOME;
                DisplayFlashDigits(board->screen, 2, 3, 10);
            } else if (ClockIsAlarmTriggered(clock)) {
                mode = MODE_ALARM_TRIGGERED;
                dots[3] = 1; // Indica que la alarma ha sido activada
                DigitalOutputDeactivate(board->led_blue);

            } else if (DigitalInputWasDeactivated(board->accept)) {
                ClockEnableAlarm(clock);
                dots[3] = 1; // Indica que la alarma está habilitada
            } else if (DigitalInputWasDeactivated(board->cancel)) {
                ClockDisableAlarm(clock);
                dots[3] = 0; // Indica que la alarma no está habilitada
            }

            break;

        case MODE_SET_TIME_MINUTES:

            if (DigitalInputWasDeactivated(board->cancel) || timeout) {
                if (last_state == MODE_UNSET) {
                    DisplayFlashDigits(board->screen, 0, 3, 10);
                    mode = MODE_UNSET; // Cancelar y volver al modo UNSET
                } else {
                    DisplayFlashDigits(board->screen, 0, 0, 0);
                    mode = MODE_HOME; // Cancelar y volver al modo HOME
                }
                timeout = false;
                timeout_counter = 0;
            }
            if (DigitalInputWasDeactivated(board->increment)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                uint8_t min = (digits[2] * 10 + digits[3] + 1) % 60;
                digits[2] = min / 10;
                digits[3] = min % 10;
            }
            if (DigitalInputWasDeactivated(board->decrement)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                uint8_t min = (digits[2] * 10 + digits[3] - 1 + 60) % 60;
                digits[2] = min / 10;
                digits[3] = min % 10;
            }
            if (DigitalInputWasDeactivated(board->accept)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                mode = MODE_SET_TIME_HOURS;
                DisplayFlashDigits(board->screen, 0, 1, 10);
            }

            break;

        case MODE_SET_TIME_HOURS:
            if (DigitalInputWasDeactivated(board->cancel) || timeout) {
                if (last_state == MODE_UNSET) {
                    DisplayFlashDigits(board->screen, 0, 3, 10);
                    mode = MODE_UNSET; // Cancelar y volver al modo UNSET
                } else {
                    DisplayFlashDigits(board->screen, 0, 0, 0);
                    mode = MODE_HOME; // Cancelar y volver al modo HOME
                }
                timeout = false;
                timeout_counter = 0;
            }
            if (DigitalInputWasDeactivated(board->increment)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                uint8_t horas = (digits[0] * 10 + digits[1] + 1) % 24;
                digits[0] = horas / 10;
                digits[1] = horas % 10;
            }
            if (DigitalInputWasDeactivated(board->decrement)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                uint8_t horas = (digits[0] * 10 + digits[1] - 1 + 24) % 24;
                digits[0] = horas / 10;
                digits[1] = horas % 10;
            }
            if (DigitalInputWasDeactivated(board->accept)) {

                DisplayFlashDigits(board->screen, 0, 0, 0);
                digitsToTime(digits, &current_time); // Convierte los dígitos a tiempo actual
                if (ClockSetTime(clock, &current_time)) {
                    mode = MODE_HOME; // Vuelve al modo HOME después de aceptar
                    last_state = MODE_HOME;
                }
            }

            break;

        case MODE_SET_ALARM_MINUTES:
            if (DigitalInputWasDeactivated(board->cancel) || timeout) {
                if (last_state == MODE_UNSET) {
                    DisplayFlashDigits(board->screen, 0, 3, 10);
                    mode = MODE_UNSET; // Cancelar y volver al modo UNSET
                } else {
                    DisplayFlashDigits(board->screen, 0, 0, 0);
                    mode = MODE_HOME; // Cancelar y volver al modo HOME
                }
                timeout = false;
                timeout_counter = 0;
            }

            if (DigitalInputWasDeactivated(board->increment)) {
                timeout = false; // Reiniciar el timeout

                uint8_t min = (digits[2] * 10 + digits[3] + 1) % 60;
                digits[2] = min / 10;
                digits[3] = min % 10;
            }
            if (DigitalInputWasDeactivated(board->decrement)) {
                timeout = false; // Reiniciar el timeout

                uint8_t min = (digits[2] * 10 + digits[3] - 1 + 60) % 60;
                digits[2] = min / 10;
                digits[3] = min % 10;
            }
            if (DigitalInputWasDeactivated(board->accept)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                mode = MODE_SET_ALARM_HOURS;
                DisplayFlashDigits(board->screen, 0, 1, 10);
            }

            break;

        case MODE_SET_ALARM_HOURS:

            if (DigitalInputWasDeactivated(board->cancel) || timeout) {
                if (last_state == MODE_UNSET) {
                    DisplayFlashDigits(board->screen, 0, 3, 10);
                    mode = MODE_UNSET; // Cancelar y volver al modo UNSET
                } else {
                    DisplayFlashDigits(board->screen, 0, 0, 0);
                    mode = MODE_HOME; // Cancelar y volver al modo HOME
                }
                timeout = false;
                timeout_counter = 0;
            }

            if (DigitalInputWasDeactivated(board->increment)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                uint8_t hora = (digits[0] * 10 + digits[1] + 1) % 24;
                digits[0] = hora / 10;
                digits[1] = hora % 10;
            }
            if (DigitalInputWasDeactivated(board->decrement)) {
                timeout = false;     // Reiniciar el timeout
                timeout_counter = 0; // Reiniciar el contador

                uint8_t hora = (digits[0] * 10 + digits[1] - 1 + 24) % 24;
                digits[0] = hora / 10;
                digits[1] = hora % 10;
            }
            if (DigitalInputWasDeactivated(board->accept)) {

                digitsToTime(digits, &alarm_time); // Convierte los dígitos a tiempo de alarma
                if (ClockSetAlarmTime(clock, &alarm_time)) {
                    dots[3] = 1; // Indica que la alarma está habilitada
                    DisplayFlashDigits(board->screen, 0, 0, 0);
                    ClockEnableAlarm(clock); // Habilita la alarma
                    if (last_state != MODE_UNSET) {
                        mode = MODE_HOME;
                    } else {
                        mode = MODE_UNSET;
                        DisplayFlashDigits(board->screen, 0, 3, 10);
                        digits[0] = 0;
                        digits[1] = 0;
                        digits[2] = 0;
                        digits[3] = 0; // Reinicia los dígitos
                    }
                }
            }

            break;

        case MODE_ALARM_TRIGGERED:

            if (DigitalInputWasDeactivated(board->cancel)) {
                ClockCancelAlarmUntilNextDay(clock);
                mode = MODE_HOME;
                DigitalOutputActivate(board->led_blue);
            } else if (DigitalInputWasDeactivated(board->accept)) {
                ClockSnoozeAlarm(clock, 5);
                mode = MODE_HOME;
                DigitalOutputActivate(board->led_blue);
            }

            // No quiero que se quede parado en el modo de alarma, así que actualizo la hora

            if (ClockGetTime(clock, &current_time)) {
                timeToDigits(digits, &current_time); // Actualizar los dígitos con la hora actual
            }

            ScreenWriteBCD(board->screen, digits, sizeof(digits));
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void DotBlinkTask(void * pvParameters) {
    (void)pvParameters;

    while (1) {

        if (mode == MODE_HOME || mode == MODE_ALARM_TRIGGERED) {
            dots[1] = !dots[1]; // Esto se ve reflejado directamente
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo
    }
}

void TimeOutTask(void * pvParameters) {
    (void)pvParameters;

    while (1) {

        if (mode == MODE_SET_ALARM_HOURS || mode == MODE_SET_ALARM_MINUTES || mode == MODE_SET_TIME_HOURS ||
            mode == MODE_SET_TIME_MINUTES) {
            timeout_counter++;
            if (timeout_counter >= 30) { // 30 segundos de inactividad
                timeout = true;
                timeout_counter = 0; // Reiniciar el contador
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo
    }
}

/* === Public function implementation ============================================================================== */

int main(void) {
    board = BoardCreate();
    clock = ClockCreate(1000);

    SysTickInit(1000);

    DisplayFlashDigits(board->screen, 0, 3, 10);
    xTaskCreate(DisplayTask, "Display", 512, NULL, 3, NULL);
    xTaskCreate(ClockTask, "Clock", 512, NULL, 1, NULL);

    xTaskCreate(ButtonTask, "Buttons", 512, NULL, 1, NULL);
    xTaskCreate(DotBlinkTask, "DotBlink", 256, NULL, 1, NULL);
    xTaskCreate(TimeOutTask, "Timeout", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1); // Nunca debería llegar acá
}

/* === End of documentation ======================================================================================== */