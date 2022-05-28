#include "Expression.h"
#include "System.h"
#include <iostream>
using namespace std;

string find_type(Expression* exp) {
	if (!(exp->type.empty())) {
		return exp->type;
	}
	System& system = System::get_instance();
	return system.findEntryInSymbolTable(exp->name)->type;
}

void checkTypeBool(Expression* exp) {
	string type = find_type(exp);
	if (type != "BOOL") {
		output::errorMismatch(yylineno);
		exit(1);
	}
}

void checkTypeBool(Expression* exp1, Expression* exp2) {
	if (find_type(exp1) != "BOOL" || find_type(exp2) != "BOOL") {
		output::errorMismatch(yylineno);
		exit(1);
	}
}

void checkByteSize(int size) {
	if (size > 255) {
		output::errorByteTooLarge(yylineno, std::to_string(size));
		exit(1);
	}
}

Expression* handleLogical(Expression* exp1, Expression* exp2) {
	if (exp2 != nullptr) {
		checkTypeBool(exp1, exp2);
	}
	else {
		checkTypeBool(exp1);
	}
	return new Expression("", "BOOL");
}

Expression* handleBinop(Expression* exp1, Expression* exp2, std::string op) {
	std::string type_exp1 = find_type(exp1);
	std::string type_exp2 = find_type(exp2);
	if (type_exp1 == "INT" && (type_exp2 == "INT" || type_exp2 == "BYTE")) {
		return new Expression("", "INT");
	}
	else if (type_exp1 == "BYTE" && type_exp2 == "INT") {
		return new Expression("", "INT");
	}
	else if (type_exp1 == "BYTE" && type_exp2 == "BYTE") {
		return new Expression("", "BYTE");
	}
	else {
		output::errorMismatch(yylineno);
		exit(1);
	}
	return nullptr;
}

Expression* handleRelop(Expression* exp1, Expression* exp2) {
	std::string type_exp1 = find_type(exp1);
	std::string type_exp2 = find_type(exp2);
	if ((type_exp1 == "INT" || type_exp1 == "BYTE") && (type_exp2 == "INT" || type_exp2 == "BYTE")) {
		return new Expression("", "BOOL");
	}
	else {
		output::errorMismatch(yylineno);
		exit(1);
	}
	
	//ASAF
	std::cout << "here" << std::endl;

	return nullptr;
}

Expression* handleByte(Expression* exp) {
	if (stoi(exp->name) > 255) {
		output::errorByteTooLarge(yylineno, exp->name);
		exit(1);
	}
	return new Expression(exp->name, "BYTE");
}

Expression* handleCast(string cast_type, Expression* exp) {
	if (cast_type == "BYTE") {
		checkByteSize(stoi(exp->name));
	}
	return new Expression(exp->name, cast_type);
}

void handleCall(Expression* id, Expression* args) {
	System& system = System::get_instance();
	SymbolTableEntry* entry = system.findEntryInSymbolTable(id->name);
	if (entry == nullptr) {
		output::errorUndefFunc(yylineno, id->name);
		exit(1);
	}
	else if (!entry->is_func) {
		output::errorUndefFunc(yylineno, id->name);
		exit(1);
	}
	SymbolTableEntryFunction* function_entry = dynamic_cast<class SymbolTableEntryFunction*>(entry);
	int number_of_args = function_entry->args_type.size();
	if (args != nullptr) {
		ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
		if (args_list->args_name.size() != number_of_args) {
			output::errorPrototypeMismatch(yylineno, function_entry->name, function_entry->args_type);
			exit(1);
		}

		reverse(args_list->args_name.begin(), args_list->args_name.end());
		reverse(args_list->args_type.begin(), args_list->args_type.end());
		reverse(args_list->args_is_const.begin(), args_list->args_is_const.end());

		for (int i = 0; i < number_of_args; i++) {
			if (function_entry->args_type[i] == args_list->args_type[i] ||
				(function_entry->args_type[i] == "INT" && args_list->args_type[i] == "BYTE")) {
				continue;
			}
			else {
				output::errorPrototypeMismatch(yylineno, function_entry->name, function_entry->args_type);
				exit(1);
			}
		}
	}
	if (args == nullptr && number_of_args != 0) {
		output::errorPrototypeMismatch(yylineno, function_entry->name, function_entry->args_type);
		exit(1);
	}
}

void handleID(Expression* id) {
	System& system = System::get_instance();
	SymbolTableEntry* entry = system.findEntryInSymbolTable(id->name);
	if (entry == nullptr) {
		output::errorUndef(yylineno, id->name);
		exit(1);
	}
	// trying to use ID as function
	if (entry->is_func) {
		output::errorUndef(yylineno, id->name);
		exit(1);
	}
}

void checkFuncInSymbolTable(Expression* id) {
	System& system = System::get_instance();
	SymbolTableEntry* entry = system.findEntryInSymbolTable(id->name);
	if (entry != nullptr) {
		output::errorDef(yylineno, id->name);
		exit(1);
	}
}

void addArgInCall(Expression* exp, Expression* arg)
{
	ExpressionFunction* function_entry = dynamic_cast<class ExpressionFunction*>(exp);
	function_entry->args_name.push_back(arg->name);
	string type = find_type(arg);
	function_entry->args_type.push_back(type);
	function_entry->args_is_const.push_back(arg->is_const);
}

void addArgInDeclaration(Expression* args_list, Expression* new_arg) {

	ExpressionFunction* function_args_list = dynamic_cast<class ExpressionFunction*>(args_list);

	//if this arg name already exists for this func, error. 
	if (std::find(function_args_list->args_name.begin(), function_args_list->args_name.end(), new_arg->name) != function_args_list->args_name.end()) {
		output::errorDef(yylineno, new_arg->name);
		exit(0);
	}
	function_args_list->args_name.push_back(new_arg->name);
	function_args_list->args_type.push_back(new_arg->type);
	function_args_list->args_is_const.push_back(new_arg->is_const);
}

