#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

//#include <stdlib.h>
#include <map>
#include "parser.hpp"


typedef pair<map<string, Node*>,int> ScopeTable;
typedef vector<ScopeTable> SymbolStack;
typedef vector<vector<Var*> > PrintStackVars;
typedef vector<vector<Func*> > PrintStackFuncs;

class SymbolTable{
public:
    SymbolStack Stack;
    PrintStackVars printStackVars;
    PrintStackFuncs printStackFuncs;

    SymbolTable();
    ~SymbolTable(){};
    void addPrintAndPrinti();

    void newScope();
    void popScope();
    void newFuncScope(Node* func);

    void insertNewFunc(Node* n_type,Node* n_id, Node* n_formals);
	void insertPrints(Type* n_type, Id* n_id, Formals* n_formals);

    void insertNewVar(Node* n_type,Node* n_id,Node* n_num = NULL); // need to change/add to n_type, n_id

    Func* getFunction(Node* n_id);
    Var* getVar(Node* n_id);
    Type* getVarType(Node* n_id);



    vector<Var*> getArgs(Node* n_id);

    bool isFuncExist(Node* n_id);
    bool isVarExist(Node* n_id);
	bool isSymbolExist(Node* n_id);
    bool isArray(Node* n_id);




};
#endif