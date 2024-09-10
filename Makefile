NAME = ircserv
CC = c++
CFLAGS = -std=c++98 #-Wall -Wextra -Werror 

SRCS = main.cpp src/Client.cpp src/Server.cpp src/Auth.cpp 

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp inc/Server.hpp inc/Client.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re