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

int main(int argc, char **argv) {
	FILE *fsmFile;
	FILE *testFile;
	if(argc == 3){
		fsmFile = fopen(argv[1],"r");
		testFile = fopen(argv[2],"r");
	}else{
		return (1);
	}

	FsmModel fsmModel;
	loadFsm(fsmFile,fsmModel);


	printf("FsmModel @ %p\n",&fsmModel);
	printf("States:\t\t%zu\n",fsmModel.getState().size());
	for (FsmState *i : fsmModel.getState()) {
		(*i).print();
	}
	printf("Transitions:\t%zu\n",fsmModel.getTransition().size());
	for (FsmTransition *i : fsmModel.getTransition()) {
		(*i).print();
		(*i).getFrom()->print();
		(*i).getTo()->print();
	}
	printf("In:\t\t%zu\n",fsmModel.getIn().size());
	for (int i : fsmModel.getIn()) {
		printf("\t%d (@%p)\n",i,&i);
	}
	printf("Out:\t\t%zu\n",fsmModel.getOut().size());
	for (int i : fsmModel.getOut()) {
		printf("\t%d (@%p)\n",i,&i);
	}

	return 0;

}



