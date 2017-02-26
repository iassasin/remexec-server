CC = g++
CPPFLAGS = -Wall -O3 -std=c++1y -flto
LDLIBS = 

SOURCES = $(wildcard *.cpp) $(wildcard cppsockets/*.cpp) $(wildcard regex/*.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

APP_NAME = remexec-server
APP = $(APP_NAME)

all: $(APP)
	strip $(APP)

boost: CPPFLAGS += -DREGEX_BOOST
boost: LDLIBS += -lboost_regex
boost: $(APP)

debug: CPPFLAGS = -Wall -g3 -std=c++1y
debug: $(APP)

boost-debug: CPPFLAGS += -DREGEX_BOOST -Wall -g3 -std=c++1y
boost-debug: LDLIBS += -lboost_regex
boost-debug: $(APP)

clean:
	rm -f $(APP) $(OBJECTS)

$(APP): $(OBJECTS)
	$(LINK.o) -flto $^ $(LDLIBS) -o $(APP)

