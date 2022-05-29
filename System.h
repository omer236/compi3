#pragma once
#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H
#include <string>
#include <stack>
#include "hw3_output.hpp"
#include "SymbolTable.h"

// main data stracture
class System {
public:
	// symbol table is the tables stack  - all of the variables and functions in all of the scopes
	std::vector<Scope> symbol_table;
	std::stack<int> offset_stack;
	static System& get_instance() {
		static System instance;
		return instance;
	}
	SymbolTableEntry* findEntryInSymbolTable(std::string entry_name);
};

void openGlobalScope();
void openScope(bool is_while = false);
void closeScope();
void handleBreakOrContinue(std::string flag);
void handleReturn();
void handleExpReturn(Expression* exp);
void handleAssign(Expression* id, Expression* exp);
void handleDec(Expression* type, Expression* id);
void handleDecAndInit(Expression* type, Expression* id, Expression* exp);
void checkIfMain();
void insertFuncToSymbolTable(Expression* ret_type, Expression* id, Expression* args);
void insertArgsToSymbolTable(Expression* args);
void insertVarToSymbolTable(Expression* type, Expression* id);
bool checkIfInWhile();
std::string returnTypeOfLastFunc();

#endif