# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# File names
SRCS = prompt-shell.cpp
OBJS = $(SRCS:.cpp=.o)
EXECUTABLE = prompt-shell

# Default target
all: $(EXECUTABLE)

# Rule to build the executable
$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXECUTABLE)

# Rule to build object files from source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJS) $(EXECUTABLE)
