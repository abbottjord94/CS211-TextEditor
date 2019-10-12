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

using namespace std;

#define ctrl(x)           ((x) & 0x1f)
#define KEY_RETURN 10
#define KEY_TAB 9
#define KEY_BACKSPACE 263

void draw_centered(WINDOW* win, int max_y, int max_x, string text);
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
	string::iterator cursor = file_buffer.begin();
	char c;
	//Argument parsing
	for(int i=0; i<argc; i++) {
		string arg = argv[i];
		if(arg.front() == '-') {
			if(arg == "--help" || arg == "-h") {
				print_usage();
				exit(1);
			} else if(arg == "--hide-gui" || arg == "-H") {
				hide_gui = true;
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
	curs_set(FALSE);

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
					ofstream outfile(filename);
					outfile << file_buffer;
					outfile.close();
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
				break;

			case KEY_TAB:
				col += 4;
				if(col > num_cols) {
					col = 0;
					row++;
				}
				file_buffer += '\t';
				cursor++;
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

			default:
				//insert(cursor,input);
				col++;
				if(col > num_cols) {
					col = 0;
					row++;
				}
				file_buffer += input;
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

void print_usage() {
	cout << "TX Text Editor v0.2 (alpha)\n";
	cout << "Usage: ./tx [options][filename]\n";
	cout << "Options:\n";
	cout << "\t--help, -h: Display this help message and quit\n";
	cout << "\t--hide_gui, -H: Hide the GUI\n";
	exit(1);
}
