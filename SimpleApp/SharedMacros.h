// *****************************************************************
// This file is forced to be include in all files in current project
// *****************************************************************
// NOTE: you need #include "SharedMacros.h" explicitly in all headers that will be moc'ed,  because qmake not process fore-included files 
//       and produce "No relevant classes found. No output generated" error when moc .h file
#pragma once
#define CONSOLE_LANGUAGE_PROBLEM	0
#define LAYOUTS						0
#define CONNECT_EXPIRED_PTR			0
#define LOADER						0
#define STYLE_SHEETS				1