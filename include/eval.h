#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"

/* These are reserved for checkmate */
#define SCORE_MAX (INT32_MAX / 2)
#define SCORE_MIN (-INT32_MAX / 2)


uint32_t compute_material(struct board_state *);

#endif
