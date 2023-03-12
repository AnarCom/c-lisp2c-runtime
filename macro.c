#include "macro.h"

void lisp__print_serialized_form(lisp__object *form){
     lisp__list *formList = &(form->value.l);
     int isList = formList->list[0].value.b;

    if(isList){
        int bracketType = formList->list[1].value.b;
        putchar(bracketType ? '[' : '(');
        for(int i = 0; i < form->value.l.list[2].value.l.size; i++){
            lisp__print_serialized_form(&(form->value.l.list[2].value.l.list[i]));
            putchar(' ');
        }
        putchar(bracketType ? ']' : ')');
    }else{
        lisp__list *string = &(form->value.l.list[2].value.l);
        for(int i = 0; i < string->size; i++){
            putchar(string->list[i].value.c);
        }
    }
}