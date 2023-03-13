#ifndef GARBAGECOLLECTOR_MAIN_H
#define GARBAGECOLLECTOR_MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum lisp__type {
    NUL,
    INT,
    FLOAT,
    CHAR,
    LIST,
    VOID,
    BOOL,
    CALLABLE
} lisp__type;

struct lisp__object;
typedef struct lisp__object lisp__object;
typedef enum lisp__type lisp__type;

struct lisp__list {
    size_t size;
    lisp__object **list;
};
typedef struct lisp__list lisp__list;

typedef struct {
    int n_args;
    void *f;
    void *clojure;
} lisp__callable;

struct lisp__object {
    lisp__type type;
    union {
        int i;
        float f;
        char c;
        lisp__list l;
        bool b;
        lisp__callable callable;
    } value;
    int ref_count;
};



#endif //GARBAGECOLLECTOR_MAIN_H
