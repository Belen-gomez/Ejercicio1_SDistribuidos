BIN_FILES  = servidor cliente

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lrt


cliente: cliente.o $(LIB_FILE)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

servidor: servidor.o $(LIB_FILE)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

$(LIB_FILE): claves.o
	$(CC) $(LDFLAGS) -shared $^ -o $@

servidor.o: servidor.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

clean:
	rm -f $(BIN_FILES) $(LIB_FILE) *.o

.PHONY: clean
