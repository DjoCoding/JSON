#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#define REGION_MIN_SIZE 1000
#define ARENA_INIT_DEFAULT() arena_init(0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct Region Region;
typedef struct Arena Arena;

struct Region {
	char *items;
	size_t count;
	size_t size;
	Region *next;
};

struct Arena {
	Region *head;
	Region *end;
};

Region *region_init(size_t size) {
	Region *region = (Region *)malloc(sizeof(Region));
	if(!region) { EXIT_ERROR("could not allocate memory for the arena region\n"); }

	region->count = 0;
	region->size = MAX(REGION_MIN_SIZE, size);
	region->next = NULL;
	region->items = (char *)malloc(sizeof(char) * region->size);

	if (!region->items) { EXIT_ERROR("could not allocate memory for the arena region\n"); }

	return region;
}

Arena arena_init(size_t size) {
	Arena arena = {0};
	arena.head = region_init(size);
	arena.end = arena.head;
	return arena;
}


void arena_add_region(Arena *self, size_t size) {
	Region *current = self->head;

	while (current->next) {
		current = current->next;
	}

	current->next = region_init(size);

	self->end = current->next; 
}


void *arena_alloc(Arena *self, size_t item_size) {
	Region *current = self->head;

	while(current) {
		if (current->count + item_size <= current->size) { break; }
		current = current->next;
	}

	if (!current) { arena_add_region(self, item_size); current = self->end; }

	void *ptr = &current->items[current->count];
	current->count += item_size;

	return ptr;
}

void arena_reset(Arena *self) {
	Region *current = self->head;
	while (current) {
		current->count = 0;
		current = current->next;
	}
}

void arena_free(Arena *self) {
	Region *current = self->head;
	while (current) {
		Region *next = current->next;
		free(current->items);
		free(current);
		current = next;
	}
	self->head = NULL;
	self->end = NULL;
}

void arena_show(Arena self) {
	Region *current = self.head;
	while (current) {
		printf("(count) = %zu, (size) = %zu\n", current->count, current->size);
		current = current->next;
	}
}

#endif //  ARENA_H
