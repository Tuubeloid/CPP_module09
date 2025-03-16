C = c++
CFLAGS = -Wall -Wextra -Werror -std=c++11
DEBUG_FLAGS = -g -O0

NAME = btc

INCLUDES = BitcoinExchange.hpp
SRCS = main.cpp BitcoinExchange.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(C) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(C) $(CFLAGS) -c $< -o $@

debug: $(OBJ)
	$(C) $(CFLAGS) $(DEBUG_FLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug