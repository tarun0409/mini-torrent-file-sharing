CC = g++
OBJ_DIR = bin
SRC_DIR = src
C_OBJ_FILE_NAMES = Utility.o TrackerClient.o TorrentClient.o TorrentServer.o torrent_client.o
T_OBJ_FILE_NAMES = Utility.o TorrentTracker.o torrent_tracker.o
C_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(C_OBJ_FILE_NAMES))
T_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(T_OBJ_FILE_NAMES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -g $< -o $@

all : client tracker

client : $(C_OBJ_FILES)
	$(CC) -o $@ $^ -lcrypto -lpthread

tracker : $(T_OBJ_FILES)
	$(CC) -o $@ $^ -lcrypto -lpthread

clean:
	-rm $(OBJ_DIR)/*.o client tracker
