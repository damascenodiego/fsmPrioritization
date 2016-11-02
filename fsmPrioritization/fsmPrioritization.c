/*
 * fsmPrioritization.c
 *
 *  Created on: 21 de out de 2016
 *      Author: damasceno
 */


#include <stdio.h>
#include <stdlib.h>
#include "lib/fsmLib.h"
#include "lib/rbTree.h"
#include "lib/treeSet.h"


int main(int argc, char **argv) {

	FILE *fsmFile;
	FILE *testFile;
	if(argc == 3){
		fsmFile = fopen(argv[1],"r");
		testFile = fopen(argv[2],"r");
	}else{
		exit(1);
	}

	FsmModel *fsmModel = loadFsm(fsmFile);
	//printFsm(fsmModel);
	printf("model loaded!!!\n");

	FsmTestSuite *fsmTest = loadTest(testFile);
	printf("test suite loaded!!!\n");
	evaluateCoverage(fsmModel,fsmTest);
	printf("test suite coverage evaluated!!!\n");
	//printTestSuite(fsmModel,fsmTest);

	printf("prioritization!!!\n");

	prioritization_lmdp(fsmTest);
	//prioritization_gmdp(fsmTest);

	//evaluateCoverage(fsmModel,fsmTest);
	//printTestSuiteCoverage(fsmTest);
	//printTestSuite(fsmModel,fsmTest);

	printf("finished!!!\n");
	return 0;
}

