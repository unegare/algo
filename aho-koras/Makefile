TARGET?=a.out
SOURCES=main.cpp
HEADERS=Bohr.hpp

CXXFLAGS+=-Wall -pedantic-errors --std=c++17

OBJECTS=$(SOURCES:.cpp=.o)

.PHONY: all
all: $(HEADERS) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $^

.PHONY: objects

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(SOURCES:.cpp=.o)
