#pragma once
#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H
#include <string>
#include <stack>
#include "hw3_output.hpp"
#include "SymbolTable.h"

using namespace std;

class MainProcess {
public:
    vector<Scope> symbol_table;
    stack<int> offset_stack;
    static MainProcess& get_instance() {
        static MainProcess instance;
        return instance;
    }
    SymbolTableEntry* getEntryInSymbolTable(string entry_name);
};

void createGlobalScope();
void openScope(bool is_while = false);
void closeScope();
void checkBreakOrContinue(string flag);
void handleReturnVoid();
void handleExpReturn(Expression* exp);
void handleAssign(Expression* id, Expression* exp);
void handleDeclaration(Expression* id);
void handleDeclarationAndInitiation(Expression* type, Expression* id, Expression* exp);
void checkIfMainExists();
void addFunctionEntryToSymbolTable(Expression* ret_type, Expression* id, Expression* args);
void addArgumentsToSymbolTable(Expression* args);
void addVariableToSymbolTable(Expression* type, Expression* id);
bool isWhile();
std::string returnTypeOfLastFunc();

#endif