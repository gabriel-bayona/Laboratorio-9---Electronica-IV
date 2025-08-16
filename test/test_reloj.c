
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

/** @file test_reloj.c
 ** @brief
 **/

/* === Headers files inclusions ==================================================================================== */
#include "unity.h"
#include "clock.h"

/* === Private macros definitions ================================================================================ */
#define CLOCK_TICKS_PER_SECOND 5 // Frecuencia del reloj simulado en Hz
#define TEST_ASSERT_TIME(hours_tens, hours_units, minutes_tens, minutes_units, seconds_tens, seconds_units,            \
                         current_time)                                                                                 \
    clock_time_t current_time = {0};                                                                                   \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Failed to set time");                                \
    ClockGetTime(clock, &current_time);                                                                                \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, current_time.bcd[0], "Difference in units seconds");                \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, current_time.bcd[1], "Difference in tens seconds");                  \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, current_time.bcd[2], "Difference in units minutes");                \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, current_time.bcd[3], "Difference in tens minutes");                  \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units, current_time.bcd[4], "Difference in units hours");                    \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens, current_time.bcd[5], "Difference in tens hours")

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */
/**
 * @brief Simula el avance del reloj en segundos.
 * @param clock Puntero al reloj que se desea simular.
 * @param seconds Cantidad de segundos a simular.
 *
 * Esta función simula el avance del reloj en la cantidad de segundos especificada,
 * llamando a ClockNewTick para cada tick del reloj.
 */
static void SimulateSeconds(clock_t clock, uint32_t seconds);
/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static void SimulateSeconds(clock_t clock, uint32_t seconds) {
    for (uint32_t i = 0; i < CLOCK_TICKS_PER_SECOND * seconds; i++) {
        ClockNewTick(clock); // Simula un tic del reloj
    }
}
/* === Header for C++ compatibility ================================================================================
 */

/* === Public macros definitions ===================================================================================
 */

/* === Public data type declarations ===============================================================================
 */

/* === Public variable declarations ================================================================================
 */

/* === Public function declarations ================================================================================
 */
/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */
clock_t clock; // Variable global para el reloj
void setUp(void) {
    // Esta función se ejecuta antes de cada prueba
    clock = ClockCreate(CLOCK_TICKS_PER_SECOND); // Crea el reloj con la frecuencia especificada
}

//  Al inicializar el reloj está en 00:00 y con hora invalida.
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {
        .bcd = {1, 2, 3, 4, 5, 6},
    };

    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// ‣Al ajustar la hora el reloj con valores correctos queda en hora y es válida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_t new_time = {
        .time =
            {
                .seconds = {4, 5},
                .minutes = {3, 0},
                .hours = {5, 1},
            },
    };

    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TIME(1, 5, 0, 3, 5, 4, current_time);
}

// Después de n ciclos de reloj la hora avanza un segundo, diez segundos, un minuto, diez minutos, una hora, diez horas
// y un día completo.
void test_clock_advance_one_second(void) {
    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones

    // un segundo
    SimulateSeconds(clock, 1); // Simula un segundo
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1, current_time);
}
void test_clock_advance_ten_seconds(void) {

    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones
    SimulateSeconds(clock, 10);              // Simula diez segundos
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0, current_time);
}
void test_clock_advance_one_minute(void) {
    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones
    SimulateSeconds(clock, 60);              // Simula un minuto
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0, current_time);
}
void test_clock_advance_ten_minutes(void) {
    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones
    SimulateSeconds(clock, 600);             // Simula diez minutos
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0, current_time);
}
void test_clock_advance_one_hour(void) {
    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones
    SimulateSeconds(clock, 3600);            // Simula una hora
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0, current_time);
}
void test_clock_advance_ten_hours(void) {
    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones
    SimulateSeconds(clock, 36000);           // Simula diez horas
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0, current_time);
}
void test_clock_advance_one_day(void) {
    ClockSetTime(clock, &(clock_time_t){0}); // aca no se verificada nada, es parte de las precondiciones

    SimulateSeconds(clock, 86400); // Simula un día completo (24 horas)
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}

// Fijar la hora de la alarma y consultarla.
void test_set_and_get_alarm_time(void) {
    static const clock_time_t alarm_time = {
        .time =
            {
                .seconds = {0, 0},
                .minutes = {5, 1},
                .hours = {2, 0},
            },
    };

    clock_time_t retrieved = {0}; // la hora de la alarma recuperada desde el reloj.

    // Seteamos la alarma
    TEST_ASSERT_TRUE(ClockSetAlarmTime(clock, &alarm_time));

    // Llamo a ClockGetAlarmTime() y espero que me copie en retrieved la hora que antes guardé con ClockSetAlarmTime()
    TEST_ASSERT_TRUE(ClockGetAlarmTime(clock, &retrieved));

    // Verifico que la hora de la alarma recuperada sea igual a la seteada
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarm_time.bcd, retrieved.bcd, sizeof(clock_time_t));
}

// Fijar la alarma y avanzar el reloj para que suene.
void test_alarm_triggers_when_time_matches(void) {
    // Seteamos la hora actual
    static const clock_time_t target_time = {.time = {
                                                 .seconds = {0, 0},
                                                 .minutes = {1, 0},
                                                 .hours = {0, 0},
                                             }};

    TEST_ASSERT_TRUE(ClockSetTime(clock, &(clock_time_t){
                                             .time =
                                                 {
                                                     .seconds = {0, 0},
                                                     .minutes = {9, 5},
                                                     .hours = {3, 2},
                                                 }
                                             // 23:59:00 para probar rollover también
                                         }));

    TEST_ASSERT_TRUE(ClockSetAlarmTime(clock, &target_time));
    ClockEnableAlarm(clock);

    // Antes de simular, la alarma no debe estar activa
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));

    // Simular el tiempo hasta alcanzar 00:01:00
    SimulateSeconds(clock, 120); // Simula 2 minutos

    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock)); // Debería sonar
}

// Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
void test_alarm_not_triggers_when_time_matches(void) {
    // Seteamos la hora actual
    static const clock_time_t target_time = {.time = {
                                                 .seconds = {0, 0},
                                                 .minutes = {1, 0},
                                                 .hours = {0, 0},
                                             }};

    TEST_ASSERT_TRUE(ClockSetTime(clock, &(clock_time_t){
                                             .time =
                                                 {
                                                     .seconds = {0, 0},
                                                     .minutes = {9, 5},
                                                     .hours = {3, 2},
                                                 }
                                             // 23:59:00 para probar rollover también
                                         }));

    TEST_ASSERT_TRUE(ClockSetAlarmTime(clock, &target_time));
    ClockDisableAlarm(clock);                      // Aseguro que esté desactivada
    TEST_ASSERT_FALSE(ClockIsAlarmEnabled(clock)); // Verifico que la alarma esté deshabilitada

    // Simular el tiempo hasta alcanzar 00:01:00
    SimulateSeconds(clock, 120);                     // Simula 2 minutos
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock)); // Debería no sonar
}

// Hacer sonar la alarma y posponerla.
void test_snoozed_alarm_triggers_after_delay(void) {
    // Seteamos la hora actual
    static const clock_time_t target_time = {.time = {
                                                 .seconds = {0, 0},
                                                 .minutes = {1, 0},
                                                 .hours = {0, 0},
                                             }};

    TEST_ASSERT_TRUE(ClockSetTime(clock, &(clock_time_t){
                                             .time =
                                                 {
                                                     .seconds = {0, 0},
                                                     .minutes = {9, 5},
                                                     .hours = {3, 2},
                                                 }
                                             // 23:59:00 para probar rollover también
                                         }));

    TEST_ASSERT_TRUE(ClockSetAlarmTime(clock, &target_time));
    ClockEnableAlarm(clock);

    // Antes de simular, la alarma no debe estar activa
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));

    // Simular el tiempo hasta alcanzar 00:01:00
    SimulateSeconds(clock, 120); // Simula 2 minutos

    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock)); // Debería sonar

    // Posponer 2 minutos
    TEST_ASSERT_TRUE(ClockSnoozeAlarm(clock, 2));

    // Verificar que la alarma no esté sonando después de posponer
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock));

    // Simular hasta que suene el pospuesto
    SimulateSeconds(clock, 120);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

// Hacer sonar la alarma y cancelarla hasta el otro dia..
void test_alarm_can_be_cancelled_until_next_day(void) {
    // Configurar la alarma para sonar a las 00:01:00
    static const clock_time_t target_time = {.time = {
                                                 .seconds = {0, 0},
                                                 .minutes = {1, 0},
                                                 .hours = {0, 0},
                                             }};

    // Inicializamos el reloj a 23:59:00
    TEST_ASSERT_TRUE(ClockSetTime(clock, &(clock_time_t){.time = {
                                                             .seconds = {0, 0},
                                                             .minutes = {9, 5},
                                                             .hours = {3, 2},
                                                         }}));

    TEST_ASSERT_TRUE(ClockSetAlarmTime(clock, &target_time));
    ClockEnableAlarm(clock);

    // Simular 2 minutos (hasta las 00:01:00)
    SimulateSeconds(clock, 120);
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock)); // La alarma debería sonar

    // Cancelamos la alarma (detenemos su sonido)
    TEST_ASSERT_TRUE(ClockCancelAlarmUntilNextDay(clock));
    TEST_ASSERT_FALSE(ClockIsAlarmTriggered(clock)); // Ya no debería sonar

    // Simulamos otro día: 00:01:00 de nuevo
    clock_time_t zero_time = {0};
    TEST_ASSERT_TRUE(ClockSetTime(clock, &zero_time));
    SimulateSeconds(clock, 60);

    // Debería volver a sonar
    TEST_ASSERT_TRUE(ClockIsAlarmTriggered(clock));
}

//  Probar get_time y con NULL como argumento
void test_get_time_with_null_arguments(void) {
    // Test con reloj NULL
    clock_time_t time;
    TEST_ASSERT_FALSE(ClockGetTime(NULL, &time));

    // Test con puntero a tiempo NULL
    TEST_ASSERT_FALSE(ClockGetTime(clock, NULL));

    // Test con ambos NULL
    TEST_ASSERT_FALSE(ClockGetTime(NULL, NULL));
}

// Tratar de ajustar la hora con un valor inválido y que los rechace
void test_set_invalid_time_values(void) {
    // Caso 1: Segundos inválidos (unidades > 9)
    clock_time_t invalid_seconds_units = {.time = {.seconds = {10, 0}, .minutes = {0, 0}, .hours = {0, 0}}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_seconds_units));

    // Caso 2: Segundos inválidos (decenas > 5)
    clock_time_t invalid_seconds_tens = {.time = {.seconds = {0, 6}, .minutes = {0, 0}, .hours = {0, 0}}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_seconds_tens));

    // Caso 3: Minutos inválidos (unidades > 9)
    clock_time_t invalid_minutes_units = {.time = {.seconds = {0, 0}, .minutes = {10, 0}, .hours = {0, 0}}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_minutes_units));

    // Caso 4: Minutos inválidos (decenas > 5)
    clock_time_t invalid_minutes_tens = {.time = {.seconds = {0, 0}, .minutes = {0, 6}, .hours = {0, 0}}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_minutes_tens));

    // Caso 5: Horas inválidas (unidades > 9)
    clock_time_t invalid_hours_units = {.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {10, 0}}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_hours_units));

    // Caso 6: Horas inválidas (horas > 23)
    clock_time_t invalid_hours = {.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {5, 2}}}; // 25:00:00
    TEST_ASSERT_FALSE(ClockSetTime(clock, &invalid_hours));

    // Caso 7: Hora exactamente en 24:00:00
    clock_time_t midnight_time = {.time = {.seconds = {0, 0}, .minutes = {0, 0}, .hours = {4, 2}}}; // 24:00:00
    TEST_ASSERT_FALSE(ClockSetTime(clock, &midnight_time));
}

// Hacer una prueba con una frecuencia de reloj diferente.
void test_clock_with_different_frequency(void) {
    const uint8_t new_ticks_per_second = 10;
    clock_t fast_clock = ClockCreate(new_ticks_per_second);

    // Seteo la hora a 00:00:00
    clock_time_t initial_time = {.time = {{0, 0}, {0, 0}, {0, 0}}};
    TEST_ASSERT_TRUE(ClockSetTime(fast_clock, &initial_time));

    // Simulo 10 tics, que con 10 Hz equivale a 1 segundo
    for (uint8_t i = 0; i < new_ticks_per_second; i++) {
        ClockNewTick(fast_clock);
    }

    clock_time_t result;
    TEST_ASSERT_TRUE(ClockGetTime(fast_clock, &result));

    // Debería haber avanzado 1 segundo → 00:00:01
    TEST_ASSERT_EQUAL_UINT8(1, result.time.seconds[0]);
    TEST_ASSERT_EQUAL_UINT8(0, result.time.seconds[1]);
    TEST_ASSERT_EQUAL_UINT8(0, result.time.minutes[0]);
    TEST_ASSERT_EQUAL_UINT8(0, result.time.minutes[1]);
    TEST_ASSERT_EQUAL_UINT8(0, result.time.hours[0]);
    TEST_ASSERT_EQUAL_UINT8(0, result.time.hours[1]);
}

/* === End of conditional blocks =================================================================================== */
