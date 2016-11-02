/*
 * fsmLib.cpp
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#include <cstdio>
#include <map>
#include <set>
#include "fsmLib.h"

void loadFsm(FILE* f,FsmModel &fsm){
	rewind(f); //rewind file reader
	printf("FsmModel @ %p\n",&fsm);
	int to;
	int in;
	int out;
	int from;
	std::map<int,FsmState> stMap;
	std::set<int> inSet,outSet;
	FsmState *f_st,*t_st;
	FsmTransition *tr;
	while(!feof(f)){
		fscanf(f,"%d -- %d / %d -> %d",&from,&in,&out,&to);
		//printf("---\n");
		//printf("FROM: %d\n IN: %d\n OUT: %d\n TO: %d\n",from,in,out,to);
		std::map<int,FsmState>::iterator it = stMap.find(from);
		if(it == stMap.end()){
			f_st = new FsmState(from);
			stMap[from] =*f_st;
			fsm.addState(*f_st);
		}else{
			f_st = &stMap[from];
		}
		it = stMap.find(to);
		if(it == stMap.end()){
			t_st = new FsmState(to);
			stMap[to] =*t_st;
			fsm.addState(*t_st);
		}else{
			t_st = &stMap[to];
		}
		if(inSet.insert(in).second){
			fsm.getIn().push_back(in);
		}
		if(outSet.insert(out).second){
			fsm.getOut().push_back(out);
		}
		if(fsm.getInitState() == nullptr){
			fsm.setInitState(&stMap[from]);
		}
		tr = new FsmTransition (f_st,in,out,f_st);
		printf("\t%d -- %d / %d -> %d (@%p) \n"
				,(*(*tr).getFrom()).getId()
				,(*tr).getIn()
				,(*tr).getOut()
				,(*(*tr).getTo()).getId()
				,tr
		);
		(*tr).print();
		(*f_st).print();
		(*t_st).print();
		fsm.addTransition(*tr);
	}
	stMap.clear();
	inSet.clear();
	outSet.clear();
}

//FsmTestSuite* loadTest(FILE* f){
//	char line[1024];
//	rewind(f); //rewind file reader
//
//	FsmTestSuite* ts = createTestSuite();
//	while(!feof(f)){
//		fgets(line,1024,f);
//		//printf("\n'%s'",line);
//		FsmTestCase *test = addTestCase(ts,line);
//		ts->noResets++;
//		ts->testCase = realloc(ts->testCase,(ts->noResets+1)*sizeof(FsmTestCase*));
//		ts->testCase[ts->noResets-1] = test;
//	}
//	ts->noResets --;
//
//	return ts;
//}

