#include "minicurses.h"
/* for debugging purposes */
void display_bboard(bboard b, int row, int col) {
    int i;
    for(i = 0; i < 64; i++) {
        move_cursor(7 - (i / 8) + row, ((i & 7) << 1) + col);
        if((1lu << i) & b)
            putchar('#');
        else
            putchar('_');
    }
    fflush(stdin);
}

void display_text(char *string, int row, int col){
    move_cursor(row, col);
    printf("%s\n", string);
}
