#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>
#include "h3_output.hpp"
#include <iostream>
#include <assert.h>
#include <vector>
#include <string>
#include <map>
using namespace std;
enum T_Type {
	T_VOID,
	T_INT,
	T_BOOL,
	T_BYTE,
	T_STRING,
	T_ARR_INT,
	T_ARR_BOOL,
	T_ARR_BYTE
};

class Node{
	public:
	virtual ~Node(){};
};
static bool isAssignableTypeExp(Node* n_type, Node* n_exp);


class Type : public Node{
	public:
	T_Type t_type;
	int size;
	Type(T_Type type,int size=1):t_type(type),size(size){}
	
	
	
	T_Type switchRegularToArrayType(){
		T_Type arr_type;
		switch(this->t_type){
			case T_INT :
				arr_type=T_ARR_INT;
				break;
			case T_BOOL :
				arr_type=T_ARR_BOOL;
				break;
			case T_BYTE :
				arr_type=T_ARR_BYTE;
				break;
			default:
				assert(0);
		}
		return arr_type;
	}
	
	
	T_Type switchArrayToRegularType(){
		T_Type regular_type;
		switch(this->t_type){
			case T_ARR_INT :
				regular_type=T_INT;
				break;
			case T_ARR_BOOL   :
				regular_type=T_BOOL;
				break;
			case T_ARR_BYTE  :
				regular_type=T_BYTE;
				break;
			default:
				assert(0);
		}
		return regular_type;
	}
	
	
	string typeToString(){
		string str;
		switch(t_type){
			case T_INT:
				str=string("INT");
				break;
			case T_BOOL:
				str= string("BOOL");
				break;
			case T_BYTE:
				str= string("BYTE");
				break;
			case T_VOID:
				str= string("VOID");
				break;
			case T_STRING:
				str= string("STRING");
				break;
			case T_ARR_BOOL:
				str= string("ARR_BOOL");
				break;
			case T_ARR_INT:
				str= string("ARR_INT");
				break;
			case T_ARR_BYTE:
				str= string("ARR_BYTE");
				break;
		}
		return str;
	}
};

class Id: public Node{
	public:
	string name;
	Id(char* name):name(name){}
	Id(string name):name(name){}
};



class Num : public Node { 
	public:
	int value;
	string str_value;
	Num(char* value):value(atoi(value)),str_value(value){}
};

class Var : public Node { 
	public:
	Type* type;
	string name;
	int offset;
	
	Var(Node* n_type,Node* n_id,Node* n_num = NULL){
		name=dynamic_cast<Id*>(n_id)->name;
		type =dynamic_cast<Type*>(n_type);
		
		Num* num=dynamic_cast<Num*>(n_num);
		if(num){
			type->size=num->value;
			type->t_type=type->switchRegularToArrayType();
		}
	}
	bool isArray(){
		return type->t_type == T_ARR_INT || type->t_type == T_ARR_BOOL ||type->t_type == T_ARR_BYTE ;
	}
	

};



class Formals : public Node{
	public:
	vector<Var*> args;
	
	Formals() {}
	Formals(Node* n_var){
		args.push_back(dynamic_cast<Var*>(n_var));
	}


	void addFormal(Node* n_var){
		args.insert(args.begin(),dynamic_cast<Var*>(n_var));
	}	
};

class Exp : public Node{
public:
	Type* type;
	Exp(T_Type type,int size=1){
		this->type = new Type(type,size);
	}
	Exp(Node* n_type){
		type=dynamic_cast<Type*>(n_type);
	}

};



class ExpList : public Node{
public:
	vector<Exp*> list;

	ExpList(Node* n_exp){
		list.push_back(dynamic_cast<Exp*>(n_exp));
	}


	void addExp(Node* n_exp){
		list.insert(list.begin(),dynamic_cast<Exp*>(n_exp));
	}
};

class Call : public Node{
public:
	Type* retType;
	Call(Node* n_type){
		retType=dynamic_cast<Type*>(n_type);

	}

};

class Func : public Node { 
	public:
	Type* retType;
	string name;
	vector<Var*> args;
	int offset;
	Func(Node* n_type,Node* n_id,Node* n_formals){
		retType=dynamic_cast<Type*>(n_type);
		name=dynamic_cast<Id*>(n_id)->name;
		args=dynamic_cast<Formals*>(n_formals)->args;
	}
	
	bool checkArgs(Node* n_explist ){
		ExpList* exp_list=dynamic_cast<ExpList*>(n_explist);
		vector<Var*>::iterator it_expected=args.begin();
		vector<Exp*>::iterator it_actual=exp_list->list.begin();
		if(args.size() != exp_list->list.size()){
			return false;
		}
		for(;it_expected != args.end() ;++it_expected,++it_actual){
			Type* expected=dynamic_cast<Type*>((*it_expected)->type);
			if(!isAssignableTypeExp(expected,*it_actual)){
				return false;
			}
		}
		return true;
		
	}
	vector<string> getStringArgs(){
		vector<string> res;
		for(vector<Var*>::iterator it=args.begin() ;it!= args.end();++it ){
			string str;
			if((*it)->isArray()){
				T_Type t_type = (*it)->type->switchArrayToRegularType();
				Type* type = new Type(t_type);
				str=output::makeArrayType(type->typeToString(),(*it)->type->size);
			}else{
				str=(*it)->type->typeToString();
			}
			res.push_back(str);

		

		}
		return res;
	}
	
	

};










/**********************************/

static string getName(Node* n_id){
	return (dynamic_cast<Id*>(n_id)->name);
}
static string  getValue(Node* n_num){
	return (dynamic_cast<Num*>(n_num)->str_value);
}

static bool isBool(Node* n_exp){
	return (dynamic_cast<Exp*>(n_exp)->type->t_type == T_BOOL);
}


static bool isNumerical(Node* n_exp){
	Type* type=dynamic_cast<Exp*>(n_exp)->type;
	return ( type->t_type == T_INT || type->t_type == T_BYTE );
}

static bool isAssignableTypeExp(Node* n_type,Node* n_exp)
{
	Type* t_expected=dynamic_cast<Type*>(n_type);
	Type* t_actual=dynamic_cast<Exp*>(n_exp)->type;
	if(t_expected->t_type == t_actual->t_type && t_expected->size == t_actual->size){
		return true;
	}
	if(t_expected->t_type == T_INT &&  t_actual->t_type== T_BYTE){
		return true;
	}
	return false;
}



static Type* getBiggerType(Node* n_exp1,Node* n_exp2) {

	//assert we get only T_INT T_BYTE numericals!!
	Type* type1=dynamic_cast<Exp*>(n_exp1)->type;
	Type* type2=dynamic_cast<Exp*>(n_exp2)->type;
	return  type1->t_type == T_INT ? type1 : type2;

}

static bool isLegalArraySize(Node* n_num){
	int val=dynamic_cast<Num*>(n_num)->value;
	return val >0 && val< 256;

}
static bool isLegalByteSize(Node* n_num){
	int val=dynamic_cast<Num*>(n_num)->value;
	return val < 256;

}


#define YYSTYPE Node*

/*
static vector<string> getStringArgs(vector<Var*> args){
	vector<string> s_args;
	for(int i = 0; i<args.size(); i++){
		s_args.push_back(args[i]->type->typeToString());
	}
	return s_args;
}
*/

#endif
