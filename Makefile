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
BIN = $(BINDIR)/$(PROJNAME)

SUBMITNAME = $(PROJNAME).zip
zip = zip

all: create_dirs
all: $(BIN).client.bin
all: $(BIN).server.bin

release: CFLAGS = -Iinclude -O2
release: new

$(BIN).client.bin: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -o $@

$(BIN).server.bin: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $@

$(OBJ)/client/%.o: $(SRC)/client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/server/%.o: $(SRC)/server/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#%.o: $(SRC)/%.c
#	$(CC) $(CFLAGS) -c $< -o $(OBJ)/$@	

#link: $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)

clean:
	rm -r $(BINDIR) $(OBJ)

create_dirs:
	@mkdir -p $(BINDIR) $(OBJ) $(OBJ)/client $(OBJ)/server

new: clean
new: all

submit:
	rm -f $(SUBMITNAME)
	zip $(SUBMITNAME) $(BIN)