#include "MainProcess.h"
#include <iostream>

using namespace std;

SymbolTableEntry* MainProcess::getEntryInSymbolTable(string entry_name) {
	vector<Scope>& symbol_table = this->symbol_table;
	for (const auto& scope : symbol_table) {
		vector<SymbolTableEntry*> current_scope = scope.scope_symbol_table;
		for (const auto& entry : current_scope) {
			if (entry->name == entry_name) {
				return entry;
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

void openScope(bool is_while) {
    MainProcess& process = MainProcess::get_instance();
	Scope scope(is_while);
    process.symbol_table.push_back(scope);
    process.offset_stack.push(process.offset_stack.top());
}

void closeScope() {
	output::endScope();
    MainProcess& process = MainProcess::get_instance();
	vector<SymbolTableEntry*> scope_symbol_table = process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table;
	for (auto const& sybmol_table_entry : scope_symbol_table) {
		if (sybmol_table_entry->is_function) {
			SymbolTableEntryFunction* function = dynamic_cast<class SymbolTableEntryFunction*>(sybmol_table_entry);
			output::printID(function->name, 0, output::makeFunctionType(function->type, function->arguments_types));
		}
		else {
			output::printID(sybmol_table_entry->name, sybmol_table_entry->offset, sybmol_table_entry->type);
		}
	}
    process.offset_stack.pop();
    process.symbol_table.pop_back();
}

void checkBreakOrContinue(string flag) {
    MainProcess& process = MainProcess::get_instance();
	Scope current_scope = process.symbol_table[process.symbol_table.size() - 1];
	if (!current_scope.is_while_scope) {
		if (flag == "BREAK") {
			output::errorUnexpectedBreak(yylineno);
			exit(1);
		}
		else if (flag == "CONTINUE"){
			output::errorUnexpectedContinue(yylineno);
			exit(1);
		}
	}
}

void handleReturnVoid() {
    MainProcess& process = MainProcess::get_instance();
	vector <SymbolTableEntry*> fun_def_symbol_table = process.symbol_table[process.symbol_table.size() - 2].scope_symbol_table;
	SymbolTableEntryFunction* function_entry = dynamic_cast<class SymbolTableEntryFunction*>(fun_def_symbol_table[fun_def_symbol_table.size() - 1]);
	if (function_entry->type == "VOID") {
	    return;
	}
    output::errorMismatch(yylineno);
    exit(1);
}

void handleExpReturn(Expression* exp) {
    MainProcess& process = MainProcess::get_instance();
    string result = "temp";
    int index_scope = process.symbol_table.size() - 2;
    for (int j = index_scope; j >= 0; j--)
    {
        int index_entry = process.symbol_table[j].scope_symbol_table.size() - 1;
        for (int i = index_entry; i >= 0; i--)
        {
            if (process.symbol_table[j].scope_symbol_table[i]->is_function)
            {
                result = process.symbol_table[j].scope_symbol_table[i]->type;
                break;
            }
        }
    }
    if (result == "VOID") {
		output::errorMismatch(yylineno);
		exit(1);
	}
	string exp_type = getExpType(exp);
	if (result != exp_type) {
		if (!(result == "INT" && exp_type == "BYTE")) {
			output::errorMismatch(yylineno);
			exit(1);
		}
	}
}

void handleAssign(Expression* id, Expression* exp) {
    MainProcess& process = MainProcess::get_instance();
	SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
	if (entry == nullptr) {
		output::errorUndef(yylineno, id->name);
		exit(1);
	}
	if (entry->is_function) {
		output::errorUndef(yylineno, id->name);
		exit(1);
	}
	string exp_type = getExpType(exp);
	if ((entry->type != exp_type) && (entry->type != "INT" || exp_type != "BYTE")) {
			output::errorMismatch(yylineno);
			exit(1);
    }
}

void handleDeclaration(Expression* type, Expression* id) {
    MainProcess& process = MainProcess::get_instance();
	SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
	if (entry == nullptr) {
		return;
	}
    output::errorDef(yylineno, id->name);
    exit(1);
}

void handleDeclarationAndInitiation(Expression* type, Expression* id, Expression* exp) {
    MainProcess& process = MainProcess::get_instance();
	SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
	if (entry != nullptr) {
		output::errorDef(yylineno, id->name);
		exit(1);
	}
	string exp_type = getExpType(exp);
	if ((type->type != exp_type) && (type->type != "INT" || exp_type != "BYTE")) {
			output::errorMismatch(yylineno);
			exit(1);
	}
}

void checkIfMainExists() {
    MainProcess& process = MainProcess::get_instance();
	SymbolTableEntry* main_entry = process.getEntryInSymbolTable("main");
	if (main_entry == nullptr) {
		output::errorMainMissing();
		exit(1);
	}
	if (main_entry->type != "VOID") {
		output::errorMainMissing();
		exit(1);
	}
	SymbolTableEntryFunction* main_fun_entry = dynamic_cast<class SymbolTableEntryFunction*>(main_entry);
	if (!(main_fun_entry->arguments_names.empty())) {
		output::errorMainMissing();
		exit(1);
	}
}

void addFunctionEntryToSymbolTable(Expression* ret_type, Expression* id, Expression* args) {
    MainProcess& process = MainProcess::get_instance();
	SymbolTableEntryFunction* function_entry;
	if (args != nullptr) {
		ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
		reverse(args_list->arguments_names.begin(), args_list->arguments_names.end());
		reverse(args_list->arguments_types.begin(), args_list->arguments_types.end());

		function_entry = new SymbolTableEntryFunction(id->name, ret_type->type, args_list->arguments_names, args_list->arguments_types);
	}
	else {
		function_entry = new SymbolTableEntryFunction(id->name, ret_type->type);
	}
    process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(function_entry);
}

void addArgumentsToSymbolTable(Expression* args) {
	if (args != nullptr) {
        MainProcess& process = MainProcess::get_instance();
		ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
		int offset = -1;
		int num_of_args = args_list->arguments_names.size();
		for (int i = 0; i < num_of_args; i++) {
			SymbolTableEntry* current_entry = new SymbolTableEntry(args_list->arguments_names[i], args_list->arguments_types[i], offset, false);
            process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(current_entry);
			offset--;
		}
	}
}

void addVariableToSymbolTable(Expression* type, Expression* id) {
    MainProcess& process = MainProcess::get_instance();
	int offset = process.offset_stack.top();
	SymbolTableEntry* var_entry = new SymbolTableEntry(id->name, type->type, offset, false);
	int new_top = process.offset_stack.top() + 1;
    process.offset_stack.pop();
    process.offset_stack.push(new_top);
    process.symbol_table[process.symbol_table.size() - 1].scope_symbol_table.push_back(var_entry);
}

bool isWhile() {
    MainProcess& process = MainProcess::get_instance();
	return process.symbol_table.back().is_while_scope;
}