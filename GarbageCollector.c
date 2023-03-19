#include "GarbageCollector.h"
#include <stdlib.h>
#include <string.h>

static int used_objects = 0;

#define DEBUG_LOG(__STRING__) printf(__STRING__)
#define MIN(A, B) (A < B ? A : B)

void gc__init() {}

void gc__destruct() {}

lisp__object *gc__new_object() {
    lisp__object *o = calloc(1, sizeof(lisp__object));
    if (o == NULL) {
        DEBUG_LOG("allocated NULL");
    }
    return gc__inc_ref_counter(o);
}

lisp__object *gc__inc_ref_counter(lisp__object *object) {
    object->ref_count++;
    return object;
}

lisp__object *gc__dec_ref_counter(lisp__object *object) {
    object->ref_count--;
    if (object->ref_count == 0) {
        if (object->type == LIST) {
            for (size_t i = 0; i < object->value.l.size; i++) {
                gc__dec_ref_counter(object->value.l.list[i]);
            }
            free(object->value.l.list);
        }else if (object->type == CALLABLE && object->value.callable.clojure != NULL) {
            lisp__object** clj = object->value.callable.clojure;
            size_t clojure_size = *(size_t *)clj;
            for(size_t i = 0; i < clojure_size; i++){
                gc__dec_ref_counter(clj[i+1]);
            }
            free(clj);
        }
        free(object);
        return NULL;
    }
    return object;
}
