CC = g++
OBJ_DIR = bin
SRC_DIR = src
C_OBJ_FILE_NAMES = Utility.o TorrentClient.o TorrentServer.o torrent_client.o
C_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(C_OBJ_FILE_NAMES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -g $< -o $@

all : client

client : $(C_OBJ_FILES)
	$(CC) -o $@ $^ -lcrypto -lpthread

clean:
	-rm $(OBJ_DIR)/*.o
