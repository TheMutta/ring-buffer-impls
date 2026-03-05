BIN := ringbuf.test
LIB := libringbuf.a
OBJ := ringbuf.o

lib: src/ringbuf.c
	$(CC) -c src/ringbuf.c -I./lib -o $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

test: lib
	$(CC) test/test.c -L. -lringbuf -I./lib -o $(BIN)

clean: 
	rm -rf $(BIN) $(LIB) $(OBJ)
