BLUE = \033[0;94m
GREEN = \e[1;92m
RED = \033[0;91m
RESET = \033[1;30m
WHITE = \033[1;97m
YELLOW = \033[1;33m
EMOJI_PACKAGE = \360\237\223\246
EMOJI_HAMMER = \360\237\224\250
EMOJI_TRASH = \360\237\227\221\357\270\217

CLIENT = cliente
SERVER = servidor
CC = gcc
CCFLAGS = #-Werror -Wextra -Wall -O3 -g

SRC_PATH = src/
OBJ_PATH = obj/

CLIENT_SRC = src/cliente.c
SERVER_SRC = src/servidor.c
CLIENT_OBJ = obj/cliente.o
SERVER_OBJ = obj/servidor.o

all: $(CLIENT) $(SERVER)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c | $(OBJ_PATH)
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c $< -o $@
	@printf "$(EMOJI_HAMMER)	$(BLUE)Compiling $(WHITE)$(@F)		$(BLUE)%-33s$(WHITE)\r" $(notdir $@)

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)

$(CLIENT): $(CLIENT_OBJ) | $(OBJ_PATH)
	@$(CC) $(CCFLAGS) -o $(CLIENT) $(CLIENT_OBJ)
	@printf "\n$(EMOJI_PACKAGE)	$(WHITE)$(CLIENT)				$(YELLOW)compiled$(WHITE)\n"

$(SERVER): $(SERVER_OBJ) | $(OBJ_PATH)
	@$(CC) $(CCFLAGS) -o $(SERVER) $(SERVER_OBJ)
	@printf "\n$(EMOJI_PACKAGE)	$(WHITE)$(SERVER)			$(YELLOW)compiled$(WHITE)\n"

clean:
	@rm -rf $(OBJ_PATH)
	@printf "$(EMOJI_TRASH)	$(WHITE)Object files			$(RED)removed$(WHITE)\n"

fclean: clean
	@rm -f $(CLIENT) $(SERVER)
	@printf "$(EMOJI_TRASH)	$(WHITE)Executables			$(RED)removed$(WHITE)\n"

re: fclean all

valgrind_client: $(CLIENT)
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all ./$(CLIENT)

valgrind_server: $(SERVER)
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all ./$(SERVER)

.PHONY: all re clean fclean valgrind_client valgrind_server