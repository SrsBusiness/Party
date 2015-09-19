#include <stdlib.h>
#include <stdio.h>
#include "pqueue.h"

#ifdef UNIT_TEST
#include "unit_tests.h"
#endif

#define pqueue_right_child(x) (2 * (x + 1))
#define pqueue_left_child(x) (2 * (x) + 1)
#define pqueue_parent(x) (((x) - 1) / 2)

/* Doubles the length until the capacity >= length */
static void resize(struct priority_queue *pqueue, size_t length) {
    while (pqueue->capacity < length) {
        pqueue->capacity *= 2;
    }
    void *new = realloc(pqueue->queue, pqueue->capacity);
    if (!new) {
        fprintf(stderr, "Error extending Priority Queue\n");
        exit(1);
    }
    pqueue->queue = new;
}

void priority_queue_init(struct priority_queue *p, size_t capacity) {
    p->length = 0;
    p->capacity = capacity;
    p->queue = malloc(sizeof(struct priority_queue_elem) * capacity);
}

void priority_queue_push(struct priority_queue *pqueue, void *data, int32_t weight) {
    if (pqueue->length >= pqueue->capacity) {
        resize(pqueue, pqueue->length + 1);
    }
    size_t current = pqueue->length++, parent = pqueue_parent(current);
    while (current && weight > pqueue->queue[parent].weight) {
        pqueue->queue[current] = pqueue->queue[parent];
        current = parent;
        parent = pqueue_parent(current);
    }
    pqueue->queue[current].weight = weight;
    pqueue->queue[current].data = data;
}

void *priority_queue_pop(struct priority_queue *pqueue) {
    if(pqueue->length <= 0)
        return NULL;
    
    struct priority_queue_elem *queue = pqueue->queue;
    void *return_val = queue[0].data;
    pqueue->length--;
    size_t current = 0, child;
    /* While a child exists */
    while((child = pqueue_left_child(current)) < pqueue->length) {
        /* If the right child is the largest */
        if (child + 1 < pqueue->length &&
                queue[child].weight < queue[child + 1].weight) {
            child++;
        }
        if (queue[pqueue->length].weight >= queue[child].weight) {
            break;
        }
        queue[current] = queue[child];
        current = child;
    }
    queue[current] = queue[pqueue->length];
    return return_val;
}

void priority_queue_destroy(struct priority_queue *pqueue) {
    free(pqueue->queue);
}
