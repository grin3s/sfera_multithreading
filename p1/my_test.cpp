#include <iostream>
#include <list>
#include "allocator.h"

template<typename T>
void print_list(std::list<T> &l) {
	for (auto it = l.begin(); it != l.end(); it++) {
		std::cout << *it << std::endl;
	}
	std::cout << std::endl;
}

int main(int argc, char **argv) {
//	std::list<int> L;
//	L.push_back(1);
//	L.push_back(2);
//	L.push_back(3);
//	print_list(L);
//	auto it = L.begin();
//	it++;
//	L.pop_front();
//	print_list(L);
//	std::cout << *it << std::endl;
//	it++;
//	std::cout << *it << std::endl;
//	L.push_back(11);
//	L.pop_front();
//	std::cout << *it << std::endl;
	char buf[20];
	Allocator allocator(buf, 20);
	auto p = allocator.alloc(10);
	return 0;
}
