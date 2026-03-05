#pragma once

#ifndef RINGBUF_H_
#define RINGBUF_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//// PUBLIC MACRO DEFINITIONS

/// Debug flag, activate for verbose messaging
#undef DEBUG

//// PUBLIC STRUCT DEFINITIONS

/// Structure containing the mutable ring buffer 
/// Fields are internal
typedef struct {
	void *mem_start; // Start of the memory region
	void *mem_end;   // End of the memory region
	size_t mem_size; // Size of the memory region

	void *last_pointer;  // Last pointer, points to first available space
	void *first_pointer; // First pointer, points to first allocated space

	size_t element_size;  // Size of the each of the alloc-able elements
	size_t element_count; // Total of alloc-able and alloc-ed elements in the region
} ringbuf_t;

//// PUBLIC FUNCTION DEFINITIONS

/// Initialize the ring buffer with the provided element size and element count and the 
/// The allocation policy is set as RINGBUF_DEFAULT
/// Return is by value and not by pointer
/// This method cannot fail, if out of memory the program will crash
ringbuf_t ringbuf_init(size_t element_size, size_t element_count);

/// Tries to allocate one element in the ring buffer, returns either the address or NULL if out of space
void *ringbuf_alloc(ringbuf_t *ringbuf);

/// Frees using the policy in the definition of the ring buffer
bool ringbuf_free(ringbuf_t *ringbuf);

#endif // RINGBUF_H_
