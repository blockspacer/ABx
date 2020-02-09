/**
 * Copyright 2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Window.h"
#include <AB/CommonConfig.h>
#if defined(AB_UNIX)
#include <ncurses.h>
#elif defined(AB_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#endif
#include <thread>

Window::Window()
{
#ifdef AB_UNIX
    initscr();
    noecho();
    curs_set(FALSE);
#else
    Clear();
    ShowCursor(false);
#endif
}

Window::~Window()
{
#ifdef AB_UNIX
    endwin();
#else
    ShowCursor(true);
#endif
}

char Window::GetChar() const
{
#ifdef AB_UNIX
    int c = getch();
#else
    int c = 0;
    if (_kbhit())
        c = _getch();
#endif
    return static_cast<char>(c);
}

void Window::HandleInput(char c)
{
    switch (c)
    {
    case 'q':
        running_ = false;
        break;
    }
}

void Window::Loop()
{
    running_ = true;
    while (running_)
    {
        char c = GetChar();
        if (c == 0)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            continue;
        }

        HandleInput(c);
    }
}

void Window::Print(int x, int y, const std::string& text)
{
#ifdef AB_UNIX
    mvprintw(x, y, text.c_str());
#else
    Goto(x, y);
    _cprintf(text.c_str());
#endif
}

void Window::Goto(int x, int y)
{
#ifdef AB_UNIX
    move(x, y);
#else
    COORD c = { static_cast<short>(x), static_cast<short>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
#endif
}

void Window::Clear()
{
#ifdef AB_UNIX
    clear();
#else
    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
#endif
}

void Window::ShowCursor(bool visible)
{
#ifdef AB_UNIX
    curs_set(visible ? TRUE : FALSE);
#else
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(out, &cursorInfo);
#endif
}