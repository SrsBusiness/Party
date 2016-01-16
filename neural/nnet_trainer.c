#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "neurons.h"

int FEN_to_arr(const char *, double *);

int main() {
    srand(time(NULL));
    FILE *f = fopen("scored_examples_final", "r");
    char line[512];
    /* Divide scores by 1000 */
    int num_positions = 0;
    while (fgets(line, sizeof(line), f)) {
        num_positions++;
    }
    printf("num_positions: %d\n", num_positions);
    double board[64];
    //struct example *examples = malloc(num_positions * sizeof(struct example));
    fseek(f, 0, SEEK_SET);
    FILE *out = fopen("fann_examples", "w");
    fprintf(out, "%d %d %d\n", num_positions, 64, 1);
    /* Initialize examples set */
    for (int i = 0; i < num_positions; i++) {
        fgets(line, sizeof(line), f);
        int score;
        char fen[512];
        sscanf(line, "%d %s", &score, fen);
        double score_d = (double)score;
        assert(FEN_to_arr(fen, board));
        /* Input */
        for (int j = 0; j < 64; j++) {
            fprintf(out, "%f ", board[j]);
        }
        fprintf(out, "\n");
        /* Outputs */
        fprintf(out, "%f\n", score_d);
        //examples[i].num_inputs = 64;
        //examples[i].num_outputs = 1;
        //examples[i].inputs = malloc(64 * sizeof(double));
        //assert(FEN_to_arr(fen, examples[i].inputs));
        //examples[i].outputs = malloc(1 * sizeof(double));
        //examples[i].outputs[0] = score_d;
    }

//    /* partition examples into training and validation set */
//    struct example *validation = malloc(num_positions / 10 * sizeof(struct example));
//    partition_examples(examples, validation, num_positions, num_positions / 10);
//    int training_size = num_positions - num_positions / 10;
//    int validation_size = num_positions / 10;
//    struct nnet n; 
//    int num_neurons[] = {64, 1024, 1};
//    nnet_init(&n, 3, num_neurons, .3);
//    //nnet_load(&n, "chess_eval");
//    printf("Layers: %d\n", n.num_layers);
//    printf("Neurons per layer: ");
//    for (int i = 0; i < n.num_layers; i++) {
//        printf("%d ", n.outputs[i].length - 1); 
//    }
//    printf("\n");
//    for (int i = 0; i < 1000; i++) {
//        double training_error;
//        printf("Training\n");
//        nnet_train(&n, examples, 2, 100000, &training_error);
//        nnet_save(&n, "chess_eval");
//        printf("Computing Error\n");
//        printf("Training Error: %f\n", training_error);
//        printf("Validation Error: %f\n", rms_error(&n, examples, 10));
//    }
}

int FEN_to_arr(const char *fen, double *input) {
    int index = 0, rank = 7, file = 0;
    while(fen[index]) {
        if (file > 7) {
            if (file != 8) { /* shouldn't happen */
                break;
            }
            file = 0;
            rank--;
            if (rank == -1) { /* successfuly parsed */
                return 1;
            }
            /* expect a '/' as the next char */
            if (fen[index] != '/') {
                break;
            }
            index++;
        }
        int empty;
        switch(fen[index]) {
            case 'k':
                input[rank * 8 + file] = -6.0 / 14;
                file++;
                break;
            case 'q':
                input[rank * 8 + file] = -5.0 / 14;
                file++;
                break;
            case 'b':
                input[rank * 8 + file] = -4.0 / 14;
                file++;
                break;
            case 'n':
                input[rank * 8 + file] = -3.0 / 14;
                file++;
                break;
            case 'r':
                input[rank * 8 + file] = -2.0 / 14;
                file++;
                break;
            case 'p':
                input[rank * 8 + file] = -1.0 / 14;
                file++;
                break;
            case 'K':
                input[rank * 8 + file] = 6.0 / 14;
                file++;
                break;
            case 'Q':
                input[rank * 8 + file] = 5.0 / 14;
                file++;
                break;
            case 'B':
                input[rank * 8 + file] = 4.0 / 14;
                file++;
                break;
            case 'N':
                input[rank * 8 + file] = 3.0 / 14;
                file++;
                break;
            case 'R':
                input[rank * 8 + file] = 2.0 / 14;
                file++;
                break;
            case 'P':
                input[rank * 8 + file] = 1.0 / 14;
                file++;
                break;
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': 
                empty = fen[index] - '0';
                for (int i = 0; i < empty; i++) {
                    input[rank * 8 + file + i] = 0.0;
                }
                file += empty;
                break;
            default:
                return 0;
        }
        index++;
    }
    return 1;
}

