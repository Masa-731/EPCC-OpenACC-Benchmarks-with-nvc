BASE_DIR := $(shell pwd)

BIN_DIR := $(BASE_DIR)/bin
SRC_DIR := $(BASE_DIR)/src

default:
	cd $(SRC_DIR);		make COMP=$(COMP);\
	mv oa $(BIN_DIR)

run:
	cd $(BIN_DIR); ./oa

clean:
	cd $(SRC_DIR); 		make clean
	cd $(BIN_DIR);		rm -f *