/*
 * fsmPrioritization.cpp
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#include <cstdio>
#include <iostream>
#include "lib/fsmLib.h"

using namespace std;

void printModel(FsmModel *fsmModel);
void printTest(FsmTestSuite *fsmTest);

int main(int argc, char **argv) {
	FILE *fsmFile;
	FILE *testFile;
	if(argc == 3){
		fsmFile = fopen(argv[1],"r");
		testFile = fopen(argv[2],"r");
	}else{
		return (1);
	}

	FsmModel *fsmModel = loadFsm(fsmFile);

	FsmTestSuite *fsmTest = loadTest(testFile,fsmModel);

//	printModel(fsmModel); printTest(fsmTest);


	auto it = (*fsmTest).getTestCase().begin();
//	it--;
//	(*(it))->print();
//	printSimpleFormat((*(it))->getSimpleFormat());

	FsmTestCase *t0 = *it;
	it++;
	FsmTestCase *t1 = *(++it);

	t0->print();
	t1->print();
	printf("ds_(t%d,t%d)=%f\n",t0->getId(),t1->getId(),calcSimpleSimilarity(t0,t1));
	printf("ds_(t%d,t%d)=%f\n",t0->getId(),t1->getId(),calcSimpleSimilarity(t0->getSimpleFormat(),t1->getSimpleFormat()));
	fflush(stdout);

	delete(fsmModel);
	delete(fsmTest);

	return 0;

}


void printModel(FsmModel *fsmModel){
	printf("FsmModel @ %p\n",fsmModel);
	printf("States:\t\t%zu\n",(*fsmModel).getState().size());
	for (FsmState *i : (*fsmModel).getState()) {
		(*i).print();
	}
	printf("Transitions:\t%zu\n",(*fsmModel).getTransition().size());
	for (FsmTransition *i : (*fsmModel).getTransition()) {
		(*i).print();
		(*i).getFrom()->print();
		(*i).getTo()->print();
	}
	printf("In:\t\t%zu\n",(*fsmModel).getIn().size());
	for (int i : (*fsmModel).getIn()) {
		printf("\t%d (@%p)\n",i,&i);
	}
	printf("Out:\t\t%zu\n",(*fsmModel).getOut().size());
	for (int i : (*fsmModel).getOut()) {
		printf("\t%d (@%p)\n",i,&i);
	}
}

void printTest(FsmTestSuite *fsmTest){
	printf("Test suite: length: %d | noResets: %d |avg length %f (@%zu)\n",(*fsmTest).getLength(),(*fsmTest).getNoResets(),(*fsmTest).getAvgLength());
	for (auto i : (*fsmTest).getTestCase()) {
		(*i).print();
	}
}
