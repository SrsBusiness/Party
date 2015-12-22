#ifndef NEURONS_H
#define NEURONS_H

struct vector {
    int length;
    double *vec;
};

/* net has layers, layers have neurons, neurons have weights
 * Relevant data: weights and outputs 
 * Every layer has an additional implicit output of 1.0 (for w0)
 **/
struct nnet {
    int num_layers;
    struct vector **weights;
    struct vector **new_weights;
    struct vector *outputs;
    struct vector *errors;
};

struct example {
    int num_inputs, num_outputs;
    double *inputs;
    double *outputs;
};

double loss2(const struct vector *a, const struct vector *b);
double dot(const struct vector *a, const struct vector *b);
double activation(const struct vector *w, const struct vector *x);
void forward_propagate(struct nnet *n);
void backward_propagate(struct nnet *n, const double *key, int length);
struct nnet *nnet_init(struct nnet *n, int num_layers,
        int *num_neurons);
void nnet_free(struct nnet *n);
void nnet_set_inputs(struct nnet *n, double *inputs);
void nnet_train(struct nnet *, struct example *, int, int);

#endif
