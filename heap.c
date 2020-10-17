
#include <string.h>


#include "heap.h"



static inline void swap_mem_(char* a, char* b, size_t elem_sz) {
	char* t = alloca(elem_sz);
	memcpy(t, a, elem_sz);
	memcpy(a, b, elem_sz);
	memcpy(b, t, elem_sz);
}

void heap_up_(heap_* h, size_t pi, size_t elem_sz);
void heap_down_(heap_* h, size_t pi, size_t elem_sz);


void heap_resize_(heap_* h, size_t new_size, size_t elem_sz) {
	if(!h->data) {
		h->data = malloc(elem_sz * 16);
		h->alloc = 16;
	}
	else {
		h->data = realloc(h->data, elem_sz * new_size);
		h->alloc = new_size;
	}
}





void heap_insert_(heap_* h, char* elem, size_t elem_sz) {
	if(h->len >= h->alloc) heap_resize_(h, h->alloc * 2, elem_sz);
	
	// insert the element at the end
	memcpy(h->data + (elem_sz * h->len), elem, elem_sz);
	
	h->len++;
	
	heap_up_(h, h->len-1, elem_sz);
}


int heap_pop_(heap_* h, char* out, size_t elem_sz) {
	if(h->len == 0) {
		return 1;
	}
	
	// copy the top element
	memcpy(out, h->data, elem_sz);
	
	// swap
	swap_mem_(h->data + ((h->len-1) * elem_sz), h->data, elem_sz);
	
	h->len--;
	
	heap_down_(h, 0, elem_sz);
	
	return 0;
}


void heap_free_(heap_* h) {
	if(h->data) free(h->data);
	h->data = NULL;
	h->len = 0;
	h->alloc = 0;
}




void heap_up_(heap_* h, size_t ci, size_t elem_sz) {
	while(ci != 0) {
		
		size_t pi = (ci - 1) / 2;
		
		int ret = h->cmp(h->data + (elem_sz * pi), h->data + (elem_sz * ci));
		if(ret > 0) break;
		
		swap_mem_(h->data + (elem_sz * pi), h->data + (elem_sz * ci), elem_sz);
		
		ci = pi;
	}
}


void heap_down_(heap_* h, size_t pi, size_t elem_sz) {
	size_t ai = (pi * 2) + 1;
	size_t bi = (pi * 2) + 2;
	size_t ci;
	int r;
	
	if(ai >= h->len) return;
	if(bi >= h->len) {
		ci = ai; // only one child
	}
	else {
		// find the smallest child
		r = h->cmp(h->data + (ai * elem_sz), h->data + (bi * elem_sz));
		ci =  r > 0 ? ai : bi; 
	}
	
// 	printf("smaller: %d of %d and %d\n", *(int*)(h->data+ci*elem_sz),*(int*)(h->data+ai*elem_sz),*(int*)(h->data+bi*elem_sz));
	
	r = h->cmp(h->data + (ci * elem_sz), h->data + (pi * elem_sz));
	if(r > 0) {
// 		printf(" -> %d < %d, swapping \n", *(int*)(h->data+ci*elem_sz),*(int*)(h->data+pi*elem_sz));
		
		swap_mem_(h->data + (ci * elem_sz), h->data + (pi * elem_sz), elem_sz);
		
		heap_down_(h, ci, elem_sz);
		return;
	}
	
}


