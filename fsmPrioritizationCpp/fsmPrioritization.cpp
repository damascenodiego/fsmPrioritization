/*
 * fsmPrioritization.cpp
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#include <cstdio>
#include <iostream>
#include <cstring>
#include "lib/fsmLib.h"
#include <time.h>

using namespace std;

void printModel(FsmModel *fsmModel);
void printTest(FsmTestSuite *fsmTest);

int main(int argc, char **argv) {

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	struct timespec start,stop;
	clock_gettime(CLOCK_REALTIME, &start);
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////


	FILE *fsmFile;
	FILE *testFile;
	FILE *testPrtzFile;


	FsmModel *fsmModel;
	FsmTestSuite *fsmTest;


	if(argc >= 3){
		fsmFile = fopen(argv[1],"r");
		fsmModel = loadFsm(fsmFile);
		fclose(fsmFile);

		testFile = fopen(argv[2],"r");
		fsmTest = loadTest(testFile,fsmModel);
		fclose(testFile);
	}else{
		return (1);
	}


	prioritization_lmdp(fsmTest);

	char * prtz = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+14));

	time_t timer;
	char buffer[26];
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	strftime(buffer, 26, "%Y_%m_%d_%H_%M", tm_info);

	strcat(prtz,argv[2]);
	strcat(prtz,buffer);
	strcat(prtz,".serial.lmdp.test");
	testPrtzFile = fopen(prtz,"w");
	saveTest(testPrtzFile,fsmTest);
	fclose(testPrtzFile);
//	strcat(prtz,".cov");
//	FILE *testCoverageFile = fopen(prtz,"w");
//	saveTestCoverage(testCoverageFile,fsmTest);
//	fclose(testCoverageFile);


	delete(fsmModel);
	delete(fsmTest);
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	clock_gettime(CLOCK_REALTIME, &stop);

	double diff = (double)((stop.tv_sec+stop.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9));

	char 	*filename = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+26));; // used just when debugging
	FILE 	*trace;  // used just when debugging

	strcat(filename,argv[2]);
	strcat(filename,buffer);
	strcat(filename,".trace");

	trace = fopen(filename, "w");

	fprintf(trace,"Filename: %s\tSerial LMDP %lf\n",argv[2],(diff)); fflush(trace);
	fprintf(stdout,"Filename: %s\tSerial LMDP %lf\n",argv[2],(diff)); fflush(stdout);
	fclose(trace);
	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

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
	printf("Test suite: length: %d | noResets: %d |avg length %f (@%p)\n",(*fsmTest).getLength(),(*fsmTest).getNoResets(),(*fsmTest).getAvgLength(),fsmTest);
	for (auto i : (*fsmTest).getTestCase()) {
		(*i).print();
	}
}
