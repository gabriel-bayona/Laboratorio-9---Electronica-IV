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

/** @file clock.h
 ** @brief
 **/

#ifndef CLOCK_H_
#define CLOCK_H_

/* === Headers files inclusions ==================================================================================== */
#include <stdbool.h>
#include <stdint.h>
/* === Headers files inclusions ==================================================================================== */

/* === Header for C++ compatibility ================================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Private macros definitions ================================================================================ */
/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/** @brief Modo del sistema para la configuración del reloj y la alarma.
 *
 * Este enum define los diferentes modos en los que el sistema puede operar,
 * permitiendo al usuario configurar la hora actual, la hora de la alarma,
 * o simplemente visualizar el tiempo.
 */

typedef enum {
    MODE_UNSET,
    MODE_HOME, // Modo de visualización del tiempo actual
    MODE_SET_TIME_MINUTES,
    MODE_SET_TIME_HOURS,
    MODE_SET_ALARM_MINUTES,
    MODE_SET_ALARM_HOURS,
    MODE_ALARM_TRIGGERED, // Modo alarma activada
} system_mode_t;

/**
 * @brief Estructura que representa el tiempo del reloj en formato BCD.
 *
 * La estructura contiene campos para los segundos, minutos y horas, cada uno representado
 * como un arreglo de dos bytes (unidades y decenas).
 *
 * @note Los valores se almacenan en formato BCD (Binary-Coded Decimal).
 * @param seconds Arreglo de dos bytes para los segundos (unidades y decenas).
 * @param minutes Arreglo de dos bytes para los minutos (unidades y decenas).
 * @param hours Arreglo de dos bytes para las horas (unidades y decenas).

 * */
typedef union {
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;
    uint8_t bcd[6];
} clock_time_t;

typedef struct clock_s * clock_t;

/*Constructor de reloj
 * @param ticks_per_second Frecuencia del reloj en ticks por segundo.
 * @return Un puntero a una instancia de reloj inicializada.
 */
clock_t ClockCreate(uint16_t ticks_per_second);

/**
 * @brief Obtiene la hora actual del reloj.
 *
 * @param clock Puntero al reloj del cual se desea obtener la hora.
 * @param result Puntero donde se almacenará la hora actual.
 * @return true si la operación fue exitosa, false en caso contrario.
 */
bool ClockGetTime(clock_t clock, clock_time_t * result);

/**
 * @brief Establece la hora del reloj.
 *
 * @param clock Puntero al reloj donde se desea establecer la hora.
 * @param new_time Puntero a la nueva hora que se desea establecer.
 * @return true si la operación fue exitosa, false si el tiempo es inválido o si el reloj es NULL.
 */
bool ClockSetTime(clock_t clock, const clock_time_t * new_time);

/**
 * @brief Establece la hora de la alarma en el reloj.
 *
 * @param clock Puntero al reloj donde se desea establecer la hora de la alarma.
 * @param alarm_time Puntero a la nueva hora de la alarma que se desea establecer.
 * @return true si la operación fue exitosa, false si el tiempo es inválido o si el reloj es NULL.
 */
bool ClockSetAlarmTime(clock_t clock, const clock_time_t * alarm_time);

/**
 * @brief Simula un tick del reloj, actualizando la hora y verificando si la alarma debe sonar.
 *
 * @param clock Puntero al reloj que se desea actualizar.
 */
void ClockNewTick(clock_t clock);

/**
 * @brief Obtiene la hora de la alarma del reloj.
 *
 * @param clock Puntero al reloj del cual se desea obtener la hora de la alarma.
 * @param alarm_time Puntero donde se almacenará la hora de la alarma.
 * @return true si la operación fue exitosa, false si el reloj es NULL o si alarm_time es NULL.
 */
bool ClockGetAlarmTime(clock_t clock, clock_time_t * alarm_time);

/**
 * @brief Habilita la alarma del reloj.
 *
 * @param clock Puntero al reloj donde se desea habilitar la alarma.
 */
void ClockEnableAlarm(clock_t clock);

/**
 * @brief Deshabilita la alarma del reloj.
 *
 * @param clock Puntero al reloj donde se desea deshabilitar la alarma.
 */
void ClockDisableAlarm(clock_t clock);

/**
 * @brief Verifica si la alarma del reloj está habilitada.
 *
 * @param clock Puntero al reloj que se desea verificar.
 * @return true si la alarma está habilitada, false en caso contrario.
 */
bool ClockIsAlarmEnabled(clock_t clock);

/**
 * @brief Verifica si la alarma del reloj está sonando.
 *
 * @param clock Puntero al reloj que se desea verificar.
 * @return true si la alarma está sonando, false en caso contrario.
 */
bool ClockIsAlarmTriggered(clock_t clock);

/**
 * @brief Compara dos tiempos del reloj para verificar si son iguales.
 *
 * @param a Puntero al primer tiempo a comparar.
 * @param b Puntero al segundo tiempo a comparar.
 * @return true si los tiempos son iguales, false en caso contrario.
 */
bool ClockTimesMatch(const clock_time_t * a, const clock_time_t * b);

/**
 * @brief Pospone la alarma del reloj por una cantidad específica de minutos.
 *
 * @param clock Puntero al reloj donde se desea posponer la alarma.
 * @param minutes_to_snooze Cantidad de minutos para posponer la alarma.
 * @return true si la operación fue exitosa, false si el reloj es NULL o si los minutos son inválidos.
 */
bool ClockSnoozeAlarm(clock_t self, uint8_t minutes_to_snooze);

/**
 * @brief Cancela la alarma del reloj hasta el próximo día.
 *
 * Esta función detiene el sonido actual de la alarma y desactiva la alarma pospuesta.
 *
 * @param self Puntero al reloj donde se desea cancelar la alarma.
 * @return true si la operación fue exitosa, false si el reloj es NULL o no es válido.
 */
bool ClockCancelAlarmUntilNextDay(clock_t self);
/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/* === End of conditional blocks =================================================================================== */
#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H_ */
