/*
 * input.c — платформо-независимая очередь событий ввода.
 */

#include "core.h"

#define EVENT_QUEUE_CAPACITY 256

static core_event_t g_queue[EVENT_QUEUE_CAPACITY];
static int g_head = 0;
static int g_tail = 0;

void core_input_push(const core_event_t *ev) {
    if (!ev) return;
    int next = (g_tail + 1) % EVENT_QUEUE_CAPACITY;
    if (next == g_head) {
        /* Очередь переполнена — отбрасываем событие. */
        return;
    }
    g_queue[g_tail] = *ev;
    g_tail = next;
}

core_event_t core_input_poll(void) {
    core_event_t empty = { CORE_EVENT_NONE, 0, 0 };
    if (g_head == g_tail) {
        return empty;
    }
    core_event_t ev = g_queue[g_head];
    g_head = (g_head + 1) % EVENT_QUEUE_CAPACITY;
    return ev;
}