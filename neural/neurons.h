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
    double alpha;
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

double dot(const double *a, const double *b, int length);
double activation(const struct vector *w, const struct vector *x);
void forward_propagate(struct nnet *n);
void backward_propagate(struct nnet *n, const double *key, int length);
struct nnet *nnet_init(struct nnet *n, int num_layers,
        int *num_neurons, double alpha);
void nnet_free(struct nnet *n);
void nnet_set_inputs(struct nnet *n, double *inputs, int length);
void nnet_train(struct nnet *, struct example *, int, int);
double rms_error(struct nnet *n, struct example *examples, int num_examples);
void nnet_save(struct nnet *n, const char *filename); 
struct nnet *nnet_load(struct nnet *n, const char *filename);
#endif
