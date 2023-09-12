#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdlib.h>

#define VECTOR_FREE(inner_type, name) \
void	vector_free(vector_##name *v) {v->capacity = 0; v->lenght = 0; free(v->inner); v->inner = NULL;}

#define VECTOR_FREE_FUNC(inner_type, name, free_func) \
void	vector_free(vector_##name *v) { for (size_t i = 0; i < v->lenght; i++) {free_func(v->inner[i]);}; \
v->capacity = 0; v->lenght = 0; free(v->inner); v->inner = NULL;}

#define VECTOR_DEFINE_MAIN(inner_type, name)\
											\
typedef struct vector_##name				\
{											\
	size_t capacity;						\
	size_t lenght;							\
	inner_type *inner;						\
} vector_##name;							\
											\
vector_##name vector_##name##_init() {		\
	return (vector_##name){0};				\
}											\
											\
void	vector_##name##_push_back(vector_##name *v, inner_type element) {	\
	v->lenght += 1;															\
	if (v->lenght > v->capacity) {											\
		if (v->capacity == 0)												\
			v->capacity = 4;												\
		v->capacity *= 2;													\
		v->inner = realloc(v->inner, sizeof(*v->inner) * v->capacity);		\
	}																		\
	v->inner[v->lenght - 1] = element;										\
}																			\
																			\
inner_type vector_##name##_get(vector_##name *v, size_t position) { return v->inner[position];}		\
inner_type *vector_##name##_get_ptr(vector_##name *v, size_t position) { return &v->inner[v->lenght - 1];}

/*! 
   \brief Declare a new vector type containing element_type and overload free function
   \param element_type Type that the vector will contain
   \param name the vector type will be prefixed by this name. ex: vector_name
   \param free_element_function The fonction to free each elements of the array before freeing the vector itself
*/
#define VECTOR_DEFINE_WITH_FREE(element_type, name, free_element_function) VECTOR_DEFINE_MAIN(element_type, name) VECTOR_FREE_FUNC(element_type, name, free_element_function)


/*! 
   \brief Declare a new vector type containing element_type
   \param element_type Type that the vector will contain
   \param name the vector type will be prefixed by this name. ex: vector_name
*/
#define VECTOR_DEFINE(element_type, name) VECTOR_DEFINE_MAIN(element_type, name) VECTOR_FREE(element_type, name)

#endif