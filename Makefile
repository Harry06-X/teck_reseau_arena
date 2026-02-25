##
## EPITECH PROJECT, 2026
## teck_reseau_arena
## File description:
## Makefile
##

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -I./include -pthread

SERVER_NAME = server
CLIENT_NAME = client

COMMON_SRC = src/common/Player.cpp \
             src/common/Warrior.cpp \
             src/common/Archer.cpp \
             src/common/Command.cpp

SERVER_SRC = src/server/main.cpp \
             src/server/Server.cpp \
             src/server/Game.cpp \
             src/server/ClientSession.cpp \
             $(COMMON_SRC)

CLIENT_SRC = src/client/main.cpp \
             $(COMMON_SRC)

SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(SERVER_OBJ)
				$(CXX) $(SERVER_OBJ) -o $(SERVER_NAME) $(CXXFLAGS)

$(CLIENT_NAME): $(CLIENT_OBJ)
				$(CXX) $(CLIENT_OBJ) -o $(CLIENT_NAME) $(CXXFLAGS)

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ)

fclean: clean
	rm -f $(SERVER_NAME) $(CLIENT_NAME)

re: fclean all

.PHONY: all clean fclean re