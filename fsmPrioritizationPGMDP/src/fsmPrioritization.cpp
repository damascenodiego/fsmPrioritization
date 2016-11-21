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
#include <vector>


using namespace std;

void printModel(FsmModel *fsmModel);
void printTest(FsmTestSuite *fsmTest);

int main(int argc, char **argv) {

	int	my_rank;
	int	num_proc;
	int	tag = 0;

	MPI::Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	//		char 	filename[46]; // used just when debugging
	//		FILE 	*trace;  // used just when debugging
	//
	//		sprintf(filename,"log/rank_%02d.trace", my_rank);
	//		trace = fopen(filename, "w");
	//
	//		fprintf(trace, "Hello fsmPrioritization! @ rank %d \n", my_rank);fflush(trace);
	//		fprintf(trace, "Total of processes: %d \n", num_proc);fflush(trace);
	//		fprintf(trace,"(RANK %d) \t noResets = %d\n",my_rank,noResets);fflush(trace);
	//		fprintf(trace,"(RANK %d) \t (t_%d,t_%d)\n",my_rank,x,y);fflush(trace);

	int out;
	int id_p[2];
	int noResets;

	if (my_rank == 0) {

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

		if(argc == 3){
			fsmFile = fopen(argv[1],"r");
			fsmModel = loadFsm(fsmFile);
			fclose(fsmFile);

			testFile = fopen(argv[2],"r");
			fsmTest = loadTest(testFile,fsmModel);
			fclose(testFile);

			if( num_proc < 2) {
				fprintf(stderr,"-np must be greater than 2 (USED: -np %d)\n",num_proc);
				MPI_Abort(MPI_COMM_WORLD,1);
			}
		}else{
			MPI_Abort(MPI_COMM_WORLD,1);
		}


		std::map<int,FsmTestCase*> ts;
		for(FsmTestCase* tc : fsmTest->getTestCase()){
			ts[tc->getId()]= tc;
		}

		// notify the number of test cases;
		noResets = ts.size();
		MPI_Bcast(&noResets,1,MPI_INT,0,MPI_COMM_WORLD);
		SimpleFsmTestCase *ttemp;
		for(int var = 0; var < noResets; var++){
			ttemp = ts[var]->getSimpleFormat();
			// sending test id
			MPI_Bcast(&(ttemp->testId),1,MPI_INT,0,MPI_COMM_WORLD);
			// sending pTot
			MPI_Bcast(&(ttemp->pTot),1,MPI_INT,0,MPI_COMM_WORLD);
			// sending p set
			MPI_Bcast(ttemp->p,ttemp->pTot,MPI_INT,0,MPI_COMM_WORLD);
			//			for (int var = 0; var < ttemp->pTot; ++var) {
			//				MPI_Bcast(&ttemp->p[var],1,MPI_INT,0,row_color[var]);
			//			}
		}

		// prioritizing test cases (gmdp)
		MPI_Status status;

		/////////////////////////////////////////
		// GET THE MOST DISTINCT PAIR OF TESTS //
		/////////////////////////////////////////
		struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		send_data->rank = my_rank;
		send_data->val = -1;
//						fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,ts.size()); fflush(trace);
		MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

		///////////////////////////////////
		// REMOVE THE MOST DISTINCT PAIR //
		///////////////////////////////////
		int pair2rm[2];
		MPI_Bcast(&pair2rm,1,MPI_2INT,recv_data->rank,MPI_COMM_WORLD);
		//		fprintf(trace,"(RANK %d) \t highest ds(%d,%d)=%f received from %d\n",my_rank,pair2rm[0],pair2rm[1],recv_data->val,recv_data->rank); fflush(trace);
		std::list<FsmTestCase*> t_prtz;
		if(ts.find(pair2rm[0])!=ts.end() && ts.find(pair2rm[1])!=ts.end()){
			t_prtz.push_back(ts[pair2rm[0]]);
			t_prtz.push_back(ts[pair2rm[1]]);
			ts.erase(pair2rm[0]);
			ts.erase(pair2rm[1]);
		}

		////////////////////////
		// UPDATE GMDP DS SUM //
		////////////////////////
		double *gmdp_arr = (double *) calloc(noResets,sizeof(double));
		int max_t;

		////////////////////////
		// UPDATE GMDP to t_0 //
		////////////////////////
		max_t = pair2rm[0];
		update_ds_sum(gmdp_arr,max_t,noResets,my_rank,num_proc);
		//for (int var = 0; var < noResets; ++var)  fprintf(stdout,"%f\t", gmdp_arr[var]); printf("\n"); fflush(stdout);
//		fprintf(trace,"(RANK %d) \t gmdp_arr: ",my_rank);
//		for (int var = 0; var < noResets; ++var) {
//			fprintf(trace,"%f\t",gmdp_arr[var]);
//		}
//		fprintf(trace,"\n"); fflush(trace);

		//		gmdp_arr[5] = -99;
		////////////////////////
		// UPDATE GMDP to t_1 //
		////////////////////////
		max_t = pair2rm[1];
		update_ds_sum(gmdp_arr,max_t,noResets,my_rank,num_proc);
//		fprintf(trace,"(RANK %d) \t Highest ds belongs to test cases t_%d (%f)\n",my_rank,max_t,recv_data->val); fflush(trace);
//		fprintf(trace,"(RANK %d) \t gmdp_arr: ",my_rank);
//		for (int var = 0; var < noResets; ++var) {
//			fprintf(trace,"%f\t",gmdp_arr[var]);
//		}
//		fprintf(trace,"\n"); fflush(trace);

		while(!ts.empty()) {
			max_t = getMaxDs(gmdp_arr,noResets);
			//fprintf(stdout,"(RANK %d) \t Highest ds t_%d = %f | Total of test cases = %zu\n",my_rank,max_t, gmdp_arr[max_t],ts.size()); fflush(stdout);
			//fprintf(trace,"(RANK %d) \t Highest ds belongs to test cases t_%d (%f)\n",my_rank,max_t,gmdp_arr[max_t]); fflush(trace);
//			fprintf(trace,"(RANK %d) \t gmdp_arr: ",my_rank);
//			for (int var = 0; var < noResets; ++var) {
//				fprintf(trace,"%f\t",gmdp_arr[var]);
//			}
//			fprintf(trace,"\n"); fflush(trace);
			pair2rm[0] = pair2rm[1] = max_t;
			MPI_Bcast(&pair2rm,1,MPI_2INT,0,MPI_COMM_WORLD);

			//for (int var = 0; var < noResets; ++var)  fprintf(stdout,"%f\t", gmdp_arr[var]); printf("\n"); fflush(stdout);			
			if(ts.size()== 1){
				gmdp_arr[max_t] = -1;
			}else{
				update_ds_sum(gmdp_arr,max_t,noResets,my_rank,num_proc);
			}
			//for (int var = 0; var < noResets; ++var)  fprintf(stdout,"%f\t", gmdp_arr[var]); printf("\n"); fflush(stdout);
//			fprintf(trace,"(RANK %d) \t gmdp_arr: ",my_rank);
//			for (int var = 0; var < noResets; ++var) {
//				fprintf(trace,"%f\t",gmdp_arr[var]);
//			}
//			fprintf(trace,"\n"); fflush(trace);

			if(ts.find(max_t)!=ts.end()){
				t_prtz.push_back(ts[max_t]);
				ts.erase(max_t);
				//fprintf(stdout,"(RANK %d) \t Test cases %d removed\n",my_rank,max_t); fflush(stdout);
			}

		}
		//fprintf(stdout,"(RANK %d) \t Total of test cases = %zu\n",my_rank,ts.size()); fflush(stdout);
		fsmTest->getTestCase().clear();
		fsmTest->getTestCase().merge(t_prtz);

		pair2rm[0] = pair2rm[1] = -1;

//		MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
		MPI_Bcast(&pair2rm,1,MPI_2INT,0,MPI_COMM_WORLD);


		time_t timer;
		char buffer[20];
		struct tm* tm_info;

		time(&timer);
		tm_info = localtime(&timer);

		strftime(buffer, 20, "%Y_%m_%d_%H_%M_%S", tm_info);

		char * prtz = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+40));
		prtz[0]='\0';
		strcat(prtz,argv[2]);
		//		strcat(prtz,".");
		//		strcat(prtz,buffer);
		strcat(prtz,".pgmdp.test");
		testPrtzFile = fopen(prtz,"w");
		if(testPrtzFile!=NULL)saveTest(testPrtzFile,fsmTest);
		fflush(testPrtzFile);
		fclose(testPrtzFile);
		//		strcat(prtz,".cov");
		//		FILE *testCoverageFile = fopen(prtz,"w");
		//		saveTestCoverage(testCoverageFile,fsmTest);
		//		fclose(testCoverageFile);


		delete(fsmModel);
		delete(fsmTest);
		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		clock_gettime(CLOCK_REALTIME, &stop);

		double diff = (double)((stop.tv_sec+stop.tv_nsec*1e-9) - (double)(start.tv_sec+start.tv_nsec*1e-9));

		char 	*filename = (char *)calloc(1,sizeof(char)*(strlen(argv[2])+40));; // used just when debugging
		FILE 	*trace;  // used just when debugging

		strcat(filename,argv[2]);
		//		strcat(filename,".");
		//		strcat(filename,buffer);
		strcat(filename,".pgmdp.trace");
		trace = fopen(filename, "a");

		fprintf(trace,"%s\t%s\t%s\t%lf\t%d\n",buffer,argv[2],prtz,(diff),num_proc); fflush(trace);
		fprintf(stdout,"%s\t%s\t%s\t%lf\t%d\n",buffer,argv[2],prtz,(diff),num_proc); fflush(stdout);
		fclose(trace);
		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
	}else{

		MPI_Bcast(&noResets,1,MPI_INT,0,MPI_COMM_WORLD);

		std::vector<SimpleFsmTestCase*> ts;

		// receives all test cases
		SimpleFsmTestCase *ti;
		for(int var = 0; var < noResets; var++){
			// allocate SimpleFsmTestCase for ti
			ti = (SimpleFsmTestCase*) malloc(sizeof(SimpleFsmTestCase));
			// sending test id
			MPI_Bcast(&(ti->testId),1,MPI_INT,0,MPI_COMM_WORLD);
			// sending pTot
			MPI_Bcast(&(ti->pTot),1,MPI_INT,0,MPI_COMM_WORLD);
			// sending p set
			ti->p = (int*) malloc(sizeof(int)*ti->pTot);
			MPI_Bcast(ti->p,ti->pTot,MPI_INT,0,MPI_COMM_WORLD);
			ts.push_back(ti);
		}


		std::map<int,std::map<int,double>*> pairSim;
		std::pair<int,int> max_ds_pair;
		double max_ds = -1;

		int keyPos_i,keyPos_f;
		keyPos_i = floor((((noResets*(noResets-1))/2.0)/(num_proc-1))* (my_rank-1));
		keyPos_f = trunc((((noResets*(noResets-1))/2.0)/(num_proc-1))* (my_rank));

		int inc=0;
		//		fprintf(trace,"(RANK %d) \t calcSimpleSimilarity calculated between positions [%d..%d)\n",my_rank,keyPos_i,keyPos_f);
		for (int i = 0; i < noResets-1; ++i) {
			if((inc+noResets-i-1) < keyPos_i){
				inc+=noResets-i-1; continue;
			}
			if(keyPos_f < inc) break;
			//			fprintf(trace,"(RANK %d) \t Iteration from %d - %d\n",my_rank,inc,((inc+noResets-i-1)));
			for (int j = i+1; j < noResets; ++j) {
				if(keyPos_i <= inc && inc < keyPos_f){
					double ds = calcSimpleSimilarity(ts[i],ts[j]);
					if(max_ds < ds){
						max_ds = ds;
						max_ds_pair.first  = i;
						max_ds_pair.second = j;
					}
					if(pairSim.find(i) == pairSim.end())  pairSim[i] = new std::map<int,double>();
					if(pairSim.find(j) == pairSim.end())  pairSim[j] = new std::map<int,double>();
					(*pairSim[i])[j] = ds;
					(*pairSim[j])[i] = ds;
//										fprintf(trace,"(RANK %d) \t calcSimpleSimilarity to test pair ds(%d,%d)=%f\n",my_rank,i,j,ds);
				}
				inc++;
			}
		}
		struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));

		//		fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,pairSim.size()); fflush(trace);

//		for(auto _ti: pairSim){
//			std::map<int,double>* m = _ti.second;
//			for(auto _tj: (*m)){
//								fprintf(trace,"t[%d][%d]=%f\n",_ti.first,_tj.first,_tj.second);
//			}
//		}

		//		fflush(trace);


		send_data->rank = recv_data->rank = my_rank;
		send_data->val  = recv_data->val = max_ds;

		int pair2rm[2];
		pair2rm[0] = max_ds_pair.first;
		pair2rm[1] = max_ds_pair.second;

		//		fprintf(trace,"(RANK %d) \t My highest ds(%d,%d)=%f\n",my_rank,pair2rm[0],pair2rm[1],max_ds); fflush(trace);
		MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
		MPI_Bcast(&pair2rm,1,MPI_2INT,recv_data->rank,MPI_COMM_WORLD);
		//		fprintf(trace,"(RANK %d) \t The highest ds(%d,%d)=%f\n",my_rank,pair2rm[0],pair2rm[1],max_ds); fflush(trace);


		struct MPI_VAL_RANK *tmp_arr = (struct MPI_VAL_RANK *) malloc(noResets*sizeof(struct MPI_VAL_RANK));

		int totDs;
		if(pairSim.find(pair2rm[0]) != pairSim.end()){
			totDs = pairSim[pair2rm[0]]->size();
		}else{
			totDs = 0;
		}
//				fprintf(trace,"(RANK %d) \t Number of ds to test %d = %d\n",my_rank,pair2rm[0],totDs); fflush(trace);
		MPI_Gather(&totDs,1,MPI_INT,&totDs,1,MPI_INT,0,MPI_COMM_WORLD);

		int count;
		if(totDs>0){
			count = 0;
			for(auto tds: (*(pairSim[pair2rm[0]]))){
				tmp_arr[count].rank = tds.first;
				tmp_arr[count].val  = tds.second;
				count++;
			}
			MPI_Send(tmp_arr,totDs,MPI_DOUBLE_INT,0,0,MPI_COMM_WORLD);
		}
		pairSim.erase(pair2rm[0]);

		if(pairSim.find(pair2rm[1]) != pairSim.end()){
			totDs = pairSim[pair2rm[1]]->size();
		}else{
			totDs = 0;
		}

		//		fprintf(trace,"(RANK %d) \t Number of ds to test %d = %d\n",my_rank,pair2rm[1],totDs); fflush(trace);
		MPI_Gather(&totDs,1,MPI_INT,&totDs,1,MPI_INT,0,MPI_COMM_WORLD);
		if(totDs>0){
			count = 0;
			for(auto tds: (*(pairSim[pair2rm[1]]))){
				tmp_arr[count].rank = tds.first;
				tmp_arr[count].val  = tds.second;
				count++;
			}
			MPI_Send(tmp_arr,totDs,MPI_DOUBLE_INT,0,0,MPI_COMM_WORLD);
		}
		pairSim.erase(pair2rm[1]);

		while(!(pairSim.empty())) {

			MPI_Bcast(&pair2rm,1,MPI_2INT,0,MPI_COMM_WORLD);
			//			fprintf(trace,"(RANK %d) \t The highest ds belongs to test %d\n",my_rank,pair2rm[0]); fflush(trace);

			if(pairSim.find(pair2rm[0]) != pairSim.end()){
				totDs = pairSim[pair2rm[0]]->size();
			}else{
				totDs = 0;
			}
			//			fprintf(trace,"(RANK %d) \t Number of ds to test %d = %d\n",my_rank,pair2rm[0],totDs); fflush(trace);
			MPI_Gather(&totDs,1,MPI_INT,&totDs,1,MPI_INT,0,MPI_COMM_WORLD);

			if(totDs>0){
				count = 0;
				for(auto tds: (*(pairSim[pair2rm[0]]))){
					tmp_arr[count].rank = tds.first;
					tmp_arr[count].val  = tds.second;
					count++;
				}
				MPI_Send(tmp_arr,totDs,MPI_DOUBLE_INT,0,0,MPI_COMM_WORLD);
			}
			pairSim.erase(pair2rm[0]);
			//			fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,pairSim.size()); fflush(trace);
		}
		send_data->rank = recv_data->rank = my_rank;
		send_data->val  = recv_data->val = -1;

		while(1){
//			MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
			MPI_Bcast(&pair2rm,1,MPI_2INT,0,MPI_COMM_WORLD);
			if(pair2rm[0] < 0 && pair2rm[1] < 0) break;
		}
		//		fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,pairSim.size()); fflush(trace);
	}

	//	MPI_Barrier(MPI_COMM_WORLD);

	//	fprintf(trace,"----(RANK %02d) \t THE END!!!---\n",my_rank);
	//	fflush(trace);
	//	fclose(trace);
	MPI::Finalize();
	exit(0);

}


