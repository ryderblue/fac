#include "arrayset.h"

#include <error.h>
#include <string.h>

int find_in_arrayset(const arrayset *array, const char *path) {
  int len = strlen(path);
  const char *a = array->a;
  int size = array->size;

  while (size > len) {
    for (int i=0;i<len+1;i++) {
      if (a[i] == 0) {
        if (i == len) return a - array->a;
        size -= i+1;
        a += i+1;
        break;
      }
      if (a[i] != path[i]) {
        int alen = strlen(a+i) + 1 + i;
        size -= alen;
        a += alen;
        break;
      }
    }
  }
  return -1;
}

void insert_to_arrayset(arrayset *array, const char *path) {
  if (is_in_arrayset(array, path)) return;
  int len = strlen(path);
  if (array->size + len + 1 > ARRAY_MAX)
    error(1,0,"NOT ENOUGH ROOM IN ARRAY!");
  strcpy(&array->a[array->size], path);
  array->size += len+1;
}

void delete_from_arrayset(arrayset *array, const char *path) {
  int where = find_in_arrayset(array, path);
  if (where < 0) return;
  int len = strlen(path);
  memmove(&array->a[where], &array->a[where+len+1],
          array->size-where-len-1);
}

int is_in_arrayset(const arrayset *array, const char *path) {
  return find_in_arrayset(array, path) >= 0;
}
