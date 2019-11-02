#ifndef TRIE_H
#define TRIE_H

#include "TrieNode.h"
#include <string>
#include <cctype>

using namespace std;

class Trie
{

private:
    TrieNode *_root = nullptr;

protected:

public:
    Trie()
    {
        _root = new TrieNode{};
    }

    virtual ~Trie()
    {
        //TODO: clean up memory
	delete _root;
    }

	//TODO: implement
    void addWord(const string &word)
    {
	TrieNode* current = _root;

	for(int i=0; i < word.length(); i++) {
		if(!current->hasChild(word[i])) {
			current->setChild(word[i], new TrieNode(word[i]));
		}
		current = current->getChild(word[i]);
	}
	current->setChild('$', new TrieNode('$'));
    }

	//TODO: implement
    vector<string> search(const string &word)
    {

        vector<string> matches;
	TrieNode* current = _root;
	string match = word;

	for(int i=0; i < word.length(); i++) {
		if(current->hasChild(word[i])) {
			current = current->getChild(word[i]);
		}
	}
	for(auto child : current->getChildren()) {
		if(child.first != '$') {
			match += child.first;
			traverseSubtree(word + child.first, matches);
		} else {
			matches.push_back(match);
		}
	}
	return matches;
    }

	void traverseSubtree(const string& match, vector<string>& matches) {
		TrieNode* current = _root;
		string temp = match;

		for(int i=0; i < match.length(); i++) {
			if(current->hasChild(match[i])) {
				current = current->getChild(match[i]);
			}
		}
		for(auto child : current->getChildren()) {
			if(child.first != '$') {
				temp += child.first;
				traverseSubtree(match + child.first, matches);
			} else {
				matches.push_back(match);
			}
		}
	}
};

#endif // !TRIE_H
