CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SRC = main.cpp Tokenizer.cpp Parser.cpp Environment.cpp Interpreter.cpp
OBJ = $(SRC:.cpp=.o)

all: miniscript

miniscript: $(OBJ)
	$(CXX) $(CXXFLAGS) -o miniscript $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f miniscript $(OBJ)
