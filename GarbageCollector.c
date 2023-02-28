#include "GarbageCollector.h"
#include <stdlib.h>
#include <string.h>

#ifndef MAX_HEAP_SIZE
#define MAX_HEAP_SIZE 1000
#endif

#ifndef HEAP_ALLOCATION_STEP_SIZE
#define HEAP_ALLOCATION_STEP_SIZE 100
#endif

#define DEBUG_LOG(__STRING__) printf(__STRING__)
#define MIN(A, B) (A < B ? A : B)

typedef struct gc__node {
    lisp__object *obj;
    struct gc__node *next;
} gc__node;

typedef struct gc__list {
    int size;
    gc__node *next;
} gc__list;

gc__list lisp__object_free = {0};
gc__list lisp__object_used = {0};


static void allocate_one(gc__list *list) {
    lisp__object *obj = calloc(1, sizeof(lisp__object));
    gc__node *node = calloc(1, sizeof(gc__node));
    //TODO: null check
    node->obj = obj;
    obj->type = NUL;
    gc__node *old_head = list->next;
    list->next = node;
    node->next = old_head;
    list->size++;
}

static void allocate_batch(gc__list *list, int batch_size) {
    for (int i = 0; i < batch_size; i++) {
        allocate_one(list);
    }
}

static gc__node *gc__list_get_first(gc__list *list) {
    gc__node *r = list->next;
    list->next = r->next;
    r->next = NULL;
    list->size--;
    return r;
}

static void gc__list_add_element(gc__list *list, gc__node *node) {
    gc__node *old_first = list->next;
    node->next = old_first;
    list->next = node;
    list->size++;
}

void gc__init() {
    allocate_batch(&lisp__object_free, HEAP_ALLOCATION_STEP_SIZE);
}

lisp__object *gc__inc_ref_counter(lisp__object *object) {
    object->ref_count++;
    return object;
}

lisp__object *gc__dec_ref_counter(lisp__object *object) {
    object->ref_count--;
    if (object->ref_count < 0) {
        DEBUG_LOG("REFERENCE COUNTER BECAME LESS THAN 0\n");
    }
    return object;
}

static void gc__clear_lisp_object(gc__node *node) {
    if (node->obj->type == LIST) {
        for (size_t i = 0; i < node->obj->value.l.size; i++) {
            gc__dec_ref_counter(&(node->obj->value.l.list[i]));
        }
        free(node->obj->value.l.list);
        memset(&node->obj, 0x00, sizeof(lisp__object));
    }
    node->obj->type = NUL;
}

static void gc__delete_first_garbage(gc__list *used, gc__list *fr) {
    while (used->size > 0 && used->next->obj->ref_count == 0) {
        gc__node *a = used->next;
        used->next = a->next;
        used->size--;
        gc__clear_lisp_object(a);
        gc__list_add_element(fr, a);
    }
}

static void gc__delete_body_garbage(gc__list *used, gc__list *fr) {

}

static void gc__delete_garbage(gc__list *used, gc__list *fr) {
    gc__delete_first_garbage(used, fr);
    gc__delete_body_garbage(used, fr);
}

lisp__object *gc__new_object() {
    if (lisp__object_free.size == 0) {
        gc__delete_garbage(&lisp__object_used, &lisp__object_free);
        //TODO: check for need an allocation
        return NULL;
    } else {
        gc__node *node = gc__list_get_first(&lisp__object_free);
        gc__list_add_element(&lisp__object_used, node);
        return gc__inc_ref_counter(node->obj);
    }
}

void test() {
//    allocate_batch(&lisp__object_free, 1000);
}
