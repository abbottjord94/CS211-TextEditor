debug:
	clang++ main.cpp Trie.h TrieNode.h Compression.h -lcurses -std=c++11 -g

release:
	clang++ main.cpp Trie.h TrieNode.h Compression.h -lcurses -std=c++11

debug-g++:
	g++ main.cpp Trie.h TrieNode.h Compression.h -lcurses -std=c++11 -g

release-g++:
	g++ main.cpp Trie.h TrieNode.h Compression.h -lcurses -std=c++11
