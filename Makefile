all: 
	gcc -g -o test test.c search.c bitboard.c pawn.c bishop.c rook.c knight.c magic.c debug.c -std=gnu99

debug:
	gcc -g -o test test.c search.c bitboard.c pawn.c bishop.c rook.c knight.c magic.c debug.c -std=gnu99 -DDEBUG

	
