/*
 * fsmLib.h
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#ifndef LIB_FSMLIB_H_
#define LIB_FSMLIB_H_

#include <list>
#include <map>

class FsmState;
class FsmTransition;
class FsmModel;

class FsmState{
	int id;
	std::map<int,FsmTransition> in;
	std::map<int,FsmTransition> out;
public:
	FsmState(){ id = -1;}
	FsmState(int i) : id(i) {
			printf("--- State %d created @ %p\n",id,this);
		}
	~FsmState() {
			//printf("--- State %d deleted @ %p\n",id,this);
		}
	void setId(int i) {id = i;}
	int & getId()			{return id;}
	std::map<int,FsmTransition> & getIn()	{return in;}
	std::map<int,FsmTransition> & getOut()	{return out;}
	void print(){

		printf("\tState%d (@%p)\n",getId(),this);
		printf("\t\tReaches %d states: ", getOut().size());
//		for(auto i : getOut()){
//			printf("%d\t",i.second.getTo().getId());
//		}
		printf("\n");
		printf("\t\tReached by %d states: ", getIn().size());
//		for(auto i : getIn()){
//			printf("%d\t",i.second.getFrom().getId());
//		}
		printf("\n");
	}
};

class FsmTransition{
	FsmState *fr;
	int in;
	int out;
	FsmState *to;
public:
	FsmTransition(){
		in = 0;
		out = 0;
	}
	FsmTransition(FsmState *f,int i, int o, FsmState *t) : fr(f), in(i) ,out(o), to(t) {
//		printf("########## DEBUG  - ini ##########\n");
//		f.print();
		(*f).getOut()[i] = *this;
//		f.print();
//		printf("########## DEBUG  - end ##########\n");
		(*t).getIn()[i] = *this;

	}
	FsmState * getFrom() 	{return fr;}
	FsmState * getTo() 	{return to;}
	int getIn() 		{return in;}
	int getOut() 		{return out;}
	void print(){
		printf("\tTransition: %d -- %d / %d -> %d (@%p)\n"
				,(*getFrom()).getId()
				,getIn()
				,getOut()
				,(*getTo()).getId()
				,this
		);
	}
};


class FsmModel{
	std::list<FsmState> 		state;
	std::list<FsmTransition> 	transition;
	FsmState 	*initSt = nullptr;
	std::list<int> fsmIn;
	std::list<int> fsmOut;
public:
	FsmModel(){};
	void addState(FsmState &item)			{ state.push_back(item) ; }
	void addTransition(FsmTransition &item)	{ transition.push_back(item); }
	std::list<FsmState> & 		getState()	 {return state;}
	std::list<FsmTransition> & getTransition() {return transition;}
	FsmState* getInitState()		{return initSt;}
	void setInitState(FsmState *s)		{initSt = s;}
	std::list<int> & getIn()	{return fsmIn;}
	std::list<int> & getOut()		{return fsmOut;}
};

void loadFsm(FILE* f,FsmModel &fsm);

#endif /* LIB_FSMLIB_H_ */
