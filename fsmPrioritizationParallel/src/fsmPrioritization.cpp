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

const static int ADD_TEST_PAIR 		= 001;
const static int GET_MOST_DISTINCT 	= 002;
const static int RMV_PAIR 			= 003;
const static int AVAILABLE_PROC		= 004;


int main(int argc, char **argv) {

	//	struct loc_val{
	//		double val;
	//		int   rank;
	//	} sendbuf,rcvbuf;

	int	my_rank;
	int	num_proc;
	int	tag = 0;


	MPI::Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	char 	filename[20]; // used just when debugging
	FILE 	*trace;  // used just when debugging


	printf("(RANK %d) \t process with rank %d started\n", my_rank, my_rank);
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

		std::list<FsmTestCase*>::iterator endi = fsmTest->getTestCase().end(); endi--;
		std::list<FsmTestCase*>::iterator endj = fsmTest->getTestCase().end();

		sprintf(filename,"log/myrank_%d.trace", my_rank);
		trace = fopen(filename, "w");
		fprintf(trace, "Hello fsmPrioritization! @ rank %d \n", my_rank);

		MPI_Request *mpirequest_bcst = (MPI_Request *) malloc(sizeof(MPI_Request));;
		MPI_Status	status;


		SimpleFsmTestCase *sf = nullptr;
		int count = 0;
		printf("(RANK %d) \t SimpleFsmTestCase BEGIN\n",my_rank);
		for(auto ti = fsmTest->getTestCase().begin(); ti != endi; ti++){
			auto tj = ti;
			for(tj++; tj != endj; tj++){
				int	dest;
//				printf("(RANK %d) \t count = %d (BEGIN)\n",my_rank,count);
				MPI_Recv(&dest,1,MPI_INT, MPI_ANY_SOURCE, AVAILABLE_PROC, MPI_COMM_WORLD,&status);
				dest = status.MPI_SOURCE;
				sf = (*ti)->getSimpleFormat();
//				printf("(RANK %d) \t Test %d sent to process %d (BEGIN)\n",my_rank,sf->testId,dest,count);
				MPI_Send(&my_rank,		1,			MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
				MPI_Send(&(sf->testId),	1,			MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
				MPI_Send(&(sf->pTot),	1,			MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
				MPI_Send(sf->p,			sf->pTot,	MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
//				printf("(RANK %d) \t Test %d sent to process %d (END)\n",my_rank,sf->testId,dest,count);

				sf = (*tj)->getSimpleFormat();
//				printf("(RANK %d) \t Test %d sent to process %d (BEGIN)\n",my_rank,sf->testId,dest,count);
				MPI_Send(&(sf->testId),	1,			MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
				MPI_Send(&(sf->pTot),	1,			MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
				MPI_Send(sf->p,			sf->pTot,	MPI_INT, dest,	ADD_TEST_PAIR, MPI_COMM_WORLD);
//				printf("(RANK %d) \t Test %d sent to process %d (END)\n",my_rank,sf->testId,dest,count);

//				printf("(RANK %d) \t count = %d (END)\n",my_rank,count);
				count++;
			}
		}
		int sendbuf = GET_MOST_DISTINCT;
		printf("(RANK %d) \t SimpleFsmTestCase END\n",my_rank);
		//		for (int dest = 0; dest < num_proc; ++dest) {
		//			MPI_Isend(&my_rank,		1,			MPI_INT, dest,	GET_MOST_DISTINCT, MPI_COMM_WORLD,mpirequest_isnd);
		//			MPI_Wait(mpirequest_isnd,&status);
		//		}
		printf("(RANK %d) \t MPI_Ibcast() BEGIN\n",my_rank);
		sendbuf = GET_MOST_DISTINCT;
		MPI_Ibcast(&sendbuf, 1, MPI_INT, 0, MPI_COMM_WORLD,mpirequest_bcst);
		MPI_Wait(mpirequest_bcst,&status);
		printf("(RANK %d) \t MPI_Ibcast() END\n",my_rank);


		//		printf("MAX_LOC.val:%f\n", rcvbuf.val);
		//		printf("MAX_LOC.rank:%d\n", rcvbuf.rank);
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
		fflush(trace);
		delete(fsmModel);
		delete(fsmTest);

		//		int rank = MPI::COMM_WORLD.Get_rank();
		//		int cnt = omp_get_thread_num();
		//		printf("MPI::COMM_WORLD.Get_rank(): %d\n",rank);
		//		printf("omp_get_thread_num(): %d\n",cnt);

	}else{

		sprintf(filename,"log/myrank_%d.trace", my_rank);
		trace = fopen(filename, "w");
		fprintf(trace, "Hello fsmPrioritization! @ rank %d \n", my_rank);
		//		printf("Hello lmdp! @ rank %d \n", my_rank);

		std::multimap<double,std::pair<int,int>> *tree = new std::multimap<double,std::pair<int,int>>;
		std::map<int,std::multimap<double,std::pair<int,int>>::iterator> *pairs = new std::map<int,std::multimap<double,std::pair<int,int>>::iterator>;

		int ok_rcv  = 0;
		int ok_snd  = 0;
		int ok_bcst  = 0;
		int sndbuf = -999;
		int rcvbuf = -999;


		MPI_Request *mpirequest_snd = (MPI_Request *) malloc(sizeof(MPI_Request));;
		MPI_Request *mpirequest_rcv = (MPI_Request *) malloc(sizeof(MPI_Request));;
		MPI_Request *mpirequest_bcst = (MPI_Request *) malloc(sizeof(MPI_Request));;
		MPI_Status	status;

		MPI_Irsend(&rcvbuf,	1,	MPI_INT, 0,	AVAILABLE_PROC,  MPI_COMM_WORLD,mpirequest_snd);
		MPI_Ibcast(&rcvbuf, 1, MPI_INT, 0, MPI_COMM_WORLD, mpirequest_bcst);

		while(true){

			MPI_Test(mpirequest_bcst,&ok_bcst,&status);
			if(ok_bcst){
				if(rcvbuf == GET_MOST_DISTINCT) break;
			}

			sleep(1);

			// check get
			MPI_Irecv(&rcvbuf,	1,	MPI_INT, 0,	ADD_TEST_PAIR,  MPI_COMM_WORLD,mpirequest_rcv);
			MPI_Test(mpirequest_rcv,&ok_rcv,&status);
			if (!ok_rcv) continue;

			SimpleFsmTestCase *test_i = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));

			MPI_Recv(&(test_i->testId), 1, MPI_INT, 0, ADD_TEST_PAIR, MPI_COMM_WORLD, &status);
			MPI_Recv(&(test_i->pTot), 1, MPI_INT, 0, ADD_TEST_PAIR, MPI_COMM_WORLD, &status);
			test_i->p 		= (int*) malloc(sizeof(int)*test_i->pTot);
			MPI_Recv(test_i->p, test_i->pTot, MPI_INT, 0, ADD_TEST_PAIR, MPI_COMM_WORLD, &status);

			//			fprintf(trace, "test[%d] = {", test_i->testId);
			//			//			printf("(RANK %d) \t test[%d] \n", test_i->testId);
			//			for (int var = 0; var < test_i->pTot; ++var) {
			//				fprintf(trace, "\t%d", test_i->p[var]);
			//				//				printf("(RANK %d) \t test[%d] = %d \n", test_i->pTot,test_i->p[var]);
			//			}
			//			fprintf(trace,"}");
			//			fprintf(trace,"\n");

			SimpleFsmTestCase *test_j = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));

			MPI_Recv(&(test_j->testId), 1, MPI_INT, 0, ADD_TEST_PAIR, MPI_COMM_WORLD, &status);
			MPI_Recv(&(test_j->pTot), 1, MPI_INT, 0, ADD_TEST_PAIR, MPI_COMM_WORLD, &status);
			test_j->p 		= (int*) malloc(sizeof(int)*test_j->pTot);
			MPI_Recv(test_j->p, test_j->pTot, MPI_INT, 0, ADD_TEST_PAIR, MPI_COMM_WORLD, &status);
			//
			//			fprintf(trace, "test[%d] = {", test_j->testId);
			//			//			printf("(RANK %d) \t test[%d] \n", test_i->testId);
			//			for (int var = 0; var < test_j->pTot; ++var) {
			//				fprintf(trace, "\t%d", test_j->p[var]);
			//				//				printf("(RANK %d) \t test[%d] = %d \n", test_i->pTot,test_i->p[var]);
			//			}
			//			fprintf(trace,"}");
			//
			//			fprintf(trace,"\n\n");

			double ds = calcSimpleSimilarity(test_i,test_j);
			fprintf(trace, "calcSimpleSimilarity(test[%d],test[%d]) = %f \n", test_i->testId, test_j->testId,ds);
			fflush(trace);

			MPI_Irsend(&rcvbuf,	1,	MPI_INT, 0,	AVAILABLE_PROC,  MPI_COMM_WORLD,mpirequest_snd);
		}

	}
	printf("\n\n----(RANK %d) \t THE END!!!---\n",my_rank);
	MPI::Finalize();
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
