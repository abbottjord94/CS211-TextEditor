debug:
	clang++ main.cpp list.h list.cpp -lcurses -std=c++11 -g

release:
	clang++ main.cpp list.h list.cpp  -lcurses -std=c++11
