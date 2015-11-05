#include "eval.h"

/* Simplest naive eval */
const int material_values[] = {-1, 9, 3, 3, 5, 1};
uint32_t compute_material(struct board_state *pos) {
    return 0;
    uint32_t material[2] = {0};
    int color, piece;
    for (color = 0; color < 2; color++) {
        for (piece = QUEEN; piece < ALL; piece++) {
            material[color] += popcnt(pos->bb[color][piece]) *
                material_values[piece];
        }
    }
    return 1000 * (material[0] - material[1]);
}
