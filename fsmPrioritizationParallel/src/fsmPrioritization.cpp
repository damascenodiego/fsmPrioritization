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

		char 	command[20], **argv_spawn;
		int *array_of_errcodes;
		int color = my_rank;
		int key = my_rank;
		MPI_Comm	cij_comm,intercomm;

		MPI_Comm_split(MPI_COMM_WORLD, color, key, &cij_comm);

		strcpy(command, "lmdp");
		argv_spawn = (char **)0;
		int nn_1_2 = (int)((fsmTest->getNoResets()*(fsmTest->getNoResets()-1))/2);

		// used by MPI_Comm_spawn
		array_of_errcodes = (int *) calloc(nn_1_2, sizeof(int));


		int root = 0;
		MPI_Comm_spawn(command, argv_spawn, nn_1_2, MPI_INFO_NULL, root, cij_comm, &intercomm, array_of_errcodes);

		MPI_Request * mpirequest = (MPI_Request *) calloc(nn_1_2, sizeof(MPI_Request));

		std:: list<FsmTestCase*> t;
		for(FsmTestCase *i : fsmTest->getTestCase()){
			t.push_back(i);
		}

		std::list<FsmTestCase*>::iterator endi = t.end(); endi--;
		std::list<FsmTestCase*>::iterator endj = t.end();;

		int tag = 0;
		int dest = 0;
		SimpleFsmTestCase *sf = nullptr;
		for(auto ti = t.begin(); ti != endi; ti++){
			auto tj = ti;
			for(tj++; tj != endj; tj++){
				sf = (*ti)->getSimpleFormat();
				MPI_Send(&(sf->testId),1,MPI_INT, dest, tag, intercomm);
				MPI_Send(&(sf->pTot),1,MPI_INT, dest, tag, intercomm);
				MPI_Send(sf->p,sf->pTot,MPI_INT, dest, tag, intercomm);

				sf = (*tj)->getSimpleFormat();
				MPI_Send(&(sf->testId),1,MPI_INT, dest, tag, intercomm);
				MPI_Send(&(sf->pTot),1,MPI_INT, dest, tag, intercomm);
				MPI_Send(sf->p,sf->pTot,MPI_INT, dest, tag, intercomm);

				dest++;
			}
		}
		struct {
		        double val;
		        int   rank;
		} sendbuf,rcvbuf;

		MPI_Reduce(&sendbuf, &rcvbuf, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_ROOT, intercomm);
		printf("MAX_LOC.val:%f\n", rcvbuf.val);
		printf("MAX_LOC.rank:%d\n", rcvbuf.rank);
		//	    sleep(5);

		//		char * prtz = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+14));
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
		//
		//		strcat(prtz,argv[2]);
		//		if(argc > 3){
		//			strcat(prtz,".gmdp.test");
		//			prioritization_gmdp(fsmTest);
		//		}else{
		//			strcat(prtz,".lmdp.test");
		//		prioritization_lmdp(fsmTest);
		//		}
		//
		//		testPrtzFile = fopen(prtz,"w");
		//		saveTest(testPrtzFile,fsmTest);
		//		fclose(testPrtzFile);
		//
		//		{
		//			strcat(prtz,".cov");
		//			FILE *testCoverageFile = fopen(prtz,"w");
		//			saveTestCoverage(testCoverageFile,fsmTest);
		//			fclose(testCoverageFile);
		//
		//		}
		delete(fsmModel);
		delete(fsmTest);

//		int rank = MPI::COMM_WORLD.Get_rank();
//		int cnt = omp_get_thread_num();
//		printf("MPI::COMM_WORLD.Get_rank(): %d\n",rank);
//		printf("omp_get_thread_num(): %d\n",cnt);

	}

	MPI::Finalize();
	printf("\n\n--- THE END (%d)!!!---",my_rank);
	exit(0);
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
