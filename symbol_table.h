//
// Created by gal weizner on 5/16/2022.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <string>
#include <vector>
#include <map>
#include "Expression.h"

using namespace std;

class SymbolTableEntry {
public:
    string name;
    string type;
    int offset;
    bool is_function = false;
    virtual ~SymbolTableEntry() = default;

    SymbolTableEntry(std::string name, std::string type, int offset, bool is_func) :
            name(name), type(type), offset(offset), is_function(is_func) {}
};

class SymbolTableEntryFunction : public SymbolTableEntry {
public:
    vector<string> arguments_names;
    vector<string> arguments_types;

    SymbolTableEntryFunction() = default;
    SymbolTableEntryFunction(const string name, const string type) : SymbolTableEntry(name, type, 0, true), arguments_names(), arguments_types() {}
    SymbolTableEntryFunction(const string name, const string type, vector<string> args_name, vector<string> args_type) :
            SymbolTableEntry(name, type, 0, true), arguments_names(args_name), arguments_types(args_type) {}

    ~SymbolTableEntryFunction() override = default;
};
class Scope {
public:
    std::vector<SymbolTableEntry*> scope_symbol_table;
    bool is_while_scope;
    Scope() = default;
    Scope(bool is_while) : is_while_scope(is_while), scope_symbol_table() {}
    void insert(SymbolTableEntry* entry) {
        scope_symbol_table.push_back(entry);
    }
    void addFuncToScope(string name, string type, vector<string> args_name, vector<string> args_type) {
        SymbolTableEntryFunction* func = new SymbolTableEntryFunction(name, type ,args_name, args_type);
        scope_symbol_table.push_back(func);
    }
};

#endif