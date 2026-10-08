/*
 * timing.c — таймеры и PRNG.
 *
 * Особенность строгого режима C11 (-std=c11, без GNU-расширений):
 * POSIX-функции clock_gettime() и nanosleep() скрыты, пока не
 * определён макрос _POSIX_C_SOURCE. Значение 200809L соответствует
 * POSIX.1-2008, где эти функции и тип struct timespec уже
 * стандартизированы.
 *
 * ВАЖНО: макрос должен быть определён ДО первого #include,
 * иначе <features.h>, подтянутый первым системным заголовком,
 * зафиксирует значение по умолчанию и объявления останутся
 * недоступными.
 */
#define _POSIX_C_SOURCE 200809L

#include "core.h"

#include <time.h>
#include <stdlib.h>

/*
 * Возвращает монотонное время в миллисекундах.
 * CLOCK_MONOTONIC не подвержен скачкам системных часов,
 * поэтому подходит для измерения интервалов между кадрами.
 */
uint64_t core_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

/*
 * Приостанавливает выполнение текущего потока на ms миллисекунд.
 * nanosleep() может быть прерван сигналом — в этом случае он
 * возвращает -1 и заполняет оставшееся время в ts. Для игрового
 * цикла это не критично, поэтому повтор не выполняем.
 */
void core_sleep_ms(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec  = (time_t)(ms / 1000);
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

/*
 * Инициализация генератора псевдослучайных чисел.
 * Вызывается один раз при старте приложения.
 */
void core_random_seed(uint32_t seed) {
    srand((unsigned)seed);
}

/*
 * Возвращает целое число в диапазоне [0, n).
 * При n <= 0 возвращает 0 (защита от деления по модулю нуля).
 */
int core_random_int(int n) {
    if (n <= 0) return 0;
    return rand() % n;
}