#include "runtime.h"
#include "main.h"
#include "GarbageCollector.h"

#define ALLOCATE_AND_CREATE_OBJ_TEMPLATE(NAME, TYPE_ARG, TYPE_OBJ, UNION_NAME) \
ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(NAME, TYPE_ARG) {                         \
    lisp__object* n = gc__new_object();                   \
    n->type = TYPE_OBJ;                                   \
    n->value. UNION_NAME = a;                             \
    return n;                                             \
}

#define SECURED_TYPES(_POINTER_NAME_)                                   \
if(_POINTER_NAME_ ->type != INT && _POINTER_NAME_ -> type != FLOAT) {   \
    printf("NOT_SUPPORTED_TYPE");                                       \
    exit(-2);                                                           \
    }

#define GET_NUMBER_VALUE(obj) \
(obj->type == INT ? obj->value.i : (obj->type == FLOAT ? obj->value.f : -1))


#define MATH_OPERATION_TEMPLATE(NAME, OPERATION)                        \
lisp__object * NAME (void * clojure, lisp__object *a, lisp__object *b) {                \
   SECURED_TYPES(a)                                                     \
   SECURED_TYPES(b)                                                     \
   lisp__object *r = gc__new_object();                                  \
   if(a->type == INT && b -> type == INT) {                             \
        r->type = INT;                                                  \
        r->value.i = a->value.i OPERATION b->value.i;                   \
   } else {                                                             \
        r->type = FLOAT;                                                \
        r->value.f = GET_NUMBER_VALUE(a) OPERATION GET_NUMBER_VALUE(b); \
   }                                                                    \
   return r;                                                            \
}

ALLOCATE_AND_CREATE_OBJ_TEMPLATE(lisp__int_constructor, int, INT, i)

ALLOCATE_AND_CREATE_OBJ_TEMPLATE(lisp__float_constructor, float, FLOAT, f)

ALLOCATE_AND_CREATE_OBJ_TEMPLATE(lisp__bool_constructor, bool, BOOL, b)

ALLOCATE_AND_CREATE_OBJ_TEMPLATE(lisp__char_constructor, char, CHAR, c)

lisp__object *lisp__callable_constructor(void *function, int n, void *clojure) {
    lisp__object *t = gc__new_object();
    t->type = CALLABLE;
    t->value.callable.f = function;
    t->value.callable.n_args = n;
    t->value.callable.clojure = clojure;
    return t;
}


MATH_OPERATION_TEMPLATE(lisp__add_operation, +)

MATH_OPERATION_TEMPLATE(lisp__mult_operation, *)

MATH_OPERATION_TEMPLATE(lisp__sub_operation, -)

MATH_OPERATION_TEMPLATE(lisp__div_operation, /)

#define COMPARATION_OPERATION_TEMPLATE(NAME, OPERATION)                 \
lisp__object* NAME (void * clojure, lisp__object *a, lisp__object *b) { \
                                                                        \
    if(a->type != b->type){                                             \
    return lisp__bool_constructor(false);                               \
    } else if(a -> type == INT){                                        \
        return lisp__bool_constructor(a->value.i == b->value.i);        \
    }                                                                   \
    printf("NOT IMPLEMENTED");                                          \
    return NULL;                                                        \
}

COMPARATION_OPERATION_TEMPLATE(lisp__equal_operation, ==)

bool lisp__is_true(lisp__object *a) {
    if (a->type != BOOL) {
        return false;
    }
    return a->value.b;
}


lisp__object *lisp__eq = NULL;
lisp__object *lisp__sub = NULL;
lisp__object *lisp__mul = NULL;

void runtime__init() {
    gc__init();
    lisp__eq = lisp__callable_constructor(lisp__equal_operation, 2, NULL);
    lisp__mul = lisp__callable_constructor(lisp__mult_operation, 2, NULL);
    lisp__sub = lisp__callable_constructor(lisp__sub_operation, 2, NULL);
}
