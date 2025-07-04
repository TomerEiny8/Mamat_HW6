     
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O0

# Source files
SRCS = main.cpp NIC_sim.cpp L2.cpp L3.cpp L4.cpp
# Object files
OBJS = $(SRCS:.cpp=.o)
# Output executable
TARGET = nic_sim.exe

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp %.h*
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
     