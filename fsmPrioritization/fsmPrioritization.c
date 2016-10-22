/*
 * fsmPrioritization.c
 *
 *  Created on: 21 de out de 2016
 *      Author: damasceno
 */


#include <stdio.h>
#include <stdlib.h>
#include "lib/fsmLib.h"


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

	printFsm(fsmModel);

	FsmTestSuite *fsmTest = loadTest(testFile);


	return 0;
}

