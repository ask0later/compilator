#ifndef DUMPlib
#define DUMPlib

#include "lang.h"
const size_t INSCRIPTION_NUM = 20;

void DumpErrorLocation(Tokens* tkns, Text* buf, const char file[]);

void DumpTokens(Tokens* tkns);
void DumpToken(Node* current);
void DumpErrors(TreeError error, Tokens* tkns, Text* buf);
void DumpNamesTable(Function* func);

#endif