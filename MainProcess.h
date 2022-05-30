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
void handleReturnVoid();
void closeScope();
void checkBreakOrContinue(string flag);
void handleAssign(Expression* id, Expression* exp);
void handleExpReturn(Expression* exp);
void handleDeclarationAndInitiation(Expression* type, Expression* id, Expression* exp);
void handleDeclarationAndInitiationAuto(Expression* type, Expression* id, Expression* exp);
void handleDeclaration(Expression* id);
void addVariableToSymbolTableAuto(Expression* type, Expression* id);
void addFunctionEntryToSymbolTable(Expression* ret_type, Expression* id, Expression* args);
void addArgumentsToSymbolTable(Expression* args);
bool isWhile();
void addVariableToSymbolTable(Expression* type, Expression* id);
void checkIfMainExists();
std::string returnTypeOfLastFunc();

#endif