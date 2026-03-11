##
## EPITECH PROJECT, 2026
## makefile
## File description:
## makefile
##


CXX         = g++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++17 -I./include -pthread

SERVER_NAME = server
CLIENT_NAME = client

BUILD_DIR   = build

SERVER_SRC  =   src/server/main.cpp \
                $(wildcard src/server/*.cpp) \
                $(wildcard src/common/*.cpp)  \
                $(wildcard src/ecs/*.cpp) \
                $(wildcard src/components/*.cpp)  \
                $(wildcard src/systems/*.cpp) \
                $(wildcard src/network/*.cpp) \
                $(wildcard src/command/*.cpp)

CLIENT_SRC  =   src/client/main.cpp \
                $(wildcard src/client/*.cpp)    \
                $(wildcard src/common/*.cpp)    \
                $(wildcard src/ecs/*.cpp)   \
                $(wildcard src/components/*.cpp)    \
                $(wildcard src/systems/*.cpp)   \
                $(wildcard src/command/*.cpp)

SERVER_OBJ  = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ  = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRC))

all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(SERVER_OBJ)
    $(CXX) $(SERVER_OBJ) -o $(SERVER_NAME) $(CXXFLAGS)

$(CLIENT_NAME): $(CLIENT_OBJ)
    $(CXX) $(CLIENT_OBJ) -o $(CLIENT_NAME) $(CXXFLAGS)

$(BUILD_DIR)/%.o: %.cpp
    @mkdir -p $(dir $@)
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
    rm -rf $(BUILD_DIR)

fclean: clean
    rm -f $(SERVER_NAME) $(CLIENT_NAME)

re: fclean all

.PHONY: all clean fclean re
