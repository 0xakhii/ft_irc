NAME = ircserv

SRCS = ParseCmd.cpp Channel.cpp kickcmd.cpp \
		startConnection.cpp autentification.cpp main.cpp Invitecmd.cpp \
		server_utils.cpp

CXX = c++

CXXFLAGS = -g3 -fsanitize=address#-Wall -Wextra -Werror  #-std=c++98

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