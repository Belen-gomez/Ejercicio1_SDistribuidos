BIN_FILES  = servidor cliente1 cliente2 cliente3
LIB_FILE = libclaves.so

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall
LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lrt

all: $(BIN_FILES) $(LIB_FILE)

cliente1: cliente1.o claves.o mensajes.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

cliente2: cliente2.o claves.o mensajes.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

cliente3: cliente3.o claves.o mensajes.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

servidor: servidor.o mensajes.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chmod +x $@

$(LIB_FILE): claves.o
	$(CC) $(LDFLAGS) -shared $^ -o $@
	
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

clean:
	rm -f $(BIN_FILES) $(LIB_FILE) *.o

.PHONY: all clean
