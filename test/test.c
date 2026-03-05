#include "ringbuf.h"
#include <stdio.h>

int main() {
	ringbuf_t ringbuf = ringbuf_init(sizeof(int), 16);

	for (int j = 0; j < 4; ++j) {
		void *addrs[16];
		for (int i = 0; i < 12; ++i) {
			addrs[i] = ringbuf_alloc(&ringbuf);
			printf("addrs[%d]: %p\n", i, addrs[i]);
		}

		for (int i = 0; i < 8; ++i) {
			printf("free addrs[%d]\n", i);
			addrs[i] = NULL;
			ringbuf_free(&ringbuf);
		}

		for (int i = 0; i < 12; ++i) {
			addrs[i] = ringbuf_alloc(&ringbuf);
			printf("addrs[%d]: %p\n", i, addrs[i]);
		}
	}

	return 0;
}
