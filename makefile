CC = g++
OBJ_DIR = bin
SRC_DIR = src
C_OBJ_FILE_NAMES = Utility.o TorrentClient.o torrent_client.o
S_OBJ_FILE_NAMES = Utility.o TorrentServer.o torrent_server.o
T_OBJ_FILE_NAMES = Utility.o create_torrent_file.o
C_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(C_OBJ_FILE_NAMES))
S_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(S_OBJ_FILE_NAMES))
T_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(T_OBJ_FILE_NAMES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -g $< -o $@

all : client server torrent_file

client : $(C_OBJ_FILES)
	$(CC) -o $@ $^ -lcrypto

server : $(S_OBJ_FILES)
	$(CC) -o $@ $^ -lcrypto -lpthread

torrent_file : $(T_OBJ_FILES)
	$(CC) -o $@ $^ -lcrypto

clean:
	-rm $(OBJ_DIR)/*.o
