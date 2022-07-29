#if !defined(MANAGEDC_LINKEDLIST)
#define MANAGEDC_LINKEDLIST

#include "managed.h"

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

struct managed_Node {
	struct managed_Node *mc_nullable next, *mc_nullable previous;
	void *mc_nonnull data;
};

struct managed_LinkedList {
	struct managed_Node 	*mc_nullable head, *mc_nullable tail;
	managed_Free_f 			*mc_nullable free;
	const size_t 			*mc_nonnull const count;
};

static void managed_linkedlist_free(struct managed_LinkedList *mc_nonnull list)
{
	struct managed_Node *node = list->head;
	
	while (node != NULL) {
		struct managed_Node *next = node->next;
		if (list->free != NULL) {
			list->free((void *)node->data);
		}
		node = next;
	}
	
	_mcinternal_ptrinfo(list)->count = 0; /* Causes the rest of the items to be skipped */
}

#define mllist_new(free) managed_linkedlist((managed_Free_f *) free)
static struct managed_LinkedList *managed_linkedlist(managed_Free_f *free)
{
	struct managed_LinkedList *list = mc_new(struct managed_LinkedList, managed_linkedlist_free);
	const size_t **ptr = NULL;
	if (list == NULL) return NULL;
	ptr = (void *)&list->count;
	*ptr = &mc_countof(list);

	_mcinternal_ptrinfo(list)->count = 0;

	list->free = free;

	return list;
}

#define mllist_add(list, data) managed_linkedlist_add(list, (void *)data)
static int managed_linkedlist_add(struct managed_LinkedList *list, void *data)
{
	struct managed_Node *node = mc_new(struct managed_Node, NULL);
	if (node == NULL) return 1;

	node->data = data;
	node->next = NULL;
	node->previous = list->tail;
	if (list->tail != NULL) {
		list->tail->next = node;
	} else {
		list->head = node;
	}
	list->tail = node;

	_mcinternal_ptrinfo(list)->count++;

	return 0;
}

#define mllist_remove(list, node) managed_linkedlist_remove(list, node)
static void managed_linkedlist_remove(struct managed_LinkedList *list, long int index)
{
	struct managed_Node *node = list->head;

	if (index < 0) index = *list->count;
	if (index >= (long int)(*list->count)) return;

	while (index-- > 0) {
		node = node->next;
	}
	if (node->previous != NULL) {
		node->previous->next = node->next;
	} else {
		list->head = node->next;
	}
	if (node->next != NULL) {
		node->next->previous = node->previous;
	} else {
		list->tail = node->previous;
	}

	_mcinternal_ptrinfo(list)->count--;

	managed_release(node);
}

#define mllist_get(list, index) managed_linkedlist_get(list, index)
static void *managed_linkedlist_get(struct managed_LinkedList *list, long int index)
{
	struct managed_Node *node = list->head;
	if (index < 0) return NULL;
	if (index >= (long int)*list->count) return NULL;

	while (index-- > 0) {
		node = node->next;
	}
	return node->data;
}

#define mllist_set(list, index, data, size) managed_linkedlist_set(list, index, data, size)
static int managed_linkedlist_set(struct managed_LinkedList *list, long int index, const void *data, size_t size)
{
	struct managed_Node *node = list->head;
	if (index < 0) return 1;
	if (index >= (long int)*list->count) return 1;

	while (index-- > 0) {
		node = node->next;
	}
	managed_release(node->data);
	node->data = managed_allocate(1, size, NULL, data);
	return 0;
}

#endif