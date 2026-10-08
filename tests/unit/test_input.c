/* test_input.c — проверка очереди событий. */

#include <stdio.h>
#include <stdlib.h>
#include "core.h"

extern void core_input_push(const core_event_t *ev);

#define ASSERT(cond, msg)                                             \
    do {                                                              \
        if (!(cond)) {                                                \
            fprintf(stderr, "FAIL %s:%d: %s\n",                       \
                    __FILE__, __LINE__, msg);                         \
            exit(1);                                                  \
        }                                                             \
    } while (0)

static void test_push_poll(void) {
    /* Очередь может содержать остатки от предыдущих тестов,
     * поэтому выгребаем всё до конца. */
    while (core_input_poll().type != CORE_EVENT_NONE) {}

    core_event_t ev1 = { CORE_EVENT_KEY_UP, 0, 0 };
    core_event_t ev2 = { CORE_EVENT_KEY_DOWN, 5, 5 };
    core_input_push(&ev1);
    core_input_push(&ev2);

    core_event_t out1 = core_input_poll();
    ASSERT(out1.type == CORE_EVENT_KEY_UP, "первое событие");

    core_event_t out2 = core_input_poll();
    ASSERT(out2.type == CORE_EVENT_KEY_DOWN, "второе событие");
    ASSERT(out2.x == 5 && out2.y == 5, "координаты второго события");

    core_event_t empty = core_input_poll();
    ASSERT(empty.type == CORE_EVENT_NONE, "очередь пуста");
}

void test_input_run(void) {
    test_push_poll();
    printf("  [OK] input\n");
}