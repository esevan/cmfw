CC = g++
SRC_DIR = ./src
OBJ_DIR = ./src/obj
LIB = -luv -lbluetooth
INC = ./inc
#OpelMessage.o:$(SRC_DIR)/OpelMessage.cc
#$(CC) -o $(OBJ_DIR)/$@ -c $< -I$(INC)

#OpelSocket.o:$(SRC_DIR)/OpelSocket.cc
#$(CC) -o $(OBJ_DIR

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cc
	$(CC) -o $@ -c $< -I$(INC) $(LIB)

__test=OpelMessage OpelSocket OpelServerSocket OpelSCModel OpelServer OpelClient
_test=$(patsubst %,$(OBJ_DIR)/%.o,$(__test))
test:$(_test)

.PHONY: clean
clean:
	rm $(OBJ_DIR)/*.o
