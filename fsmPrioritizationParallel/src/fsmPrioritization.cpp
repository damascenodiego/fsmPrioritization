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
#include <mpi.h>
#include <omp.h>
#include <unistd.h>

using namespace std;

void printModel(FsmModel *fsmModel);
void printTest(FsmTestSuite *fsmTest);

int main(int argc, char **argv) {

	int         my_rank, num_proc;

	MPI::Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	if (my_rank == 0) {
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


		char * prtz = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+14));
//		{
//			strcat(prtz,argv[2]);
//			strcat(prtz,".cov");
//			FILE *testCoverageFile = fopen(prtz,"w");
//			saveTestCoverage(testCoverageFile,fsmTest);
//			fflush(testCoverageFile);
//			fclose(testCoverageFile);
//
//		}
//		free(prtz);
//		prtz = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+14));

		strcat(prtz,argv[2]);
		if(argc > 3){
			strcat(prtz,".gmdp.test");
			prioritization_gmdp(fsmTest);
		}else{
			strcat(prtz,".lmdp.test");
			prioritization_lmdp(fsmTest);
		}

		testPrtzFile = fopen(prtz,"w");
		saveTest(testPrtzFile,fsmTest);
		fclose(testPrtzFile);

		{
			strcat(prtz,".cov");
			FILE *testCoverageFile = fopen(prtz,"w");
			saveTestCoverage(testCoverageFile,fsmTest);
			fclose(testCoverageFile);

		}
		delete(fsmModel);
		delete(fsmTest);

		int rank = MPI::COMM_WORLD.Get_rank();
		int cnt = omp_get_thread_num();
		printf("MPI::COMM_WORLD.Get_rank(): %d\n",rank);
		printf("omp_get_thread_num(): %d\n",cnt);

	}

	//sleep (10); //Sleeps for 1000 ms or 1 sec

	MPI::Finalize();
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
