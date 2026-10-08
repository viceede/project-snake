/* test_main.c — минимальный раннер модульных тестов. */

#include <stdio.h>
#include <stdlib.h>

extern void test_renderer_run(void);
extern void test_input_run(void);
extern void test_api_run(void);

int main(void) {
    printf("=== Модульные тесты ядра ===\n");
    test_renderer_run();
    test_input_run();
    test_api_run();
    printf("Все модульные тесты пройдены успешно.\n");
    return 0;
}