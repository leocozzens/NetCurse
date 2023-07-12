CC = gcc
CFLAGS = -Iinclude -g -Wall
SRC = src
OBJ = obj
BINDIR = bin
PROJNAME = TextLink
CLIENT_SRCS = $(wildcard $(SRC)/client/*.c)
CLIENT_OBJS = $(patsubst $(SRC)/client/%.c, $(OBJ)/client/%.o, $(CLIENT_SRCS))
SERVER_SRCS = $(wildcard $(SRC)/server/*.c)
SERVER_OBJS = $(patsubst $(SRC)/server/%.c, $(OBJ)/server/%.o, $(SERVER_SRCS))
SHARED = $(wildcard $(SRC)/*.c)
BIN = $(BINDIR)/$(PROJNAME)

SUBMITNAME = $(PROJNAME).zip
zip = zip

all: create_dirs
all: $(BIN).client.bin
all: $(BIN).server.bin

+: all
+: jclient

release: CFLAGS = -Iinclude -O2
release: new

$(BIN).client.bin: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -Isrc/client/include $(CLIENT_OBJS) $(SHARED) -o $@

$(BIN).server.bin: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -Isrc/server/include $(SERVER_OBJS) $(SHARED) -o $@

$(OBJ)/client/%.o: $(SRC)/client/%.c
	$(CC) $(CFLAGS) -Isrc/client/include -c $< -o $@

$(OBJ)/server/%.o: $(SRC)/server/%.c
	$(CC) $(CFLAGS) -Isrc/server/include -c $< -o $@

jclient:
	ant -f java_client/build.xml

clean:
	rm -r $(BINDIR) $(OBJ)

create_dirs:
	@mkdir -p $(BINDIR) $(OBJ) $(OBJ)/client $(OBJ)/server

new: clean
new: all

submit:
	rm -f $(SUBMITNAME)
	zip $(SUBMITNAME) $(BIN)