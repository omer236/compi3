#include "MainProcess.h"
#include <iostream>

using namespace std;
using namespace output;

SymbolTableEntry* MainProcess::getEntryInSymbolTable(string entry_name) {
    vector<Scope>& symbol_table = this->symbol_table;
    for (const auto& scope : symbol_table) {
        for (const auto& current_entry : scope.scope_symbol_table) {
            if (current_entry->name == entry_name) {
                return current_entry;
            }
        }
    }
    return nullptr;
}

void createGlobalScope() {
    MainProcess& process = MainProcess::get_instance();
    Scope global_scope;
    vector<string> print_arguments_names;
    print_arguments_names.push_back("print_arg_string");
    vector<string> print_arguments_types;
    print_arguments_types.push_back("STRING");
    global_scope.addFuncToScope("print", "VOID", print_arguments_names, print_arguments_types);
    vector<string> printi_arguments_names;
    printi_arguments_names.push_back("printi_arg_string");
    vector<string> printi_arguments_types;
    printi_arguments_types.push_back("INT");
    global_scope.addFuncToScope("printi", "VOID", printi_arguments_names, printi_arguments_types);
    process.offset_stack.push(0);
    process.symbol_table.push_back(global_scope);
}

void openScope(bool if_while) {
    MainProcess& process = MainProcess::get_instance();
    Scope scope(if_while);
    process.symbol_table.push_back(scope);
    process.offset_stack.push(process.offset_stack.top());
}
void handleReturnVoid() {
    MainProcess& process = MainProcess::get_instance();
    vector <SymbolTableEntry*> function_definition = process.symbol_table[process.symbol_table.size() - 2].scope_symbol_table;
    SymbolTableEntryFunction* entry = dynamic_cast<class SymbolTableEntryFunction*>(function_definition[function_definition.size() - 1]);
    if (entry->type == "VOID") {
        return;
    }
    errorMismatch(yylineno);
    exit(1);
}
void closeScope() {
    endScope();
    MainProcess& process = MainProcess::get_instance();
    vector<SymbolTableEntry*> scope_symbol_table = process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table;
    for (auto const& sybmol_table_entry : scope_symbol_table) {
        if (sybmol_table_entry->is_function) {
            SymbolTableEntryFunction* function = dynamic_cast<class SymbolTableEntryFunction*>(sybmol_table_entry);
            printID(function->name, 0, makeFunctionType(function->type, function->arguments_types));
        }
    }
    for (auto const& sybmol_table_entry : scope_symbol_table) {
        if (!sybmol_table_entry->is_function) {
            printID(sybmol_table_entry->name, sybmol_table_entry->offset, sybmol_table_entry->type);
        }
    }
    process.offset_stack.pop();
    process.symbol_table.pop_back();
}

void checkBreakOrContinue(string flag) {
    MainProcess& process = MainProcess::get_instance();
    Scope current = process.symbol_table[process.symbol_table.size() - 1];
    if (!current.is_while_scope&&flag == "BREAK") {
        errorUnexpectedBreak(yylineno);
        exit(1);
    }
    else if (!current.is_while_scope && flag == "CONTINUE"){
        errorUnexpectedContinue(yylineno);
        exit(1);
    }
}

void handleAssign(Expression* id, Expression* exp) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry_of_id = process.getEntryInSymbolTable(id->name);
    string exp_type = getExpType(exp);
    if (entry_of_id == nullptr) {
        errorUndef(yylineno, id->name);
        exit(1);
    }
    else if (entry_of_id->is_function) {
        errorUndef(yylineno, id->name);
        exit(1);
    }
    else if ((entry_of_id->type != "INT" || exp_type != "BYTE")&&(entry_of_id->type != exp_type)) {
        errorMismatch(yylineno);
        exit(1);
    }
}

void handleExpReturn(Expression* exp) {
    string function_type = "temp";
    MainProcess& process = MainProcess::get_instance();
    int j = process.symbol_table.size() - 2;
    while (j >= 0)
    {
        int i = process.symbol_table[j].scope_symbol_table.size() - 1;
        while ( i >= 0)
        {
            if (process.symbol_table[j].scope_symbol_table[i]->is_function)
            {
                function_type = process.symbol_table[j].scope_symbol_table[i]->type;
                break;
            }
            i--;
        }
        j--;
    }
    if (function_type == "VOID") {
        errorMismatch(yylineno);
        exit(1);
    }
    string exp_type = getExpType(exp);
    if ((function_type != exp_type)&&(function_type != "INT" || exp_type != "BYTE")) {
        errorMismatch(yylineno);
        exit(1);
    }
}

void handleDeclarationAndInitiation(Expression* type, Expression* id, Expression* exp) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry_of_id = process.getEntryInSymbolTable(id->name);
    if (entry_of_id != nullptr) {
        errorDef(yylineno, id->name);
        exit(1);
    }
    string exp_type = getExpType(exp);
    if ((type->type != exp_type) && (type->type != "INT" || exp_type != "BYTE")) {
        errorMismatch(yylineno);
        exit(1);
    }
}
void handleDeclarationAndInitiationAuto(Expression* type, Expression* id, Expression* exp) {
    //cout << "type->type: " << type->type << endl;

    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry_of_id = process.getEntryInSymbolTable(id->name);
    if (entry_of_id != nullptr) {
        errorDef(yylineno, id->name);
        exit(1);
    }
    string exp_type = getExpType(exp);
    //cout << "exp_type: " << exp_type << endl;
    if(exp_type == "VOID" || exp_type == "STRING")
    {
        errorMismatch(yylineno);
        exit(1);
    }
    /*if ((type->type != exp_type) && (type->type != "INT" || exp_type != "BYTE")) {
        errorMismatch(yylineno);
        exit(1);
    }*/
}
void handleDeclaration(Expression* id) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        return;
    }
    errorDef(yylineno, id->name);
    exit(1);
}

void addFunctionEntryToSymbolTable(Expression* ret_type, Expression* id, Expression* args) {
    SymbolTableEntryFunction* function_entry;
    MainProcess& process = MainProcess::get_instance();
    if (args != nullptr) {
        ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
        reverse(args_list->arguments_names.begin(), args_list->arguments_names.end());
        reverse(args_list->arguments_types.begin(), args_list->arguments_types.end());
        function_entry = new SymbolTableEntryFunction(id->name, ret_type->type, args_list->arguments_names, args_list->arguments_types);
    }
    if (args == nullptr) {
        function_entry = new SymbolTableEntryFunction(id->name, ret_type->type);
    }
    process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(function_entry);
}

void addArgumentsToSymbolTable(Expression* args) {
    if (args == nullptr)
        return;
    MainProcess& process = MainProcess::get_instance();
    ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
    for (int i = 0; i < args_list->arguments_names.size(); i++) {
        SymbolTableEntry* current_entry = new SymbolTableEntry(args_list->arguments_names[i], args_list->arguments_types[i], -1+i*-1, false);
        process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(current_entry);
    }
}
bool isWhile() {
    MainProcess& process = MainProcess::get_instance();
    return process.symbol_table.back().is_while_scope;
}
void addVariableToSymbolTable(Expression* type, Expression* id) {
    int offset_new;
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* var_entry = new SymbolTableEntry(id->name, type->type, process.offset_stack.top(), false);
    offset_new = process.offset_stack.top() + 1;
    process.offset_stack.pop();
    process.offset_stack.push(offset_new);
    process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(var_entry);
}

void addVariableToSymbolTableAuto(Expression* type, Expression* id) {
    int offset_new;
    MainProcess& process = MainProcess::get_instance();
    string exp_type = getExpType(type);
    SymbolTableEntry* var_entry = new SymbolTableEntry(id->name, exp_type, process.offset_stack.top(), false);
    offset_new = process.offset_stack.top() + 1;
    process.offset_stack.pop();
    process.offset_stack.push(offset_new);
    process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(var_entry);
}
void checkIfMainExists() {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* main_entry = process.getEntryInSymbolTable("main");
    if (main_entry == nullptr) {
        errorMainMissing();
        exit(1);
    }
    else if (main_entry->type != "VOID") {
        errorMainMissing();
        exit(1);
    }
    else if (dynamic_cast<class SymbolTableEntryFunction*>(main_entry)->arguments_names.size()!=0) {
        errorMainMissing();
        exit(1);
    }
}
