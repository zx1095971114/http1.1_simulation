SRC_DIR := src
OBJ_DIR := obj
# all src files
SRC := $(wildcard $(SRC_DIR)/*.c)
# all objects
OBJ := $(OBJ_DIR)/y.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/example.o $(OBJ_DIR)/log.o $(OBJ_DIR)/send_code.o $(OBJ_DIR)/my_segment.o $(OBJ_DIR)/file_motion.o
# all binaries
BIN := example echo_server echo_client
# C compiler
CC  := gcc
# C PreProcessor Flag
CPPFLAGS := -Iinclude
# compiler flags
CFLAGS   := -g -Wall
# DEPS = parse.h y.tab.h

default: all
all : example echo_server echo_client

example: $(OBJ)
	$(CC) $^ -o $@

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l
	flex -o $@ $^

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/parser.y
	yacc -d $^
	mv y.tab.c $@
	mv y.tab.h $(SRC_DIR)/y.tab.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/log.c -o obj/log.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/send_code.c -o obj/send_code.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/my_segment.c -o obj/my_segment.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/file_motion.c -o obj/file_motion.o

echo_server: $(OBJ_DIR)/echo_server.o
	$(CC) -Werror -I ./include  $^ obj/parse.o obj/y.tab.o obj/lex.yy.o obj/log.o obj/send_code.o obj/my_segment.o obj/file_motion.o -o $@

echo_client: $(OBJ_DIR)/echo_client.o
	$(CC) -Werror $^ -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ) $(BIN) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.*
	$(RM) -r $(OBJ_DIR)
