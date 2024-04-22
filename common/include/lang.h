#ifndef LANGlib
#define LANGlib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "tree.h"

const size_t MAX_NUM_VARS   = 10;
const size_t MAX_NUM_TOKENS = 50;
const size_t MAX_NUM_TABLES = 10;
const size_t MAX_NUM_NAMES  = 10;
const size_t MAX_NUM_FUNCTIONS = 10;

enum MorseAlhabet
{
    MORSE_A = 65,
    MORSE_B,
    MORSE_C,
    MORSE_D, 
    MORSE_E,
    MORSE_F,
    MORSE_G,
    MORSE_H,
    MORSE_I,
    MORSE_J,
    MORSE_K,
    MORSE_L,
    MORSE_M,
    MORSE_N,
    MORSE_O,
    MORSE_P,
    MORSE_Q,
    MORSE_R, 
    MORSE_S, 
    MORSE_T, 
    MORSE_U,
    MORSE_V,
    MORSE_W,     
    MORSE_X,
    MORSE_Y,
    MORSE_Z,
    MORSE_0 = 0, 
    MORSE_1, 
    MORSE_2, 
    MORSE_3, 
    MORSE_4, 
    MORSE_5, 
    MORSE_6, 
    MORSE_7, 
    MORSE_8, 
    MORSE_9,  
    MORSE_POINT, 
    MORSE_COMMA, 
    MORSE_COLON,
    MORSE_SEMICOLON, 
    MORSE_BRACKET, 
    MORSE_APOSTROPHE,
    MORSE_QUOTES,
    MORSE_DASH, //-
    MORSE_SLASH,
    MORSE_QUESTION_MARK,
    MORSE_EXPOINT
};


struct Tokens
{
    Node**      tokens;
    size_t      position;
    size_t      size;
};

struct Name
{
    Type        type;
    size_t      id;
    size_t      name_size;
    char        name[MAX_SIZE_NAME];
};

struct Function
{
    Tree        tree;
    Name*       vars;
    char        name[MAX_SIZE_NAME];
    size_t      id_fun;
    size_t      num_names;
};

//------------------------------------------------------------------------

struct MorseAlpha
{
    MorseAlhabet    value;
    char            encoding[MAX_SIZE_NAME];
};

const MorseAlpha morse_alpha[] = \
   {{MORSE_A, ".-"  }, {MORSE_B, "-..."}, {MORSE_W, ".--" }, {MORSE_G, "--." }, {MORSE_D, "-.." }, {MORSE_E, "."   },
    {MORSE_V, "...-"}, {MORSE_Z, "--.."}, {MORSE_I, ".."  }, {MORSE_J, ".---"}, {MORSE_K, "-.-" }, {MORSE_L, ".-.."},
    {MORSE_M, "--"  }, {MORSE_N, "-."  }, {MORSE_O, "---" }, {MORSE_P, ".--."}, {MORSE_R, ".-." }, {MORSE_S, "..." },
    {MORSE_T, "-"   }, {MORSE_U, "..-" }, {MORSE_F, "..-."}, {MORSE_H, "...."}, {MORSE_C, "-.-."}, {MORSE_Q, "--.-"},
    {MORSE_Y, "-..-"}, {MORSE_X, "-.--"}};
    
const MorseAlpha morse_digit[] = 
   {{MORSE_1, ".----"}, {MORSE_2, "..---"}, {MORSE_3, "...--"}, {MORSE_4, "....-"}, {MORSE_5, "....."},
    {MORSE_6, "-...."}, {MORSE_7, "--..."}, {MORSE_8, "---.."}, {MORSE_9, "----."}, {MORSE_0, "-----"}};
    
const MorseAlpha morse_alnum[] = 
   {{MORSE_A, ".-"  }, {MORSE_B, "-..."}, {MORSE_W, ".--" }, {MORSE_G, "--." }, {MORSE_D, "-.." }, {MORSE_E, "."   },
    {MORSE_V, "...-"}, {MORSE_Z, "--.."}, {MORSE_I, ".."  }, {MORSE_J, ".---"}, {MORSE_K, "-.-" }, {MORSE_L, ".-.."},
    {MORSE_M, "--"  }, {MORSE_N, "-."  }, {MORSE_O, "---" }, {MORSE_P, ".--."}, {MORSE_R, ".-." }, {MORSE_S, "..." },
    {MORSE_T, "-"   }, {MORSE_U, "..-" }, {MORSE_F, "..-."}, {MORSE_H, "...."}, {MORSE_C, "-.-."}, {MORSE_Q, "--.-"},
    {MORSE_Y, "-..-"}, {MORSE_X, "-.--"},
    {MORSE_1, ".----"}, {MORSE_2, "..---"}, {MORSE_3, "...--"}, {MORSE_4, "....-"}, {MORSE_5, "....."},
    {MORSE_6, "-...."}, {MORSE_7, "--..."}, {MORSE_8, "---.."}, {MORSE_9, "----."}, {MORSE_0, "-----"}};
    

const MorseAlpha morse_symbols[] = 
   {{MORSE_POINT,         "......"},
    {MORSE_COMMA,         "-.-.-."},
    {MORSE_COLON,         "---..."},
    {MORSE_SEMICOLON,     "-.-.-" },
    {MORSE_BRACKET,       "-.--.-"},
    {MORSE_APOSTROPHE,    ".----."},
    {MORSE_QUOTES,        ".-..-."},
    {MORSE_DASH,          "-....-"},
    {MORSE_SLASH,         "-..-." },
    {MORSE_QUESTION_MARK, "..--.."},
    {MORSE_EXPOINT,       "--..--"}};



int CtorTokens(Tokens* tkns, Text* buf, err_allocator* err_alloc);
int DtorTokens(Tokens* tkns);


int CtorFunctions(Function** funcs, err_allocator* err_alloc);
int DtorFunctions(Function** funcs);

int CompleteTokens(Tokens* tkns, Text* buf, err_allocator* err_alloc);

int ParseNameOrNumber(Tokens* tkns, Text* buf, err_allocator* err_alloc);
int ParseOperator(Tokens* tkns, Text* buf);

bool FindSymbol(char* source, const MorseAlpha* morse_symbol, const size_t num_symbols, MorseAlhabet* id);
int ReadMorseCode(MorseAlhabet* id_value, size_t* i_vars, Text* buf);

int MatchNamesTable(Function** func, size_t index_func, char* name_var, size_t* id_var);
int AAAAAAAAA(Function* func, char* name_var, size_t* id_var);


Node* GetGrammar(Tokens* tkns, Function** funcs, size_t* index_func, err_allocator* err_alloc);
Node* GetOperators(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);

Node* GetExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node* GetBoolingExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);

Node* GetTerm(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node* GetUnary(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);

Node* GetPrimaryExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node* GetBoolPrimaryExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node* GetVariable(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node* GetNumber(Tokens* tkns, err_allocator* err_alloc);
Node* GetArgument(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);

Node*  GetFunction(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node* GetWhileOrIf(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node*    GetAssign(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);

Node*  GetInOutput(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);
Node*    GetReturn(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc);


int TranslateMorseCode(char* name_var, MorseAlhabet* var, size_t* size_name);
size_t GetFunctionOffset(Function** funcs, size_t index_func);

#endif