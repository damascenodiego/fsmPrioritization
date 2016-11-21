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

		//		// prioritizing test cases (lmdp)
		MPI_Comm * allTests 		= (MPI_Comm *) malloc(sizeof(MPI_Comm));
		struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		send_data->rank = my_rank;
		send_data->val = -1;

		std::list<FsmTestCase*> t_prtz;
		//				fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,ts.size()); fflush(trace);

		int pair2rm[2];
		for (int t_id = 0; t_id < noResets; t_id+=2)
		{
			if(ts.size() == 1) break;

			MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
			MPI_Bcast(&pair2rm,1,MPI_2INT,recv_data->rank,MPI_COMM_WORLD);

			int x_prtz = pair2rm[0];
			int y_prtz = pair2rm[1];
			//						fprintf(trace,"(RANK %d) \t highest ds(%d,%d)=%f received from %d\n",my_rank,x_prtz,y_prtz,recv_data->val,recv_data->rank); fflush(trace);

			if(ts.find(x_prtz)!=ts.end() && ts.find(y_prtz)!=ts.end()){
				t_prtz.push_back(ts[x_prtz]);
				t_prtz.push_back(ts[y_prtz]);
				ts.erase(x_prtz);
				ts.erase(y_prtz);
			}
			//						fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,ts.size()); fflush(trace);
		}
		if(ts.size() == 1){
			t_prtz.push_back(ts.begin()->second);
			ts.clear();
		}
		//				fprintf(trace,"(RANK %d) \t Total of test cases = %zu\n",my_rank,ts.size()); fflush(trace);
		fsmTest->getTestCase().clear();
		fsmTest->getTestCase().merge(t_prtz);

		pair2rm[0] = pair2rm[1] = -1;

		MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
		MPI_Bcast(&pair2rm,1,MPI_2INT,recv_data->rank,MPI_COMM_WORLD);


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
		strcat(prtz,".parall.lmdp.test");
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
		strcat(filename,".parall.trace");
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


		std::multimap<double,std::pair<int,int>> simPair;

		int keyPos_i,keyPos_f;
		keyPos_i = keyPos_f = (((noResets*(noResets-1))/2.0)/(num_proc-1));

		keyPos_i *= (my_rank-1);
		keyPos_f *= (my_rank);

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
					std::pair<int,int> p(i,j);
					simPair.insert(std::pair<double,std::pair<int,int>>(ds,p));
					//					fprintf(trace,"(RANK %d) \t calcSimpleSimilarity to test pair ds(%d,%d)=%f\n",my_rank,i,j,ds);
				}
				inc++;
			}
		}
		struct MPI_VAL_RANK *send_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));
		struct MPI_VAL_RANK *recv_data = (struct MPI_VAL_RANK *) malloc(sizeof(struct MPI_VAL_RANK));

		//		fprintf(trace,"(RANK %d) \t Total of SimpleSimilarity values = %zu\n",my_rank,simPair.size()); fflush(trace);

		int pair2rm[2];
		while(!simPair.empty()) {

			std::multimap<double,std::pair<int,int>>::reverse_iterator rit = simPair.rbegin();
			double max = rit->first;
			do {
				max = rit->first;
				rit++;
			}while(max <= rit->first && rit != simPair.rend());
			rit--;

			send_data->rank = recv_data->rank = my_rank;
			send_data->val  = recv_data->val = rit->first;

			pair2rm[0] = rit->second.first;
			pair2rm[1] = rit->second.second;

			//			fprintf(trace,"(RANK %d) \t My highest ds(%d,%d)=%f\n",my_rank,pair2rm[0],pair2rm[1],rit->first); fflush(trace);
			MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
			MPI_Bcast(&pair2rm,1,MPI_2INT,recv_data->rank,MPI_COMM_WORLD);

			if(pair2rm[0] < 0 && pair2rm[1] < 0) break;

			//			fprintf(trace,"(RANK %d) \t The highest ds(%d,%d)=%f was received from %d\n",my_rank,pair2rm[0],pair2rm[1],recv_data->val,recv_data->rank); fflush(trace);

			std::multimap<double,std::pair<int,int>>::iterator it = simPair.begin();
			while (it != simPair.end()) {
				if( 	it->second.first  == pair2rm[0] ||
						it->second.first  == pair2rm[1] ||
						it->second.second == pair2rm[0] ||
						it->second.second == pair2rm[1]
				){
					it = simPair.erase(it);
				}else{
					++it;
				}
			}
		}
		send_data->rank = recv_data->rank = my_rank;
		send_data->val  = recv_data->val = -1;

		while(1){
			if(pair2rm[0] < 0 && pair2rm[1] < 0) break;
			MPI_Allreduce(send_data, recv_data, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
			MPI_Bcast(&pair2rm,1,MPI_2INT,recv_data->rank,MPI_COMM_WORLD);
		}
		//		fprintf(trace,"(RANK %d) \t Total of SimpleSimilarity values = %zu\n",my_rank,simPair.size()); fflush(trace);
	}

	//	MPI_Barrier(MPI_COMM_WORLD);

	//	fprintf(trace,"----(RANK %02d) \t THE END!!!---\n",my_rank);
	//	fflush(trace);
	//	fclose(trace);
	MPI::Finalize();
	exit(0);

}


