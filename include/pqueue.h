#include <stdint.h>

struct priority_queue_elem {
    int32_t weight;
    void *data;
};

struct priority_queue {
    size_t length;
    size_t capacity;
    struct priority_queue_elem *queue;
};

void priority_queue_init(struct priority_queue *p, size_t capacity);
void priority_queue_push(struct priority_queue *pqueue, void *data, int32_t weight);
void *priority_queue_pop(struct priority_queue *pqueue);
