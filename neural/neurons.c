#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neurons.h"

#define ALPHA   0.2
#define M_E     2.718281828459045 


/* Computes the sum of squares loss (difference) of the two vectors */
double loss2(const struct vector *a, const struct vector *b) {
    assert(a->length == b->length);
    double loss = 0.0;
    for (int i = 0; i < a->length; i++) {
        loss += pow(a->vec[i] - b->vec[i], 2);
    }
    return loss;
}

double dot(const struct vector *a, const struct vector *b) {
    assert(a->length == b->length);
    double product = 0;
    for (int i = 0; i < a->length; i++) {
        product += a->vec[i] * b->vec[i];
    }
    return product;
}

void vadd(const struct vector *a, const struct vector *b, struct vector *c) {
    assert(a->length == b->length && a->length == c->length);
    for (int i = 0; i < a->length; i++) {
        c->vec[i] = a->vec[i] + b->vec[i];
    }
}

void vsub(const struct vector *a, const struct vector *b, struct vector *c) {
    assert(a->length == b->length && a->length == c->length);
    for (int i = 0; i < a->length; i++) {
        c->vec[i] = a->vec[i] - b->vec[i];
    }
}

double vsum(const struct vector *a) {
    double sum = 0;
    for (int i = 0; i < a->length; i++) {
        sum += a->vec[i]; 
    }
    return sum;
}

/* Logistic activation function given weights w and inputs x */
double activation(const struct vector *w, const struct vector *x) {
    return 1.0 / (1.0 + pow(M_E, -dot(w, x)));
}

/* random double between -1 and 1 */
static double rand_double() {
    return (double)(rand() - RAND_MAX / 2) / (double)(RAND_MAX / 2);
}

/* Initializes network 
 * Takes in the number of layers and the number of neurons per layer.
 * A neuron implicitly feeds its output to all the neurons in the
 * successor layer. n should point to already allocated memory
 **/

struct nnet *nnet_init(struct nnet *n, int num_layers,
        int *num_neurons) {
    n->num_layers = num_layers;
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

void nnet_set_inputs(struct nnet *n, double *inputs) {
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
        }
    }
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
        for (int j = 0; j < n->new_weights[last_layer][i].length; j++) {
            /* For all weights in the node */
            /* ith component of the output vector */
            /* jth component of the input vector */
            double x = n->outputs[last_layer - 1].vec[j];
            /* update the jth component of the weight vector */
            n->new_weights[last_layer][i].vec[j] =
                n->weights[last_layer][i].vec[j] +
                ALPHA * n->errors[last_layer].vec[i] * x;
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
                    ALPHA * n->outputs[i - 1].vec[k] * n->errors[i].vec[j];
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

/* Deterministic version of stochastic gradient descent */
void nnet_train(struct nnet *n, struct example *examples, int num_examples, int iterations) {
    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < num_examples; j++) {
            nnet_set_inputs(n, examples[j].inputs);
            forward_propagate(n);
            backward_propagate(n, examples[j].outputs, examples[j].num_outputs);
        }
    }
}
