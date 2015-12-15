#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "neurons.h"


/* Computes the sum of squares loss (difference) of the two vectors */
double loss2(struct vector *a, struct vector *b) {
    assert(a->length == b->length);
    double loss = 0.0;
    for (int i = 0; i < a->length; i++) {
        loss += pow(a->vec[i] - b->vec[i], 2);
    }
    return loss;
}

double dot(struct vector *a, struct vector *b) {
    assert(a->length == b->length);
    double product = 0;
    for (int i = 0; i < a->length; i++) {
        product += a->vec[i] * b->vec[i];
    }
    return product;
}

/* Logistic activation function given weights w and inputs x */
double activation(struct vector *w, struct vector *x) {
    return 1.0 / (1.0 + pow(M_E, -dot(w, x)));
}

/* random double between 0 and 1 */
static double rand_double() {
    return (double)rand() / (double)RAND_MAX;
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
    n->outputs = malloc(num_layers * sizeof(struct vector));
    
    /* layer 0: only the outputs are important, no weights */
    n->outputs[0].length = num_neurons[0] + 1;
    n->outputs[0].vec = malloc(n->outputs[0].length * sizeof(double));
    n->outputs[0].vec[0] = 1.0;

    for (int i = 1; i < num_layers; i++) {
        /* For each layer */
        n->outputs[i].length = num_neurons[i] + 1;
        n->outputs[i].vec = malloc(n->outputs[i].length * sizeof(double));
        n->outputs[i].vec[0] = 1.0;
        n->weights[i] = malloc(n->outputs[i].length * sizeof(struct vector));
        for (int j = 1; j < n->outputs[i].length; j++) {
            /* For each neuron */
            n->weights[i][j].length = n->outputs[i - 1].length;
            n->weights[i][j].vec =
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
        }
        free(n->outputs[i].vec);
        free(n->weights[i]);
    }
    free(n->weights);
    free(n->outputs);
}

void nnet_set_inputs(struct nnet *n, double *inputs) {
    for (int i = 1; i < n->outputs[0].length; i++) {
        n->outputs[0].vec[i] = inputs[i - 1];
    }
}

/* forward-propogation */
void forward_propogate(struct nnet *n) {
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
void back_propogate(struct nnet *n) {
}
