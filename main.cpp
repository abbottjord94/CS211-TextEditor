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
#include "list.h"
#include <vector>

using namespace std;

#define ctrl(x)           ((x) & 0x1f)
#define KEY_RETURN 10

void draw_centered(WINDOW* win, int max_y, int max_x, string text);
void draw_document(WINDOW* win, int max_y, int max_x, vector<CharacterNode*> doc);
void print_usage();

int main(int argc, char* argv[])
{
	WINDOW* main_window = nullptr;
	int num_cols = 0;
	int num_rows = 0;
	int row = 1, col = 0;
	string filename;
	string buf = "";
	bool hide_gui = false;
	bool prompt_savefile = false;
	vector<CharacterNode*> document;
	CharacterNode* cursor;

	document.push_back(new CharacterNode);
	cursor = document[0];

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
			/*
			case ctrl('o'):
				if(prompt_savefile) {
					//probably create a new window here for naming the file
				}
				ofstream outfile(filename);
				break;
			*/
			case KEY_RESIZE:
				resize_term(0, 0);
				getmaxyx(main_window, num_rows, num_cols);
				break;
			case KEY_RETURN:
				row++;
				col = 0;
				buf += '\n';
				break;
			default:
				//insert(cursor,input);
				col++;
				if(col > num_cols) {
					col = 0;
					row++;
				}
				buf += input;
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
