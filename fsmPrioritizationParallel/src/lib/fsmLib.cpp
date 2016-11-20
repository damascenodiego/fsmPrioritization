/*
 * fsmLib.cpp
 *
 *  Created on: 2 de nov de 2016
 *      Author: damasceno
 */

#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <algorithm>
#include "fsmLib.h"

FsmModel * loadFsm(FILE* f){
	FsmModel *fsm = new FsmModel();
	rewind(f); //rewind file reader
	//printf("FsmModel @ %p\n",(*fsm));
	int to;
	int in;
	int out;
	int from;
	std::map<int,FsmState*> stMap;
	std::set<int> inSet,outSet;
	FsmState *f_st,*t_st;
	FsmTransition *tr;
	int id = 0;
	while(!feof(f)){
		fscanf(f,"%d -- %d / %d -> %d",&from,&in,&out,&to);
		//printf("---\n");
		//printf("FROM: %d\n IN: %d\n OUT: %d\n TO: %d\n",from,in,out,to);
		std::map<int,FsmState*>::iterator it = stMap.find(from);
		if(it == stMap.end()){
			f_st = new FsmState(from);
			stMap[from] =f_st;
			//			printf("address: %p %p\n",stMap[from],f_st);
			(*fsm).addState(*f_st);
		}else{
			f_st = stMap[from];
		}
		it = stMap.find(to);
		if(it == stMap.end()){
			t_st = new FsmState(to);
			stMap[to] =t_st;
			(*fsm).addState(*t_st);
		}else{
			t_st = stMap[to];
		}
		if(inSet.insert(in).second){
			(*fsm).getIn().push_back(in);
		}
		if(outSet.insert(out).second){
			(*fsm).getOut().push_back(out);
		}
		if((*fsm).getInitState() == nullptr){
			(*fsm).setInitState(stMap[from]);
		}
		tr = new FsmTransition (id,f_st,in,out,t_st);
		//		(*tr).print();
		//		(*f_st).print();
		//		(*t_st).print();
		(*fsm).addTransition(*tr);
		id++;
	}
	stMap.clear();
	inSet.clear();
	outSet.clear();
	return fsm;
}

FsmTransition * nextTransition(FsmState* s0, int input){
	for (auto i : s0->getOut()) {
		if((i.second)->getIn() == input){
			return i.second;
		}
	}
	return nullptr;
}

void evaluateCoverage(FsmModel *model, FsmTestCase *tc){
	FsmTransition *tr   = nullptr;
	FsmState      *curr = nullptr;

	curr = model->getInitState();
	std::set<FsmTransition*> trSet;
	for (int i : tc->getInput()) {
		tr = nextTransition(curr,i);
		if(trSet.insert(tr).second){
			tc->getP().push_back(tr);
		}
		curr = tr->getTo();
	}
	trSet.clear();
}

FsmTestSuite* loadTest(FILE* f,FsmModel *m = nullptr){
	char line[1024];
	//rewind(f); //rewind file reader

	FsmTestSuite* ts = new FsmTestSuite();
	int slen = 0;
	int id = -1;
	while(!feof(f)){
		fgets(line,1024,f);
		slen = strlen(line)-1;
		if(slen < 3) continue;
		//		line[slen] = '\0';
		//		printf("'%s'\n",line); fflush(stdout);
		int var;
		long l;
		FsmTestCase * tc = new FsmTestCase(++id);
		for (var = 0; var < slen; var+=3) {
			l = 0;
			l += (line[var]   - '0')*100;
			l += (line[var+1] - '0')*10;
			l += (line[var+2] - '0')*1;
			//			printf("%d\t",l);
			(*tc).getInput().push_back(l);
		}
		ts->getTestCase().push_back(tc);
		if(m != nullptr) evaluateCoverage(m,tc);
		ts->setLength(ts->getLength()+tc->getLength());
		line[0] = '\0';
	}
	ts->setAvgLength(((double)ts->getLength())/ts->getNoResets());
	if(m != nullptr) ts->setModel(m);
	return ts;
}

void saveTest(FILE* f,FsmTestSuite* ts){
	rewind(f);
	//int count = 0;
	for(FsmTestCase *it : ts->getTestCase()){
		for(int i : (it)->getInput()){
			fprintf(f,"%03d",i);
		}
		fprintf(f,"\n");
//		fflush(f);
//		it++;
	}
}

void saveTestCoverage(FILE* f,FsmTestSuite* ts){
	std::set<FsmTransition*> trSet;
	std::set<FsmState*> stSet;
	fprintf(f,"Transition Coverage (%lu states)\t",ts->getModel()->getTransition().size());
	fprintf(f,"State Coverage (%lu states)\n",ts->getModel()->getState().size());
	for(FsmTestCase *it : ts->getTestCase()){
		for(FsmTransition *tr : it->getP()){
			trSet.insert(tr);
			stSet.insert(tr->getTo());
			stSet.insert(tr->getFrom());
		}
		fprintf(f,"%lu\t%lu\n",trSet.size(),stSet.size());
	}
}

void printSimpleFormat(SimpleFsmTestCase * sf){
	if(sf != nullptr){
		printf("Test Case #%d:" \
				"\n\tLength: %d" \
				"\n\tpTot: %d"  \
				"\n\tp:",sf->testId,sf->testLength,sf->pTot);
		for(int i = 0; i <  sf->pTot; i++) {
			printf("\t%d",sf->p[i]);
		}
		printf("\n");
	}
}

double calcSimpleSimilarity(FsmTestCase *t0,FsmTestCase *t1){
	if(t0 == nullptr || t1 == nullptr) return -1;
	std::set<int> t0Tr;
	std::set<int> t1Tr;

	int t0len = t0->getLength();
	int t1len = t1->getLength();

	for(FsmTransition * t : t0->getP()) t0Tr.insert(t->getId());
	for(FsmTransition * t : t1->getP()) t1Tr.insert(t->getId());

	int ndt = 0;

	for(int t : t0Tr) {
		if(t1Tr.find(t) == t1Tr.end())  ndt++;
	}

	for(int t : t1Tr) {
		if(t0Tr.find(t) == t0Tr.end())  ndt++;
	}

	t0Tr.clear();
	t1Tr.clear();

	//printf("ndt = %d\n",ndt);
	double ds = ndt / ((t0len+t1len)/2.0);
	return ds;

}

double calcSimpleSimilarity(SimpleFsmTestCase *t0, SimpleFsmTestCase *t1){
	std::set<int> t0Tr;
	std::set<int> t1Tr;
	std::set<int> diff;

	int t0len = (*t0).testLength;
	int t1len = (*t1).testLength;

	for (int var = 0; var < (*t0).pTot; ++var) {
		t0Tr.insert((*t0).p[var]);
	}

	for (int var = 0; var < (*t1).pTot; ++var) {
		t1Tr.insert((*t1).p[var]);
	}

	//	int ndt = 0;
	//
	//	for(int t : t0Tr) {
	//		if(t1Tr.find(t) == t1Tr.end())  ndt++;
	//	}
	//
	//	for(int t : t1Tr) {
	//		if(t0Tr.find(t) == t0Tr.end())  ndt++;
	//	}

	//printf("ndt = %d\n",ndt);
	//double ds = ndt / ((t0len+t1len)/2.0);

	set_difference(t0Tr.begin(), t0Tr.end(), t1Tr.begin(), t1Tr.end(), inserter(diff, diff.begin()));
	double ds = diff.size() / ((t0Tr.size()+t1Tr.size())/2.0);

	t0Tr.clear();
	t1Tr.clear();
	diff.clear();

	return ds;

}


void prioritization_lmdp(FsmTestSuite* ts){
	std:: list<FsmTestCase*> tcs;
	std:: list<FsmTestCase*> t;
	for(FsmTestCase *i : ts->getTestCase()){
		t.push_back(i);
	}
	//	printf("t.size() = %d\n", t.size());
	std::list<FsmTestCase*>::iterator endi;
	std::list<FsmTestCase*>::iterator endj;

	std::list<FsmTestCase*>::iterator 	max_ti;
	std::list<FsmTestCase*>::iterator 	max_tj;
	double 								max_ds = -1;
	double 	tmp_ds;
	while (t.size()>0){
		if(t.size()>1){
			max_ds = -1;
			endi = t.end(); endi--;
			endj = t.end();
			for(auto ti = t.begin(); ti != endi; ti++){
				auto tj = ti;
				for(tj++; tj != endj; tj++){
					tmp_ds = calcSimpleSimilarity(*ti,*tj);
					if(tmp_ds > max_ds){
						max_ds = tmp_ds;
						max_ti = ti;
						max_tj = tj;
					}
				}
			}
			//			printf("t.size() = %d\n", t.size());
			//			printf("tcs.size() = %d\n", tcs->size());
			tcs.push_back(*max_ti);
			tcs.push_back(*max_tj);
			t.erase(max_ti);
			t.erase(max_tj);
			//			printf("\tcalcSimpleSimilarity(t[%d],t[%d]) = %f \n", (*max_ti)->getId(),(*max_tj)->getId(),max_ds);
			//			printf("t.size() = %d\n", t.size());
			//			printf("tcs.size() = %d\n", tcs.size());
		}else{
			tcs.push_back(*t.begin());
			t.erase(t.begin());
			//			printf("t.size() = %d\n", t.size());
			//			printf("tcs.size() = %d\n", tcs.size());
		}

	}
	ts->getTestCase().clear();
	ts->getTestCase().merge(tcs);
	//	for(FsmTestCase *t : tcs) {
	////		printf("\tt[%d]\n", (t)->getId());
	//		ts->getTestCase().pop_front();
	//		ts->getTestCase().push_back(t);
	//	}
}

void prioritization_gmdp(FsmTestSuite* ts){
	std:: list<FsmTestCase*> tcs;
	std:: list<FsmTestCase*> t;
	for(FsmTestCase *i : ts->getTestCase()){
		t.push_back(i);
	}
	std::list<FsmTestCase*>::iterator endi = t.end(); endi--;
	std::list<FsmTestCase*>::iterator endj = t.end();;

	double 								max_ds = -1;
	std::list<FsmTestCase*>::iterator 	it;
	std::list<FsmTestCase*>::iterator 	max_ti;
	std::list<FsmTestCase*>::iterator 	max_tj;

	double 	tmp_ds;
	for(auto ti = t.begin(); ti != endi; ti++){
		auto tj = ti;
		for(tj++; tj != endj; tj++){
			tmp_ds = calcSimpleSimilarity(*ti,*tj);
			if(tmp_ds > max_ds){
				max_ds = tmp_ds;
				max_ti = ti;
				max_tj = tj;
			}
		}
	}
	tcs.push_back(*max_ti);
	tcs.push_back(*max_tj);

	//	(*max_ti)->print(); (*max_tj)->print();

	double *ds_sum = (double *)calloc(ts->getNoResets(),sizeof(double));
	//for (int var = 0; var < ts->getNoResets(); ++var)  printf("%f\t", ds_sum[var]); printf("\n"); fflush(stdout);
	update_ds_sum(t,*max_ti,ds_sum);
	//for (int var = 0; var < ts->getNoResets(); ++var)  printf("%f\t", ds_sum[var]); printf("\n"); fflush(stdout);
	update_ds_sum(t,*max_tj,ds_sum);
	//for (int var = 0; var < ts->getNoResets(); ++var)  printf("%f\t", ds_sum[var]); printf("\n"); fflush(stdout);

	t.erase(max_ti);
	t.erase(max_tj);
	while (t.size()>0){
		max_ds = -1;
		for(auto ti = t.begin(); ti != t.end(); ti++){
			tmp_ds = ds_sum[(*ti)->getId()];
			if(tmp_ds > max_ds) {
				max_ds = tmp_ds;
				max_ti = ti;
			}
		}
		tcs.push_back(*max_ti);
		update_ds_sum(t,*max_ti,ds_sum);
		//for (int var = 0; var < ts->getNoResets(); ++var)  printf("%f\t", ds_sum[var]); printf("\n"); fflush(stdout);
		t.erase(max_ti);
	}
	for(FsmTestCase *t : tcs) {
		ts->getTestCase().pop_front();
		ts->getTestCase().push_back(t);
	}
}


void update_ds_sum(std::list<FsmTestCase*> &ts, FsmTestCase* tc,double* ds_sum){
	ds_sum[tc->getId()] = -1;
	for(FsmTestCase * t : ts){
		if(ds_sum[t->getId()] >= 0.0)ds_sum[t->getId()] += calcSimpleSimilarity(t,tc);
	}
}

int toTriangMatrix(int xpos,int ypos,int noReset){
	//	fprintf(stderr,"(%d,%d)\n",(xpos<ypos)? xpos : ypos,(xpos>ypos)? xpos : ypos);
	int tmPos = 0;
	int i = (xpos<ypos)? xpos : ypos;
	int j = ((xpos>ypos)? xpos : ypos)-i;
	int lTot = noReset-1;
	//	fprintf(stderr,"(%d,%d) = %d\n",(xpos<ypos)? xpos : ypos,(xpos>ypos)? xpos : ypos,tmPos);

	while(i > 0){
		tmPos+= lTot;
		//		fprintf(stderr,"(%d,%d) = %d\n",(xpos<ypos)? xpos : ypos,(xpos>ypos)? xpos : ypos,tmPos);
		lTot--;
		i--;
	}
	tmPos+= j;
	return tmPos;
}

// code below was taken from
// http://stackoverflow.com/questions/14690481/how-to-know-the-all-the-ranks-of-the-processor-that-are-part-of-a-communicator-i
void print_comm_ranks(MPI_Comm comm, FILE*f) {
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

void calculateXY(int &x,int &y, int noResets, int my_rank){
	int tmp = noResets;
	x = 0;
	y = my_rank;
	tmp--;
	while ((y) > tmp ){
		(x)++;
		(y) -= tmp;
		tmp--;
	}
	(y)  += (x) ;
}
