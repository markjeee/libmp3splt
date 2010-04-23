/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "splt_array.h"

splt_array *splt_array_new()
{
  splt_array *array = malloc(sizeof(splt_array));
  if (array == NULL)
  {
    return NULL;
  }

  array->elements = NULL;
  array->number_of_elements = 0;

  return array;
}

int splt_array_append(splt_array *array, void *element)
{
  if (!array || !element)
  {
    return 2;
  }

  if (array->number_of_elements == 0)
  {
    array->elements = malloc(sizeof(element));
    if (!array->elements)
    {
      return -1;
    }
    array->elements[0] = element;
    array->number_of_elements++;
  }
  else
  {
    size_t malloc_number = sizeof(element) * (array->number_of_elements + 1);
    void **new_elements = realloc(array->elements, malloc_number);
    if (! new_elements)
    {
      return -1;
    }
    array->elements = new_elements;
    array->elements[array->number_of_elements] = element;
    array->number_of_elements++;
  }

  return 0;
}

void splt_array_clear(splt_array *array)
{
  if (!array)
  {
    return;
  }

  if (array->elements)
  {
    free(array->elements);
    array->elements = NULL;
    array->number_of_elements = 0;
  }
}

void splt_array_free(splt_array **array)
{
  if (!array)
  {
    return;
  }

  if (*array)
  {
    splt_array_clear(*array);
    free(*array);
    *array = NULL;
  }
}

void *splt_array_get(splt_array *array, int index)
{
  if (!array)
  {
    return NULL;
  }

  if (index < 0 || index >= array->number_of_elements)
  {
    return NULL;
  }

  return array->elements[index];
}

int splt_array_length(splt_array *array)
{
  if (!array)
  {
    return -1;
  }

  return array->number_of_elements;
}

/*int main()
{
  splt_array *array = splt_array_new();

  int element = 183;
  splt_array_append(array, (int *)element);
  element = 87;
  splt_array_append(array, (int *)element);

  assert(splt_array_length(array) == 2);

  int inside_int = (int) splt_array_get(array, 0);
  assert(inside_int == 183);

  inside_int = (int) splt_array_get(array, 1);
  assert(inside_int == 87);

  int *a = splt_array_get(array, 2);
  assert(a == NULL);

  a = splt_array_get(array, -1);
  assert(a == NULL);

  splt_array_clear(array);
  assert(splt_array_length(array) == 0);

  splt_array_free(&array);
  assert(array == NULL);

  return EXIT_SUCCESS;
}*/

