/*
 * stddef.h - standard definitions
 */

#pragma once

#include <stddef.h>
#include <errno.h>

#include <base/compiler.h>
#include <base/types.h>
#include <base/assert.h>

/*
 * NOTE: Some code in this file is derived from the public domain CCAN project.
 * http://ccodearchive.net/
 */

#define check_type(expr, type)                  \
	((typeof(expr) *)0 != (type *)0)
#define check_types_match(expr1, expr2)         \
	((typeof(expr1) *)0 != (typeof(expr2) *)0)

/**
 * container_of - get pointer to enclosing structure
 * @member_ptr: pointer to the structure member
 * @containing_type: the type this member is within
 * @member: the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 */
#ifndef container_of
#define container_of(member_ptr, containing_type, member)               \
	((containing_type *)                                            \
	 ((char *)(member_ptr)                                          \
	  - offsetof(containing_type, member))                          \
	  + check_types_match(*(member_ptr), ((containing_type *)0)->member))
#endif

/**
 * container_of_var - get pointer to enclosing structure using a variable
 * @member_ptr: pointer to the structure member
 * @container_var: a pointer of same type as this member's container
 * @member: the name of this member within the structure.
 */
#define container_of_var(member_ptr, container_var, member)             \
	container_of(member_ptr, typeof(*container_var), member)

/**
 * ARRAY_SIZE - get the number of elements in a visible array
 * @arr: the array whose size you want.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * max - picks the maximum of two expressions
 *
 * Arguments @a and @b are evaluated exactly once
 */
#define max(a, b) \
	({typeof(a) _a = (a); \
	  typeof(b) _b = (b); \
	  _a > _b ? _a : _b;})

/**
 * min - picks the minimum of two expressions
 *
 * Arguments @a and @b are evaluated exactly once
 */
#define min(a, b) \
	({typeof(a) _a = (a); \
	  typeof(b) _b = (b); \
	  _a < _b ? _a : _b;})

/**
 * is_power_of_two - determines if an integer is a power of two
 * @x: the value
 *
 * Returns true if the integer is a power of two.
 */
#define is_power_of_two(x) ((x) != 0 && !((x) & ((x) - 1)))

/**
 * align_up - rounds a value up to an alignment
 * @x: the value
 * @align: the alignment (must be power of 2)
 *
 * Returns an aligned value.
 */
#define align_up(x, align)			\
	({assert(is_power_of_two(align));	\
	 (((x) - 1) | ((typeof(x))(align) - 1)) + 1;})

/**
 * align_down - rounds a value down to an alignment
 * @x: the value
 * @align: the alignment (must be power of 2)
 *
 * Returns an aligned value.
 */
#define align_down(x, align)			\
	({assert(is_power_of_two(align));	\
	 ((x) & ~((typeof(x))(align) - 1));})

/**
 * div_up - divides two numbers, rounding up to an integer
 * @x: the dividend
 * @d: the divisor
 *
 * Returns a rounded-up quotient.
 */
#define div_up(x, d) ((((x) + (d) - 1)) / (d))

/**
 * __cstr - converts a value to a string
 */
#define __cstr_t(x...)	#x
#define __cstr(x...)	__cstr_t(x)

/**
 * BIT - generates a value with one set bit by index
 * @n: the bit index to set
 *
 * Returns a long-sized constant.
 */
#define BIT(n) (1UL << (n))

/* common sizes */
#define KB	(1024)
#define MB	(1024 * KB)
#define GB	(1024 * MB)