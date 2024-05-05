#include "lang.h"
#include "operators.h"
#include "error_allocator.h"
#include "tree.h"
#include "operators.h"


int CtorFunctions(Function** funcs, err_allocator* err_alloc)
{
    (*funcs) = (Function*) calloc(MAX_NUM_FUNCTIONS, sizeof(Function));
    if ((*funcs) == NULL)
    {
        INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
        err_alloc->need_call = true;
        return 1;
    }

    for (size_t i = 0; i < MAX_NUM_FUNCTIONS; i++)
    {
        (*funcs)[i].vars = (Name*) calloc(MAX_NUM_NAMES, sizeof(Name));
        if ((*funcs)[i].vars == NULL)
        {
            INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
            err_alloc->need_call = true;
            return 1;
        }
    }
    
    return 0;
}

int DtorFunctions(Function** funcs)
{
    for(size_t i = 0; i < MAX_NUM_FUNCTIONS; i++)
    {
        free((*funcs)[i].vars);
    }
    
    free(*funcs);

    return 0;
}

int CtorTokens(Tokens* tkns, Text* buf, err_allocator* err_alloc)
{
    tkns->size = buf->size_buffer;
    tkns->position = 0;
    tkns->tokens = (Node**) calloc(tkns->size, sizeof(Node*));
    if (!tkns->tokens)
    {
        INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
        err_alloc->need_call = true;
        return 1;
    }
    
    return 0;
}

int DtorTokens(Tokens* tkns)
{
    for (size_t i = 0; i < tkns->size; i++)
        free(tkns->tokens[i]);

    tkns->size = (size_t) INT_MAX;
    tkns->position = (size_t) INT_MAX;
    free(tkns->tokens);
    
    return 0;
}

static void SkipSpaces(Text* buf)
{
    while (isspace(buf->str[buf->position]))
        buf->position++;
}

int CompleteTokens(Tokens* tkns, Text* buf, err_allocator* err_alloc)
{
    size_t old_position = 0;

    while (buf->str[buf->position] != '\0')
    {
        old_position = buf->position;
        SkipSpaces(buf);
        
        if (buf->position == buf->size_buffer)
            break;

        ParseOperator(tkns, buf);
        
        ParseNameOrNumber(tkns, buf, err_alloc);

        if (old_position == buf->position)
        {
            tkns->tokens[tkns->position] = CreateOperator(NO_OP, NULL, NULL);
            tkns->tokens[tkns->position]->text_pos = buf->position;
            INSERT_ERROR_NODE(err_alloc, "unknown instruction or operator");
            err_alloc->need_call = true;
            return 1;
        }
    }

    tkns->tokens[tkns->position] = CreateOperator(END, NULL, NULL);
    tkns->tokens[tkns->position]->text_pos = tkns->position;

    tkns->position++;

    tkns->size = tkns->position;

    Node** ptr = (Node**) realloc(tkns->tokens, tkns->size * sizeof(Node*));

    if (ptr == NULL) 
    {
        INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault (realloc)");
        err_alloc->need_call = true;
        return 1;
    }

    tkns->tokens = ptr;
    tkns->position = 0;

    return 0;
}   


int ParseOperator(Tokens* tkns, Text* buf)
{
    for (size_t i = 0; i < sizeof(language_cmds) / sizeof(Command); i++)
    {   
        if (language_cmds[i].size_name == 0)
        {
            /*DO NOTHING*/
        }
        else if (strncmp(buf->str + buf->position, language_cmds[i].name, language_cmds[i].size_name) == 0)
        {
            tkns->tokens[tkns->position] = CreateOperator(language_cmds[i].id, NULL, NULL);
            tkns->tokens[tkns->position]->text_pos = buf->position;
        
            tkns->position++;
            buf->position += language_cmds[i].size_name;

            return 0;
        }
    }
    return 0;
}


int ParseNameOrNumber(Tokens* tkns, Text* buf, err_allocator* err_alloc)
{
    if (buf->str[buf->position] == '|')
    {
        (buf->position)++;

        MorseAlhabet morse_word[MAX_SIZE_NAME] = {};

        size_t i_vars = 0;
        size_t text_pos = buf->position - 1;
        
        if (ReadMorseCode(morse_word, &i_vars, buf) != 0)
        {
            err_alloc->need_call = true;
            INSERT_ERROR_NODE(err_alloc, "invalid executing ReadMorseCode");
            return 1;
        }
        
        double value = 0;

        if (((morse_word[0]) - MORSE_0) > (MORSE_9 - MORSE_0))
        {
            char name_var[MAX_SIZE_NAME] = {};
            size_t size_name = 0;
            

            if (TranslateMorseCode(name_var, morse_word, &size_name) != 0)
            {
                err_alloc->need_call = true;
                INSERT_ERROR_NODE(err_alloc, "invalid executing ReadMorseCode");
                return 1;
            }

            size_t id_name = 0;

            if (tkns->tokens[tkns->position - 1]->data.id_op == DEFINE)
            {
                tkns->tokens[tkns->position] = CreateFunction(id_name, name_var, NULL, NULL);
            }
            else
            {
                tkns->tokens[tkns->position] = CreateVariable(id_name, name_var, NULL, NULL);
            }

            tkns->tokens[tkns->position]->text_pos = text_pos;
            tkns->position++;
            
            return 0;
        }

        for (size_t i = 0; i < i_vars; i++)
        {
            if ((morse_word[i] - MORSE_0) > 10)
            {
                err_alloc->need_call = true;
                INSERT_ERROR_NODE(err_alloc, "syntax error");
                return 1;
            }
            value = 10 * value + (double)(morse_word[i] - MORSE_0);
        }

        tkns->tokens[tkns->position] = CreateNumber(value, NULL, NULL);
        tkns->tokens[tkns->position]->text_pos = text_pos;
        tkns->position++;

        return 0;
    }

    return 0;


}


bool FindSymbol(char* source, const MorseAlpha* morse_symbol, const size_t num_symbols, MorseAlhabet* id)
{
    for (size_t i = 0; i < num_symbols; i++)
    {
        if (strcmp(source, morse_symbol[i].encoding) == 0)
        {
            (*id) = morse_symbol[i].value;

            return true;
        }
    }
    
    return false;
}


int ReadMorseCode(MorseAlhabet* id_value, size_t* i_vars, Text* buf)
{
    char var[SIZE_MORSE_SYMBOL] = {};
    size_t i_var  = 0;
   
    MorseAlhabet id = MORSE_0;

    while (buf->str[buf->position] != '|')
    {
        var[i_var] = buf->str[buf->position];
        buf->position++;
        i_var++;

        if (buf->str[buf->position] == ' ')
        {
            FindSymbol(var, morse_alnum, sizeof(morse_alnum) / sizeof(MorseAlpha), &id);
    
            id_value[*i_vars] = id;

            (*i_vars)++;

            if (*i_vars == MAX_SIZE_NAME) 
                return 1;

            memset(var, 0, SIZE_MORSE_SYMBOL);
            i_var = 0;
            buf->position++;
        }
    }

    if (i_var != 0)
    {
        FindSymbol(var, morse_alnum, sizeof(morse_alnum) / sizeof(MorseAlpha), &id);

        id_value[*i_vars] = id;
        (*i_vars)++;
    }

    buf->position++;

    return 0;
}


int TranslateMorseCode(char* name_var, MorseAlhabet* var, size_t* size_name)
{
    size_t i = 0;
    while (var[i] != 0)
    {
        name_var[i] = var[i];
        i++;
        if (i == MAX_SIZE_NAME)
            return 1;
    }
    (*size_name) = i;

    return 0;
}

int AAAAAAAAA(Function* func, char* name_var, size_t* id_var)
{
    bool match = false;

    for (size_t i_var = 0; i_var < func->num_names; i_var++)
    {
        if (strncmp(name_var, func->vars[i_var].name, func->vars[i_var].name_size) == 0)
        {
            match = true;
            (*id_var) = func->vars[i_var].id;
            return 0;
        }
    }

    if (match == false)
    {
        size_t size_name = strlen(name_var);

        (*id_var) = func->num_names; //+ func_it->funcs[func_it->size].offset;

        func->vars[func->num_names].id = (*id_var);
        func->vars[func->num_names].type = VARIABLE;
        func->vars[func->num_names].name_size = size_name;

        memcpy(func->vars[func->num_names].name, name_var, size_name);
        (func->num_names)++;
    }

    return 0;
}

int MatchNamesTable(Function** func, size_t index_func, char* name_var, size_t* id_var)
{
    bool match = false;

    Function* current = &((*func)[index_func]);

    // ((*func)[index_func]).

    for (size_t i = 0; i < current->num_names; i++)
    {
        if (strncmp(name_var, current->vars[i].name, current->vars[i].name_size) == 0)
        {
            match = true;
            (*id_var) = current->vars[i].id;
            return 0;
        }
    }

    if (match == false)
    {
        size_t size_name = strlen(name_var);

        (*id_var) = current->num_names + GetFunctionOffset(func, index_func);

        current->vars[current->num_names].id = (*id_var);
        current->vars[current->num_names].type = VARIABLE;
        current->vars[current->num_names].name_size = size_name;

        memcpy(current->vars[current->num_names].name, name_var, size_name);
        (current->num_names)++;
    }

    return 0;
}


size_t GetFunctionOffset(Function** funcs, size_t index_func)
{
    size_t offset = 0;

    for (size_t i = 0; i < index_func; i++)
    {
        offset += (*funcs)[i].num_names;
    }

    return offset;
}




Node* GetGrammar(Tokens* tkns, Function** funcs, size_t* index_func, err_allocator* err_alloc)
{
    while (tkns->tokens[tkns->position]->data.id_op == DEFINE)
    {
        (tkns->position)++;
        
        (*funcs)[*index_func].tree.root = GetFunction(tkns, funcs, *index_func, err_alloc);

        if ((*funcs)[*index_func].tree.root == NULL || err_alloc->need_call == true)
        {
            INSERT_ERROR_NODE(err_alloc, "function parsing is failed");
            err_alloc->need_call = true;
            return NULL; 
        }

        (*index_func)++;
    }

    return (*funcs)[*index_func - 1].tree.root;
}


Node* GetFunction(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* value_1 = NULL;

    if (tkns->tokens[tkns->position]->type == FUNCTION)
    {
        value_1 = tkns->tokens[tkns->position];
        memcpy((*funcs)[index_func].name, value_1->data.name, strlen(value_1->data.name) + 1);
        
        value_1->data.id_var = index_func;
        
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected function");
        err_alloc->need_call = true;
        return NULL;
    }

    value_1->left = GetArgument(tkns, funcs, index_func, err_alloc);
    if (err_alloc->need_call == true)
    {
        INSERT_ERROR_NODE(err_alloc, "invalid executing GetArgument");
        return NULL;
    }    

    if (tkns->tokens[tkns->position]->data.id_op == L_CURLY_BRACKET)
    {
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <{>");
        err_alloc->need_call = true;
        return NULL;
    }

    value_1->right = GetOperators(tkns, funcs, index_func, err_alloc);

    if (tkns->tokens[tkns->position]->data.id_op == R_CURLY_BRACKET)
    {
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <}>");
        return NULL;
    }

    return value_1;
}


Node* GetOperators(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* node_operator = NULL;

    Node* current_token = tkns->tokens[tkns->position];

    if (current_token->type == OPERATOR)
    {
        if ((tkns->tokens[tkns->position]->data.id_op == OP_CONDITION) || (tkns->tokens[tkns->position]->data.id_op == OP_LOOP))
        {
            node_operator = GetWhileOrIf(tkns, funcs, index_func, err_alloc);
        }
        else if (tkns->tokens[tkns->position]->data.id_op == RET)
        {
            node_operator = GetReturn(tkns, funcs, index_func, err_alloc);
        }
        else if ((tkns->tokens[tkns->position]->data.id_op == INPUT) || (tkns->tokens[tkns->position]->data.id_op == OUTPUT))
        {
            node_operator = GetInOutput(tkns, funcs, index_func, err_alloc);
        }
    }
    else if (tkns->tokens[tkns->position]->type == VARIABLE)
    {
        node_operator = GetAssign(tkns, funcs, index_func, err_alloc);
    }
    
    if (node_operator)
    {
        node_operator->right = GetOperators(tkns, funcs, index_func, err_alloc);
    }

    return node_operator;
}

Node* GetInOutput(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* node_in_out = NULL;

    if ((tkns->tokens[tkns->position]->data.id_op == INPUT) || (tkns->tokens[tkns->position]->data.id_op == OUTPUT))
    {
        node_in_out = tkns->tokens[tkns->position];
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected in/out put");
        err_alloc->need_call = true;
        return NULL;
    }

    if (tkns->tokens[tkns->position]->data.id_op == L_BRACKET)
    {
        tkns->position++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <(>");
        err_alloc->need_call = true;
        return NULL;
    }
    
    node_in_out->left = GetVariable(tkns, funcs, index_func, err_alloc);

    if (tkns->tokens[tkns->position]->data.id_op == R_BRACKET)
    {
        tkns->position++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <)>");
        err_alloc->need_call = true;
        return NULL;
    }

    Node* current = NULL;

    if (tkns->tokens[tkns->position]->data.id_op == SEMICOLON)
    {
        current = tkns->tokens[tkns->position];
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <;>");
        err_alloc->need_call = true;
        return NULL;
    }

    current->left = node_in_out;

    return current;
}

Node* GetArgument(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    if (tkns->tokens[tkns->position]->data.id_op == L_BRACKET)
    {
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <(>");
        err_alloc->need_call = true;
        return NULL;
    }

    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        (tkns->position)++;
        return NULL;
    }

    Node* value_1 = NULL;
    Node* value_2 = NULL;

    do 
    {
        if (tkns->tokens[tkns->position]->type == VARIABLE)
        {
            value_1 = GetExpression(tkns, funcs, index_func, err_alloc);
        }
        else if (tkns->tokens[tkns->position]->type == OPERATOR)
        {
            if (tkns->tokens[tkns->position]->data.id_op != COMMA)
                value_1 = GetExpression(tkns, funcs, index_func, err_alloc);
        }
        if (tkns->tokens[tkns->position]->data.id_op == COMMA)
        {
            value_2 = tkns->tokens[tkns->position];
            (tkns->position)++;

            value_2->left = value_1;
            value_2->right = GetExpression(tkns, funcs, index_func, err_alloc);
            
            value_1 = value_2;
        }
        
    } while ((tkns->tokens[tkns->position]->data.id_op != R_BRACKET));

    (tkns->position)++;

    return value_1;
}

Node* GetReturn(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* node_ret = NULL;

    if (tkns->tokens[tkns->position]->data.id_op == RET)
    {
        node_ret = tkns->tokens[tkns->position];
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected ret");
        err_alloc->need_call = true;
        return NULL;
    }

    node_ret->left = GetExpression(tkns, funcs, index_func, err_alloc); 

    if (tkns->tokens[tkns->position]->data.id_op == SEMICOLON)
    {
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <;>");
        
        err_alloc->need_call = true;
        return NULL;
    }

    return node_ret;
}

Node* GetWhileOrIf(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* node_cond = NULL;
    Node* next_node = NULL;
    
    if ((tkns->tokens[tkns->position]->data.id_op == OP_CONDITION) || (tkns->tokens[tkns->position]->data.id_op == OP_LOOP))
    {
        node_cond = tkns->tokens[tkns->position];
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected if or while");
        err_alloc->need_call = true;
        return NULL;
    }

    node_cond->left = GetBoolPrimaryExpression(tkns, funcs, index_func, err_alloc);
    
    if (tkns->tokens[tkns->position]->data.id_op == L_CURLY_BRACKET)
    {
        (tkns->position)++;
        
        node_cond->right = GetOperators(tkns, funcs, index_func, err_alloc);

        if (tkns->tokens[tkns->position]->data.id_op == R_CURLY_BRACKET)
        {
            (tkns->position)++;
        }
        else
        {
            INSERT_ERROR_NODE(err_alloc, "expected <}>");
            err_alloc->need_call = true;
            return NULL;
        }


        if (tkns->tokens[tkns->position]->data.id_op == SEMICOLON)
        {
            next_node = tkns->tokens[tkns->position];
            next_node->left = node_cond;
            (tkns->position)++;
        }
        else
        {
            INSERT_ERROR_NODE(err_alloc, "expected <;>");
            err_alloc->need_call = true;
            return NULL;
        }
    }
    else 
    {
        INSERT_ERROR_NODE(err_alloc, "expected <{>");
        err_alloc->need_call = true;
        return NULL;
    }

    return next_node;
}

Node* GetAssign(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* node_var = GetVariable(tkns, funcs, index_func, err_alloc);
    Node* node_assign  = NULL;
    
    if (tkns->tokens[tkns->position]->data.id_op == OP_ASSIGN)
    {
        node_assign = tkns->tokens[tkns->position];
        (tkns->position)++;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <=>");
        err_alloc->need_call = true;
        return NULL;
    }

    Node* node_expr = GetExpression(tkns, funcs, index_func, err_alloc);
    
    node_assign->left  = node_var;
    node_assign->right = node_expr;

    Node* current = NULL;

    if (tkns->tokens[tkns->position]->data.id_op == SEMICOLON)
    {
        current = tkns->tokens[tkns->position];
        (tkns->position)++;
        current->left = node_assign;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected <;>");
        err_alloc->need_call = true;
        return NULL;
    }

    return current;
}

Node* GetExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* value_1 = GetTerm(tkns, funcs, index_func, err_alloc);
    Node* value_3 = NULL;

    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        while ((tkns->tokens[tkns->position]->data.id_op == OP_ADD) || (tkns->tokens[tkns->position]->data.id_op == OP_SUB))
        {
            value_3 = tkns->tokens[tkns->position];
            tkns->position++;
            
            Node* value_2 = GetTerm(tkns, funcs, index_func, err_alloc);
            
            value_3->left = value_1;
            value_3->right = value_2;

            value_1 = value_3;
        }
    }
    
    return value_1;
}

Node* GetTerm(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* value_1 = GetUnary(tkns, funcs, index_func, err_alloc);
    
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        while ((tkns->tokens[tkns->position]->data.id_op == OP_MUL) || (tkns->tokens[tkns->position]->data.id_op == OP_DIV))
        {
            Node* value_3 = tkns->tokens[tkns->position];
            tkns->position++;
            
            Node* value_2 = GetUnary(tkns, funcs, index_func, err_alloc);
            value_3->left = value_1;
            value_3->right = value_2;
            
            value_1 = value_3;
        }
    }
    
    return value_1;
}

Node* GetUnary(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* value_1 = NULL;
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        for (size_t i = 0; i < sizeof(unary_cmds) / sizeof(Command); i++)
        {
            if (unary_cmds[i].id == tkns->tokens[tkns->position]->data.id_op)
            {
                value_1 = tkns->tokens[tkns->position];
                tkns->position++;
                Node* value_2 = GetPrimaryExpression(tkns, funcs, index_func, err_alloc);

                value_1->right = value_2;
            }
        }
    }
    
    if (value_1 == NULL)
    {
        value_1 = GetPrimaryExpression(tkns, funcs, index_func, err_alloc);
    }
    return value_1;
}

Node* GetBoolingExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    Node* value_1 = GetExpression(tkns, funcs, index_func, err_alloc);
    Node* value_3 = NULL;

    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        if ((tkns->tokens[tkns->position]->data.id_op == OP_ABOVE) || (tkns->tokens[tkns->position]->data.id_op == OP_BELOW) || (tkns->tokens[tkns->position]->data.id_op == OP_EQUAL) || (tkns->tokens[tkns->position]->data.id_op == OP_NO_EQUAL))
        {
            value_3 = tkns->tokens[tkns->position];
            tkns->position++;
            
            Node* value_2 = GetExpression(tkns, funcs, index_func, err_alloc);
            
            value_3->left = value_1;
            value_3->right = value_2;

            value_1 = value_3;
        }
    }
    
    return value_1;
}

Node* GetBoolPrimaryExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        if (tkns->tokens[tkns->position]->data.id_op == L_BRACKET)
        {
            tkns->position++;
            Node* val = GetBoolingExpression(tkns, funcs, index_func, err_alloc);
            if (tkns->tokens[tkns->position]->data.id_op == R_BRACKET)
                tkns->position++;
            else
                return NULL;

            return val;
        }
    }

    return GetVariable(tkns, funcs, index_func, err_alloc);
}

Node* GetPrimaryExpression(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    if (tkns->tokens[tkns->position]->type == OPERATOR)
    {
        if (tkns->tokens[tkns->position]->data.id_op == L_BRACKET)
        {
            tkns->position++;
            Node* val = GetExpression(tkns, funcs, index_func, err_alloc);
            if (tkns->tokens[tkns->position]->data.id_op == R_BRACKET)
                tkns->position++;
            else
                return NULL;

            return val;
        }
    }
    return GetVariable(tkns, funcs, index_func, err_alloc);
}

Node* GetVariable(Tokens* tkns, Function** funcs, size_t index_func, err_allocator* err_alloc)
{
    if (tkns->tokens[tkns->position]->type == VARIABLE)
    {
        Node* var = tkns->tokens[tkns->position];
        (tkns->position)++;

        size_t id = 0;

        if (tkns->tokens[tkns->position]->data.id_op == L_BRACKET)
        {
            var->type = FUNCTION;
            var->left = GetArgument(tkns, funcs, index_func, err_alloc);
            
            bool match = false;

            for (size_t i = 0; i < index_func + 1; i++)
            {
                if (strncmp((*funcs)[i].name, var->data.name, strlen(var->data.name)) == 0)
                {
                    id = i;
                    match = true;
                }
            }

            if (!match)
            {
                INSERT_ERROR_NODE(err_alloc, "function missing declared");
                err_alloc->need_call = true;
                return NULL;
            }

            var->data.id_fun = id;
        }
        else
        {
            MatchNamesTable(funcs, index_func, var->data.name, &id);
            var->data.id_var = id;
        }

        return var;
    }
    
    return GetNumber(tkns, err_alloc);
}

Node* GetNumber(Tokens* tkns, err_allocator* err_alloc)
{
    if (tkns->tokens[tkns->position]->type == NUMBER)
    {
        Node* num = tkns->tokens[tkns->position];
        tkns->position++;
        return num;
    }
    else
    {
        INSERT_ERROR_NODE(err_alloc, "expected number");
        err_alloc->need_call = true;
        return NULL;
    }

    return NULL;
}