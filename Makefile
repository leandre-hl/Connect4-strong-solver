CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

# Object files directory
OBJDIR = obj

# Source files
SRC = main.c src/game.c src/user_interface.c src/ai.c src/transposition_table.c

# Object files (in OBJDIR)
OBJ = $(SRC:.c=.o)
OBJ := $(addprefix $(OBJDIR)/, $(OBJ))

TARGET = connect4

.PHONY: all compile run clean

all: compile run

compile: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)