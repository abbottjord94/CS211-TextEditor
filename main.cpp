#ifdef _WIN32
//Windows includes
#include "curses.h"
#include "panel.h"
#include "curspriv.h"
#else
//Linux / MacOS includes
#include <curses.h>
#endif
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <utility>
#include "Trie.h"
#include "Compression.h"

using namespace std;

#define ctrl(x)           ((x) & 0x1f)
#define KEY_RETURN 10
#define KEY_TAB 9
#define KEY_BACKSPACE 263

void draw_centered(WINDOW* win, int max_y, int max_x, string text);
void insertionSortR(vector<string>& data, int n);
void bubbleSort(vector<string>& data);
void quickSortHelper(vector<string>& data, int start, int end_i);
void quickSort(vector<string>& data);
void print_usage();

int main(int argc, char* argv[])
{
	WINDOW* main_window = nullptr;
	int num_cols = 0;
	int num_rows = 0;
	int row = 1, col = 0;
	string filename;
	string file_buffer = "";
	int buffer_length = 0;
	bool hide_gui = false;
	bool prompt_savefile = false;
	bool use_compression = false;
	bool use_selection_sort = false;
	bool use_insertion_sort = false;
	bool use_bubble_sort = false;
	bool use_quick_sort = false;
	string::iterator cursor = file_buffer.begin();
	char c;
	Trie keyword_trie;
	string temp_word, match;
	vector<string> matches;
	unordered_map<string, string> compression_codes{};
	unordered_map<string, string> decompression_codes{};
	unordered_map<string, int> freq_table{};
	priority_queue<pair<string,int>, vector<pair<string, int>>> pq{};
	vector<string> words;
	string temp_file_buffer = "";

	//Argument parsing
	for(int i=0; i<argc; i++) {
		string arg = argv[i];
		if(arg.front() == '-') {
			if(arg == "--help" || arg == "-h") {
				print_usage();
				exit(1);
			} else if(arg == "--hide-gui" || arg == "-H") {
				hide_gui = true;
			} else if(arg == "--use-compression" || arg == "-C") {
				use_compression = true;
			} else if(arg == "--use-selection-sort" || arg == "-S") {
				use_selection_sort = true;
			} else if(arg == "--use-insertion-sort" || arg == "-I") {
				use_insertion_sort = true;
			} else if(arg == "--use-bubble-sort" || arg == "-B") {
				use_bubble_sort = true;
			} else if(arg == "--use-quick-sort" || arg == "-Q") {
				use_quick_sort = true;
			} else {
				cout << "Invalid argument: " << argv[i] << endl;
				print_usage();
				exit(1);
			}
		}
		if(argc > 1) filename = argv[argc-1];
		else {
			filename = "[no filename specified]";
			prompt_savefile = true;
		}
	}

	ifstream keyword_infile("keywords.txt");
	while(!keyword_infile.eof()) {
		string word;
		getline(keyword_infile,word);
		keyword_trie.addWord(word);
	}

	//SETUP
	//initialize our window
	main_window = initscr();

	//resize our window
	getmaxyx(main_window, num_rows, num_cols);
	resize_term(num_rows - 1, num_cols - 1);

	//turn keyboard echo
	noecho();
	cbreak();

	//turn on keypad input
	keypad(main_window, TRUE);

	//hide the cursor
	curs_set(TRUE);

	//MAIN PROGRAM LOGIC GOES HERE
	//pause for user input
	bool keep_going = true;
	if(!hide_gui) {
		ostringstream temp_str{};
		temp_str << "TX Text Editor v0.2: " << filename;
		draw_centered(main_window,0,num_cols,temp_str.str().c_str());
	}

	if(!prompt_savefile) {
		ifstream infile(filename);
		infile >> file_buffer;
		if(use_compression) {
			string codefile_name = filename + ".codes";
			string c;
			string word = "";
			vector<string> parts{};
			ifstream codefile(codefile_name);
			while(codefile.good()) {
				getline(codefile, c);
				for(int i=0; i<c.length(); i++) {
					if(c[i] == ':') {
						parts.push_back(word);
						word = "";
					} else {
						if(c[i] == '\n') {
						} else {
							word += c[i];
						}
					}
				}
				decompression_codes.insert(make_pair(parts[0], parts[1]));
				parts.pop_back();
				parts.pop_back();
			}
			file_buffer = decompress(file_buffer, decompression_codes);
		}
		for(int i=0; i<file_buffer.length(); i++) {
			if(file_buffer[i] == '\t') col+=4;
			else if(file_buffer[i] == '\n') {
				mvwaddch(main_window,row,col,file_buffer[i]);
				col = 0;
				row++;
			} else {
				mvwaddch(main_window,row,col,file_buffer[i]);
				col++;
			}
		}
		cursor = file_buffer.end()-1;
	}
	//draw_document(main_window,num_rows,num_cols,document);
	while (keep_going == true)
	{
		int input = wgetch(main_window);

		//Curses documentation says to use KEY_RESIZE, but you can also use
		//is_termresized.  In real life, use either/or but not both.
		if (is_term_resized(num_rows,num_cols) == true)
		{
			resize_term(0, 0);
			getmaxyx(main_window, num_rows, num_cols);
		}

		switch (input)
		{
			case ctrl('c'):
				keep_going = false;
				break;

			case ctrl('o'):
				if(prompt_savefile) {
					//probably create a new window here for naming the file
				} else {
					if(use_compression) {
						ofstream codefile(filename + ".codes");
						compression_codes = generate_alphabet_codes(file_buffer);
						for(auto c : reverse(compression_codes)) {
							codefile << c.first << ":" << c.second << '\n';
						}
						codefile.close();
						ofstream out(filename);
						out << compress(file_buffer, compression_codes);
						out.close();
					} else {
						ofstream outfile(filename);
						outfile << file_buffer;
						outfile.close();
					}
				}
				break;
			case ctrl('a'):
				matches = keyword_trie.search(temp_word);
				if(matches.empty()) break;
				else {
					match = matches[0];
					for(int i=temp_word.length()-1; i<match.length(); i++) {
						mvwaddch(main_window,row,col,match[i]);
						col++;
					}
				}
				temp_word = "";
				break;
			case ctrl('e'):
				if(!prompt_savefile) {
				if(use_selection_sort) {
					words = str_split(file_buffer);
					for(auto w : words) {
						unordered_map<string, int>::iterator search = freq_table.find(w);
						if(search == freq_table.end()) {
						//add word to frequency table
							freq_table.insert(make_pair(w,1));
						}
						else {
						//increment frequency table by one
							search->second = search->second+1;
						}
					}
					for(auto c : freq_table) {
						pq.push(c);
					}
					while(!pq.empty()) {
						pair<string, int> q = pq.top();
						for(int i=0; i<q.second; i++) {
							temp_file_buffer += q.first + " ";
						}
						file_buffer = temp_file_buffer;
						pq.pop();
					}
					ofstream outfile(filename);
					outfile << temp_file_buffer;
					outfile.close();
					}
				else if(use_insertion_sort) {
					words = str_split(file_buffer);
					insertionSortR(words, words.size());
					for(auto w : words) {
						temp_file_buffer += w + " ";
					}
					ofstream outfile(filename);
					outfile << temp_file_buffer;
					outfile.close();
				}
				else if(use_bubble_sort) {
					words = str_split(file_buffer);
					bubbleSort(words);
					for(auto w : words) {
						temp_file_buffer += w + " ";
					}
					ofstream outfile(filename);
					outfile << temp_file_buffer;
					outfile.close();
				}
				else if(use_quick_sort) {
					words = str_split(file_buffer);
					//quickSort(words);
					for(auto w : words) {
						temp_file_buffer += w + " ";
					}
					ofstream outfile(filename);
					outfile << temp_file_buffer;
					outfile.close();
				}
				}
				break;
			case KEY_RESIZE:
				resize_term(0, 0);
				getmaxyx(main_window, num_rows, num_cols);
				break;

			case KEY_RETURN:
				row++;
				col = 0;
				file_buffer += '\n';
				cursor++;
				temp_word = "";
				break;

			case KEY_TAB:
				col += 4;
				if(col > num_cols) {
					col = 0;
					row++;
				}
				file_buffer += '\t';
				cursor++;
				temp_word = "";
				break;

			case KEY_BACKSPACE:
				if(col == 0 && row == 1) break;
				else if (col == 0 && row > 1) {
					cursor--;
					mvwdelch(main_window,row,col);
					cursor = file_buffer.erase(cursor);
				} else {
					mvwdelch(main_window,row,col);
					col--;
					cursor--;
					cursor = file_buffer.erase(cursor);
				}
				break;
/*
			case KEY_SPACE:
				temp_word = "";
				mvwaddch(main_window,row,col,' ');
				col++;
				break;
*/
			default:
				//insert(cursor,input);
				col++;
				if(col > num_cols) {
					col = 0;
					row++;
				}
				file_buffer += input;
				temp_word += input;
				if(file_buffer.length() <= 1) cursor = file_buffer.begin();
				else cursor++;
				mvwaddch(main_window,row,col,input);
				/*
				if(col < num_cols) {
					cursor = cursor->next;
				} else {
					col = 0;
					row++;
					document[row] = new CharacterNode;
					cursor->next = document[row];
					cursor = document[row];
				}
				*/
				//draw_document(main_window,num_rows,num_cols,document);
				break;
		}
		if(file_buffer.length() <= 1) cursor = file_buffer.begin();
		c = *cursor;
		buffer_length = file_buffer.length();
	}
	//end curses mode
	endwin();
}

void draw_centered(WINDOW* win, int max_y, int max_x, string text)
{
	int length = text.length();
	int left_margin = (max_x - length) / 2;
	int top_margin = (max_y - 1) / 2;
	mvwprintw(win, top_margin, left_margin, text.c_str());
}

/*
void draw_document(WINDOW* win, int max_y, int max_x, vector<CharacterNode*> doc) {
	for(int i=1; i<max_y; i++) {
		int column = 0;
		if(doc[i-1]->next != NULL) {
			mvwaddch(win, i, column, doc[i-1]->data);
			doc = doc->next;
		}
	}
}
*/
void insertionSortR(vector<string>& data, int n) {
	string x;
	int j;
	if(n > 0) {
		insertionSortR(data, data.size()-1);
		x = data[n];
		j = n-1;
		while(j >=0 && data[j] > x) {
			data[j+1] = data[j];
			j--;
		}
	}
}

void bubbleSort(vector<string>& data) {
	int n = data.size();
	bool swapped = true;
	for(int i=0; i<data.size(); i++) {
		swapped = false;
		for(int j=1; j<data.size()-i; j++) {
			if(data[j-1] > data[j]) {
				string temp = data[j-1];
				data[j-1] = data[j];
				data[j] = temp;
				swapped = true;
			}
		}
		if(!swapped) {
			break;
		}
	}
}

void quickSortHelper(vector<string>& data, int start, int end_i) {
	if(end_i <= start) return;
	if(end_i - start == 1) {
		if(data[end_i] < data[start]) {
			string temp = data[end_i];
			data[end_i] = data[start];
			data[start] = temp;
		}
		return;
	}
	string first = data[start];
	string last = data[end_i];
	int mid = (start + end_i) / 2;
	string middle = data[mid];
	int pivot = start;

	if( (middle > first && middle < last) || (middle < first && middle > last) ) pivot = mid;
	else if( (last > first && last < middle) || (last < first && last > middle) ) pivot = end_i;

	string pivot_value = data[pivot];
	data[pivot] = data[end_i];
	data[end_i] = pivot_value;

	int i = start;
	int j = end_i;
	while(i < j) {
		while(data[i]< pivot_value && i < j) {
			i++;
		}
		while(data[j] > pivot_value && i < j) {
			j--;
		}
		if(i < j) {
			string temp = data[i];
			data[i] = data[j];
			data[j] = temp;
		}
	}
	string temp = data[i];
	data[i] = pivot_value;
	data[end_i] = temp;

	quickSortHelper(data, start, i-1);
	quickSortHelper(data, i+1, end_i);
}

void quickSort(vector<string>& data) {
	quickSortHelper(data, 0, data.size()-1);
}

void print_usage() {
	cout << "TX Text Editor v0.2 (alpha)\n";
	cout << "Usage: ./tx [options][filename]\n";
	cout << "Options:\n";
	cout << "\t--help, -h: Display this help message and quit\n";
	cout << "\t--hide_gui, -H: Hide the GUI\n";
	cout << "\t--use_compression, -C: Attempt to use compression when opening and saving files\n";
	cout << "\t--use-selection-sort, -S: Use selection sort for the word sorting feature\n";
	cout << "\t--use-insertion-sort, -I: Use insertion sort for the word sorting feature\n";
	cout << "\t--use-bubble-sort, -B: Use bubble sort for the word sorting feature\n";
	cout << "\t--use-quick-sort, -Q: Use quick sort for the word sorting feature\n";
	exit(1);
}
