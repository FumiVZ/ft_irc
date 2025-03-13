NAME            = bot

CC                      = c++
CFLAGS          = -Wall -Wextra -Werror -std=c++98

BUILD_DIR       = build

SRC                     = main.cpp \
	Wordle.cpp

HEADER          = Wordle.hpp

OBJ                     = $(SRC:%.cpp=$(BUILD_DIR)/%.o)

all:            $(NAME)

$(BUILD_DIR):
						mkdir -p $(BUILD_DIR)

$(NAME):        $(BUILD_DIR) $(OBJ) $(HEADER)
						$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(BUILD_DIR)/%.o:               %.cpp
						$(CC) $(CFLAGS) -c $< -o $@

clean:
						rm -rf $(BUILD_DIR)

fclean:         clean
						rm -f $(NAME)

re:                     fclean all

.PHONY:         all clean fclean re