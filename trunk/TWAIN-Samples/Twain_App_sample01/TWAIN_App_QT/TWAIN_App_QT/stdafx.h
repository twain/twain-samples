#include <QtGui>
#include <QtScript\QtScript>

#ifdef _WIN32
#include "windows.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#endif //#ifdef _WIN32

#ifndef TRACE
#ifdef _DEBUG
#define TRACE OutputDebugStringFmt
#else //#ifdef _DEBUG
#define TRACE
#endif //#ifdef _DEBUG
#endif //#ifndef TRACE

void OutputDebugStringFmt(LPCTSTR lpOutputString, ...);


