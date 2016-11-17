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
#include <math.h>

using namespace std;

void printModel(FsmModel *fsmModel);
void printTest(FsmTestSuite *fsmTest);

const static int ADD_TEST_PAIR 		= 001;
const static int GET_MOST_DISTINCT 	= 002;
const static int RMV_PAIR 			= 003;
const static int AVAILABLE_PROC		= 004;


int main(int argc, char **argv) {

	int	my_rank;
	int	num_proc;
	int	tag = 0;



	MPI::Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	time_t timer;
	char buffer[26];
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	strftime(buffer, 26, "%Y_%m_%d_%H_%M", tm_info);


//	char 	filename[46]; // used just when debugging
//	FILE 	*trace;  // used just when debugging
//
//	sprintf(filename,"log/log_%s_rank_%02d.trace", buffer, my_rank);
//	trace = fopen(filename, "w");

	int x;
	int y;
	int noResets = (int) ((0.5 + sqrt(0.25+2*(num_proc))));
	calculateXY(x,y,noResets,my_rank);

	int x_prtz,y_prtz;
	int rankMax;
	int removeTest_r = -1;
	int removeTest_c = -1;

//	fprintf(trace, "Hello fsmPrioritization! @ rank %d \n", my_rank);fflush(trace);
//	fprintf(trace, "Total of processes: %d \n", num_proc);fflush(trace);
//	fprintf(trace,"(RANK %d) \t noResets = %d\n",my_rank,noResets);fflush(trace);
//	fprintf(trace,"(RANK %d) \t (t_%d,t_%d)\n",my_rank,x,y);fflush(trace);


	int out;
	int id_p[2];

	MPI_Comm * row_color 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));
	MPI_Comm * col_color 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));

	SimpleFsmTestCase *ti = nullptr;
	SimpleFsmTestCase *tj = nullptr;

	if (my_rank == 0) {

		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		char 	filename[46]; // used just when debugging
		FILE 	*trace;  // used just when debugging

		sprintf(filename,"log/logParall_%s.trace", buffer);
		trace = fopen(filename, "w");

		double begin, end;

		begin = MPI_Wtime();
		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		FILE *fsmFile;
		FILE *testFile;
		FILE *testPrtzFile;

		FsmModel *fsmModel;
		FsmTestSuite *fsmTest;

		if(argc == 3){
			fsmFile = fopen(argv[1],"r");
			fsmModel = loadFsm(fsmFile);
			fclose(fsmFile);

			testFile = fopen(argv[2],"r");
			fsmTest = loadTest(testFile,fsmModel);
			fclose(testFile);

			double nn12 = (fsmTest->getNoResets()*(fsmTest->getNoResets()-1))/2.0;
			if( num_proc != nn12+1) {
				fprintf(stderr,"-np must be %f\n",nn12+1);
				MPI_Abort(MPI_COMM_WORLD,1);
			}
		}else{
			MPI_Abort(MPI_COMM_WORLD,1);
		}


		// row_color is filled until (var < noResets-1)
		// (n-1) groups to broadcast test cases for each row from process 0
		for (int var = 0; var < noResets-1; ++var) {
			out = MPI_Comm_split(MPI_COMM_WORLD, (var+1),    0, &row_color[var]);
//			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}

		// column_color is filled from 1 to (var < noResets)
		// (n-1) groups to broadcast test cases for each column from process 0
		for (int var = 1; var < noResets; ++var) {
			out = MPI_Comm_split(MPI_COMM_WORLD, (var+1)*10,    0, &col_color[var]);
//			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}

		//		prints info about all processes from row_color[var]
		//		MPI_Barrier(MPI_COMM_WORLD);
		//		for (int var = 0; var < noResets-1; ++var) {
		//			fprintf(trace,"(RANK %d) \t row_color[%d]\n",my_rank,var);
		//			print_comm_ranks(row_color[var],trace);
		//		}
		//		// prints info about all processes from col_color[var]
		//		for (int var = 1; var < noResets; ++var) {
		//			fprintf(trace,"(RANK %d) \t col_color[%d]\n",my_rank,var);
		//			print_comm_ranks(col_color[var],trace);
		//		}

		std::map<int,FsmTestCase*> ts;
		int id = 0;
		for(FsmTestCase* tc : fsmTest->getTestCase()){
			ts[id]=tc;
			//			tc->print();
			id++;
		}
		ti = ts[0]->getSimpleFormat();
		tj = ts[1]->getSimpleFormat();

		SimpleFsmTestCase *ttemp;
		for(int var = 0; var < noResets-1; var++){
			ttemp = ts[var]->getSimpleFormat();
			// sending test id
			MPI_Bcast(&(ttemp->testId),1,MPI_INT,0,row_color[var]);
			// sending pTot
			MPI_Bcast(&(ttemp->pTot),1,MPI_INT,0,row_color[var]);
			// sending p set
			MPI_Bcast(ttemp->p,ttemp->pTot,MPI_INT,0,row_color[var]);
			//			for (int var = 0; var < ttemp->pTot; ++var) {
			//				MPI_Bcast(&ttemp->p[var],1,MPI_INT,0,row_color[var]);
			//			}
		}
		for(int var = 1; var < noResets; var++){
			ttemp = ts[var]->getSimpleFormat();
			// sending test id
			MPI_Bcast(&(ttemp->testId),1,MPI_INT,0,col_color[var]);
			// sending pTot
			MPI_Bcast(&(ttemp->pTot),1,MPI_INT,0,col_color[var]);
			// sending p set
			MPI_Bcast(ttemp->p,ttemp->pTot,MPI_INT,0,col_color[var]);
			//			for (int var = 0; var < ttemp->pTot; ++var) {
			//				MPI_Bcast(&ttemp->p[var],1,MPI_INT,0,col_color[var]);
			//			}
		}


		// prioritizing test cases (lmdp)
		MPI_Comm * allTests 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
		int color = 0;
		out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank,  allTests);
//		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,color,my_rank,my_rank,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

		struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		send_data->rank = my_rank;

//		fprintf(trace,"(RANK %d) \t val_rank created \n",my_rank); fflush(trace);

		send_data->val = -1;

		std::list<FsmTestCase*> t_prtz;

		for (int t_id = 0; t_id < noResets; t_id+=2) {
			if(ts.size() == 1) break;
//			fprintf(trace,"(RANK %d) \t mpi_reduce - begin \n",my_rank); fflush(trace);
			MPI_Reduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, *allTests);
//			fprintf(trace,"(RANK %d) \t mpi_reduce - end \n",my_rank); fflush(trace);
			rankMax = recv_data->rank;
			calculateXY(x_prtz,y_prtz,noResets,rankMax);


			removeTest_r = x_prtz;
			removeTest_c = y_prtz;

			MPI_Bcast(&removeTest_r,1,MPI_INT,0,*allTests);
			MPI_Bcast(&removeTest_c,1,MPI_INT,0,*allTests);

//			fprintf(stderr,"(RANK %d) \t Test %d: t_%d (ds = %f)\n",my_rank,t_id,x_prtz,recv_data->val);
//			fprintf(stderr,"(RANK %d) \t Test %d: t_%d (ds = %f)\n",my_rank,t_id+1,y_prtz,recv_data->val);
			t_prtz.push_back(ts[x_prtz]);
			t_prtz.push_back(ts[y_prtz]);
			ts.erase(x_prtz);
			ts.erase(y_prtz);
		}
		if(ts.size() == 1){
			FsmTestCase * last = ts.begin()->second;
			int lastId = last->getId();
			MPI_Bcast(&lastId,1,MPI_INT,0,*allTests);
			MPI_Bcast(&lastId,1,MPI_INT,0,*allTests);
			t_prtz.push_back(ts.begin()->second);
		}
		fsmTest->getTestCase().clear();
		fsmTest->getTestCase().merge(t_prtz);


		char * prtz = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+14));
		strcat(prtz,argv[2]);
		strcat(prtz,".lmdp.test");
		testPrtzFile = fopen(prtz,"w");
		saveTest(testPrtzFile,fsmTest);
		fclose(testPrtzFile);
		strcat(prtz,".cov");
		FILE *testCoverageFile = fopen(prtz,"w");
		saveTestCoverage(testCoverageFile,fsmTest);
		fclose(testCoverageFile);

		delete(fsmModel);
		delete(fsmTest);

		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		end = MPI_Wtime();

		double diff = end-begin;
		fprintf(trace,"TIME %f\n",(diff)); fflush(trace);
		fprintf(stdout,"TIME %f\n",(diff)); fflush(stdout);
		fclose(trace);
		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

	}else{
		int color;

		// row_color is filled until (var < noResets-1)
		// (n-1) groups to receive test cases for each row from process 0
		// only for (var == x) is included in color != MPI_UNDEFINED
		for (int var = 0; var < noResets-1; ++var) {
			if(var != x){
				color = MPI_UNDEFINED;
			}else{
				color = (var+1) ;
			}
			out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank, &row_color[var]);
//			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}

		// col_color is filled until (var < noResets-1)
		// (n-1) groups to receive test cases for each row from process 0
		// only for (var == y) is included in color != MPI_UNDEFINED
		for (int var = 1; var < noResets; ++var) {
			if(var != y){
				color = MPI_UNDEFINED;
			}else{
				color = (var+1)*10 ;
			}
			out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank,  &col_color[var]);
//			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}

		//		MPI_Barrier(MPI_COMM_WORLD);

		// allocate SimpleFsmTestCase for ti
		ti = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));

		// receiving ti data through bcast
		MPI_Bcast(&ti->testId,1,MPI_INT,0,row_color[x]);
		MPI_Bcast(&ti->pTot,1,MPI_INT,0,row_color[x]);
//		fprintf(trace,"(RANK %d) \t t_%d has %d elements at p\n",my_rank,ti->testId,ti->pTot); fflush(trace);
		ti->p = (int*) malloc(sizeof(int)*ti->pTot);
		MPI_Bcast(ti->p,ti->pTot,MPI_INT,0,row_color[x]);
		//		for (int var = 0; var < ti->pTot; ++var) {
		//			MPI_Bcast(&ti->p[var],1,MPI_INT,0,row_color[x]);
		//		}
//		fprintf(trace,"(RANK %d) \t t_%d.p = {",my_rank,ti->testId);
//		for (int var = 0; var < ti->pTot; ++var) {
//			fprintf(trace,"\t%d",ti->p[var]);
//		}
//		fprintf(trace,"\t}\n");
//		fflush(trace);

		// allocate SimpleFsmTestCase for tj
		tj = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));

		// receiving tj data through bcast
		MPI_Bcast(&tj->testId,1,MPI_INT,0,col_color[y]);
		MPI_Bcast(&tj->pTot,1,MPI_INT,0,col_color[y]);
//		fprintf(trace,"(RANK %d) \t t_%d has %d elements at p\n",my_rank,tj->testId,tj->pTot); fflush(trace);
		tj->p = (int*) malloc(sizeof(int)*tj->pTot);
		MPI_Bcast(tj->p,tj->pTot,MPI_INT,0,col_color[y]);
		//		for (int var = 0; var < tj->pTot; ++var) {
		//			MPI_Bcast(&tj->p[var],1,MPI_INT,0,col_color[y]);
		//		}
//		fprintf(trace,"(RANK %d) \t t_%d.p = {",my_rank,tj->testId);
//		for (int var = 0; var < tj->pTot; ++var) {
//			fprintf(trace,"\t%d",tj->p[var]);
//		}
//		fprintf(trace,"\t}\n");
//		fflush(trace);



		// prioritizing test cases (lmdp)
		MPI_Comm * allTests 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
		color = 0;
		out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank,  allTests);
//		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,color,my_rank,my_rank,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

		struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		send_data->rank = my_rank;

//		fprintf(trace,"(RANK %d) \t val_rank created \n",my_rank); fflush(trace);

		send_data->val = calcSimpleSimilarity(ti,tj);
		//		fprintf(stderr,"-----(RANK %d) \t ds(t_%d,t_%d) = %f\n",my_rank,x,y,send_data->val);

		MPI_Status status_r;
		MPI_Status status_c;
		MPI_Request *req_r = (MPI_Request*) malloc(sizeof(MPI_Request));
		MPI_Request *req_c = (MPI_Request*) malloc(sizeof(MPI_Request));


		int flag_r = 1,flag_c = 1;

		for (int t_id = 0; t_id < noResets; t_id+=2) {
			MPI_Reduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, *allTests);

			MPI_Bcast(&removeTest_r,1,MPI_INT,0,*allTests);
			MPI_Bcast(&removeTest_c,1,MPI_INT,0,*allTests);

			if(removeTest_r == x || removeTest_r == y){
				//				fprintf(stderr,"-----(RANK %d) \t (flag req_r!)\n",my_rank);
				send_data->val = -1;
			}
			if(removeTest_c == y || removeTest_c == x){
				//				fprintf(stderr,"-----(RANK %d) \t (flag req_c!)\n",my_rank);
				send_data->val = -1;
			}
		}
	}

	//	// prioritizing test cases (lmdp)
	//	MPI_Comm * allTests 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
	//	int color = 0;
	//	out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank,  allTests);
	//	fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,color,my_rank,my_rank,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
	//
	//	struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
	//	struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
	//	send_data->rank = my_rank;
	//
	//	fprintf(trace,"(RANK %d) \t val_rank created \n",my_rank); fflush(trace);
	//
	//	int x_prtz,y_prtz;
	//	int rankMax;
	//	int removeTest_r = -1;
	//	int removeTest_c = -1;
	//
	//	if(my_rank == 0 ){
	//		send_data->val = -1;
	//
	//		for (int t_id = 0; t_id < noResets; t_id+=2) {
	//			fprintf(trace,"(RANK %d) \t mpi_reduce - begin \n",my_rank); fflush(trace);
	//			MPI_Reduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, *allTests);
	//			fprintf(trace,"(RANK %d) \t mpi_reduce - end \n",my_rank); fflush(trace);
	//			rankMax = recv_data->rank;
	//			calculateXY(x_prtz,y_prtz,noResets,rankMax);
	//
	//
	//			removeTest_r = x_prtz;
	//			removeTest_c = y_prtz;
	//
	//			MPI_Bcast(&removeTest_r,1,MPI_INT,0,*allTests);
	//			MPI_Bcast(&removeTest_c,1,MPI_INT,0,*allTests);
	//
	//			fprintf(stderr,"(RANK %d) \t Test %d: t_%d (ds = %f)\n",my_rank,t_id,x_prtz,recv_data->val);
	//			fprintf(stderr,"(RANK %d) \t Test %d: t_%d (ds = %f)\n",my_rank,t_id+1,y_prtz,recv_data->val);
	//
	//		}
	//	}else{
	//		send_data->val = calcSimpleSimilarity(ti,tj);
	////		fprintf(stderr,"-----(RANK %d) \t ds(t_%d,t_%d) = %f\n",my_rank,x,y,send_data->val);
	//
	//		MPI_Status status_r;
	//		MPI_Status status_c;
	//		MPI_Request *req_r = (MPI_Request*) malloc(sizeof(MPI_Request));
	//		MPI_Request *req_c = (MPI_Request*) malloc(sizeof(MPI_Request));
	//
	//
	//		int flag_r = 1,flag_c = 1;
	//
	//		for (int t_id = 0; t_id < noResets; t_id+=2) {
	//			MPI_Reduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, *allTests);
	//
	//			MPI_Bcast(&removeTest_r,1,MPI_INT,0,*allTests);
	//			MPI_Bcast(&removeTest_c,1,MPI_INT,0,*allTests);
	//
	//			if(removeTest_r == x || removeTest_r == y){
	////				fprintf(stderr,"-----(RANK %d) \t (flag req_r!)\n",my_rank);
	//				send_data->val = -1;
	//			}
	//			if(removeTest_c == y || removeTest_c == x){
	////				fprintf(stderr,"-----(RANK %d) \t (flag req_c!)\n",my_rank);
	//				send_data->val = -1;
	//			}
	//		}
	//	}



//	MPI_Barrier(MPI_COMM_WORLD);

//	fprintf(trace,"----(RANK %02d) \t THE END!!!---\n",my_rank);
//	fflush(trace);
//	fclose(trace);
	MPI::Finalize();
	exit(0);

}


