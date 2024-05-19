NAME = ircserv

SRCS = Server.cpp ParseCmd.cpp

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address -std=c++98

OBJS = ${SRCS:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJS}
	@echo "\033[1;32mCompiling...\033[0;m"
	@${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}
	@echo "\033[1;32mDone!\033[0;m"

clean:
	@echo "\033[1;31mCleaning...\033[0;m"
	@rm -rf ${OBJS}
	@echo "\033[1;31mDone!\033[0;m"

fclean: clean
	@echo "\033[1;31mFull Cleaning...\033[0;m"
	@rm -rf ${NAME}
	@echo "\033[1;31mDone!\033[0;m"

re: fclean all