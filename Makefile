CC=gcc

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# NAME=libft_malloc_$(HOSTTYPE).so
NAME=libft_malloc.so
NAME_ARCH=libft_malloc_$(HOSTTYPE).so

CFILES = lib.c utils.c
CFLAGS = -Wall -Wextra -Werror

OBJ_DIR= ./.obj/
OBJ_FILES=$(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(CFILES)))

all: $(NAME)

$(NAME): $(NAME_ARCH)
	ln -s $(NAME_ARCH) $(NAME)

$(NAME_ARCH): $(OBJ_FILES)
	$(CC) --shared $(OBJ_FILES) -o $(NAME_ARCH)

$(OBJ_DIR)%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build: all clean

re: clean all

fclean: clean
	@rm -f $(NAME)

clean:
	@rm -f $(OBJ_FILES)
	@rm -rf $(OBJ_DIR)
