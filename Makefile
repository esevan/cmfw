IDIR =./inc
SDIR =./src
CC=g++
CFLAGS=-I$(IDIR)
OPT=-Wall

ODIR=$(SDIR)/obj
LDIR =./lib

LIBS=-lm -luv -lbluetooth -fpermissive

TEST_DIR=test
TEST_BIN=./test/bin
TEST_ODIR=./test/obj

##Compile for comm_bt.c
#$(ODIR)/comm_bt.o: $(SDIR)/comm_bt.c
#	gcc -c -fPIC -o $@ $< $(CFLAGS) $(OPT) $(LIBS)
#
#comm_bt.o: $(ODIR)/comm_bt.o
#
#$(LDIR)/libcommbt.a: $(ODIR)/comm_bt.o
#	ar rcs $@ $^
#
#libcommbt.a: $(LDIR)/libcommbt.a
#libcommbt.so: comm_bt.o
#	gcc -shared -Wl,-soname,libcommbt.so.1 -o $(LDIR)/libcommbt.so $(ODIR)/comm_bt.o
#
##Compile for comm_core.c
#$(ODIR)/comm_core.o: $(SDIR)/comm_core.cpp 
#	$(CC) -c -fPIC $^ $(CFLAGS) $(OPT) -o $@ $(LIBS) -lcommbt
#
#comm_core.o: $(ODIR)/comm_core.o
#
#libcommcore.a: comm_bt.o comm_core.o
#	ar rcs $(LDIR)/$@ $(ODIR)/comm_bt.o $(ODIR)/comm_core.o
#
#libcommcore.so.1: comm_bt.o comm_core.o
#	gcc -shared -Wl,-soname,libcommcore.so.1 -o $(LDIR)/libcommcore.so $(IDIR)/comm_util.h $(ODIR)/comm_bt.o $(ODIR)/comm_core.o


test_bin=$(patsubst %,$(TEST_BIN)/%,$(_test))
$(TEST_BIN)/%:$(TEST_DIR)/%.c
	$(CC) -g src/comm_bt.cc src/comm_core.cc $< $(CFLAGS) -I$(IDIR) -o $@ $(LIBS)

_msg_test=msg_server msg_client
msg_bin=$(patsubst %,$(TEST_BIN)/%,$(_msg_test))
msg_test:$(msg_bin)
_file_test=file_server file_client
file_bin=$(patsubst %,$(TEST_BIN)/%,$(_file_test))
file_test:$(file_bin)
_companion=companion_server
companion_bin=$(patsubst %,$(TEST_BIN)/%,$(_companion));
companion_test:$(companion_bin)

test:msg_test file_test




.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(TEST_BIN)/msg_server $(TEST_BIN)/msg_client $(TEST_BIN)/file_server $(TEST_BIN)/file_client
