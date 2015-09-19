#include "pqueue.h"

void priority_queue_push_test() {
    struct priority_queue p;
    priority_queue_init(&p, 16);
    int64_t i;
    for (i = 0; i < 10; i++) {
        priority_queue_push(&p, (void *)i, i);
    }
    assert_int_equal(10, p.length);
    
    assert_int_equal(9, (int64_t)p.queue[0].data);
    assert_int_equal(8, (int64_t)p.queue[1].data);
    assert_int_equal(5, (int64_t)p.queue[2].data);
    assert_int_equal(6, (int64_t)p.queue[3].data);
    assert_int_equal(7, (int64_t)p.queue[4].data);
    assert_int_equal(1, (int64_t)p.queue[5].data);
    assert_int_equal(4, (int64_t)p.queue[6].data);
    assert_int_equal(0, (int64_t)p.queue[7].data);
    assert_int_equal(3, (int64_t)p.queue[8].data);
    assert_int_equal(2, (int64_t)p.queue[9].data);
    priority_queue_destroy(&p);
}

void priority_queue_pop_test() {
    struct priority_queue p;
    priority_queue_init(&p, 16);
    int64_t i;
    for (i = 0; i < 10; i++) {
        priority_queue_push(&p, (void *)i, i);
    }

    int64_t data;
    for (i = 9; i >= 0; i--) {
        data = (int64_t)priority_queue_pop(&p);
        assert_int_equal(i, data);
    }
    assert_int_equal(0, p.length);
    priority_queue_destroy(&p);
}
