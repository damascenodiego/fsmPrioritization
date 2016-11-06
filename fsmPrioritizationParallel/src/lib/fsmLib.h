/*
 * fsmLib.h
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#ifndef LIB_FSMLIB_H_
#define LIB_FSMLIB_H_

#include <cstdio>
#include <iostream>

#include <list>
#include <map>

class FsmState;
class FsmTransition;
class FsmModel;

typedef struct _SimpleFsmTestCase{
	int  testId;
	int  testLength;
	int  pTot;
	int* p;
} SimpleFsmTestCase;

class FsmElement { };

class FsmState : FsmElement{
	int id;
	std::map<int,FsmTransition*> in;
	std::map<int,FsmTransition*> out;
public:
	FsmState(){ id = -1;}
	FsmState(int i) : id(i) {
		//			printf("--- State %d created @ %p\n",id,this);
	}
	~FsmState() {
//		printf("--- State #%d: deleted @ %p\n",id,this);
		in.clear();
		out.clear();
	}
	void setId(int i) {id = i;}
	int & getId()			{return id;}
	std::map<int,FsmTransition*> & getIn()	{return in;}
	std::map<int,FsmTransition*> & getOut()	{return out;}
	void print(){

		printf("\tState %d (@%p)\n",getId(),this);
		printf("\t\tReaches %zu states\n", getOut().size());
		std::map<int,FsmTransition*>::iterator it = getOut().begin();
		//		while(it != getOut().end()){
		//			FsmTransition *tp = it->second;
		//			tp->getTo();
		//			printf("%d\t",(*st).getId());
		//		}
		//		printf("\n");
		printf("\t\tReached by %zu states\n", getIn().size());
		//		for(auto i : getIn()){
		//			printf("%d\t",i.second.getFrom().getId());
		//		}
		//		printf("\n");
	}
};

class FsmTransition : FsmElement{
	int id;
	FsmState *fr;
	int in;
	int out;
	FsmState *to;
public:
	FsmTransition() : id(0), fr(nullptr), in(0), out(0), to(nullptr) { }
	~FsmTransition(){
//		printf("--- Transition #%d: %d -- %d / %d -> %d deleted (@%p)\n"
//				,id
//				,(*getFrom()).getId()
//				,getIn()
//				,getOut()
//				,(*getTo()).getId()
//				,this
//		);
	}
	FsmTransition(int trId,FsmState *f,int i, int o, FsmState *t) : id(trId), fr(f), in(i) ,out(o), to(t) {
		(*f).getOut()[i] = this;
		(*t).getIn()[i] = this;

	}
	int getId(){ return id; }
	FsmState * getFrom() 	{return fr;}
	FsmState * getTo() 	{return to;}
	int getIn() 		{return in;}
	int getOut() 		{return out;}
	void print(){
		printf("\tTransition #%d: %d -- %d / %d -> %d (@%p)\n"
				,id
				,(*getFrom()).getId()
				,getIn()
				,getOut()
				,(*getTo()).getId()
				,this
		);
	}
};


class FsmModel{
	std::list<FsmState*> 		state;
	std::list<FsmTransition*> 	transition;
	FsmState 	*initSt = nullptr;
	std::list<int> fsmIn;
	std::list<int> fsmOut;
public:
	FsmModel(){};
	~FsmModel(){
		for(auto i : transition) delete(i);
		for(auto i : state) delete(i);
		state.clear();
		transition.clear();
		fsmIn.clear();
		fsmOut.clear();
//		printf("--- FsmModel deleted @ %p\n",this);
	};
	void addState(FsmState &item)			{ state.push_back(&item) ; }
	void addTransition(FsmTransition &item)	{ transition.push_back(&item); }
	std::list<FsmState*> & 		getState()	 {return state;}
	std::list<FsmTransition*> & getTransition() {return transition;}
	FsmState* getInitState()		{return initSt;}
	void setInitState(FsmState *s)		{initSt = s;}
	std::list<int> & getIn()	{return fsmIn;}
	std::list<int> & getOut()		{return fsmOut;}
};


class FsmTestSuite;
class FsmTestCase;


class FsmTestCase{
	int id;
	std::list<int> input;
	std::list<FsmTransition*> p;
	SimpleFsmTestCase * simpleFormat = nullptr;
public:
	FsmTestCase(int tid = 0) : id(tid) { }
	~FsmTestCase() {
//		printf("--- FsmTestCase #%d: deleted @ %p\n",id,this);
		input.clear();
		p.clear();
	}
	int getId(){ return id; }
	std::list<int> & getInput(){ return input;}
	std::list<FsmTransition*> & getP() {return p;}
	int getLength(){ return input.size(); }

	SimpleFsmTestCase* getSimpleFormat(){
		if(simpleFormat == nullptr){
			simpleFormat 			= (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));
			simpleFormat->testId	= getId();
			simpleFormat->testLength= input.size();
			simpleFormat->pTot 		= p.size();
			simpleFormat->p 		= (int*) malloc(sizeof(int)*simpleFormat->pTot);
			int count = -1;
			for(auto i : p) {
				simpleFormat->p[++count] = (*i).getId();
			}
		}
		return simpleFormat;
	}

	void print(){
		printf("FsmTestCase #%d (@ %p) length: %d\n",id,this, getLength());
		if(p.empty()){
			for (int l : input) {
				printf("%d\t",l);
			}
			printf("\n");
		}else{
			for (auto i : p) {
				i->print();
			}
			printf("\n");
		}
	}
};

class FsmTestSuite{
	FsmModel* model;
	std::list<FsmTestCase*> testCase;
	int 	length;
	double 	avgLength;
public:
	FsmTestSuite() { model = nullptr; length = avgLength = 0;}
	~FsmTestSuite() {
		for(auto i : getTestCase()) delete i;
		testCase.clear();
// 		printf("--- FsmTestSuite deleted @ %p\n",this);
	}
	std::list<FsmTestCase*> & getTestCase()	{ return testCase; }
	int getLength(){ return length;}
	double getAvgLength(){ return avgLength;}
	void setLength(int i){ length = i;}
	void setAvgLength(double i){ avgLength = i;}
	int getNoResets(){ return testCase.size();}
	void setModel(FsmModel* m) { model = m;}
	FsmModel* getModel() { return model;}
};

FsmModel* loadFsm(FILE* f);
FsmTestSuite* loadTest(FILE* f,FsmModel *m);
void saveTest(FILE* f,FsmTestSuite* ts);
void saveTestCoverage(FILE* f,FsmTestSuite* ts);
void printSimpleFormat(SimpleFsmTestCase * sf);
double calcSimpleSimilarity(FsmTestCase *t0,FsmTestCase *t1);
double calcSimpleSimilarity(SimpleFsmTestCase *t0, SimpleFsmTestCase *t1);
void prioritization_lmdp(FsmTestSuite* ts);
void prioritization_gmdp(FsmTestSuite* ts);
void update_ds_sum(std::list<FsmTestCase*> &ts, FsmTestCase* tc,double* ds_sum);

#endif /* LIB_FSMLIB_H_ */
