     
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O0

#OBJS = test_code.o L4.o L3.o L2.o
#TARGET = test_code

#all: $(TARGET)

# Source files
SRCS = main.cpp NIC_sim.cpp L2.cpp L3.cpp L4.cpp
# Object files
OBJS = $(SRCS:.cpp=.o)
# Output executable
TARGET = nic_sim

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
     