#include "ringbuf.h"

#include <sys/mman.h>
#include <assert.h>

#include <stdio.h>

#ifdef DEBUG
#define PRINT_DEBUG(ringbuf) \
	printf("%s:%d -> ringbuf info\n - mem_start: %p\n - mem_end: %p\n - mem_size: %d\n - last_pointer: %p\n - first_pointer: %p\n - element_size: %d\n - element_count: %d\n", __FILE__, __LINE__, ringbuf->mem_start, ringbuf->mem_end, ringbuf->mem_size, ringbuf->last_pointer, ringbuf->first_pointer, ringbuf->element_size, ringbuf->element_count)
#else
#define PRINT_DEBUG(ringbuf) (void)ringbuf
#endif

ringbuf_t ringbuf_init(size_t element_size, size_t element_count) {
	size_t mem_size = element_size * element_count;

	// Allocation through mmap on POSIX systems
	void *mem_start = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	void *mem_end = (void*)((uintptr_t)mem_start + mem_size);

	// The method cannot fail
	assert(mem_start != NULL);

	// Filling in the ringbuffer structure
	ringbuf_t ringbuf = (ringbuf_t){
		.mem_start = mem_start,
		.mem_end = mem_end,
		.mem_size = mem_size,
		.last_pointer = mem_start,
		.first_pointer = mem_start,
		.element_size = element_size,
		.element_count = element_count,
	};

	// Printing debug info if needed
	{
		ringbuf_t *ringbuf_ptr = &ringbuf;
		PRINT_DEBUG(ringbuf_ptr);
	}

	return ringbuf;
}

// Implementation function for the inner part of the allocation
static void *ringbuf_alloc_inner(ringbuf_t *ringbuf) {
	// Calculating the next possible last pointer
	void *next_last_pointer = (void*)((uintptr_t)ringbuf->last_pointer + ringbuf->element_size);

	// Checking if the ring has eaten its tail
	if (next_last_pointer != ringbuf->first_pointer) {
		// No it hasnt, there is still space

		void *return_ptr = ringbuf->last_pointer;
		ringbuf->last_pointer = next_last_pointer;

		return return_ptr;
	} else {
		// It has, the ring is full
		return NULL;
	}
}

void *ringbuf_alloc(ringbuf_t *ringbuf) {
	PRINT_DEBUG(ringbuf);

	// Checking if we need to wrap around
	if (ringbuf->last_pointer != ringbuf->mem_end) {
		// No, just proceed with allocating
		return ringbuf_alloc_inner(ringbuf);
	} else {
		// Yes, we need to wrap around

		// Checking if there is space to wrap around
		if (ringbuf->first_pointer != ringbuf->mem_start) {
			// Yes, there is space
			// Proceed with allocation with the new wrapped pointer
			ringbuf->last_pointer = ringbuf->mem_start;
			return ringbuf_alloc_inner(ringbuf);
		} else {
			// No, the memory is full
			return NULL;
		}
	}
}

bool ringbuf_free(ringbuf_t *ringbuf) {
	PRINT_DEBUG(ringbuf);

	// Checking if the buffer is already freed
	if (ringbuf->first_pointer != ringbuf->last_pointer) {
		// Calculating the next possible first pointer
		void *next_first_pointer = (void*)((uintptr_t)ringbuf->first_pointer + ringbuf->element_size);

		// Checking if there is a wrap around
		if (next_first_pointer != ringbuf->mem_end) {
			// No, just set it normally
			ringbuf->first_pointer = next_first_pointer;
		} else {
			// Yes, set it to the beginning of memory
			ringbuf->first_pointer = ringbuf->mem_start;
		}

		// Freeing successfull
		return true;
	} else {
		// Yes, already freed, nothing to do
		// Signaling however that this free was not necessary
		return false;
	}
}
