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


void print_comm_ranks(MPI_Comm comm, FILE*f)
{
	MPI_Group grp, world_grp;

	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


	MPI_Comm_group(MPI_COMM_WORLD, &world_grp);
	MPI_Comm_group(comm, &grp);

	int grp_size;

	MPI_Group_size(grp, &grp_size);

	int *ranks = (int*)malloc(grp_size * sizeof(int));
	int *world_ranks = (int*)malloc(grp_size * sizeof(int));

	for (int i = 0; i < grp_size; i++)
		ranks[i] = i;

	MPI_Group_translate_ranks(grp, grp_size, ranks, world_grp, world_ranks);

	for (int i = 0; i < grp_size; i++){
		if(f == nullptr){
			fprintf(stderr,"(RANK %d) \t comm[%d] has world rank %d\n", my_rank,i, world_ranks[i]);
		}else{
			fprintf(f,"(RANK %d) \t comm[%d] has world rank %d\n", my_rank, i, world_ranks[i]);
		}
	}


	free(ranks); free(world_ranks);

	MPI_Group_free(&grp);
	MPI_Group_free(&world_grp);
}

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

	//	MPI_Comm * x_color 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
	//	MPI_Comm * y_color 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
	//	//
	//	//
	//	out = MPI_Comm_split(MPI_COMM_WORLD, (x+1), my_rank, x_color);
	//	fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, row_color[%d]) = %s (%d)\n",my_rank,(x+1),my_rank,x,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
	//
	//	out = MPI_Comm_split(MPI_COMM_WORLD, (y+1)*10, my_rank, y_color);
	//	fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, col_color[%d]) = %s (%d)\n",my_rank,(y+1)*10,my_rank,y,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

	//	out = MPI_Comm_split(MPI_COMM_WORLD, (x+1), 0, x_color);
	//	fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, row_color[%d]) = %s (%d)\n",my_rank,(x+1),my_rank,x,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
	//
	//	out = MPI_Comm_split(MPI_COMM_WORLD, (y+1)*10, 0, y_color);
	//	fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, col_color[%d]) = %s (%d)\n",my_rank,(y+1)*10,my_rank,y,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

	MPI_Comm * row_color 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));
	MPI_Comm * col_color 		= (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));


	//	MPI_Comm * comm_0 = (MPI_Comm *) malloc(noResets*sizeof(MPI_Comm));
	//	for(int var = 0; var < noResets; var++){
	//		out = MPI_Comm_split(MPI_COMM_WORLD, (var+1), 00, &comm_0[var]);
	//		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, row_color[%d]) = %s (%d)\n",
	//				my_rank,(var+1),		00,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

	//		out = MPI_Comm_split(MPI_COMM_WORLD, (var+1)*10, 00, &comm_0[var]);
	//		fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %d, %d, col_color[%d]) = %s (%d)\n",
	//				my_rank,(var+1)*10,		00,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);

	//			for(int var2 = var+1; var2 < noResets; var2++){
	//				out = MPI_Comm_split(MPI_COMM_WORLD, (var+1), _rank, &row_color[var]);
	//				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),_rank,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
	//				out = MPI_Comm_split(MPI_COMM_WORLD, (var2+1)*10, _rank, &col_color[var2]);
	//				fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, col_color[%02d]) = %s (%d)\n",my_rank,(var2+1),_rank,var2,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
	//				_rank++;
	//			}
	//
	//}


	//	SimpleFsmTestCase *ti = nullptr;
	//	SimpleFsmTestCase *tj = nullptr;

	if (my_rank == 0) {
		FILE *fsmFile;
		FILE *testFile;
		//		FILE *testPrtzFile;

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
			MPI_Abort(MPI_COMM_WORLD,1);
		}

		// broadcast tests

		for (int var = 0; var < noResets-1; ++var) {
			out = MPI_Comm_split(MPI_COMM_WORLD, (var+1),    0, &row_color[var]);
			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}

		for (int var = 1; var < noResets; ++var) {
			out = MPI_Comm_split(MPI_COMM_WORLD, (var+1)*10,    0, &col_color[var]);
			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}
		MPI_Barrier(MPI_COMM_WORLD);

		for (int var = 0; var < noResets-1; ++var) {
			fprintf(trace,"(RANK %d) \t row_color[%d]\n",my_rank,var);
			print_comm_ranks(row_color[var],trace);
		}
		//
		for (int var = 1; var < noResets; ++var) {
			fprintf(trace,"(RANK %d) \t col_color[%d]\n",my_rank,var);
			print_comm_ranks(col_color[var],trace);
		}


		//			for(int var = 0; var < noResets-1; var++){
		//				for(int var2 = 1; var2 < noResets; var2++){
		//					out = MPI_Comm_split(MPI_COMM_WORLD, (var+1),    0, &row_color[var]);
		//					//			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//					out = MPI_Comm_split(MPI_COMM_WORLD, (var2+1)*10, 0, &col_color[var2]);
		//					//			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, col_color[%02d]) = %s (%d)\n",my_rank,(var2+1)*10,0,var2,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		//				}
		//			}
		//
		//			for(int var = 0; var < noResets-1; var++){
		//					fprintf(trace,"(RANK %d) \t row_color[%d]\n",my_rank,var);
		//					print_comm_ranks(row_color[var],trace);
		//				}
		//	for(int var2 = 1; var2 < noResets; var2++){
		//		fprintf(trace,"(RANK %d) \t col_color[%d]\n",my_rank,var2);
		//		print_comm_ranks(col_color[var2],trace);
		//	}
		//		for(int var = 0; var < noResets-1; var++){
		//			fprintf(trace,"(RANK %d) \t row_color[%d]\n",my_rank,var);
		//			print_comm_ranks(row_color[var],trace);
		//		}
		//		for(int var2 = 1; var2 < noResets; var2++){
		//			fprintf(trace,"(RANK %d) \t col_color[%d]\n",my_rank,var2);
		//			print_comm_ranks(col_color[var2],trace);
		//		}
		for(int var = 0; var < noResets-1; var++){
			int sendbuf = -100*(var+1);
			fprintf(trace,"(RANK %d) \t row_color[%d] = ",my_rank,var);
			MPI_Bcast(&sendbuf,1,MPI_INT,0,row_color[var]);
			fprintf(trace,"%d\n",sendbuf);
		}
		for(int var2 = 1; var2 < noResets; var2++){
			int sendbuf = -100*(var2+1);
			fprintf(trace,"(RANK %d) \t col_color[%d] = ",my_rank,var2);
			MPI_Bcast(&sendbuf,1,MPI_INT,0,col_color[var2]);
			fprintf(trace,"%d\n",sendbuf);
		}
		//		for (int id = 0; id < noResets; ++id) {
		//			int sendbuf = -100*(id+1);
		//			fprintf(trace,"(RANK %d) \t sendbuf[%d] = ",my_rank,id);
		//			MPI_Bcast(&sendbuf,1,MPI_INT,0,comm_0[id]);
		//			fprintf(trace,"%d\n",sendbuf);
		//		}
	}else{

		MPI_Status status;

		int rcv_x = -9999;
		int rcv_y = -9999;
		int color;

		for (int var = 0; var < noResets-1; ++var) {
			if(var != x){
				color = MPI_UNDEFINED;
			}else{
				color = (var+1) ;
			}
			out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank, &row_color[var]);
			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}

		for (int var = 1; var < noResets; ++var) {
			if(var != y){
				color = MPI_UNDEFINED;
			}else{
				color = (var+1)*10 ;
			}
			out = MPI_Comm_split(MPI_COMM_WORLD, color,    my_rank,  &col_color[var]);
			fprintf(trace,"(RANK %d) \t MPI_Comm_split(MPI_COMM_WORLD, %02d, %02d, row_color[%02d]) = %s (%d)\n",my_rank,(var+1),0,var,((out == MPI_SUCCESS)?"MPI_SUCCESS":"MPI_MPI_ERR"),out);
		}
		MPI_Barrier(MPI_COMM_WORLD);

		/*
		MPI_Request *reqx = (MPI_Request *) malloc(sizeof(MPI_Request));
		MPI_Request *reqy = (MPI_Request *) malloc(sizeof(MPI_Request));

		MPI_Ibcast(&rcv_x,1,MPI_INT,0,row_color[x],reqx);
		MPI_Ibcast(&rcv_y,1,MPI_INT,0,col_color[y],reqy);
		MPI_Wait(reqx,&status);
		MPI_Wait(reqy,&status);
		 */

		MPI_Bcast(&rcv_x,1,MPI_INT,0,row_color[x]);
		fprintf(trace,"(RANK %d) \t row_color[%d] = %d\n",my_rank,x,rcv_x); fflush(trace);

		MPI_Bcast(&rcv_y,1,MPI_INT,0,col_color[y]);
		fprintf(trace,"(RANK %d) \t col_color[%d] = %d\n",my_rank,y,rcv_y); fflush(trace);
	}


	//	MPI_Barrier(MPI_COMM_WORLD);
	//

	fflush(trace);
	printf("----(RANK %02d) \t THE END!!!---\n",my_rank);
	MPI::Finalize();
	exit(0);

}


