INC = ../inc
OBJ_DIR = ../src/obj
LIB = -luv -lbluetooth
SRC_DIR = src
BIN_DIR = bin

$(BIN_DIR)/%:$(SRC_DIR)/%.cpp
	g++ -o $@ $(OBJ_DIR)/* $< -I$(INC) $(LIB)
__test = msg_test_s msg_test_c file_test_s file_test_c
_test = $(patsubst %, $(BIN_DIR)/%, $(__test))
test:$(_test)

.PHONY: clean
clean:
	rm $(BIN_DIR)/*
