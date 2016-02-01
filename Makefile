CC		=	clang
OBJFLAGS	=	-c -g -Wall -Iinclude --std=c11 -O3
BINFLAGS	=	-g -Wall -Iinclude --std=c11 -O3

SRC		=	src
LIB		=	lib
OBJ		=	obj
BIN		=	bin
TARGET		=	$(BIN)/main
SRCFILES	:= $(wildcard $(SRC)/*.c)
OBJECTS 	:= $(patsubst $(SRC)/%.c,$(OBJ)/%.o, $(SRCFILES))

UNIT	=	tests/unit

all: $(TARGET)

debug: OBJFLAGS += -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN)
	$(CC) $(BINFLAGS) -o $@ main.c $(OBJECTS) $(LINKFLAGS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(OBJFLAGS) $< -o $@

.PHONY: tests
test: OBJFLAGS += -DUNIT_TEST
test: LINKFLAGS += -lcmocka
test: $(OBJECTS) $(TARGET)
	$(CC) $(BINFLAGS) -o $(UNIT)/tests $(OBJECTS) $(UNIT)/*.c $(LINKFLAGS)
	$(UNIT)/tests

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(TARGET)

$(BIN) $(OBJ):
	mkdir -p $@
