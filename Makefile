BLUE = \033[0;94m
GREEN = \e[1;92m
RED = \033[0;91m
RESET = \033[1;30m
WHITE = \033[1;97m
YELLOW = \033[1;33m
EMOJI_PACKAGE = \360\237\223\246
EMOJI_HAMMER = \360\237\224\250
EMOJI_TRASH = \360\237\227\221\357\270\217


CC = gcc
CCFLAGS = -Werror -Wextra -Wall -O3 -g $(INC)

LIB_PATH = lib
LIB = $(LIB_PATH)/libdiogo.a

NAME = ims
SRC_PATH = src/
OBJ_PATH = obj/

INC = $(addprefix -I, $(shell find . -type d))
SRC = $(wildcard $(SRC_PATH)*.c $(SRC_PATH)**/*.c)
OBJ = $(SRC:$(SRC_PATH)%.c=$(OBJ_PATH)%.o)

all: $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c | $(OBJ_PATH)
	 @mkdir -p $(dir $@)
	 @$(CC) $(CCFLAGS) $(INC) -c $< -o $@
	 @printf "$(EMOJI_HAMMER)	$(BLUE)Compiling $(WHITE)$(NAME)		$(BLUE)%-33s$(WHITE)\r" $(notdir $@)

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

$(LIB):
	@make -sC $(LIB_PATH)

$(NAME): $(LIB) $(OBJ)
	@$(CC) $(CCFLAGS) -o $(NAME) $(OBJ) $(LIB)
	@printf  "\n$(EMOJI_PACKAGE)	$(WHITE)$(NAME)				$(YELLOW)compiled$(WHITE)\n"

clean:
	@rm -rf $(OBJ_PATH)
	@make clean -sC $(LIB_PATH)

fclean: clean
	@rm -f $(NAME)
	@make fclean -sC $(LIB_PATH)
	@printf "$(EMOJI_TRASH)	$(WHITE)$(NAME)				$(RED)removed$(WHITE)\n"

re: fclean all

valgrind: all
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all ./$(NAME)

.PHONY: all re clean fclean valgrind