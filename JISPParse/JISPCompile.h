/*
** JISPCompile.h
**
** JISPCompile.h/cpp - stack machine for evaluation
**
** Copyright (c) 2009 - Jason Shankel
*/

#ifndef JISPCOMPILE_H
#define JISPCOMPILE_H

namespace JISP
{
    bool CompileFile(const wchar_t *filepath);
    bool CompileString(const char *data);
}

#endif // JISPCOMPILE_H