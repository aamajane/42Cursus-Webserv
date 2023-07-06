# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aamajane <aamajane@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/06 16:14:19 by aamajane          #+#    #+#              #
#    Updated: 2023/07/06 16:14:20 by aamajane         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv

CC		=	c++

CFLAGS	=	-Wall -Wextra -Werror -Wshadow -std=c++98

RM		=	rm -rf

OBJS	=	$(SRCS:.cpp=.o)

SRCS	=	src/webserv.cpp \
			src/utils.cpp \
			src/configuration/Location.cpp \
			src/configuration/ServerParser.cpp \
			src/configuration/ConfigServer.cpp \
			src/networking/ManageServers.cpp \
			src/networking/Client.cpp \
			src/request/Request.cpp \
			src/response/Response.cpp \
			src/response/MimeTypes.cpp \
			src/response/methods/Get.cpp \
			src/response/methods/Post.cpp \
			src/response/methods/Delete.cpp \
			src/response/Cgi.cpp


all:		$(NAME)

$(NAME):	$(OBJS)
			@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
			@mkdir -p www/uploads

.cpp.o:		${SRCS}
			@$(CC) $(FLAGS) -c $< -o $@

clean:
			@(RM) $(OBJS)

fclean:		clean
			@(RM) $(NAME)
			@(RM) www/uploads

re:			fclean all

.PHONY:		all clean fclean re
