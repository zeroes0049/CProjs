// Tetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <Windows.h>
using namespace std;


int fieldwidth = 16;
int fieldheight = 10;
int offset = 2;
int tetwidth = 4;
string tetro[7];
unsigned char* pfield = nullptr;

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

bool IsFit() {
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

	char* field = new char[fieldwidth*fieldheight];
	for (int y = 0; y < fieldheight; y++) {
		for (int x = 0; x < fieldwidth; x++) {
			field[GetIndex(x, y, fieldwidth)] = (x == 0 || x == fieldwidth - 1 ||  y == fieldheight - 1) ? 9 : 0;
		}
	}

	int curX = fieldwidth/2;
	int curY = 0;
	bool gameover = false;

	while (!gameover) {

		// timing ==========================
		this_thread::sleep_for(50ms);

		// input ===========================

		// left
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 !=0 && IsFit())
		{
			curX--;
		}
		// right
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000 != 0 && IsFit())
		{
			curX++;
		}
		// down
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000 != 0 && IsFit())
		{
			curY++;
		}

		// logic ===========================
		int curPiece = rand() % 7 + 1;

		// display =========================

		// draw piece
		for (int y = 0; y < tetwidth; y++) {
			for (int x = 0; x < tetwidth; x++) {
				field[GetIndex(curX + x, curY + y, fieldwidth)] = curPiece;
			}
		}

		// draw field
		for (int y = 0; y < fieldheight; y++) {
			for (int x = 0; x < fieldwidth; x++) {
				screen[GetIndex(x + offset, y + offset, screenwidth)] = " ABCDEFG=#"[field[GetIndex(x, y, fieldwidth)]];
			}
		}



		WriteConsoleOutputCharacter(hConsole, screen, screenwidth*screenheight, { 0,0 }, &dwBytesWritten);
	}
	



	return 0;
}

