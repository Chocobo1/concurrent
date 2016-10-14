# Makefile

# compiler options
#CXX      ?= g++  # -fsanitize=leak -fsanitize=undefined -fsanitize=thread  # -fsanitize=address
CXXFLAGS   = -std=c++11 -pipe -Wall -Wextra -Wpedantic -O2
#CXXFLAGS  = -std=c++11 -pipe -Wall -Wextra -Wpedantic -Og -g -ggdb
LDFLAGS	   = -pthread  # -s
SRC_NAME   = tests tests_helpers ../helpers tests_semaphore ../semaphore
EXECUTABLE = tests
SRC_EXT    = cpp


# targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(patsubst %,%.o,$(SRC_NAME))
	$(CXX) $(LDFLAGS) $(patsubst %,%.o,$(SRC_NAME)) -o $(EXECUTABLE)
	@echo

$(patsubst %,%.o,$(SRC_NAME)): %.o : %.$(SRC_EXT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(patsubst %,./%.o,$(SRC_NAME)) ./$(EXECUTABLE)
