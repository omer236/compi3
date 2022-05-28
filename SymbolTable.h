#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <string>
#include <vector>
#include <map>
#include "Expression.h"

// typedef std::pair<std::string, std::string> FUNC_ARG;

// proprties of entry in symbol table
class SymbolTableEntry {
public:
	int offset;
	std::string name;
	std::string type;
	bool is_const;
	bool is_func = false;
	virtual ~SymbolTableEntry() = default;

	SymbolTableEntry(std::string name, std::string type, int offset, bool is_const, bool is_func) :
		name(name), type(type), offset(offset), is_const(is_const), is_func(is_func) {}
};

class SymbolTableEntryFunction : public SymbolTableEntry {
public:
	// pair of name and type of each argument
	//std::vector<FUNC_ARG> args; //TODO: add func that will calculate args offset by their position in the vector
	std::vector<std::string> args_name;
	std::vector<std::string> args_type;
	std::vector<bool> args_is_const;

	// return type == type
	SymbolTableEntryFunction() = default;
	SymbolTableEntryFunction(const std::string name, const std::string type) : SymbolTableEntry(name, type, 0, false, true), args_name(), args_type(), args_is_const() {}
	SymbolTableEntryFunction(const std::string name, const std::string type, std::vector<std::string> args_name, std::vector<std::string> args_type, std::vector<bool> args_is_const) :
		SymbolTableEntry(name, type, 0, false, true), args_name(args_name), args_type(args_type), args_is_const(args_is_const) {}

	~SymbolTableEntryFunction() override = default;
};
// one sybmol table - all the functions and var in one scope
class Scope {
public:
	// map of std::string is name of var or function and SymbolTableEntry
	std::vector<SymbolTableEntry*> scope_symbol_table;
	//SymbolTableEntryFunction* function;
	bool is_while_scope;
	Scope() = default;
	Scope(bool is_while) : is_while_scope(is_while), scope_symbol_table() {}
	void insert(SymbolTableEntry* entry) {
		scope_symbol_table.push_back(entry);
	}
	void addFuncToScope(std::string name, std::string type, std::vector<std::string> args_name, std::vector<std::string> args_type, std::vector<bool> args_is_const) {
		SymbolTableEntryFunction* func = new SymbolTableEntryFunction(name, type,args_name, args_type, args_is_const);
		scope_symbol_table.push_back(func);
	}
};

#endif