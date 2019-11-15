#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <functional>

using namespace std;

class PairComparer {
	public:
		bool operator()(const pair<string, int>& left, const pair<string, int>& right)
		{
   			return left.second < right.second;
		}
};

vector<string> str_split(string data) {
	string word = "";
	vector<string> words;
	for(int i=0; i<data.length(); i++) {
		if(data[i] == ' ' || data[i] == '\t' || data[i] == '\n') {
			if(word.empty()) {
				word = "";
			} else {
				words.push_back(word);
				word = "";
			}
		}
		else if(data[i] == '.' || data[i] == ',' || data[i] == '(' || data[i] == ')' || data[i] == ';' || data[i] == ':') {
			if(word.empty()) {
				word = "";
			} else {
				words.push_back(word);
				word = "";
			}
		}
		else {
			word += data[i];
		}
	}
	words.push_back(word);
	return words;
}

string compress(string data, unordered_map<string, string> codes) {
	string word = "";
	string output = "";
	unordered_map<string, string>::iterator it;
	for(int i=0; i<data.length(); i++) {
		if(data[i] == ' ' ||
		data[i] == '\t' ||
		data[i] == '\n' ||
		data[i] == '.' ||
		data[i] == ',' ||
		data[i] == '(' ||
		data[i] == ')' ||
		data[i] == ';' ||
		data[i] == ':') {
			it = codes.find(word);
			if(it == codes.end()) {
				output += word;
			} else {
				output += it->second;
			}
			output += data[i];
			word = "";
		} else {
			word += data[i];
		}
	}
	return output;
}

string decompress(string data, unordered_map<string, string> codes) {
	string word = "";
	string output = "";
	unordered_map<string, string>::iterator it;
	for(int i=0; i<data.length(); i++) {
		if(data[i] == ' ' ||
		data[i] == '\t' ||
		data[i] == '\n' ||
		data[i] == '.' ||
		data[i] == ',' ||
		data[i] == '(' ||
		data[i] == ')' ||
		data[i] == ';' ||
		data[i] == ':') {
			it = codes.find(word);
			if(it == codes.end()) {
				output += word;
			} else {
				output += it->second;
			}
			output += data[i];
			word = "";
		} else {
			word += data[i];
		}
	}
	return output;
}

unordered_map<string, string> generate_alphabet_codes(string data) {
	char next_char = 'a';
	string offset = "";
	vector<string> words = str_split(data);
	unordered_map<string, int> freq_table{};
	priority_queue<pair<string, int>, vector<pair<string, int>>, PairComparer> pq;
	unordered_map<string, string> codes;
	for(int i=0; i<words.size(); i++) {
		unordered_map<string, int>::iterator search = freq_table.find(words[i]);
		if(search == freq_table.end()) {
			freq_table.insert(make_pair(words[i], 1));
		} else {
			search->second = search->second+1;
		}
	}
	for(auto w : freq_table) {
		pq.push(w);
	}
	while(!pq.empty()) {
		pair<string, int> item = pq.top();
		string code = offset + next_char;
		codes.insert(make_pair(item.first, code));
		if(next_char == 'z') {
			if(offset.empty()) {
				offset = 'a';
			} else {
				if(offset.back() == 'z') {
					offset+='a';
				} else {
					offset.back() += 1;
				}
			};
			next_char = 'a';
		}
		else {
			next_char++;
		}
		//cout << item.first << ": " << item.second << endl;
		pq.pop();
	}
	return codes;
}

unordered_map<string, string> generate_binary_codes(string data) {
	char next_char = '0';
	string offset = "";
	vector<string> words = str_split(data);
	unordered_map<string, int> freq_table{};
	priority_queue<pair<string, int>, vector<pair<string, int>>, PairComparer> pq;
	unordered_map<string, string> codes;
	for(int i=0; i<words.size(); i++) {
		unordered_map<string, int>::iterator search = freq_table.find(words[i]);
		if(search == freq_table.end()) {
			freq_table.insert(make_pair(words[i], 1));
		} else {
			search->second = search->second+1;
		}
	}
	for(auto w : freq_table) {
		pq.push(w);
	}
	while(!pq.empty()) {
		pair<string, int> item = pq.top();
		string code = offset + next_char;
		codes.insert(make_pair(item.first, code));
		if(next_char == '1') {
			if(offset.empty()) {
				offset = '0';
			} else {
				if(offset.back() == '1') {
					offset+='0';
				} else {
					offset.back() += 1;
				}
			};
			next_char = '0';
		}
		else {
			next_char++;
		}
		//cout << item.first << ": " << item.second << endl;
		pq.pop();
	}
	return codes;
}

unordered_map<string, string> reverse(unordered_map<string, string> codes) {
	unordered_map<string, string> output;

	for(auto c : codes) {
		output.insert(make_pair(c.second, c.first));
	}
	return output;
}
