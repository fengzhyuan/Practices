  

CXXFLAGS = -O2 -I./include -I/usr/local/include  -std=c++11 #-rdynamic -ffloat-store


SOURCE_ALL = src/
CXX = g++ 


TARGET_ALL = demo

$(TARGET_ALL):	
	$(CXX) $(SOURCE_ALL)*.cpp  $(CXXFLAGS) -o  $(TARGET_ALL)

all:	$(TARGET_ALL)

clean:
	rm -f $(OBJS) $(TARGET_ALL) 


