/*
 * fuzz_input_parser.c — фаззинг обработчика очереди событий.
 *
 * Подаём байты как поток событий и убеждаемся, что очередь
 * корректно обрабатывает произвольные входные данные.
 */

#include <stdint.h>
#include <stddef.h>
#include "core.h"

extern void core_input_push(const core_event_t *ev);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    for (size_t i = 0; i + 2 < size; i += 3) {
        core_event_t ev;
        ev.type = (core_event_type_t)(data[i] % 14);
        ev.x    = (int8_t)data[i + 1];
        ev.y    = (int8_t)data[i + 2];
        core_input_push(&ev);
    }
    while (core_input_poll().type != CORE_EVENT_NONE) {}
    return 0;
}