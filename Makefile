CC=gcc

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# NAME=libft_malloc_$(HOSTTYPE).so
NAME=libft_malloc.so
NAME_ARCH=libft_malloc_$(HOSTTYPE).so

CFILES = lib.c utils.c
CFLAGS = -Wall -g -Wextra  -fPIC -O0

OBJ_DIR= ./.obj/
OBJ_FILES=$(addprefix $(OBJ_DIR), $(patsubst %.c, %.o, $(CFILES)))

all: $(NAME)

$(NAME): $(NAME_ARCH)
	ln -sf $(NAME_ARCH) $(NAME)

$(NAME_ARCH): $(OBJ_FILES)
	$(CC) -fPIC --shared $(OBJ_FILES) -o $(NAME_ARCH) -lm

$(OBJ_DIR)%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(NAME) main.c
	gcc -O0 -g -L. -lft_malloc -Wl,-rpath, main.c -o main

build: all clean

re: clean all

fclean: clean
	@rm -f $(NAME) $(NAME_ARCH)

clean:
	@rm -f $(OBJ_FILES)
	@rm -rf $(OBJ_DIR)
	@rm -f main
