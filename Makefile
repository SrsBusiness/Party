CC		=	clang
OBJFLAGS	=	-c -g -Wall -Iinclude --std=c11
BINFLAGS	=	-g -Wall -Iinclude --std=c11

SRC		=	src
LIB		=	lib
OBJ		=	obj
BIN		=	bin
TARGET		=	$(BIN)/test
SRCFILES	:= $(wildcard $(SRC)/*.c)
OBJECTS 	:= $(patsubst $(SRC)/%.c,$(OBJ)/%.o, $(SRCFILES))

UNIT	=	tests/unit

all: tests

$(TARGET): $(OBJECTS) $(BIN)
	$(CC) $(BINFLAGS) -o $@ test.c $(OBJECTS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(OBJFLAGS) $< -o $@

.PHONY: tests
tests: $(OBJECTS)
	$(CC) $(BINFLAGS) -o $(UNIT)/tests $(OBJECTS) $(UNIT)/*.c -lcmocka
	$(UNIT)/tests

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(TARGET)

$(BIN) $(OBJ):
	mkdir -p $@

debug:
	gcc -g -o test test.c search.c bitboard.c pawn.c bishop.c rook.c knight.c magic.c debug.c -std=gnu99 -DDEBUG

	
