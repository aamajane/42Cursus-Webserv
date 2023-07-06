
NAME = webserv

CC = clang++

FLAGS = --std=c++98 -Wall -Wextra -Werror -fsanitize=address 

SRCS = src/webserv.cpp \
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

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@mkdir -p www/uploads

.cpp.o: ${SRCS}
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)
	@rm -rf www/uploads

re: fclean all

.PHONY: all clean fclean re run
