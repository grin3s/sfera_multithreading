TEST_FILES = ../thirdparty/gtest/gtest-all.cc ../thirdparty/gtest/gtest_main.cc
SRC = allocator.cpp allocator_test.cpp
HDR = allocator.h


all: tests.done

my_test: allocator.h allocator.cpp my_test.cpp
	g++ -O1 -g -std=c++11 -o my_test allocator.cpp my_test.cpp

allocator_test: $(SRC) $(HDR)
	g++ -O1 -g -std=c++11 -o allocator_test $(SRC) -I../thirdparty $(TEST_FILES) -lpthread

tests.done: allocator_test
	./allocator_test
	touch tests.done
	
clean:
	rm -f my_test
	rm -f allocator_test
