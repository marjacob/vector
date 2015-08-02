#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "vector.h"

#ifndef VEC_INITCAP
	#define VEC_INITCAP (8)
#endif

inline static void _vector_shift_left(struct vector *vec, size_t index)
{
	DEBUG_PRINT("vec = %p, index = %zu\n", vec, index);

	if (index < vec->count - 1) {
		for (size_t i = index; i < vec->count; ++i) {
			vec->mem[i] = vec->mem[1 + i];
		}
	}

	--vec->count;
}

int vector_add(struct vector *vec, void *item)
{
	DEBUG_PRINT("vec = %p, item = %p\n", vec, item);

	int rc = VECE_OK;

	if (VECE_OK == (rc = vector_reserve(vec, vec->count + 1))) {
		vec->mem[vec->count++] = item;
	}
	
	return rc;
}

int vector_apply(struct vector *vec, void (*func)(void *))
{
	DEBUG_PRINT("vec = %p, func = %p\n", vec, func);

	int rc = VECE_OK;
	void *item = NULL;

	if (vec && func) {
		for (size_t i = 0, j = vector_count(vec); i < j; ++i) {
			rc = vector_get(vec, i, &item);
			assert(VECE_OK == rc);
			func(item);
		}
	} else {
		rc = VECE_NULL_POINTER;
	}

	return rc;
}

int vector_delete(struct vector *vec, size_t index, void **item)
{
	DEBUG_PRINT("vec = %p, index = %zu, item = %p\n", vec, index, item);
	
	int rc = VECE_OUT_OF_BOUNDS;
	
	if (vec->count > index) {
		if (item) {
			*item = vec->mem[index];
		}

		_vector_shift_left(vec, index);
		rc = VECE_OK;
	}

	return rc;
}

int vector_free(struct vector *vec)
{
	DEBUG_PRINT("vec = %p\n", vec);

	int rc = VECE_NULL_POINTER;
	
	if (vec) {
		free(vec->mem);
		rc = vector_init(vec);
	}
	
	return rc;
}

int vector_get(struct vector *vec, size_t index, void **item)
{
	DEBUG_PRINT("vec = %p, index = %zu, item = %p\n", vec, index, item);

	if (!vec) {
		return VECE_NULL_POINTER;
	}

	if (vec->count > index) {
		*item = vec->mem[index];
		return VECE_OK;
	}

	return VECE_OUT_OF_BOUNDS;
}

int vector_init(struct vector *vec)
{
	DEBUG_PRINT("vec = %p\n", vec);

	int rc = VECE_NULL_POINTER;
	
	if (vec) {
		vec->count = 0;
		vec->max = 0;
		vec->mem = NULL;
		
		rc = VECE_OK;
	}
	
	return rc;
}

int vector_reserve(struct vector *vec, size_t size)
{
	DEBUG_PRINT("vec = %p, size = %zu\n", vec, size);

	if (!vec) {
		return VECE_NULL_POINTER;
	} else if (size <= vec->max) {
		return VECE_OK;
	} else if (vec->max) {
		do {
			vec->max *= 2;
		} while (size > vec->max);
	} else {
		vec->max = VEC_INITCAP;
	}
	
	if (!(vec->mem = realloc(vec->mem, vec->max * sizeof(void *)))) {
		return ENOMEM == errno ? VECE_OUT_OF_MEMORY : VECE_OK;
	}
	
	return VECE_OK;
}

int vector_set(struct vector *vec, size_t index, void *item)
{
	DEBUG_PRINT("vec = %p, index = %zu, item = %p\n", vec, index, item);

	if (!vec) {
		return VECE_NULL_POINTER;
	}

	if (vec->count > index) {
		vec->mem[index] = item;
		return VECE_OK;
	}
 
	return VECE_OUT_OF_BOUNDS;
}

int vector_swap(struct vector *vec, size_t idx1, size_t idx2)
{
	DEBUG_PRINT("vec = %p, idx1 = %zu, idx2 = %zu\n", vec, idx1, idx2);

	int rc = VECE_OK;
	void *item1 = NULL;
	void *item2 = NULL;

	if (VECE_OK != (rc = vector_get(vec, idx1, &item1))) {
		DEBUG_PRINT("rc = %d\n", rc);
		return rc;
	}

	if (VECE_OK != (rc = vector_get(vec, idx2, &item2))) {
		DEBUG_PRINT("rc = %d\n", rc);
		return rc;
	}
	
	rc = vector_set(vec, idx1, item2);
	assert(VECE_OK == rc);
	rc = vector_set(vec, idx2, item1);
	assert(VECE_OK == rc);

	return rc;
}

