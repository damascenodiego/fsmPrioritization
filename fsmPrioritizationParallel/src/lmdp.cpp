/*
 * helloSpawn.cpp
 *
 *  Created on: 6 de nov de 2016
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

int main2(int argc, char **argv) {

	int     my_rank, num_proc, parent_rank;
	char 	filename[20]; // used just when debugging
	FILE 	*trace;  // used just when debugging


	MPI_Comm    intercomm;
	MPI_Status	status;

	MPI::Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	// recouvering intercommunicator to interact to "father"!
	MPI_Comm_get_parent(&intercomm);

	SimpleFsmTestCase *test_i = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));

	MPI_Recv(&(test_i->testId), 1, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);
	parent_rank = status.MPI_SOURCE;

	MPI_Recv(&(test_i->pTot), 1, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);
	test_i->p 		= (int*) malloc(sizeof(int)*test_i->pTot);
	MPI_Recv(test_i->p, test_i->pTot, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);


	SimpleFsmTestCase *test_j = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));

	MPI_Recv(&(test_j->testId), 1, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);
	MPI_Recv(&(test_j->pTot), 1, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);
	test_j->p 		= (int*) malloc(sizeof(int)*test_j->pTot);
	MPI_Recv(test_j->p, test_j->pTot, MPI_INT, 0, MPI_ANY_TAG, intercomm, &status);


	sprintf(filename,"log/rank_%d_r%d_c%d_myrank%d.trace", parent_rank, test_i->testId, test_j->testId, my_rank);
	trace = fopen(filename, "w");



	//	printing just for debugging purposes
	fprintf(trace, "Hello lmdp! @ rank %d \n", my_rank);
	//	printf("Hello lmdp! @ rank %d \n", my_rank);

	fprintf(trace, "test[%d] \n", test_i->testId);
	//	printf("test[%d] \n", test_i->testId);
	for (int var = 0; var < test_i->pTot; ++var) {
		fprintf(trace, "\ttest[%d] = %d \n", test_i->pTot,test_i->p[var]);
		//		printf("\ttest[%d] = %d \n", test_i->pTot,test_i->p[var]);
	}

	fprintf(trace,"\n\n");
	fprintf(trace, "test[%d] \n", test_j->testId);
	//	printf("\n\n");
	//	printf("test[%d] \n", test_j->testId);
	for (int var = 0; var < test_j->pTot; ++var) {
		fprintf(trace, "\ttest[%d] = %d \n", test_j->pTot,test_j->p[var]);
		//		printf( "\ttest[%d] = %d \n", test_j->pTot,test_j->p[var]);
	}

	double ds = calcSimpleSimilarity(test_i,test_j);
	struct {
	        double val;
	        int   rank;
	} sendbuf,rcvbuf;
	sendbuf.val	= ds;
	sendbuf.rank 	= my_rank;

	MPI_Reduce(&sendbuf, &rcvbuf, 1, MPI_DOUBLE_INT, MPI_MAXLOC, parent_rank, intercomm);

	fprintf(trace, "\tcalcSimpleSimilarity(test[%d],test[%d]) = %f \n", test_i->testId, test_j->testId,ds);


	fflush(0);
	fclose(trace);
//	printf("\n\n--- THE END (%d)!!!---",my_rank);
	MPI_Finalize();
	exit(0);
}


