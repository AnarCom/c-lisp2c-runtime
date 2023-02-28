#include "runtime.h"
#include "main.h"
#include "GarbageCollector.h"

// type constructors
#define ALLOCATE_AND_CREATE_OBJ_TEMPLATE(NAME, TYPE_ARG, TYPE_OBJ, UNION_NAME) \
ALLOCATE_AND_CREATE_OBJ_TEMPLATE_DEF(NAME, TYPE_ARG) {                         \
    lisp__object* n = gc__new_object();                                        \
    n->type = TYPE_OBJ;                                                        \
    n->value. UNION_NAME = a;                                                  \
    return n;                                                                  \
}

#define SECURED_TYPES(_POINTER_NAME_)                                   \
if(_POINTER_NAME_ ->type != INT && _POINTER_NAME_ -> type != FLOAT) {   \
    printf("NOT_SUPPORTED_TYPE");                                       \
    exit(-2);                                                           \
    }

#define GET_NUMBER_VALUE(obj) \
(obj->type == INT ? (float) obj->value.i : (obj->type == FLOAT ? obj->value.f : -1))


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

// math operations: +, -, /, -

#define MATH_OPERATION_TEMPLATE(NAME, OPERATION)                        \
lisp__object * NAME (void * clojure, lisp__object *a, lisp__object *b) {\
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

MATH_OPERATION_TEMPLATE(lisp__add_operation, +)

MATH_OPERATION_TEMPLATE(lisp__mult_operation, *)

MATH_OPERATION_TEMPLATE(lisp__sub_operation, -)

MATH_OPERATION_TEMPLATE(lisp__div_operation, /)

bool lisp__is_true(lisp__object *a) {
    if (a->type != BOOL) {
        return false;
    }
    return a->value.b;
}

lisp__object *lisp__sub = NULL;
lisp__object *lisp__mul = NULL;
lisp__object *lisp__add = NULL;
lisp__object *lisp__div = NULL;

// boolean operations: &&, ||, !

lisp__object *lisp__and = NULL;
lisp__object *lisp__or = NULL;
lisp__object *lisp__not = NULL;

#define CHECK_IS_BOOL(NAME)                             \
if(NAME->type != BOOL) {                                \
    printf("TYPE OF LOGICAL OPERATOR IS NOT NULL\n");   \
    return NULL;                                        \
}


#define BOOLEAN_OPERATION_TEMPLATE(NAME, OPERATION)                                     \
lisp__object * NAME(void *clojure, lisp__object* first, lisp__object* second) {         \
CHECK_IS_BOOL(first);                                                                   \
CHECK_IS_BOOL(second);                                                                  \
first = gc__inc_ref_counter(first);                                                     \
second = gc__inc_ref_counter(second);                                                   \
lisp__object* ans = lisp__bool_constructor(first->value.b OPERATION second->value.b);   \
gc__dec_ref_counter(first);                                                             \
gc__dec_ref_counter(second);                                                            \
return gc__dec_ref_counter(ans);                                                        \
}

BOOLEAN_OPERATION_TEMPLATE(lisp__and_operation, &&)

BOOLEAN_OPERATION_TEMPLATE(lisp__or_operation, ||)

lisp__object *lisp__not_operation(void *clojure, lisp__object *first) {
    CHECK_IS_BOOL(first);
    gc__inc_ref_counter(first);
    lisp__object *ans = lisp__bool_constructor(first->value.b);
    gc__dec_ref_counter(first);
    return ans;
}

// comparation operations >=, >, <, <=, ==
lisp__object *lisp__eq = NULL;
#define COMPARATION_OPERATION_TEMPLATE(NAME, OPERATION)                 \
lisp__object* NAME (void * clojure, lisp__object *a, lisp__object *b) { \
    gc__inc_ref_counter(a);                                             \
    gc__inc_ref_counter(b);                                             \
    lisp__object *ans = NULL;                                           \
    if (a->type != b->type) {                                           \
        ans = lisp__bool_constructor(false);                            \
    } else if(a -> type == INT){                                        \
        return lisp__bool_constructor(a->value.i == b->value.i);        \
    }                                                                   \
    printf("NOT IMPLEMENTED");                                          \
    return NULL;                                                        \
}

COMPARATION_OPERATION_TEMPLATE(lisp__equal_operation, ==)

#define LISP_INIT_RUNTIME_TEMPLATE(VARIABLE_NAME, C_FUNCT, N) \
VARIABLE_NAME = lisp__callable_constructor(C_FUNCT, N, NULL)

void runtime__init() {
    gc__init();
    LISP_INIT_RUNTIME_TEMPLATE(lisp__eq, lisp__equal_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__mul, lisp__mult_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__sub , lisp__sub_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__add, lisp__add_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__div, lisp__div_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__or, lisp__or_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__and, lisp__and_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__not, lisp__not_operation, 1);
//    lisp__eq = lisp__callable_constructor(lisp__equal_operation, 2, NULL);
//    lisp__mul = lisp__callable_constructor(lisp__mult_operation, 2, NULL);
//    lisp__sub = lisp__callable_constructor(lisp__sub_operation, 2, NULL);
//    lisp__add = lisp__callable_constructor(lisp__add_operation, 2, NULL);
//    lisp__div = lisp__callable_constructor(lisp__div_operation, 2, NULL);
//    lisp__or = lisp__callable_constructor(lisp__or_operation, 2, NULL);
//    lisp__and = lisp__callable_constructor(lisp__and_operation, 2, NULL);
//    lisp__not = lisp__callable_constructor(lisp__not_operation, 1, NULL);
}
