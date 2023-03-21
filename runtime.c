#include "runtime.h"
#include "main.h"
#include "GarbageCollector.h"
#include <assert.h>
#include <string.h>
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


lisp__object *lisp__null_constructor() {
    lisp__object *o = gc__new_object();
    o->type = NUL;
    return o;
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
   gc__dec_ref_counter(a);                                              \
   gc__dec_ref_counter(b);                                              \
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
    lisp__object *ans = lisp__bool_constructor(!first->value.b);
    gc__dec_ref_counter(first);
    return ans;
}

// comparation operations >=, >, <, <=, ==
lisp__object *lisp__eq = NULL;
#define COMPARATION_OPERATION_TEMPLATE(NAME, OPERATION)                 \
lisp__object* NAME (void * clojure, lisp__object *a, lisp__object *b) { \
    lisp__object *ans = NULL;                                           \
    if (a->type != b->type) {                                           \
        ans = lisp__bool_constructor(false);                            \
    } else if(a -> type == INT){                                        \
        ans = lisp__bool_constructor(a->value.i == b->value.i);        \
    } else if (a -> type == CHAR){                                      \
        ans = lisp__bool_constructor(a->value.c == b->value.c);         \
    } else {                                                            \
    assert(false);                                                      \
    }                                                                   \
                                                                        \
    gc__dec_ref_counter(a);                                             \
    gc__dec_ref_counter(b);                                             \
    return ans;                                                        \
}

COMPARATION_OPERATION_TEMPLATE(lisp__equal_operation, ==)

#define LISP_INIT_RUNTIME_TEMPLATE(VARIABLE_NAME, C_FUNCT, N) \
VARIABLE_NAME = lisp__callable_constructor(C_FUNCT, N, NULL)

// List block: head, tail, append, constructor, size
lisp__object *lisp__list_constructor() {
    lisp__object *list = gc__new_object();
    list->type = LIST;
    list->value.l.size = 0;
    list->value.l.list = NULL;
    return list;
}

#define CHECK_THAT_LIST(obj) \
    assert(obj->type == LIST)

lisp__object *lisp__head = NULL;

lisp__object *lisp__list_head(void *clojure, lisp__object *object) {
    CHECK_THAT_LIST(object);
//    if (object->value.l.size == 0) {
//        return lisp__null_constructor();
//    }
    assert(object->value.l.size != 0);

    gc__dec_ref_counter(object);
    return gc__inc_ref_counter(object->value.l.list[0]);
}

lisp__object *lisp__tail = NULL;

lisp__object *lisp__list_tail(void *clojure, lisp__object *object) {
    CHECK_THAT_LIST(object);
//    assert(object->value.l.size != 0);
//TODO: if 0 size ?
        if (object->value.l.size == 0) {
            return lisp__list_constructor();
        }
//    lisp__object *nl = lisp__list_constructor();
//    nl->value.l.size = object->value.l.size - 1;
//    nl->value.l.list = calloc(nl->value.l.size, sizeof(lisp__object));
//    for (int i = 1; i < (int) (object->value.l.size); i++) {
//        memcpy(&(nl->value.l.list[i - 1]), &(object->value.l.list[i]), sizeof(lisp__object));
//    }
//    return nl;
    lisp__object *lst = lisp__list_constructor();
    lst->value.l.size = object->value.l.size - 1;
    lst->value.l.list = calloc(lst->value.l.size, sizeof(lisp__object *));
    for (size_t i = 1; i < object->value.l.size; i++) {
        lst->value.l.list[i-1] = gc__inc_ref_counter(object->value.l.list[i]);
    }
    gc__dec_ref_counter(object);
    return lst;
}

lisp__object *lisp__append = NULL;

lisp__object *lisp__list_append(void *clojure, lisp__object *list, lisp__object *object) {
    CHECK_THAT_LIST(list);
//    lisp__object *nl = lisp__list_constructor();
//    nl->value.l.size = list->value.l.size + 1;
//    nl->value.l.list = calloc(nl->value.l.size, sizeof(lisp__object));
//    memcpy(nl->value.l.list, list->value.l.list, list->value.l.size * sizeof(lisp__object));
//    memcpy(&(nl->value.l.list[nl->value.l.size - 1]), object, sizeof(lisp__object));
    lisp__object *nl = lisp__list_constructor();
    nl->value.l.size = list->value.l.size + 1;
    nl->value.l.list = calloc(nl->value.l.size, sizeof(lisp__object *));
    for (size_t i = 0; i < list->value.l.size; i++) {
        nl->value.l.list[i] = gc__inc_ref_counter(list->value.l.list[i]);
    }
    nl->value.l.list[nl->value.l.size - 1] = gc__inc_ref_counter(object);
    gc__dec_ref_counter(list);
    gc__dec_ref_counter(object);
    return nl;
}

lisp__object *lisp__size = NULL;

lisp__object *lisp__list_size(void *clojure, lisp__object *object) {
    CHECK_THAT_LIST(object);
    gc__dec_ref_counter(object);
    return lisp__int_constructor((int) object->value.l.size);
}

lisp__object *lisp__print = NULL;
static void lisp__print_elem(lisp__object* obj, bool end_string);
lisp__object *lisp__print_function(void* clojure, lisp__object* obj){
        lisp__print_elem(obj, true);
    return lisp__null_constructor();
}
static void lisp__print_elem(lisp__object* obj, bool end_string){
    switch (obj->type) {
        case INT:
            printf("%d", obj->value.i);
            break;
        case FLOAT:
            printf("%f", obj->value.f);
            break;
        case BOOL:
            printf("%s", obj->value.b ? "TRUE" : "FALSE");
            break;
        case NUL:
            printf("NULL");
            break;
        case CHAR:
            printf("'%c'", obj->value.c);
            break;
        case LIST:

            printf("[");
            for(size_t i = 0; i < obj->value.l.size; i++){
                lisp__print_elem(obj->value.l.list[i], false);
                printf(", ");
            }
            printf("]");
            break;
        case VOID:
            printf("VOID");
            break;
        case CALLABLE:
            printf("callable");
            break;
    }
    if(end_string){
        printf("\n");
    }
}

void runtime__init() {
    gc__init();
    LISP_INIT_RUNTIME_TEMPLATE(lisp__eq, lisp__equal_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__mul, lisp__mult_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__sub, lisp__sub_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__add, lisp__add_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__div, lisp__div_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__or, lisp__or_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__and, lisp__and_operation, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__not, lisp__not_operation, 1);

    LISP_INIT_RUNTIME_TEMPLATE(lisp__head, lisp__list_head, 1);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__tail, lisp__list_tail, 1);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__append, lisp__list_append, 2);
    LISP_INIT_RUNTIME_TEMPLATE(lisp__size, lisp__list_size, 1);

    LISP_INIT_RUNTIME_TEMPLATE(lisp__print, lisp__print_function, 1);
}
