# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mgoudin <mgoudin@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/10 16:21:49 by mgoudin           #+#    #+#              #
#    Updated: 2023/05/12 17:34:56 by mgoudin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ircserv

SRCS	= 	main.cpp 

OBJS	= ${SRCS:.cpp=.o}

CC		= c++
RM		= rm -f

CFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic

%.o: %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME):	$(OBJS)
	$(CC) -o $@ $^ 

clean:
	${RM} ${OBJS}

fclean:	clean
	${RM} ${NAME}

re: fclean all

.PHONY:	all clean fclean re