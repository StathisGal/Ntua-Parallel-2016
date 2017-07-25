include ./Makefile.include

INCLUDE_DIR = $(TBB_DIR)/include/
LIBRARY_DIR = $(TBB_DIR)/build/$(LIBRARY_VER)

# CXX = g++
# To compile in clones:
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11 -DUSE_LAMBDAS 
LDFLAGS =
LIBS = -ltbb -lrt

CXXFLAGS += -I$(INCLUDE_DIR)
 
OBJS = fib.o

all : util.o 
	$(CXX) $(LDFLAGS) util.o -o fw_tbb_naive_parallelfor_v1 -L$(LIBRARY_DIR) $(LIBS)


%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm -f fw_tbb_naive_parallelfor_v1
