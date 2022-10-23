#ifndef ARRAY_H
#define ARRAY_H

#include "parse.h"
#include <stdlib.h>

struct Array {
  ASTNode **array;
  size_t used;
  size_t size;
};

void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, ASTNode *node);
void freeArray(Array *a);

#endif

