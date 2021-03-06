#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neurons.h"

#define ALPHA   0.1

#ifndef M_E
#define M_E 2.7182818284590452354
#endif


/* Computes the sum of squares loss (difference) of the two vectors */
double loss2(const struct vector *a, const struct vector *b) {
    assert(a->length == b->length);
    double loss = 0.0;
    for (int i = 0; i < a->length; i++) {
        loss += pow(a->vec[i] - b->vec[i], 2);
    }
    return loss;
}

double dot(const double *a, const double *b, int length) {
    double product = 0;
    for (int i = 0; i < length; i++) {
        product += a[i] * b[i];
    }
    return product;
}

void vadd(const double *a, const double *b, double *c, int length) {
    for (int i = 0; i < length; i++) {
        c[i] = a[i] + b[i];
    }
}

void vsub(const double *a, const double *b, double *c, int length) {
    for (int i = 0; i < length; i++) {
        c[i] = a[i] - b[i];
    }
}

double vsum(const double *a, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += a[i]; 
    }
    return sum;
}

double dist_euc_2(const double *a, const double *b, int length) {
    double dist = 0.0;
    for (int i = 0; i < length; i++) {
        //printf("expected: %f, hypothesis: %f\n", a[i], b[i]);
        //getchar();
        dist += pow(a[i] - b[i], 2);
    }
    return dist;
}

/* Logistic activation function given weights w and inputs x */
double activation(const struct vector *w, const struct vector *x) {
    return 1.0 / (1.0 + pow(M_E, -dot(w->vec, x->vec, w->length)));
}

/* random double between -1 and 1 */
static double rand_double() {
    return (double)(rand() - RAND_MAX / 2) / ((double)(RAND_MAX / 2)) / 100.0;
}

/* Initializes network 
 * Takes in the number of layers and the number of neurons per layer.
 * A neuron implicitly feeds its output to all the neurons in the
 * successor layer. n should point to already allocated memory
 **/

struct nnet *nnet_init(struct nnet *n, int num_layers,
        int *num_neurons, double alpha) {
    n->num_layers = num_layers;
    n->alpha = alpha;
    n->weights = malloc(num_layers * sizeof(struct vector *));
    n->new_weights = malloc(num_layers * sizeof(struct vector *));
    n->outputs = malloc(num_layers * sizeof(struct vector));
    n->errors = malloc(num_layers * sizeof(struct vector));

    /* layer 0: only the outputs are important, no weights or errors */
    n->outputs[0].length = num_neurons[0] + 1;
    n->outputs[0].vec = malloc(n->outputs[0].length * sizeof(double));
    n->outputs[0].vec[0] = 1.0;

    for (int i = 1; i < num_layers; i++) {
        /* For each layer */
        n->outputs[i].length = n->errors[i].length = num_neurons[i] + 1;
        n->outputs[i].vec = malloc(n->outputs[i].length * sizeof(double));
        n->outputs[i].vec[0] = 1.0;
        n->errors[i].vec = malloc(n->errors[i].length * sizeof(double));
        n->weights[i] = malloc(n->outputs[i].length * sizeof(struct vector));
        n->new_weights[i] = malloc(n->outputs[i].length * sizeof(struct vector));
        for (int j = 1; j < n->outputs[i].length; j++) {
            /* For each neuron 
             * 0th neuron has no weights */
            n->weights[i][j].length = n->new_weights[i][j].length = n->outputs[i - 1].length;
            n->weights[i][j].vec =
                malloc(n->outputs[i - 1].length * sizeof(double));
            n->new_weights[i][j].vec =
                malloc(n->outputs[i - 1].length * sizeof(double));
            /* randomize weights */
            for (int k = 0; k < n->outputs[i - 1].length; k++) {
                n->weights[i][j].vec[k] = rand_double();
            }
        }
    }
    return n;
}

void nnet_free(struct nnet *n) {
    /* layer 0: */
    free(n->outputs[0].vec);

    for (int i = 1; i < n->num_layers; i++) {
        for (int j = 1; j < n->outputs[i].length; j++) {
            free(n->weights[i][j].vec);
            free(n->new_weights[i][j].vec);
        }
        free(n->weights[i]);
        free(n->new_weights[i]);
        free(n->outputs[i].vec);
        free(n->errors[i].vec);
    }
    free(n->weights);
    free(n->new_weights);
    free(n->outputs);
    free(n->errors);
}

void nnet_set_inputs(struct nnet *n, double *inputs, int length) {
    assert(length + 1 == n->outputs[0].length);
    for (int i = 1; i < n->outputs[0].length; i++) {
        n->outputs[0].vec[i] = inputs[i - 1];
    }
}

/* forward-propogation */
void forward_propagate(struct nnet *n) {
    /* layer 0 by convention are the inputs to the network */
    /* last layer contains the outputs of the network */
    for (int i = 1; i < n->num_layers; i++) {
        /* For each layer */
        /* propogate the activation signals for this layer */
        for (int j = 1; j < n->outputs[i].length; j++) { /* */
            /* For each neuron */
            n->outputs[i].vec[j] = 
                activation(&n->weights[i][j],
                        &n->outputs[i - 1]); 
            assert(n->outputs[i].vec[j] <= 1.0);
        }
    }

    /* last layer is linear activation */
    //int last_layer = n->num_layers - 1;
    //for (int i = 1; i < n->outputs[last_layer].length; i++) {
    //    n->outputs[last_layer].vec[i] =
    //        dot(n->weights[last_layer][i].vec, n->outputs[last_layer - 1].vec, 
    //                n->weights[last_layer][i].length);
    //}
    //exit(0);
}

/* adjusts the weights according to the error, called after forward_propogate */
/* Takes in the net and the 'answer key' */
void backward_propagate(struct nnet *n, const double *key, int length) {
    int last_layer = n->num_layers - 1;
    assert(n->outputs[last_layer].length == length + 1);
    struct vector key_v = {length, (double *)key};
    /* Output layer handled specially */ 
    for (int i = 1; i < n->outputs[last_layer].length; i++) {
        /* For all nodes in the last layer */
        double h = n->outputs[last_layer].vec[i];
        n->errors[last_layer].vec[i] =  h * (1 - h) * (key[i - 1] - h);
        //n->errors[last_layer].vec[i] = (key[i - 1] - h);
        for (int j = 0; j < n->new_weights[last_layer][i].length; j++) {
            /* For all weights in the node */
            /* ith component of the output vector */
            /* jth component of the input vector */
            double x = n->outputs[last_layer - 1].vec[j];
            /* update the jth component of the weight vector */
            n->new_weights[last_layer][i].vec[j] =
                n->weights[last_layer][i].vec[j] +
                n->alpha * n->errors[last_layer].vec[i] * x;
        }
    }
    /* Inner layers: from second-to-last to the 1st layer
     * 0th layer is ignored for backprop */
    for (int i = n->num_layers - 2; i > 0; i--) {
        /* for each layer : ith layer */
        for (int j = 1; j < n->outputs[i].length; j++) {
            /* for each neuron : jth neuron */
            double h = n->outputs[i].vec[j];
            /* accumulate the error from all the neurons in the subsequent
             * layer*/
            double sum = 0.0;
            for (int k = 1; k < n->errors[i + 1].length; k++) {
                sum += n->weights[i + 1][k].vec[j] * n->errors[i + 1].vec[k];
            }
            n->errors[i].vec[j] = h * (1 - h) * sum;
            for (int k = 0; k < n->new_weights[i][j].length; k++) {
                /* for each weight : kth weight/input */
                n->new_weights[i][j].vec[k] = n->weights[i][j].vec[k] +
                    n->alpha * n->outputs[i - 1].vec[k] * n->errors[i].vec[j];
            }
        }
    }
    /* Copy over the new weights */
    for (int i = 1; i < n->num_layers; i++) {
        for (int j = 1; j < n->outputs[i].length; j++) {
            memcpy(n->weights[i][j].vec, n->new_weights[i][j].vec,
                    n->outputs[i - 1].length * sizeof(double));
        }
    }
}

/* Selects size_a random examples from examples and moves them to a 
 * modifies both examples and a
 **/
void partition_examples(struct example *examples, struct example *a, int num_examples, int size_a) {
    int len_orig = num_examples, len_a = 0; 
    for (int i = 0; i < size_a; i++) {
        /* swap a random example with the last one in examples */
        int rand_index = rand() % len_orig;
        struct example tmp = examples[rand_index];
        examples[rand_index] = examples[len_orig - 1];
        examples[len_orig - 1] = tmp;
        /* Move the last example into a */
        a[len_a++] = tmp;
        len_orig--;
    }
}

/* stochastic gradient descent */
void nnet_train(struct nnet *n, struct example *examples, int num_examples, int iterations, double *error) {
    double total_error = 0.0;
    int last_layer = n->num_layers - 1;
    for (int i = 0; i < iterations; i++) {
        int index = rand() % num_examples;
        nnet_set_inputs(n, examples[index].inputs, examples[index].num_inputs);
        //printf("INPUTS\n");
        //for (int j = 0; j < n->outputs[0].length; j++) {
        //    printf("%f ", n->outputs[0].vec[j]); 
        //}
        //printf("\n");
        forward_propagate(n);
        total_error += dist_euc_2(examples[index].outputs, &n->outputs[last_layer].vec[1],
                examples[index].num_outputs);
        backward_propagate(n, examples[index].outputs, examples[index].num_outputs);
    }
    *error = sqrt(total_error / iterations);
}

/* compute root mean square of the error */
double rms_error_rand(struct nnet *n, struct example *examples, int num_examples, int samples) {
    double total_error = 0.0;
    int last_layer = n->num_layers - 1;
    for (int i = 0; i < samples; i++) {
        int index = rand() % num_examples;
        nnet_set_inputs(n, examples[index].inputs, examples[index].num_inputs);
        forward_propagate(n);
        total_error += dist_euc_2(examples[index].outputs, &n->outputs[last_layer].vec[1],
                examples[index].num_outputs);
    }
    return sqrt(total_error / num_examples);
}

/* compute root mean square of the error */
double rms_error(struct nnet *n, struct example *examples, int num_examples) {
    double total_error = 0.0;
    int last_layer = n->num_layers - 1;
    for (int i = 0; i < num_examples; i++) {
        nnet_set_inputs(n, examples[i].inputs, examples[i].num_inputs);
        forward_propagate(n);
        total_error += dist_euc_2(examples[i].outputs, &n->outputs[last_layer].vec[1],
                examples[i].num_outputs);
    }
    return sqrt(total_error / num_examples);
}

void nnet_save(struct nnet *n, const char *filename) {
    FILE *f = fopen(filename, "w");
    fwrite(&n->num_layers, sizeof(n->num_layers), 1, f);
    fwrite(&n->alpha, sizeof(n->alpha), 1, f);
    /* Write # of neurons in each layer */
    for (int i = 0; i < n->num_layers; i++) {
        fwrite(&n->outputs[i].length,
                sizeof(n->outputs[i].length), 1, f);
    }

    /* Write the weights */
    for (int i = 1; i < n->num_layers; i++) {
        for (int j = 1; j < n->outputs[i].length; j++) {
            fwrite(n->weights[i][j].vec, sizeof(double),
                    n->weights[i][j].length, f);
        }
    }
    fclose(f);
}

struct nnet *nnet_load(struct nnet *n, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        return NULL;
    }
    fread(&n->num_layers, sizeof(n->num_layers), 1, f);
    fread(&n->alpha, sizeof(n->alpha), 1, f);
    n->weights = malloc(n->num_layers * sizeof(struct vector *));
    n->new_weights = malloc(n->num_layers * sizeof(struct vector *));
    n->outputs = malloc(n->num_layers * sizeof(struct vector));
    n->errors = malloc(n->num_layers * sizeof(struct vector));

    int *num_neurons = malloc(n->num_layers * sizeof(int));
    fread(num_neurons, sizeof(int), n->num_layers, f);

    n->outputs[0].length = num_neurons[0];
    n->outputs[0].vec = malloc(n->outputs[0].length * sizeof(double));
    n->outputs[0].vec[0] = 1.0;

    for (int i = 1; i < n->num_layers; i++) {
        n->outputs[i].length = n->errors[i].length = num_neurons[i];
        n->outputs[i].vec = malloc(n->outputs[i].length * sizeof(double));
        n->outputs[i].vec[0] = 1.0;
        n->errors[i].vec = malloc(n->errors[i].length * sizeof(double));
        n->weights[i] = malloc(n->outputs[i].length * sizeof(struct vector));
        n->new_weights[i] = malloc(n->outputs[i].length * sizeof(struct vector));
        for (int j = 1; j < n->outputs[i].length; j++) {
            n->weights[i][j].length = n->new_weights[i][j].length = n->outputs[i - 1].length;
            n->weights[i][j].vec =
                malloc(n->outputs[i - 1].length * sizeof(double));
            n->new_weights[i][j].vec = 
                malloc(n->outputs[i - 1].length * sizeof(double));
            for (int k = 0; k < n->outputs[i - 1].length; k++) {
                fread(&n->weights[i][j].vec[k], sizeof(double), 1, f);
            }
        }
    }
    return n;
}
