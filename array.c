#include "array.h"

void initArray(Array *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(ASTNode *));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, ASTNode* node) {
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(ASTNode *));
  }
  a->array[a->used++] = node;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

