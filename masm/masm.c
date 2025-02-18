#include <stdio.h>
#include <stdlib.h>

static const char* int_format = "%ld ";
static const char* empty_str = "";

static long data[] = {4, 8, 15, 16, 23, 42};
static size_t data_length = sizeof(data) / sizeof(long);

static void print_int(long value) {
    printf(int_format, value);
    fflush(NULL);
}

static int p(long value) {
    return value & 1;
}

static void* add_element(long value, void* next) {
    void* node = malloc(sizeof(data));
    if (node == NULL) {
        abort();
    }
    *(long*)node = value;
    *(void**)((char*)node + 8) = next;
    return node;
}

static void m(void* list, void (*f)(long)) {
    if (list != NULL) {
        long value = *(long*)list;
        void* next = *(void**)((char*)list + 8);
        f(value);
        m(next, f);
    }
}

static void f(void* list, long* result, int (*p)(long)) {
    *result = (long)list;
    if (list != NULL) {
        long value = *(long*)list;
        void* next = *(void**)((char*)list + 8);
        if (p(value)) {
            list = add_element(value, list);
        }
        f(next, result, p);
    }
}

int main() {
    void* list = NULL;
    for (size_t i = 0; i < data_length; i++) {
        list = add_element(data[i], list);
    }
    m(list, print_int);
    puts(empty_str);
    long result = 0;
    f(list, &result, p);
    m(&result, print_int);
    puts(empty_str);
    return 0;
}
