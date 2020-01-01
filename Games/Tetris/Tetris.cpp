// Tetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <Windows.h>
using namespace std;


int fieldwidth = 12;
int fieldheight = 12;
int offset = 2;
int tetwidth = 4;
string tetro[7];
unsigned char* field = nullptr;

// get index of matrix according x and y without rotate
int GetIndex(int px, int py, int weight) {
	return py * weight + px;
}

// rotate vector
int Rotate(int px, int py, int rotate) {
	int index = 0;
	switch (rotate%tetwidth)
	{
	// 0
	case 0:
		index = py * tetwidth + px;
		break;
	// 90
	case 1:
		index = tetwidth * (tetwidth - 1) + py - tetwidth * px;
		break;
	// 180
	case 2:
		index = tetwidth * tetwidth - 1 - tetwidth * py - px;
		break;
	// 270
	case 3:
		index = tetwidth - 1 - py + tetwidth * px;
		break;
	}
	return index;
}

bool IsFit(int px, int py, int rotate, int pieceNum) {
	for (int y = 0; y < tetwidth; y++) {
		for (int x = 0; x < tetwidth; x++) {
			int pieceIndex = Rotate(x, y, rotate);
			int fieldIndex = GetIndex(px + x, py + y, fieldwidth);

			if (px + x < 0 || px + x >= fieldwidth)
			{
				//return false;
			}

			if (py + y >= fieldheight)
			{
				//return false;
			}

			if (tetro[pieceNum][pieceIndex] != L'.' && field[fieldIndex] != 0)
			{
				return false;
			}
		}
	}


	return true;
}



int main()
{
	// Get screen info
	CONSOLE_SCREEN_BUFFER_INFO consoleinfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleinfo);
	int screenwidth = consoleinfo.dwSize.X;
	int screenheight = consoleinfo.dwSize.Y;

	// Create screen buffer
	wchar_t* screen = new wchar_t[screenwidth*screenheight];
	for (int i = 0; i < screenwidth*screenheight; i++) {
		screen[i] = ' ';
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Tetronimos 4x4
	tetro[0].append("..X...X...X...X."); 
	tetro[1].append("..X..XX...X.....");
	tetro[2].append(".....XX..XX.....");
	tetro[3].append("..X..XX..X......");
	tetro[4].append(".X...XX...X.....");
	tetro[5].append(".X...X...XX.....");
	tetro[6].append("..X...X..XX.....");

	field = new unsigned char[fieldwidth*fieldheight];
	for (int y = 0; y < fieldheight; y++) {
		for (int x = 0; x < fieldwidth; x++) {
			field[GetIndex(x, y, fieldwidth)] = (x == 0 || x == fieldwidth - 1 ||  y == fieldheight - 1) ? 9 : 0;
		}
	}

	int curX = fieldwidth/2;
	int curY = 0;
	int rotate = 0;
	int curPiece = 0;
	int speed = 20;
	int timeCounter = 0;
	bool gameover = false;
	bool hasRotate = false;
	bool forceDown = false;
	vector<int> lines;

	while (!gameover) {

		// timing ==========================
		this_thread::sleep_for(50ms);
		timeCounter++;
		if (timeCounter >= speed)
		{
			timeCounter = 0;
			forceDown = true;
		}
		else
		{
			forceDown = false;	
		}

		// input ===========================

		// left
		if ((GetAsyncKeyState(VK_LEFT) & 0x8000) !=0)
		{
			if (IsFit(curX - 1, curY, rotate, curPiece))
			{
				curX--;
			}
		}
		// right
		if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0 && IsFit(curX + 1, curY, rotate, curPiece))
		{
			curX++;
		}
		// down
		if ((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0 && IsFit(curX, curY + 1, rotate, curPiece))
		{
			curY++;
		}
		// rorate
		if ((GetAsyncKeyState(VK_UP) & 0x8000) != 0)
		{
			if (hasRotate == false && IsFit(curX, curY, rotate + 1, curPiece))
			{
				rotate++;
				hasRotate = true;
			}
		}
		else
		{
			hasRotate = false;
		}


		// logic ===========================
		//int curPiece = rand() % 7 + 1;
		if (forceDown)
		{
			// down 1 step
			if (IsFit(curX, curY + 1, rotate, curPiece))
			{
				curY++;
			}
			else
			{
				// store the current piece
				for (int y = 0; y < tetwidth; y++) {
					for (int x = 0; x < tetwidth; x++) {
						if (tetro[curPiece][Rotate(x, y, rotate)] != L'.') {
							field[GetIndex(curX + x, curY + y, fieldwidth)] = curPiece + 1;
						}
					}
				}

				// check for lines
				for (int y = curY; y < curY + tetwidth && y < fieldheight -1; y++) {
					bool fillLine = true;

					for (int x = 1; x < fieldwidth - 1; x++) {
						fillLine &= field[GetIndex(x, y, fieldwidth)] != 0;
					}

					if (fillLine)
					{
						for (int x = 1; x < fieldwidth - 1; x++) {
							field[GetIndex(x, y, fieldwidth)] = 8;
						}
						lines.push_back(y);
					}
				}

				// pick new piece
				curX = fieldwidth / 2;
				curY = 0;
				rotate = 0;
				curPiece = rand() % 7;

				// check game over
				gameover = !IsFit(curX, curY, rotate, curPiece);
			}
		}




		// display =========================
		// draw field
		for (int y = 0; y < fieldheight; y++) {
			for (int x = 0; x < fieldwidth; x++) {
				screen[GetIndex(x + offset, y + offset, screenwidth)] = " ABCDEFG=#"[field[GetIndex(x, y, fieldwidth)]];
			}
		}

		// draw piece
		for (int y = 0; y < tetwidth; y++) {
			for (int x = 0; x < tetwidth; x++) {
				if (tetro[curPiece][Rotate(x, y, rotate)] != L'.') {
					screen[GetIndex(curX + offset + x, curY + offset + y, screenwidth)] = " ABCDEFG=#"[curPiece + 1];
				}
			}
		}

		// delete lines 
		if (!lines.empty())
		{
			WriteConsoleOutputCharacter(hConsole, screen, screenwidth* screenheight, { 0,0 }, & dwBytesWritten);
			this_thread::sleep_for(100ms);
			for (int line : lines) {
				for (int y = line; y > 0 ; y--) {
					for (int x = 1; x < fieldwidth - 1; x++) {
						field[GetIndex(x, y, fieldwidth)] = field[GetIndex(x, y - 1, fieldwidth)];
						field[GetIndex(x, 0, fieldwidth)] = 0;
					}
				}
			}

			lines.clear();
		}

		WriteConsoleOutputCharacter(hConsole, screen, screenwidth*screenheight, { 0,0 }, &dwBytesWritten);
	}
	

	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << endl;
	system("pause");
	return 0;
}