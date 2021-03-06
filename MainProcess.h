#pragma once
#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H
#include <string>
#include <stack>
#include "hw3_output.hpp"
#include "symbol_table.h"

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
void handleReturnVoid();
void handleAssign(Expression* id, Expression* exp);
void checkBreakOrContinue(string flag);
void handleExpReturn(Expression* exp);
void handleDeclarationAndInitiation(Expression* type, Expression* id, Expression* exp);
void handleDeclarationAndInitiationAuto(Expression* type, Expression* id, Expression* exp);
void handleDeclaration(Expression* id);
void addArgumentsToSymbolTable(Expression* args);
void addVariableToSymbolTableAuto(Expression* type, Expression* id);
void addFunctionEntryToSymbolTable(Expression* ret_type, Expression* id, Expression* args);
bool isWhile();
void addVariableToSymbolTable(Expression* type, Expression* id);
void checkIfMainExists();
std::string returnTypeOfLastFunc();

#endif