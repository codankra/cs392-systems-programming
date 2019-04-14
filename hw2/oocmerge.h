#ifndef OOCMERGE_H
#define OOCMERGE_H

#include <stdio.h>

int verifySort(FILE* input, FILE* output);

FILE* merge(FILE* arr1, FILE* arr2);

FILE* mSplit(FILE* list, int first, int last);

#endif