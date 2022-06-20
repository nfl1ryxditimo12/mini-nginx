NAME        :=  webserv

CXX         :=  ~/goinfre/clang14/bin/clang++
CXXFLAGS    :=  -std=c++98 -Wall -Wextra -Werror -g  #-fsanitize=undefined# -fsanitize=address
CXXDEBUG    :=  ~/goinfre/clang14/bin/clang++
CXXDEBUGFLAGS :=  -std=c++98 -Wall -Wextra -Werror -g3 -pedantic -fsanitize=undefined -fsanitize=address

UTIL_DIR	:=	Util
UTIL_SRCS	:=	Util.cpp Token.cpp
UTIL_SRCS	:=	$(addprefix $(UTIL_DIR)/, $(UTIL_SRCS))
UTIL_INCLUDES	:=	Util.hpp Token.hpp
UTIL_INCLUDES	:=	$(addprefix $(UTIL_DIR)/, $(UTIL_INCLUDES))

CONFIGURE_DIR		:=	Configure
CONFIGURE_SRCS	:=	Configure.cpp ConfParser.cpp Server.cpp Location.cpp InnerOption.cpp
CONFIGURE_SRCS	:=	$(addprefix $(CONFIGURE_DIR)/, $(CONFIGURE_SRCS))
CONFIGURE_INCLUDES	:=	Configure.hpp ConfParser.hpp Server.hpp Location.hpp InnerOption.hpp
CONFIGURE_INCLUDES	:=	$(addprefix $(CONFIGURE_DIR)/, $(CONFIGURE_INCLUDES))

SRCS        :=  main.cpp $(UTIL_SRCS) $(CONFIGURE_SRCS)
OBJS        :=  $(SRCS:.cpp=.o)

INCLUDES    :=  $(UTIL_INCLUDES) $(CONFIGURE_INCLUDES)

RM          :=  rm -f

ifeq ($(DEBUG), 1)
  CXX := $(CXXDEBUG)
  CXXFLAGS := $(CXXDEBUGFLAGS)
endif

.PHONY      :    all
all         :    $(NAME)

$(NAME)     :    $(OBJS) $(INCLUDES)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o         :    %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -I$(UTIL_DIR) -I$(CONFIGURE_DIR) -o $@

.PHONY      :    clean
clean       :
	$(RM) $(OBJS)

.PHONY      :    fclean
fclean      :    clean
	$(RM) $(NAME)

.PHONY      :    re
re          :
	make fclean
	make all

.PHONY      :   debug
debug       :
make DEBUG=1 re
