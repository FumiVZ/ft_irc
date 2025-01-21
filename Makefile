NAME = ircserv
CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

BUILD_DIR = build
SRCS_DIR = code/
SERVER_DIR = $(SRCS_DIR)server/
HEADERS_DIR = header/

SERVER_HEADERS = $(addprefix $(SERVER_DIR), \
					$(addprefix $(HEADERS_DIR), \
						Client.hpp \
						Server.hpp \
						Channel.hpp \
						replies.hpp \
						Message.hpp \
						) \
					)

ALL_HEADERS = $(SERVER_HEADERS)
SERVER_SRCS = $(addprefix $(SERVER_DIR), \
					Client.cpp \
					Server.cpp \
					Parse_cmd.cpp \
					Channel.cpp \
					$(addprefix replies/, \
						$(addprefix errors/, \
							err_needmoreparam.cpp \
							err_nosuchnick.cpp \
							) \
						rpl_welcome.cpp \
						rpl_namreply.cpp \
						rpl_endofnames.cpp \
						rpl_topic.cpp \
						) \
					$(addprefix parsing/, \
						Message.cpp \
						) \
				)

SRCS = $(SERVER_SRCS) \
		$(SRCS_DIR)main.cpp

INC_DIR = -I$(CLIENT_DIR)header -I$(SERVER_DIR)header

OBJS = $(patsubst $(SRCS_DIR)%.cpp,$(BUILD_DIR)/$(SRCS_DIR)%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/$(SRCS_DIR)
	@mkdir -p $(BUILD_DIR)/$(CLIENT_DIR)
	@mkdir -p $(BUILD_DIR)/$(SERVER_DIR)

$(NAME): $(BUILD_DIR) $(OBJS)
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
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