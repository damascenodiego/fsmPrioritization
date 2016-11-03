/*
 * fsmLib.cpp
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include "fsmLib.h"

FsmModel * loadFsm(FILE* f){
	FsmModel *fsm = new FsmModel();
	rewind(f); //rewind file reader
	printf("FsmModel @ %p\n",(*fsm));
	int to;
	int in;
	int out;
	int from;
	std::map<int,FsmState*> stMap;
	std::set<int> inSet,outSet;
	FsmState *f_st,*t_st;
	FsmTransition *tr;
	int id = 0;
	while(!feof(f)){
		fscanf(f,"%d -- %d / %d -> %d",&from,&in,&out,&to);
		//printf("---\n");
		//printf("FROM: %d\n IN: %d\n OUT: %d\n TO: %d\n",from,in,out,to);
		std::map<int,FsmState*>::iterator it = stMap.find(from);
		if(it == stMap.end()){
			f_st = new FsmState(from);
			stMap[from] =f_st;
			//			printf("address: %p %p\n",stMap[from],f_st);
			(*fsm).addState(*f_st);
		}else{
			f_st = stMap[from];
		}
		it = stMap.find(to);
		if(it == stMap.end()){
			t_st = new FsmState(to);
			stMap[to] =t_st;
			(*fsm).addState(*t_st);
		}else{
			t_st = stMap[to];
		}
		if(inSet.insert(in).second){
			(*fsm).getIn().push_back(in);
		}
		if(outSet.insert(out).second){
			(*fsm).getOut().push_back(out);
		}
		if((*fsm).getInitState() == nullptr){
			(*fsm).setInitState(stMap[from]);
		}
		tr = new FsmTransition (id,f_st,in,out,t_st);
		//		(*tr).print();
		//		(*f_st).print();
		//		(*t_st).print();
		(*fsm).addTransition(*tr);
		id++;
	}
	stMap.clear();
	inSet.clear();
	outSet.clear();
	return fsm;
}

FsmTransition * nextTransition(FsmState* s0, int input){
	for (auto i : s0->getOut()) {
		if((i.second)->getIn() == input){
			return i.second;
		}
	}
	return nullptr;
}

void evaluateCoverage(FsmModel *model, FsmTestCase *tc){
	FsmTransition *tr   = nullptr;
	FsmState      *curr = nullptr;

	curr = model->getInitState();
	std::set<FsmTransition*> trSet;
	for (int i : tc->getInput()) {
		tr = nextTransition(curr,i);
		if(trSet.insert(tr).second){
			tc->getP().push_back(tr);
		}
		curr = tr->getTo();
	}
	trSet.clear();
}

FsmTestSuite* loadTest(FILE* f,FsmModel *m = nullptr){
	char line[1024];
	rewind(f); //rewind file reader

	FsmTestSuite* ts = new FsmTestSuite();
	int slen = 0;
	int id = 0;
	while(!feof(f)){
		fgets(line,1024,f);
		slen = strlen(line)-1;
		if(slen < 3) continue;
		//		line[slen] = '\0';
		//		printf("'%s'\n",line); fflush(stdout);
		int var;
		long l;
		FsmTestCase * tc = new FsmTestCase(++id);
		for (var = 0; var < slen; var+=3) {
			l = 0;
			l += (line[var]   - '0')*100;
			l += (line[var+1] - '0')*10;
			l += (line[var+2] - '0')*1;
			//			printf("%d\t",l);
			(*tc).getInput().push_back(l);
		}
		ts->getTestCase().push_back(tc);
		if(m != nullptr) evaluateCoverage(m,tc);
		ts->setLength(ts->getLength()+tc->getLength());
		line[0] = '\0';
	}
	ts->setAvgLength(((double)ts->getLength())/ts->getNoResets());
	if(m != nullptr) ts->setFsmModel(m);
	return ts;
}

void saveTest(FILE* f,FsmTestSuite* ts){
	rewind(f);

	for(FsmTestCase *tc : ts->getTestCase()){
		for(int i : tc->getInput()){
			fprintf(f,"%03d",i);
		}
		fprintf(f,"\n");
	}
}

void printSimpleFormat(SimpleFsmTestCase * sf){
	if(sf != nullptr){
		printf("Test Case #%d:" \
				"\n\tLength: %d" \
				"\n\tpTot: %d"  \
				"\n\tp:",sf->testId,sf->testLength,sf->pTot);
		for(int i = 0; i <  sf->pTot; i++) {
			printf("\t%d",sf->p[i]);
		}
		printf("\n");
	}
}

double calcSimpleSimilarity(FsmTestCase *t0,FsmTestCase *t1){
	std::set<int> t0Tr;
	std::set<int> t1Tr;

	int t0len = t0->getLength();
	int t1len = t1->getLength();

	for(FsmTransition * t : t0->getP()) t0Tr.insert(t->getId());
	for(FsmTransition * t : t1->getP()) t1Tr.insert(t->getId());

	int ndt = 0;

	for(int t : t0Tr) {
		if(t1Tr.find(t) == t1Tr.end())  ndt++;
	}

	for(int t : t1Tr) {
		if(t0Tr.find(t) == t0Tr.end())  ndt++;
	}

	t0Tr.clear();
	t1Tr.clear();

	//printf("ndt = %d\n",ndt);
	double ds = ndt / ((t0len+t1len)/2.0);
	return ds;

}

double calcSimpleSimilarity(SimpleFsmTestCase *t0, SimpleFsmTestCase *t1){
	std::set<int> t0Tr;
	std::set<int> t1Tr;

	int t0len = (*t0).testLength;
	int t1len = (*t1).testLength;

	for (int var = 0; var < (*t0).pTot; ++var) {
		t0Tr.insert((*t0).p[var]);
	}

	for (int var = 0; var < (*t1).pTot; ++var) {
		t1Tr.insert((*t1).p[var]);
	}

	int ndt = 0;

	for(int t : t0Tr) {
		if(t1Tr.find(t) == t1Tr.end())  ndt++;
	}

	for(int t : t1Tr) {
		if(t0Tr.find(t) == t0Tr.end())  ndt++;
	}

	t0Tr.clear();
	t1Tr.clear();

	//printf("ndt = %d\n",ndt);
	double ds = ndt / ((t0len+t1len)/2.0);
	return ds;

}


void prioritization_lmdp(FsmTestSuite* ts){
	std:: list<FsmTestCase*> tcs;
	std:: list<FsmTestCase*> t;
	for(FsmTestCase *i : ts->getTestCase()){
		t.push_back(i);
	}
	std::list<FsmTestCase*>::iterator endi;
	std::list<FsmTestCase*>::iterator endj;

	double 			tmp_ds;

	double 								max_ds = -1;
	std::list<FsmTestCase*>::iterator 	it;
	std::list<FsmTestCase*>::iterator 	max_ti;
	std::list<FsmTestCase*>::iterator 	max_tj;
	while (t.size()>0){
		if(t.size()>1){
			endi = t.end(); endi--;
			endj = t.end();
			for(auto ti = t.begin(); ti != endi; ti++){
				auto tj = ti;
				for(tj++; tj != endj; tj++){
					tmp_ds = calcSimpleSimilarity(*ti,*tj);
					if(tmp_ds > max_ds){
						max_ds = tmp_ds;
						max_ti = ti;
						max_tj = tj;
					}
				}
			}
			it = max_ti; it--;
			tcs.push_back(*max_ti);
			tcs.push_back(*max_tj);
			t.erase(max_ti);
			t.erase(max_tj);
		}else{
			tcs.push_back(*t.begin());
			t.erase(t.begin());
		}

	}
	ts->getTestCase().clear();
	ts->getTestCase() = tcs;

}

void prioritization_gmdp(FsmTestSuite* ts){

}

