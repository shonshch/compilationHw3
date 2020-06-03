//
// Created by akler on 23-May-18.
//

#include "SymbolTable.hpp"


static void PrintFuncScope(vector<Func*> vec);
static void PrintScopeVec(vector<Var*> vec);

SymbolTable::SymbolTable(){
    newScope(); // make the function scopes

    vector<Func*> vec;
    printStackFuncs.push_back(vec);

    addPrintAndPrinti();
}
//initialize the table for our needs change nmw

void SymbolTable::addPrintAndPrinti(){

    //creating Print and Printi Functions and enter them to table


    Type* print_type = new Type(T_STRING);
    Id* p_name = new Id(string("string"));//ILAN:print?
    Id* pf_name = new Id(string("print"));//ILAN:print?
    Var* p_arg = new Var(print_type, p_name);

    Formals* p_formals = new Formals(p_arg);

    Node* print = new Func(print_type,p_name,p_formals);

    //TODO: need to check if need extra attantion for T_Byte

    Type* printi_type = new Type(T_INT);
    Id* pi_name = new Id(string("int"));//ILAN:printi?
    Id* pfi_name = new Id(string("printi"));//ILAN:printi?
    Var* pi_arg = new Var(printi_type, pi_name);

    Formals* pi_formals = new Formals(pi_arg);

    Node* printi = new Func(printi_type,pi_name,pi_formals);

    //insert the print and printi to the bottom map in the vector

	Type* printi_retType = new Type(T_VOID);
	Type* print_retType = new Type(T_VOID);

	insertNewFunc(print_retType, pf_name,p_formals);
	insertNewFunc(printi_retType, pfi_name,pi_formals);


}
void SymbolTable::newScope(){
    int offset = 0;

    if(!Stack.empty()) {
        offset = Stack.back().second;
    }
    map<string, Node*> scope;
    Stack.push_back(ScopeTable(scope,offset));


    vector<Var*> vec;
    printStackVars.push_back(vec);
}

void SymbolTable::popScope(){
        output::endScope();
        if(Stack.size() > 1){
            PrintScopeVec(printStackVars.back());
            printStackVars.pop_back();
        }else{
            PrintFuncScope(printStackFuncs.back());
            printStackFuncs.pop_back();
        }
        Stack.pop_back();

}

void SymbolTable::newFuncScope(Node* func){
    Func* function = static_cast<Func*>(func);
    newScope();

    int offset = 0;

    //TODO:: check if the insert of the func args is OK (from left(-1) to right(-100))
    for(vector<Var*>::iterator it = function->args.begin(); it!= function->args.end(); ++it){
       
		(*it)->offset = offset - (*it)->type->size;
		//cout << "ilan name:"<<(*it)->name<<endl;
		
        Stack.back().first[(*it)->name] = *it; //insert the var into the scope map of vars FROM NODE*

		printStackVars.back().push_back(*it);

		offset -= (*it)->type->size; //the next var wil enter after to make it more readable
    }


}
void SymbolTable::insertNewFunc(Node* n_type,Node* n_id, Node* n_formals){


    //asserts we already initialize the Table so we have a ScopeTable at Table[0]
    Func* function = new Func(n_type, n_id ,n_formals);

    Stack[0].first[function->name] = function;
    if(function->name!= "print" && function->name != "printi"){
		newFuncScope(function);
	}

    printStackFuncs[0].push_back(function);

}



void SymbolTable::insertNewVar(Node* n_type,Node* n_id,Node* n_num){
    Var* new_var = new Var(n_type, n_id, n_num);

    new_var->offset = Stack.back().second;
    Stack.back().second+=new_var->type->size;
    Stack.back().first[new_var->name] =  new_var;


    printStackVars.back().push_back(new_var);

}
bool SymbolTable::isSymbolExist(Node* n_id) {
	// Id* id = dynamic_cast<Id*>(n_id);

	if (getFunction(n_id) || getVar(n_id)) {
		return true;
	}

	return false;
}
bool SymbolTable::isFuncExist(Node* n_id){
   // Id* id = dynamic_cast<Id*>(n_id);

    if(getFunction(n_id)){
        return true;
    }

    return false;
}
bool SymbolTable::isVarExist(Node* n_id){

   // Id* id = dynamic_cast<Id*>(n_id);

    if(getVar(n_id)){
        return true;
    }
    return false;
}

bool SymbolTable::isArray(Node * n_id)
{
	Var* var = getVar(n_id);

	return var->isArray();
}



Func* SymbolTable::getFunction(Node* n_id){
    string name = dynamic_cast<Id*>(n_id)->name;

    map<string, struct Node*> functions_map = Stack[0].first;
    map<string, struct Node*>::iterator function = functions_map.find(name);

    if (function != functions_map.end()){
        return dynamic_cast<Func*>((*function).second);
    }
    return NULL;
}


Var* SymbolTable::getVar(Node* n_id){

    Id* id = dynamic_cast<Id*>(n_id);
    string name = id->name;

    for (int i = 1; i < Stack.size();i++) {
        map<string, struct Node* > var_map = Stack[i].first;
        map<string, struct Node*>::iterator var = var_map.find(name);

        //map.find returns itr to the element, if not found returns map.end
        if (var != var_map.end()){
            return dynamic_cast<Var*>((*var).second);
        }
    }

    return NULL;
}


Type* SymbolTable::getVarType(Node* n_id){
    //assert var in table
    Var* var = dynamic_cast<Var*>(getVar(n_id));
    return var->type;

}
vector<Var*> SymbolTable::getArgs(Node* n_id){

   // Id* id = dynamic_cast<Id*>(n_id);
   // string name = id->name;

    Func* func = static_cast<Func*>(getFunction(n_id));
    if(func != NULL){
        return func->args;
    }
    vector<Var*> empty_vec;
    return empty_vec;
}

static void PrintScopeVec(vector<Var*> vec){

    for(vector<Var*>::iterator it = vec.begin(); it != vec.end(); ++it){
        string name = (*it)->name;
        int offset =(*it)->offset;
		Type* type = ((*it)->type);
		string str_type;

        if((*it)->isArray()){//ILAN CHANGED HERE BECAUSE WE NEED INT TYPE AND NOT ARR_TYPE
            int size = (*it)->type->size;
			T_Type t_type = type->switchArrayToRegularType();
			type = new Type(t_type);
            str_type = output::makeArrayType( type->typeToString(),size);
		}
		else {
			str_type=type->typeToString();
		}

        output::printID(name,offset,str_type);
    }

}
static void PrintFuncScope(vector<Func*> vec){

    for(vector<Func*>::iterator it = vec.begin(); it!= vec.end(); ++it){
        string name = (*it)->name;
        string retType = (*it)->retType->typeToString();
		vector<string> args = (*it)->getStringArgs();
        string type = output::makeFunctionType(retType, args);
        output::printID(name,0,type);
    }

}