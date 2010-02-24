# Compiler
CC          = /usr/bin/gcc
# Compiler flags
CC_FLAGS    = -c -MD
# Application name
NAME        = mstore
# Source file dir
SRC_DIR     = src
# Objs dir
OBJS_DIR    = objs
# Bin dir
BIN_DIR     = bin
# Source files
SRC_FILES   := $(wildcard $(addsuffix /*.c,$(SRC_DIR)))
SRC_FILES   := $(SRC_FILES) $(SRC_DIR)/parser.c
# Obj files
OBJS        := $(addprefix ./$(OBJS_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))
# Linked libraries
LIBS        = -L /usr/lib/happycoders -lnsl -lsocket
# Dependency files
DEPS        := $(OBJS:%.o=%.d)

.PHONY: clean test rebuild
vpath %.c $(SRC_DIR)

################################################################################

all: $(NAME)

$(NAME): ./$(BIN_DIR)/$(NAME)

./$(BIN_DIR)/$(NAME): $(OBJS)
	[ -d ./$(BIN_DIR) ] || mkdir ./$(BIN_DIR)
	$(CC) $(LIBS) -o $@ $^

./$(OBJS_DIR)/%.o: %.c
	[ -d ./$(OBJS_DIR) ] || mkdir ./$(OBJS_DIR)
	$(CC) $(CC_FLAGS) -o $@ $<

-include $(DEPS)

clean:
	-rm -rf ./$(OBJS_DIR) ./$(BIN_DIR)
	-rm -f $(SRC_DIR)/parser.c

rebuild: clean all

test:
	./test/run
	
$(SRC_DIR)/parser.c: $(SRC_DIR)/parser.rl
	ragel $< -o $@
