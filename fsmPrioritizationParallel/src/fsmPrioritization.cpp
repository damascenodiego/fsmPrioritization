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

	sprintf(filename,"log/myrank_%02d.trace", my_rank);
	trace = fopen(filename, "w");
	fprintf(trace, "Hello fsmPrioritization! @ rank %d \n", my_rank);
	fprintf(trace, "Total of processes: %d \n", num_proc);


	//	SimpleFsmTestCase *ti = nullptr;
	//	SimpleFsmTestCase *tj = nullptr;
	//
	//	MPI_Comm * comm_0 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
	//	MPI_Comm_split(MPI_COMM_WORLD, 0, my_rank, comm_0);
	//
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

			int nn12 = (fsmTest->getNoResets()*(fsmTest->getNoResets()-1))/2;
			if( num_proc != nn12+1) {
				fprintf(stderr,"-np must be %d\n",nn12);
				MPI_Abort(MPI_COMM_WORLD,1);
			}
			fclose(testFile);
		}else{
			return (1);
		}


		int out;
		int local,lr;
		int noResets = fsmTest->getTestCase().size();
		MPI_Comm * comm_0 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));

		for (int var = 1; var < num_proc; ++var) {
			out = MPI_Comm_split(MPI_COMM_WORLD, 0, var, comm_0);
			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
			fflush(trace);
			//			MPI_Comm_size(*comm_0, &local);
			//			MPI_Comm_rank(*comm_0, &lr);
			//			fprintf(trace,"(RANK %d ) Color %d \t Local rank %d \t MPI_Comm_size %d\n",my_rank,0,lr,local);
			fflush(trace);
		}

		MPI_Comm * comm_x 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));
		auto endi = fsmTest->getTestCase().end(); endi--;
		for (auto xpos = fsmTest->getTestCase().begin(); xpos != endi; ++xpos) {
			auto ypos = xpos;
			for (ypos++; ypos != fsmTest->getTestCase().end(); ++ypos) {
				int xid = (*xpos)->getId();
				int yid = (*ypos)->getId();
				int nproc = toTriangMatrix(xid,yid,noResets);

				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) (BEGIN)\n",my_rank,xid+1,nproc+1);
				out = MPI_Comm_split(MPI_COMM_WORLD, xid+1, nproc+1, &comm_x[xid]);
				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,xid+1,nproc+1,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
				fflush(trace);

				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) (BEGIN)\n",my_rank,yid+1,nproc+1);
				out = MPI_Comm_split(MPI_COMM_WORLD, yid+1, nproc+1, &comm_x[yid]);
				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,yid+1,nproc,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
				fflush(trace);
			}
		}
		//
		//
		//		int sendbuf;
		//
		//		MPI_Comm * comm_undef 	= (MPI_Comm *) malloc(sizeof(MPI_Comm));
		//
		//		//
		//		int rank_id=0;
		//
		///*		for (auto xpos = fsmTest->getTestCase().begin(); xpos != fsmTest->getTestCase().end(); ++xpos) {
		//			for (auto ypos = xpos; ypos != fsmTest->getTestCase().end(); ++ypos) {
		//				ypos++;
		//				if((*xpos)->getId()!=(*ypos)->getId()){
		//					int nproc = toTriangMatrix((*xpos)->getId(),(*ypos)->getId(),noResets);
		//					out = MPI_Comm_split(MPI_COMM_WORLD, (*xpos)->getId(), nproc, &comm_x[(*xpos)->getId()]);
		//					fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, comm_x) = %s (%d)\n",my_rank,(*xpos)->getId(),nproc,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//
		//					out = MPI_Comm_split(MPI_COMM_WORLD, (*ypos)->getId(), nproc, &comm_y[(*ypos)->getId()]);
		//					fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, comm_x) = %s (%d)\n",my_rank,(*ypos)->getId(),nproc,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//				}
		//				sendbuf = (*xpos)->getId();
		//				//			MPI_Ibcast(&sendbuf, 1, MPI_INT, 0, comm_x[sendbuf],reqx);
		//				MPI_Bcast(&sendbuf, 1, MPI_INT, 0, comm_x[sendbuf]);
		//				fprintf(trace,"(RANK %d) \t MPI_Bcast: t_%d (END)\n",my_rank,sendbuf);
		//
		//				sendbuf = (*ypos)->getId();
		//				//			MPI_Ibcast(&sendbuf, 1, MPI_INT, 0, comm_y[sendbuf],reqy);
		//				MPI_Bcast(&sendbuf, 1, MPI_INT, 0, comm_y[sendbuf]);
		//				fprintf(trace,"(RANK %d) \t MPI_Bcast: t_%d (END)\n",my_rank,sendbuf);
		//				fflush(trace);
		//
		//			}
		//		}*/
		//		for (int varx = 0; varx < noResets-1; ++varx) {
		//			for (int vary = varx+1; vary < noResets; ++vary) {
		//				if((varx == x) || (vary == y)){
		//					if((varx == x)){
		//						out = MPI_Comm_split(MPI_COMM_WORLD, x+1, rank_id, comm_x);
		//						fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, comm_x) = %s (%d)\n",my_rank,x,rank_id,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//					}
		//					if((vary == y)){
		//						out = MPI_Comm_split(MPI_COMM_WORLD, y+1, rank_id, comm_y);
		//						fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, comm_y) = %s (%d)\n",my_rank,y,rank_id,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//					}
		//				}
		//				if(((varx != x)) && (vary != y)){
		//					out = MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank_id, comm_undef);
		//					fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, %d, comm_undef) = %s (%d)\n",my_rank,rank_id,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//				}
		//				fflush(trace);
		//				rank_id++;
		//			}
		//		}
		//
		//
		//		//		int local,lr;
		//		//		MPI_Comm_size(*comm_x, &local);
		//		//		MPI_Comm_rank(*comm_x, &lr);
		//		//		fprintf(trace,"(RANK %d ) Color %d \t Local rank %d \t MPI_Comm_size %d\n",my_rank,sendbuf,lr,local); fflush(trace);
		//		//		fflush(trace);
		//		//
		//		//		MPI_Comm_size(*comm_y, &local);
		//		//		MPI_Comm_rank(*comm_y, &lr);
		//		//		fprintf(trace,"(RANK %d ) Color %d \t Local rank %d \t MPI_Comm_size %d\n",my_rank,sendbuf,lr,local); fflush(trace);
		//		//		fflush(trace);
		//		//
		//		//		MPI_Barrier(MPI_COMM_WORLD);
		//		//
		////		MPI_Request * reqx = (MPI_Request *) malloc(sizeof(MPI_Request));
		////		MPI_Request * reqy = (MPI_Request *) malloc(sizeof(MPI_Request));
		////
		////		for(auto tc = fsmTest->getTestCase().begin(); tc != fsmTest->getTestCase().end(); tc++){
		////			fprintf(trace,"(RANK %d) \t MPI_Bcast: t_%d (BEGIN)\n",my_rank,sendbuf);
		////			fflush(trace);
		////
		////			sendbuf = (*tc)->getId();
		////			if(sendbuf == x){
		////				ti = (*tc)->getSimpleFormat();
		////			}else if(sendbuf == y){
		////				tj = (*tc)->getSimpleFormat();
		////			}
		////			MPI_Bcast(&sendbuf, 1, MPI_INT, 0, comm_x[sendbuf]);
		////			MPI_Bcast(&sendbuf, 1, MPI_INT, 0, comm_y[sendbuf]);
		////			//			MPI_Ibcast(&sendbuf, 1, MPI_INT, 0, comm_x[sendbuf],reqx);
		////			//			MPI_Ibcast(&sendbuf, 1, MPI_INT, 0, comm_y[sendbuf],reqy);
		////
		////			fprintf(trace,"(RANK %d) \t MPI_Bcast: t_%d (END)\n",my_rank,sendbuf);
		////			fflush(trace);
		////		}
		//
		//		fflush(trace);
		//		delete(fsmModel);
		//		delete(fsmTest);
		//
		//
	}else{
		int tmp = (int) ((0.5 + sqrt(0.25+2*(num_proc))));
		int noResets = tmp;
		int x = 0;
		int y = my_rank;
		tmp--;
		while (y > tmp ){
			x++;
			y -= tmp;
			tmp--;
		}

		y += x ;


		fprintf(trace,"(RANK %d) \t noResets = %d\n",my_rank,noResets);
		fprintf(trace,"(RANK %d) \t (t_%d,t_%d)\n",my_rank,x,y);
		fflush(trace);

		int out;

		MPI_Comm * comm_0 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
		int local,lr;
		for (int var = 1; var < num_proc; ++var) {
			out = MPI_Comm_split(MPI_COMM_WORLD, 0, var, comm_0);
			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
			fflush(trace);
			//			MPI_Comm_size(*comm_0, &local);
			//			MPI_Comm_rank(*comm_0, &lr);
			//			fprintf(trace,"(RANK %d ) Color %d \t Local rank %d \t MPI_Comm_size %d\n",my_rank,0,lr,local);
			//			fflush(trace);
		}


		MPI_Comm * comm_x 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));
		MPI_Comm * comm_y 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));

		for (int xpos = 0; xpos < noResets -1 ; ++xpos) {
			for (auto ypos = xpos+1; ypos < noResets; ++ypos) {
				int xid = (xpos);
				int yid = (ypos);
				int nproc = toTriangMatrix(xid,yid,noResets);

				out = MPI_Comm_split(MPI_COMM_WORLD, xid+1, nproc+1, &comm_x[xid]);
				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,xid+1,nproc,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
				fflush(trace);

				out = MPI_Comm_split(MPI_COMM_WORLD, yid+1, nproc+1, &comm_x[yid]);
				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,yid+1,nproc,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
				fflush(trace);
			}
		}



/*		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,0,my_rank,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		out = MPI_Comm_split(MPI_COMM_WORLD, x+1, my_rank, comm_x);
		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,x+1,my_rank,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

		out = MPI_Comm_split(MPI_COMM_WORLD, y+1, my_rank, comm_y);
		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d) = %s (%d)\n",my_rank,y+1,my_rank,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
*/


		//		MPI_Comm_split(MPI_COMM_WORLD, x, 0, comm_x);
		//		MPI_Comm_split(MPI_COMM_WORLD, y, 0, comm_y);
		//		//		int out;
		//		//		int rank_id=0;
		//		//		for (int varx = 0; varx < noResets-1; ++varx) {
		//		//			for (int vary = varx+1; vary < noResets; ++vary) {
		//		//				if((varx == x) || (vary == y)){
		//		//					if((varx == x)){
		//		//						out = MPI_Comm_split(MPI_COMM_WORLD, x, rank_id, comm_x);
		//		//						fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, comm_x) = %s (%d)\n",my_rank,x,rank_id,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//		//					}
		//		//					if((vary == y)){
		//		//						out = MPI_Comm_split(MPI_COMM_WORLD, y, rank_id, comm_y);
		//		//						fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, comm_y) = %s (%d)\n",my_rank,y,rank_id,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//		//					}
		//		//				}
		//		//				if(((varx != x)) && (vary != y)){
		//		//					out = MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank_id, comm_undef);
		//		//					fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, %d, comm_undef) = %s (%d)\n",my_rank,rank_id,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//		//				}
		//		//				//				out = MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank_id, comm_x);
		//		//				fflush(trace);
		//		//				rank_id++;
		//		//			}
		//		//		}
		//
		//		int local,lr;
		//		MPI_Comm_size(*comm_x, &local);
		//		MPI_Comm_rank(*comm_x, &lr);
		//		fprintf(trace,"(RANK %d ) Color %d \t Local rank %d \t MPI_Comm_size %d\n",my_rank,x,lr,local);
		//		fflush(trace);
		//
		//		MPI_Comm_size(*comm_y, &local);
		//		MPI_Comm_rank(*comm_y, &lr);
		//		fprintf(trace,"(RANK %d ) Color %d \t Local rank %d \t MPI_Comm_size %d\n",my_rank,y,lr,local);
		//		fflush(trace);
		//
		//		//		MPI_Barrier(MPI_COMM_WORLD);
		//
		//		MPI_Status	status;
		//		int rcvbuf_x = -999;
		//		int rcvbuf_y = -999;
		//		int rcvbuf_ptot = -999;
		//
		//		MPI_Request * reqx = (MPI_Request *) malloc(sizeof(MPI_Request));
		//		MPI_Request * reqy = (MPI_Request *) malloc(sizeof(MPI_Request));
		//
		//		//		MPI_Ibcast(&rcvbuf_x, 1, MPI_INT, 0, *comm_x,reqx);
		//		//		MPI_Ibcast(&rcvbuf_y, 1, MPI_INT, 0, *comm_y,reqy);
		//		//		MPI_Wait(reqx,&status);
		//		//		MPI_Wait(reqy,&status);
		//
		//		MPI_Bcast(&rcvbuf_x, 1, MPI_INT, MPI_ROOT, *comm_x);
		//		fprintf(trace,"(RANK %d) \t MPI_Bcast: rcvbuf_x = t_%d  \n",my_rank,rcvbuf_x);
		//		fflush(trace);
		//		MPI_Bcast(&rcvbuf_y, 1, MPI_INT, MPI_ROOT, *comm_y);
		//		fprintf(trace,"(RANK %d) \t MPI_Bcast: rcvbuf_y = t_%d  \n",my_rank,rcvbuf_y);
		//		fflush(trace);
		//
		//		fprintf(trace, "(RANK %d) \t Test x = %d \n", my_rank,rcvbuf_x);
		//		fprintf(trace, "(RANK %d) \t Test y = %d \n", my_rank,rcvbuf_y);
		//
	}



	fflush(trace);





	//	printf("----(RANK %d) \t THE END!!!---\n",my_rank);
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
