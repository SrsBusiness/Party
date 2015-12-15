#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "neurons.h"

void loss2_test(void **state);
void dot_test(void **state);
void activation_test(void **state);
void propagate_test(void **state);

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(loss2_test),
        cmocka_unit_test(dot_test),
        cmocka_unit_test(activation_test),
        cmocka_unit_test(propagate_test)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

void loss2_test(void **state) {
    double a[3] = {9.0, 6.0, 3.0};
    double b[3] = {4.0, 8.0, 12.0};
    struct vector vec_a = {3, a}, vec_b = {3, b};
    int loss = (int)loss2(&vec_a, &vec_b);
    assert_int_equal(110, loss);
}

void dot_test(void **state) {
    double a[3] = {9.0, 6.0, 3.0};
    double b[3] = {4.0, 8.0, 12.0};
    struct vector vec_a = {3, a}, vec_b = {3, b};
    int dot_p = (int)dot(&vec_a, &vec_b);
    assert_int_equal(120, dot_p);
}

void activation_test(void **state) {
    double a[3] = {.2, .4, .3};
    double b[3] = {4.0, 8.0, 12.0};
    struct vector vec_a = {3, a}, vec_b = {3, b};
    double output = activation(&vec_a, &vec_b);
    assert_true(0.99949979889292051016269624597043730318546295166015625 == output);
}

void propagate_test(void **state) {
    struct nnet n;
    int num_neurons[] = {3, 3, 3};
    nnet_init(&n, 3, num_neurons); 

    /* hard code weights */
    n.weights[1][1].vec[0] = 0.2;
    n.weights[1][1].vec[1] = 0.2;
    n.weights[1][1].vec[2] = 0.5;
    n.weights[1][1].vec[3] = 0.8;

    n.weights[1][2].vec[0] = 0.2;
    n.weights[1][2].vec[1] = 0.7;
    n.weights[1][2].vec[2] = 0.43;
    n.weights[1][2].vec[3] = 0.76;

    n.weights[1][3].vec[0] = 0.2;
    n.weights[1][3].vec[1] = 0.1;
    n.weights[1][3].vec[2] = 0.21;
    n.weights[1][3].vec[3] = 0.38;

    n.weights[2][1].vec[0] = 0.2;
    n.weights[2][1].vec[1] = 0.94;
    n.weights[2][1].vec[2] = 1.2;
    n.weights[2][1].vec[3] = 2.0;

    n.weights[2][2].vec[0] = 0.2;
    n.weights[2][2].vec[1] = 0.0;
    n.weights[2][2].vec[2] = 0.1;
    n.weights[2][2].vec[3] = 0.3;

    n.weights[2][3].vec[0] = 0.2;
    n.weights[2][3].vec[1] = 0.14;
    n.weights[2][3].vec[2] = 0.5;
    n.weights[2][3].vec[3] = 0.89;

    ///* set the inputs */
    double inputs[] = {0.76, 1.4, 0.85};
    nnet_set_inputs(&n, inputs);
    forward_propogate(&n); 

    assert_true(n.outputs[2].vec[1] > 0.969883 && n.outputs[2].vec[1] < 0.969885);
    assert_true(n.outputs[2].vec[2] > 0.622628 && n.outputs[2].vec[2] < 0.62263);
    assert_true(n.outputs[2].vec[3] > 0.800552 && n.outputs[2].vec[3] < 0.800554);

    nnet_free(&n);
}
