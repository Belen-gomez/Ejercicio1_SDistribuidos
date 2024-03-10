BIN_FILES  = servidor cliente
LIB_FILE = libclaves.so

CC = gcc


CPPFLAGS = -I$(INSTALL_PATH)/include -Wall
LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lrt

all: $(BIN_FILES) $(LIB_FILE)

cliente: cliente.o claves.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

servidor: servidor.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

$(LIB_FILE): claves.o
	$(CC) $(LDFLAGS) -shared $^ -o $@
	
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

clean:
	rm -f $(BIN_FILES) $(LIB_FILE) *.o

.PHONY: all clean
