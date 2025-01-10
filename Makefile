NAME = ircserv
CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3

BUILD_DIR = build
SRCS_DIR = code/
CLIENT_DIR = $(SRCS_DIR)client/
SERVER_DIR = $(SRCS_DIR)server/

HEADERS_DIR = header/
CLIENT_HEADERS = $(addprefix $(CLIENT_DIR), $(HEADERS_DIR)client.hpp)
SERVER_HEADERS = $(addprefix $(SERVER_DIR), $(HEADERS_DIR)server.hpp)
ALL_HEADERS = $(CLIENT_HEADERS) $(SERVER_HEADERS)

MAIN_SRC = $(SRCS_DIR)main.cpp
CLIENT_SRC = $(addprefix $(CLIENT_DIR), client.cpp)
SERVER_SRC = $(addprefix $(SERVER_DIR), server.cpp parse_cmd.cpp)
SRCS = $(MAIN_SRC) $(CLIENT_SRC) $(SERVER_SRC)

INC_DIR = -I$(CLIENT_DIR)header -I$(SERVER_DIR)header

OBJS = $(patsubst $(SRCS_DIR)%.cpp,$(BUILD_DIR)/$(SRCS_DIR)%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/$(SRCS_DIR)
	@mkdir -p $(BUILD_DIR)/$(CLIENT_DIR)
	@mkdir -p $(BUILD_DIR)/$(SERVER_DIR)

$(NAME): $(BUILD_DIR) $(OBJS)
	@$(CC) $(CPPFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "Build complete: $(NAME)"

$(BUILD_DIR)/$(SRCS_DIR)%.o: $(SRCS_DIR)%.cpp $(ALL_HEADERS)
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(INC_DIR) -c $< -o $@
	@echo "Compiled: $<"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

fclean: clean
	@rm -rf $(NAME)
	@echo "Full clean complete"

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re