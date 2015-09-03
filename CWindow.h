#pragma once

#include <windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <random>
#include <iostream>
#include <chrono>
#include "resource.h"
#include <vector>
#include <map>
#include <time.h>
#include <fstream>
#include "JapanSolver.h"

#define FILL 1
#define EMPTY 2
#define NONE 0

class CWindow {
public:
    CWindow();
    ~CWindow();

    // Зарегистрировать класс окна
    static bool RegisterClass();
    // Создать экземпляр окна
    bool Create();
    // Показать окно
    void Show( int cmdShow );
	HWND GetHandle();
    HWND GetHandleDialog();
	void startNewGame();

protected:
    void OnDestroy();
	void OnSize( LPARAM lparam );
	void OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	void OnClose();
	void SaveFile();
    void OnPaint();
    void OnClick( LPARAM lParam );
	LRESULT OnCtlColorEdit(HDC dc, HWND hEdit, CWindow *win);

private:

    HWND handle; // хэндл окна
    HWND handleDlg;
    std::vector< std::vector<int> > numbers;
	std::vector< std::vector<HWND> > edits;
	std::map<HWND, int> iEdit, jEdit;
    void checkWin();
    void initialize();
    void loadFile();
	bool solve();
    int sizeX;
	int sizeY;
	int blocksNum;
    int newSize;
    bool loadedFromFile;
	HBRUSH whiteBrush, greyBrush, blackBrush;
	int qWidth, qHeight;

    static INT_PTR __stdcall DialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static INT_PTR __stdcall Dialog2Proc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

    static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};
