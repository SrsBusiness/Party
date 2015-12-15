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
   struct vector *outputs;
};

double loss2(struct vector *a, struct vector *b);
double dot(struct vector *a, struct vector *b);
double activation(struct vector *w, struct vector *x);
void forward_propogate(struct nnet *n);
void back_propogate();
struct nnet *nnet_init(struct nnet *n, int num_layers,
        int *num_neurons);
void nnet_free(struct nnet *n);
void nnet_set_inputs(struct nnet *n, double *inputs);

#endif
