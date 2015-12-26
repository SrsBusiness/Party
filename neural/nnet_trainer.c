#include <stdio.h>

int FEN_to_arr(const char *, double *);

int main() {
    //char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char *fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    double input[64];
    if (FEN_to_arr(fen, input)) {
        printf("Success\n");
    } else {
        printf("Failure\n");
    }

    for (int i = 7; i >= 0; i--) { /* rank */
        for (int j = 0; j < 8; j++) { /* file */
            printf("%f ", input[i * 8 + j]);
        }
        printf("\n");
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

