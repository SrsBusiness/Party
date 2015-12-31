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
    struct example *examples = malloc(num_positions * sizeof(struct example));
    fseek(f, 0, SEEK_SET);

    /* Initialize examples set */
    for (int i = 0; i < num_positions; i++) {
        fgets(line, sizeof(line), f);
        int score;
        char fen[512];
        sscanf(line, "%d %s", &score, fen);
        double score_d = (double)score;
        examples[i].num_inputs = 64;
        examples[i].num_outputs = 1;
        examples[i].inputs = malloc(64 * sizeof(double));
        assert(FEN_to_arr(fen, examples[i].inputs));
        examples[i].outputs = malloc(1 * sizeof(double));
        examples[i].outputs[0] = score_d;
    }
    struct nnet n; 
    int num_neurons[] = {64, 256, 1};
    nnet_init(&n, 3, num_neurons, .3);
    for (int i = 0; i < 1000; i++) {
        nnet_train(&n, examples, 2, 1000);
        nnet_save(&n, "chess_eval");
        printf("Error: %f\n", rms_error(&n, examples, 1));
    }
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

