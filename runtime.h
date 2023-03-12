#ifndef GARBAGECOLLECTOR_RUNTIME_H
#define GARBAGECOLLECTOR_RUNTIME_H

#include "main.h"

#define ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(NAME, TYPE_ARG) \
lisp__object* NAME (TYPE_ARG a)

#define EXTERN_GENERATOR(NAME) \
extern lisp__object * NAME
EXTERN_GENERATOR(lisp__eq);
EXTERN_GENERATOR(lisp__sub);
EXTERN_GENERATOR(lisp__mul);
EXTERN_GENERATOR(lisp__add);
EXTERN_GENERATOR(lisp__div);

EXTERN_GENERATOR(lisp__and);
EXTERN_GENERATOR(lisp__or);
EXTERN_GENERATOR(lisp__not);

EXTERN_GENERATOR(lisp__head);
EXTERN_GENERATOR(lisp__tail);
EXTERN_GENERATOR(lisp__append);
EXTERN_GENERATOR(lisp__size);

lisp__object *lisp__list_constructor();
bool lisp__is_true(lisp__object *a);

void runtime__init();

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__int_constructor, int);

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__float_constructor, float);

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__bool_constructor, bool);

ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(lisp__char_constructor, char);

lisp__object *lisp__callable_constructor(void *function, int n, void *clojure);

lisp__object *lisp__list_head(void *clojure, lisp__object *object);
lisp__object *lisp__list_tail(void *clojure, lisp__object *object);
lisp__object *lisp__list_append(void *clojure, lisp__object *list, lisp__object *object);
lisp__object *lisp__list_size(void *clojure, lisp__object *object);

#endif //GARBAGECOLLECTOR_RUNTIME_H
