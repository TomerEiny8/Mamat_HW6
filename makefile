     
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
OBJS = test_code.o L4.o L3.o L2.o
TARGET = test_code

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
     