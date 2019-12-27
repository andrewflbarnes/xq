# Lifted from https://github.com/TheNetAdmin/Makefile-Templates SmallProject

# tool marcros
CC := gcc
CCFLAG :=
CLDFLAG := $(shell xml2-config --libs)
DBGFLAG := -g
CCOBJFLAG := $(CCFLAG) -c $(shell xml2-config --cflags)

# path marcros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# compile marcros
TARGET_NAME := xq
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets
$(TARGET): $(OBJ)
	$(CC) $(CLDFLAG) $(CCFLAG) -o $@ $?

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) $(DBGFLAG) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CCFLAG) $(DBGFLAG) $? -o $@

$(BIN_PATH) $(OBJ_PATH):
	mkdir $@

# phony rules
.PHONY: all
all: $(BIN_PATH) $(OBJ_PATH) $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
