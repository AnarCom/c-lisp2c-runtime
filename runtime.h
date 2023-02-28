#ifndef GARBAGECOLLECTOR_RUNTIME_H
#define GARBAGECOLLECTOR_RUNTIME_H

#include "main.h"

#define ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(NAME, TYPE_ARG) \
lisp__object* NAME (TYPE_ARG a)
extern lisp__object *lisp__eq;
extern lisp__object *lisp__sub;
extern lisp__object *lisp__mul;

bool lisp__is_true(lisp__object *a);

void runtime__init();

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__int_constructor, int);

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__float_constructor, float);

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__bool_constructor, bool);

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__char_constructor, char);

lisp__object *lisp__callable_constructor(void *function, int n, void *clojure);
#endif //GARBAGECOLLECTOR_RUNTIME_H
