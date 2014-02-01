/****************************************************
* Pacman For Console Level Editor                   *
* By: Rev. Dr. Mike Billars (doctormike@gmail.com)  *
* Date: 2006-12-12                                  *
*                                                   *
* Please see file COPYING for details on licensing  *
*       and redistribution of this program          *
*                                                   *
****************************************************/

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "pacman.h"

#define FILE_ERR "You must specify a file to use (load and save)"
#define SAVE_ERR "Error while saving"

void CheckScreenSize();                             //Make sure resolution is at least 32x29
void CreateWindows(int y, int x, int y0, int x0);   //Make ncurses windows
void DrawWindow();                                  //Refresh display
void ExitProgram(const char *message);              //Exit gracefully
void GetInput();                                    //Get user input
void InitCurses();                                  //Start up ncurses
void LoadLevel();                                   //Load level into memory
void MainLoop();                                    //Main loop
void SaveLevel();                                   //Save level to specified file

//For ncurses
WINDOW * win;
WINDOW * status;

//For colors
enum { Wall = 1, Normal, Pellet, PowerUp, GhostWall, Ghost1, Ghost2, Ghost3, Ghost4, BlueGhost, Pacman, Cursor};

//I know global variables are bad, but it's just sooo easy!
int Level[29][28] = { 0 };				//Main level array
int Loc[6][2] = { 0 };					//Ghosts, Pacman, and cursor locations
int tleft = 0;						//How long left for invincibility
char *filename = NULL;				//Name of file to load/save

int main(int argc, char *argv[]) {

	if((argc > 1) && (strlen(argv[1]) > 1)) {
		filename = argv[1];
		LoadLevel();

		InitCurses();
		CheckScreenSize();
		CreateWindows(29, 28, 1, 1);

		DrawWindow();
		Loc[5][0]=13;
		Loc[5][1]=13;
		MainLoop();
	}
	else
		ExitProgram(FILE_ERR);

}

void CheckScreenSize() {
        //Make sure the window is big enough
        int h, w; getmaxyx(stdscr, h, w);
        if((h < 32) || (w < 29)) {
                endwin();
                fprintf(stderr, "\nSorry.\n");
                fprintf(stderr, "To edit levels for Pacman for Console, your console window\n");
		fprintf(stderr, "must be at least 32x29 Please resize your window/resolution\n");
		fprintf(stderr, "and re-run the program.\n\n");
                ExitProgram("");
        }
}

void CreateWindows(int y, int x, int y0, int x0) {
	win = newwin(y, x, y0, x0);
	status = newwin(3, 28, 29, 1);
}

void DrawWindow() {
	int a = 0; int b = 0;
	char chr = ' ';
	int attr;

	//Display level array
	for(a = 0; a < 29; a++) for(b = 0; b < 28; b++) {
		switch(Level[a][b]) {
		case 0: chr = ' '; attr = A_NORMAL; wattron(win, COLOR_PAIR(Normal));    break;
		case 1: chr = ' '; attr = A_NORMAL; wattron(win, COLOR_PAIR(Wall));      break;
		case 2: chr = '.'; attr = A_NORMAL; wattron(win, COLOR_PAIR(Pellet));    break;
		case 3: chr = '*'; attr = A_BOLD;   wattron(win, COLOR_PAIR(PowerUp));   break;
		case 4: chr = ' '; attr = A_NORMAL; wattron(win, COLOR_PAIR(GhostWall)); break;
		case 5: chr = '&'; attr = A_NORMAL; wattron(win, COLOR_PAIR(Ghost1));    break;
		case 6: chr = '&'; attr = A_NORMAL; wattron(win, COLOR_PAIR(Ghost2));    break;
		case 7: chr = '&'; attr = A_NORMAL; wattron(win, COLOR_PAIR(Ghost3));    break;
		case 8: chr = '&'; attr = A_NORMAL; wattron(win, COLOR_PAIR(Ghost4));    break;
		case 9: chr = 'C'; attr = A_NORMAL; wattron(win, COLOR_PAIR(Pacman));    break;
		}
		mvwaddch(win, a, b, chr | attr);
	}

	//Display the cursor
	wattron(win,COLOR_PAIR(Cursor));
	mvwaddch(win, Loc[5][0], Loc[5][1], 'X');

	//Display info in status
	wmove(status, 1, 1);
	wattron(status, COLOR_PAIR(Normal));
	mvwprintw(status, 1,  0, "0)     1)       2)     3)");
	mvwprintw(status, 2,  0, "4)     5-8)     9)     F/C/Q");

	wattron(status, COLOR_PAIR(Normal));	mvwaddch(status, 1,  3, ' ');
	wattron(status, COLOR_PAIR(Wall));	mvwaddch(status, 1, 10, ' ');
	wattron(status, COLOR_PAIR(Pellet));	mvwaddch(status, 1, 19, '.');
	wattron(status, COLOR_PAIR(PowerUp));	mvwaddch(status, 1, 26 , '*' | A_BOLD);
	wattron(status, COLOR_PAIR(GhostWall));	mvwaddch(status, 2,  3, ' ');
	wattron(status, COLOR_PAIR(Ghost1));	mvwaddch(status, 2, 12, '&');
	wattron(status, COLOR_PAIR(Pacman));	mvwaddch(status, 2, 19, 'C');

	wrefresh(status);
	wrefresh(win);
}

void ExitProgram(const char *message) {
	endwin();

	//Must save file
	if(!message)
		printf("\n\nFile saved as: %s\n", filename);

	else
		printf("%s\n\n", message);

	exit(0);
}

void GetInput() {
	int ch;
	int a=0;
	int b=0;

	static int chtmp;

	ch = getch();

	//Buffer input
	do {
		DrawWindow();
		ch = getch();
	} while (ch == ERR);

	switch (ch) {
		case KEY_UP:    case 'w': case 'W':
			Loc[5][0]--;
			if(Loc[5][0]<0) { Loc[5][0]=28; }
			break;

		case KEY_DOWN:  case 's': case 'S':
			Loc[5][0]++;
			if(Loc[5][0]>28) { Loc[5][0]=0; }
			break;

		case KEY_LEFT:  case 'a': case 'A':
			Loc[5][1]--;
			if(Loc[5][1]<0) { Loc[5][1]=27; }
			break;

		case KEY_RIGHT: case 'd': case 'D':
			Loc[5][1]++;
			if(Loc[5][1]>27) { Loc[5][1]=0; }
			break;

		case 'q': case 'Q':
			//Save and exit
			SaveLevel();
			ExitProgram(NULL);
			break;

		//Draw a blank space
		case '0': case ' ':
			Level[Loc[5][0]][Loc[5][1]]=0;
			break;

		//Draw a wall
		case '1':
			Level[Loc[5][0]][Loc[5][1]]=1;
			break;

		//Draw a pellet
		case '2':
			Level[Loc[5][0]][Loc[5][1]]=2;
			break;

		//Draw a powerup
		case '3':
			Level[Loc[5][0]][Loc[5][1]]=3;
			break;

		//Draw a ghost/spawning wall
		case '4':
			Level[Loc[5][0]][Loc[5][1]]=4;
			break;

		//Draw a ghost/pacman
		case '5': case '6': case '7':
		case '8': case '9':

			//Move ghost/pacman to new location
			Level[Loc[5][0]][Loc[5][1]]=ch-'0';

			//Erase character from previous location and replace with BLANK
			if(Level[Loc[ch-'0'-5][0]][Loc[ch-'0'-5][1]]==ch-'0')
				Level[Loc[ch-'0'-5][0]][Loc[ch-'0'-5][1]]=0;

			//Set Loc variable to new location
			Loc[ch-'0'-5][0]=Loc[5][0];
			Loc[ch-'0'-5][1]=Loc[5][1];

			break;

		//Fill blank spaces with pellets
		case 'f': case 'F':

			for(a=0; a<29; a++)
			for(b=0; b<28; b++)
				if(Level[a][b]==0) Level[a][b]=2;
			break;

		//Fill pellet spaces with blanks
		case 'c': case 'C':

			for(a=0; a<29; a++)
			for(b=0; b<28; b++)
				if(Level[a][b]==2) Level[a][b]=0;
			break;

	}
}

void InitCurses() {
	initscr();
	start_color();
	curs_set(0);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	nonl();
	cbreak();
	noecho();

	init_pair(Normal,    COLOR_WHITE,   COLOR_BLACK);
	init_pair(Wall,      COLOR_WHITE,   COLOR_WHITE);
	init_pair(Pellet,    COLOR_WHITE,   COLOR_BLACK);
	init_pair(PowerUp,   COLOR_BLUE,    COLOR_BLACK);
	init_pair(GhostWall, COLOR_WHITE,   COLOR_CYAN);
	init_pair(Ghost1,    COLOR_RED,     COLOR_BLACK);
	init_pair(Ghost2,    COLOR_CYAN,    COLOR_BLACK);
	init_pair(Ghost3,    COLOR_MAGENTA, COLOR_BLACK);
	init_pair(Ghost4,    COLOR_YELLOW,  COLOR_BLACK);
	init_pair(BlueGhost, COLOR_BLUE,    COLOR_RED);
	init_pair(Pacman,    COLOR_YELLOW,  COLOR_BLACK);
	init_pair(Cursor,    COLOR_BLUE,    COLOR_YELLOW);
}

void LoadLevel() {

	int a = 0; int b = 0;
	FILE *fin;

	//Open file
	fin = fopen(filename, "r");

	//File doesn't exist, start new
	if(!(fin)) {
		for(a=0; a<28; a++) { Level[a][0]=1; Level[a][27]=1; }
		for(a=0; a<28; a++) { Level[0][a]=1; Level[28][a]=1; }
		Loc[0][0]=2; Loc[0][1]=2;		Loc[1][0]=2; Loc[1][1]=4;
		Loc[2][0]=2; Loc[2][1]=6;		Loc[3][0]=2; Loc[3][1]=8;
		Loc[4][0]=19; Loc[4][1]=19;
		Level[Loc[0][0]][Loc[0][1]]=5;
		Level[Loc[1][0]][Loc[1][1]]=6;
		Level[Loc[2][0]][Loc[2][1]]=7;
		Level[Loc[3][0]][Loc[3][1]]=8;
		Level[Loc[4][0]][Loc[4][1]]=9;
	}
	else {
		//Open file and load the level into the array
		for(a = 0; a < 29; a++) for(b = 0; b < 28; b++) {
			fscanf(fin, "%d", &Level[a][b]);
			if(Level[a][b] == 5) { Loc[0][0]=a; Loc[0][1]=b; }
			if(Level[a][b] == 6) { Loc[1][0]=a; Loc[1][1]=b; }
			if(Level[a][b] == 7) { Loc[2][0]=a; Loc[2][1]=b; }
			if(Level[a][b] == 8) { Loc[3][0]=a; Loc[3][1]=b; }
			if(Level[a][b] == 9) { Loc[4][0]=a; Loc[4][1]=b; }
		}
	}
}

void MainLoop() {
	do {
		GetInput();
		DrawWindow();
	} while (1);
}

void SaveLevel() {
	int a=0;
	int b=0;
	FILE *fout;

	//Open file
	fout = fopen(filename, "w");
	if(!(fout)) ExitProgram(SAVE_ERR);

	for(a = 0; a < 29; a++) for(b = 0; b < 28; b++) {
		fprintf(fout, "%d ", Level[a][b]);
		fprintf(fout, "\n");
	}
	fprintf(fout, "1");
	fclose(fout);
}
