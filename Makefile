NAME        :=  webserv

CXX         :=  clang++
CXXFLAGS    :=  -std=c++98 -Wall -Wextra -Werror -g  -fsanitize=undefined -fsanitize=address

UTIL_DIR            :=  Util
UTIL_SRCS           :=  Util.cpp Token.cpp
UTIL_SRCS           :=  $(addprefix $(UTIL_DIR)/, $(UTIL_SRCS))
UTIL_INCLUDES       :=  Util.hpp Token.hpp
UTIL_INCLUDES       :=  $(addprefix $(UTIL_DIR)/, $(UTIL_INCLUDES))

CONFIGURE_DIR       :=  Configure
CONFIGURE_SRCS      :=  Configure.cpp ConfParser.cpp Server.cpp Location.cpp InnerOption.cpp
CONFIGURE_SRCS      :=  $(addprefix $(CONFIGURE_DIR)/, $(CONFIGURE_SRCS))
CONFIGURE_INCLUDES  :=  Configure.hpp ConfParser.hpp Server.hpp Location.hpp InnerOption.hpp
CONFIGURE_INCLUDES  :=  $(addprefix $(CONFIGURE_DIR)/, $(CONFIGURE_INCLUDES))

CONTROLLER_DIR      :=  Controller
CONTROLLER_SRCS     :=  Repository.cpp Validator.cpp Response.cpp ResponseHeader.cpp
CONTROLLER_SRCS     :=  $(addprefix $(CONTROLLER_DIR)/, $(CONTROLLER_SRCS))
CONTROLLER_INCLUDES :=  Repository.hpp Validator.hpp Response.hpp ResponseHeader.hpp
CONTROLLER_INCLUDES :=  $(addprefix $(CONTROLLER_DIR)/, $(CONTROLLER_INCLUDES))

ROUTER_DIR          :=  Router
ROUTER_SRCS         :=  Socket.cpp Kernel.cpp Request.cpp
ROUTER_SRCS         :=  $(addprefix $(ROUTER_DIR)/, $(ROUTER_SRCS))
ROUTER_INCLUDES     :=  Socket.hpp Kernel.hpp Request.hpp
ROUTER_INCLUDES     :=  $(addprefix $(ROUTER_DIR)/, $(ROUTER_INCLUDES))

SRCS        :=  main.cpp $(UTIL_SRCS) $(CONFIGURE_SRCS) $(CONTROLLER_SRCS) $(ROUTER_SRCS)
OBJS        :=  $(SRCS:.cpp=.o)

INCLUDES    :=  $(UTIL_INCLUDES) $(CONFIGURE_INCLUDES) $(CONTROLLER_INCLUDES) $(ROUTER_INCLUDES)

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
	$(CXX) $(CXXFLAGS) -c $^ -I$(UTIL_DIR) -I$(CONFIGURE_DIR) -I$(CONTROLLER_DIR) -I$(ROUTER_DIR) -o $@

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
