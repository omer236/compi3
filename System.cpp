#include "System.h"
#include <iostream>

using namespace std;

SymbolTableEntry* System::findEntryInSymbolTable(string entry_name) {
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

void openGlobalScope() {
	System& system = System::get_instance();
	Scope global_scope;
	vector<string> print_args_name;
	print_args_name.push_back("print_arg_string");
	vector<string> print_args_type;
	print_args_type.push_back("STRING");
	global_scope.addFuncToScope("print", "VOID", print_args_name, print_args_type);

	vector<string> printi_args_name;
	print_args_name.push_back("printi_arg_string");
	vector<string> printi_args_type;
	printi_args_type.push_back("INT");
	global_scope.addFuncToScope("printi", "VOID", printi_args_name, printi_args_type);

	system.offset_stack.push(0);
	system.symbol_table.push_back(global_scope);
}

void openScope(bool is_while) {
	System& system = System::get_instance();
	Scope scope(is_while);
	system.symbol_table.push_back(scope);
	system.offset_stack.push(system.offset_stack.top());
}

void closeScope() {
	output::endScope();
	System& system = System::get_instance();
	vector<SymbolTableEntry*> scope_symbol_table = system.symbol_table[system.symbol_table.size() - 1].scope_symbol_table;
	for (auto const& sybmol_table_entry : scope_symbol_table) {
		if (sybmol_table_entry->is_func) {
			SymbolTableEntryFunction* function = dynamic_cast<class SymbolTableEntryFunction*>(sybmol_table_entry);
			output::printID(function->name, 0, output::makeFunctionType(function->type, function->args_type));
		}
		else {
			output::printID(sybmol_table_entry->name, sybmol_table_entry->offset, sybmol_table_entry->type);
		}
	}
	system.offset_stack.pop();
	system.symbol_table.pop_back();
}

void handleBreakOrContinue(string flag) {
	System& system = System::get_instance();
	Scope current_scope = system.symbol_table[system.symbol_table.size() - 1];
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

void handleReturn() {
	System& system = System::get_instance();
	vector <SymbolTableEntry*> fun_def_symbol_table = system.symbol_table[system.symbol_table.size() - 2].scope_symbol_table;
	SymbolTableEntryFunction* function_entry = dynamic_cast<class SymbolTableEntryFunction*>(fun_def_symbol_table[fun_def_symbol_table.size() - 1]);
	if (function_entry->type == "VOID") {
	    return;
	}
    output::errorMismatch(yylineno);
    exit(1);
}

void handleExpReturn(Expression* exp) {
    System& system = System::get_instance();
    string result = "temp";
    int index_scope = system.symbol_table.size() - 2;
    for (int j = index_scope; j >= 0; j--)
    {
        int index_entry = system.symbol_table[j].scope_symbol_table.size() - 1;
        for (int i = index_entry; i >= 0; i--)
        {
            if (system.symbol_table[j].scope_symbol_table[i]->is_func)
            {
                result = system.symbol_table[j].scope_symbol_table[i]->type;
                break;
            }
        }
    }
    if (result == "VOID") {
		output::errorMismatch(yylineno);
		exit(1);
	}
	string exp_type = find_type(exp);
	if (result != exp_type) {
		if (!(result == "INT" && exp_type == "BYTE")) {
			output::errorMismatch(yylineno);
			exit(1);
		}
	}
}

void handleAssign(Expression* id, Expression* exp) {
	System& system = System::get_instance();
	SymbolTableEntry* entry = system.findEntryInSymbolTable(id->name);
	if (entry == nullptr) {
		output::errorUndef(yylineno, id->name);
		exit(1);
	}
	if (entry->is_func) {
		output::errorUndef(yylineno, id->name);
		exit(1);
	}
	string exp_type = find_type(exp);
	if ((entry->type != exp_type) && (entry->type != "INT" || exp_type != "BYTE")) {
			output::errorMismatch(yylineno);
			exit(1);
    }
}

void handleDec(Expression* type, Expression* id) {
	System& system = System::get_instance();
	SymbolTableEntry* entry = system.findEntryInSymbolTable(id->name);
	if (entry == nullptr) {
		return;
	}
    output::errorDef(yylineno, id->name);
    exit(1);
}

void handleDecAndInit(Expression* type, Expression* id, Expression* exp) {
	System& system = System::get_instance();
	SymbolTableEntry* entry = system.findEntryInSymbolTable(id->name);
	if (entry != nullptr) {
		output::errorDef(yylineno, id->name);
		exit(1);
	}
	string exp_type = find_type(exp);
	if ((type->type != exp_type) && (type->type != "INT" || exp_type != "BYTE")) {
			output::errorMismatch(yylineno);
			exit(1);
	}
}

void checkIfMain() {
	System& system = System::get_instance();
	SymbolTableEntry* main_entry = system.findEntryInSymbolTable("main");
	if (main_entry == nullptr) {
		output::errorMainMissing();
		exit(1);
	}
	if (main_entry->type != "VOID") {
		output::errorMainMissing();
		exit(1);
	}
	SymbolTableEntryFunction* main_fun_entry = dynamic_cast<class SymbolTableEntryFunction*>(main_entry);
	if (!(main_fun_entry->args_name.empty())) {
		output::errorMainMissing();
		exit(1);
	}
}

void insertFuncToSymbolTable(Expression* ret_type, Expression* id, Expression* args) {
	System& system = System::get_instance();
	SymbolTableEntryFunction* function_entry;
	if (args != nullptr) {
		ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
		reverse(args_list->args_name.begin(), args_list->args_name.end());
		reverse(args_list->args_type.begin(), args_list->args_type.end());

		function_entry = new SymbolTableEntryFunction(id->name, ret_type->type, args_list->args_name, args_list->args_type);
	}
	else {
		function_entry = new SymbolTableEntryFunction(id->name, ret_type->type);
	}
	system.symbol_table[system.symbol_table.size() - 1].scope_symbol_table.push_back(function_entry);
}

void insertArgsToSymbolTable(Expression* args) {
	if (args != nullptr) {
		System& system = System::get_instance();
		ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
		int offset = -1;
		int num_of_args = args_list->args_name.size();
		for (int i = 0; i < num_of_args; i++) {
			SymbolTableEntry* current_entry = new SymbolTableEntry(args_list->args_name[i], args_list->args_type[i], offset, false);
			system.symbol_table[system.symbol_table.size() - 1].scope_symbol_table.push_back(current_entry);
			offset--;
		}
	}
}

void insertVarToSymbolTable(Expression* type, Expression* id) {
	System& system = System::get_instance();
	int offset = system.offset_stack.top();
	SymbolTableEntry* var_entry = new SymbolTableEntry(id->name, type->type, offset, false);
	int new_top = system.offset_stack.top() + 1;
	system.offset_stack.pop();
	system.offset_stack.push(new_top);
	system.symbol_table[system.symbol_table.size() - 1].scope_symbol_table.push_back(var_entry);
}

bool checkIfInWhile() {
	System& system = System::get_instance();
	return system.symbol_table.back().is_while_scope;
}