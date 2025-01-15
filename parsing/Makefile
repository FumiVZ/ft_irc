NAME = a.out
CFLAGS = -Wall -Wextra -Werror -g3
LFLAGS = -std=c++98
BUILD_DIR = build
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(BUILD_DIR)/%.o)
HEADERS = $(wildcard *.hpp)

all: $(BUILD_DIR) $(NAME)

$(BUILD_DIR):
		mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
		c++ -c $(CFLAGS) $< -o $@

$(NAME): $(OBJ) $(HEADERS)
		c++ $(CFLAGS) $(OBJ) -o $(NAME) $(LFLAGS)

clean:
		rm -rf $(BUILD_DIR)

fclean: clean
		rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re