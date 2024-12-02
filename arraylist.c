#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"

void al_init(arraylist_t *L, unsigned cap)
{
    L->length = 0;
    L->capacity = cap;
    L->data = malloc(sizeof(int) * cap);
}


void al_destroy(arraylist_t *L)
{
    free(L->data);
}

void al_append(arraylist_t *L, int item)
{
    if (L->length == L->capacity) {
	L->capacity *= 2;
	// printf("doubling capacity to %d\n", L->capacity);
	L->data = realloc(L->data, L->capacity * sizeof(int));
    }

    L->data[L->length] = item;
    L->length++;

}
