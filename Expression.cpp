#include "Expression.h"
#include "MainProcess.h"
#include <iostream>
using namespace std;
using namespace output;

void errorMismatch();

void checkIfBoolBin(Expression* exp1, Expression* exp2) {
    if(!(getExpType(exp2) == "BOOL" && getExpType(exp1) == "BOOL"))
        errorMismatch();
}
Expression* logicalExpression(Expression* expression1, Expression* expression2) {
    Expression* exp_bool=new Expression("", "BOOL");
    if (expression2 == nullptr) {
        checkIfBoolUn(expression1);
    }
    else {
        checkIfBoolBin(expression1, expression2);
    }
    return exp_bool;
}
Expression* handleBinop(Expression* expression1, Expression* expression2) {
    string expression_type = getExpType(expression1), expression_type1 = getExpType(expression2);
    if ((expression_type1 == "INT"&&expression_type == "BYTE")||(expression_type == "INT" && (expression_type1 == "BYTE" || expression_type1 == "INT"))) {
        return new Expression("", "INT");
    }
    else if (expression_type1 == "BYTE" && expression_type == "BYTE") {
        return new Expression("", "BYTE");
    }
    else {
        errorMismatch();
    }
    return nullptr;
}

Expression* handleCast(string type, Expression* expression) {
    string expression_type = getExpType(expression);
    if ((type == "INT" && (expression_type == "BYTE" || expression_type == "INT"))||
        (type == "BYTE" && (expression_type == "BYTE" || expression_type == "INT")))
    {
        return new Expression(expression->name, type);
    }
    else
    {
        errorMismatch();
    }
}
Expression* handleRelop(Expression* expression1, Expression* expression2) {
    string expression_type = getExpType(expression1);
    string expression_type1 = getExpType(expression2);
    if ((expression_type == "BYTE" || expression_type == "INT") && (expression_type1 == "BYTE"||expression_type1 == "INT" )) {
        return new Expression("", "BOOL");
    }
    else {
        errorMismatch();
    }
    return nullptr;
}
Expression* handleByte(Expression* expression) {
    if (stoi(expression->name) < 256) {
        return new Expression(expression->name, "BYTE");
    }
    errorByteTooLarge(yylineno, expression->name);
    exit(1);
}
string getExpType(Expression* exp) {
    if (exp->type.size()!=0) {
        return exp->type;
    }
    return MainProcess::get_instance().getEntryInSymbolTable(exp->name)->type;
}


void checkIfBoolUn(Expression* exp) {
    string type = getExpType(exp);
    if (type == "BOOL") {
        return;
    }
    errorMismatch();
}
void checkIfFuncAlreadyInSymbolTable(Expression* id) {
    SymbolTableEntry* current_entry = MainProcess::get_instance().getEntryInSymbolTable(id->name);
    if (current_entry == nullptr) {
        return;
    }
    errorDef(yylineno, id->name);
    exit(1);
}
void checkByteSize(int size) {
    if (size < 256) {
        return;
    }
    errorByteTooLarge(yylineno, to_string(size));
    exit(1);
}
void addArgInDeclaration(Expression* args_list, Expression* new_arg) {
    ExpressionFunction* function_args_list = dynamic_cast<class ExpressionFunction*>(args_list);
    if (find(function_args_list->arguments_names.begin(), function_args_list->arguments_names.end(), new_arg->name) != function_args_list->arguments_names.end()) {
        errorDef(yylineno, new_arg->name);
        exit(0);
    }
    function_args_list->arguments_types.push_back(new_arg->type);
    function_args_list->arguments_names.push_back(new_arg->name);
}

void addArgToFunction(Expression* expression, Expression* arguments)
{
    ExpressionFunction* entry = dynamic_cast<class ExpressionFunction*>(expression);
    entry->arguments_names.push_back(arguments->name);
    entry->arguments_types.push_back(getExpType(arguments));
}
void checkID(Expression* id) {
    MainProcess& process = MainProcess::get_instance();
    SymbolTableEntry* entry = process.getEntryInSymbolTable(id->name);
    if (entry == nullptr) {
        errorUndef(yylineno, id->name);
        exit(1);
    }
    if (entry != nullptr && entry->is_function) {
        errorUndef(yylineno, id->name);
        exit(1);
    }
}
void handleCall(Expression* id, Expression* args) {
    SymbolTableEntry* entry = MainProcess::get_instance().getEntryInSymbolTable(id->name);
    SymbolTableEntryFunction* entry_func = dynamic_cast<class SymbolTableEntryFunction*>(entry);

    if (entry != nullptr && entry->is_function== false) {
        errorUndefFunc(yylineno, id->name);
        exit(1);
    }
    else if (entry == nullptr) {
        errorUndefFunc(yylineno, id->name);
        exit(1);
    }
    int num_arguments = entry_func->arguments_types.size();
    if (args == nullptr&&!(num_arguments == 0)) {
        errorPrototypeMismatch(yylineno, entry_func->name, entry_func->arguments_types);
        exit(1);
    }
    else if (args != nullptr) {
        ExpressionFunction* args_list = dynamic_cast<class ExpressionFunction*>(args);
        reverse(args_list->arguments_names.begin(), args_list->arguments_names.end());
        reverse(args_list->arguments_types.begin(), args_list->arguments_types.end());
        if (args_list->arguments_names.size() != num_arguments) {
            errorPrototypeMismatch(yylineno, entry_func->name, entry_func->arguments_types);
            exit(1);
        }
        for (int i = 0; i < num_arguments; i++) {
            if (entry_func->arguments_types[i] != args_list->arguments_types[i]&&!(((entry_func->arguments_types[i] == "INT") && (args_list->arguments_types[i] == "BYTE"))))
            {
                errorPrototypeMismatch(yylineno, entry_func->name, entry_func->arguments_types);
                exit(1);
            }
        }
    }
}






