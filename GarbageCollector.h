#ifndef GARBAGECOLLECTOR_GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_GARBAGECOLLECTOR_H

#include "main.h"

void gc__init();

void gc__destruct();

lisp__object * gc__new_object();

lisp__object * gc__inc_ref_counter(lisp__object * object);
lisp__object * gc__dec_ref_counter(lisp__object * object);

void test();
#endif //GARBAGECOLLECTOR_GARBAGECOLLECTOR_H
