#include "fsmLib.h"

//char* readLine(FILE *f){
//	char *line = calloc(1024,sizeof(char));
//
//	char *c;
//	int count = -1;
//	while((c = fgetc(f)) != '\n' && !feof(f)){
//		line[++count] = c;
//	}
//	line[count+1] = '\0';
//	return line;
//}

FsmState* addState(FsmModel* m, int id){
	FsmState * stVar;
	int var;
	for (var = 0; var <= m->stateTot; ++var) {
		stVar = m->fsmState[var];
		if(stVar->id == id){
			return stVar;
		}

	}
	m->fsmState = incrementState(m);
	m->fsmState[m->stateTot] = createState(id);
	return m->fsmState[m->stateTot];
}



FsmTransition* addTransition(FsmModel* m, int f, int i, int o, int t){
	FsmTransition* frVar;
	FsmState* fr = addState(m,f);
	FsmState* to = addState(m,t);

	if(m->init == NULL) m->init = fr;

	addIn(m,i);
	addOut(m,o);

	int var;
	for (var = 0; var <= fr-> outTot; ++var) {
		frVar = fr->out[var];
		if(frVar->in == i){
			return frVar;
		}

	}
	m->fsmTr = incrementTr(m);
	frVar = createTransition(i,o);
	addTrFrom(frVar,fr);
	addTrTo  (frVar,to);
	m->fsmTr[m->trTot] = frVar;
	return frVar;
}

int addIn(FsmModel* m, int id){
	int var;
	for (var = 0; var <= m->inTot; ++var) {
		if(m->fsmIn[var] == id){
			return m->fsmIn[var];
		}
	}
	m->inTot++;
	m->fsmIn = (int**)realloc(m->fsmIn,(m->inTot+1)*sizeof(int*));
	m->fsmIn[m->inTot] = id;
	return m->fsmIn[m->inTot];
}

int addOut(FsmModel* m, int id){
	int var;
	for (var = 0; var <= m->outTot; ++var) {
		if(m->fsmOut[var] == id){
			return m->fsmOut[var];
		}

	}
	m->outTot++;
	m->fsmOut = (int**)realloc(m->fsmOut,(m->outTot+1)*sizeof(int*));
	m->fsmOut[m->outTot] = id;
	return m->fsmOut[m->outTot];
}

FsmModel* loadFsm(FILE* f){
	rewind(f); //rewind file reader

	int to;
	int in;
	int out;
	int from;
	FsmModel *fsm = createFsm();
	//while((c = readLine(f)) && !feof(f)){
	while(!feof(f)){
		fscanf(f,"%d -- %d / %d -> %d",&from,&in,&out,&to);
		//		fscanf(f,"%d",&from);fscanf(f,"%s");
		//		fscanf(f,"%d",&in);fscanf(f,"%s");
		//		fscanf(f,"%d",&out);fscanf(f,"%s");
		//		fscanf(f,"%d",&to);
		//printf("---\n");
		//printf("FROM: %d\n IN: %d\n OUT: %d\n TO: %d\n",from,in,out,to);
		addTransition(fsm,from,in,out,to);
		//fgets(c,sizeof(c),f); printf("LINE: %s\n",c);
		//addStateTo(fsm,c); addIn(fsm,c); addOut(fsm,c); addStateFrom(fsm,c);
	}

	return fsm;
}

FsmTestSuite* loadTest(FILE* f){
	char line[1024];
	rewind(f); //rewind file reader

	FsmTestSuite* ts = createTestSuite();
	while(!feof(f)){
		fgets(line,1024,f);
		//printf("\n'%s'",line);
		FsmTestCase *test = addTestCase(ts,line);
		ts->noResets++;
		ts->testCase = realloc(ts->testCase,(ts->noResets+1)*sizeof(FsmTestCase*));
		ts->testCase[ts->noResets-1] = test;
	}
	ts->noResets --;

	return ts;
}

FsmModel* createFsm(void){
	FsmModel *fsm = (FsmModel *)calloc(1,sizeof(FsmModel));
	fsm->fsmIn = NULL;
	fsm->fsmOut = NULL;
	fsm->fsmState = NULL;
	fsm->fsmTr = NULL;
	fsm->init = NULL;

	fsm->inTot=-1;
	fsm->outTot=-1;
	fsm->stateTot=-1;
	fsm->trTot=-1;
	return fsm;
}

FsmTestSuite* createTestSuite(void){
	FsmTestSuite *ts = (FsmTestSuite *)calloc(1,sizeof(FsmTestSuite));
	ts->testCase = (FsmTestCase**)calloc(1,sizeof(FsmTestCase*));
	ts->avgLength = 0 ;
	ts->length = 0 ;
	ts->noResets = 0 ;
	ts->p = (FsmTransition**)calloc(1,sizeof(FsmTransition*));
	ts->q = (FsmState**)calloc(1,sizeof(FsmState*));
	ts->qTot = -1 ;
	ts->pTot = -1 ;
	return ts;
}

FsmTestCase* createTestCase(void){
	FsmTestCase *tc = (FsmTestCase *)calloc(1,sizeof(FsmTestCase));
	tc->input = (int*)calloc(1,sizeof(int*));
	tc->output = (int*)calloc(1,sizeof(int*));
	tc->p = (FsmTransition**)calloc(1,sizeof(FsmTransition*));
	tc->q = (FsmState**)calloc(1,sizeof(FsmState*));
	tc->length = 0 ;
	tc->qTot = -1 ;
	tc->pTot = -1 ;
	return tc;
}


FsmState* createState(int id){
	FsmState *out= (FsmModel *)calloc(1,sizeof(FsmState));
	out->in  = NULL;
	out->out = NULL;
	out->inTot  = -1;
	out->outTot = -1;
	out->id = id;
	return out;
}


FsmTransition* createTransition(int i, int o){
	FsmTransition *out= (FsmTransition *)calloc(1,sizeof(FsmTransition));
	out->in  = i;
	out->out = o;
	return out;
}



FsmState** incrementState(FsmModel * m){
	m->stateTot++;
	int sz = sizeof(FsmState*);
	if(m->fsmState == NULL) {

		m->fsmState = (FsmState**) malloc(sz);
	}
	else {
		m->fsmState = (FsmState**)realloc(m->fsmState,sz*(m->stateTot+1));
	}

	return m->fsmState;
}


FsmTransition** incrementTr(FsmModel * m){
	m->trTot++;
	int sz = sizeof(FsmTransition*);
	if(m->fsmTr == NULL) {

		m->fsmTr = (FsmTransition**)malloc(sz);
	}
	else {
		m->fsmTr = (FsmTransition**)realloc(m->fsmTr,sz*(m->trTot+1));
	}


	return m->fsmTr;
}


void addTrFrom(FsmTransition* tr, FsmState * s){
	int sz = sizeof(FsmTransition*);
	s->outTot++;
	tr->fr = s;
	if(s->out == NULL) {
		s->out = (FsmTransition**) malloc(sz);
	}
	else {
		s->out = (FsmTransition**)realloc(s->out,sz*(s->outTot+1));
	}
	s->out[s->outTot] = tr;
	return;
}

void addTrTo(FsmTransition* tr, FsmState * s){
	int sz = sizeof(FsmTransition*);
	s->inTot++;
	tr->to = s;
	if(s->in== NULL) {
		s->in= (FsmTransition**) malloc(sz);
	}
	else {
		s->in = (FsmTransition**)realloc(s->in,sz*(s->inTot+1));
	}
	s->in[s->inTot] = tr;
}

void printFsm(FsmModel* m){
	int var;
	printf("FSM model:");

	printf("\n\tInput (%d):",m->inTot+1);
	for (var = 0; var <= m->inTot; ++var) {
		printf("\n\t\t%d ",m->fsmIn[var]);
	}
	printf("\n\tOutput (%d):",m->outTot+1);
	for (var = 0; var <= m->outTot; ++var) {
		printf("\n\t\t%d ",m->fsmOut[var]);
	}
	printf("\n\tStates (%d):",m->stateTot+1);
	for (var = 0; var <= m->stateTot; ++var) {
		printf("\n\t\t%d ",m->fsmState[var]->id);
	}
	printf("\n\tTransitions (%d):",m->trTot+1);
	for (var = 0; var <= m->trTot; ++var) {
		printf("\n\t\t%d -- %d / %d -> %d"
				,m->fsmTr[var]->fr->id
				,m->fsmTr[var]->in
				,m->fsmTr[var]->out
				,m->fsmTr[var]->to->id
		);
	}
}

void printTestSuite(FsmModel* m,FsmTestSuite* ts){
	int var,var2,var3;
	printf("\n\nTest Suite: (q%:%f | p:%f)",
			(((double)ts->qTot+1)/(m->stateTot+1)),
			(((double)ts->pTot+1)/(m->trTot+1))
			);
	for (var = 0; var < ts->noResets; ++var) {
		printf("\nTestCase %d:",var);
		FsmTestCase *tc = ts->testCase[var];

		int tcl = tc->length;
		for (var2 = 0; var2 < tcl; ++var2) {
			printf("\t%d",tc->input[var2]);
		}
		printf("\t\t\t\nStates covered (%f):\t",(((double)tc->qTot+1)/(m->stateTot+1)));
//		printf("\t\t\t\nStates covered:\t");
		for (var3 = 0; var3 <= tc->qTot; ++var3) {
			printf("\t%d",tc->q[var3]->id);
		}
		printf("\t\t\t\nTransitions covered (%f):\t",(((double)tc->pTot+1)/(m->trTot+1)));
//		printf("\t\t\t\nTransitions covered:\t");
		for (var3 = 0; var3 <= tc->pTot; ++var3) {
			printf("\n\t\t\t\t\t%d -- %d / %d -> %d"
					,tc->p[var3]->fr->id
					,tc->p[var3]->in
					,tc->p[var3]->out
					,tc->p[var3]->to->id
			);
		}
	}
	printf("\nCummulative q:\t");
	for (var = 0; var < ts->noResets; ++var)  printf("%f\t",ts->cummq[var]);

	printf("\nCummulative p:\t");
	for (var = 0; var < ts->noResets; ++var)  printf("%f\t",ts->cummp[var]);

}

FsmTestCase* addTestCase(FsmTestSuite *t,char *line){
	int slen = strlen(line)-1;
	int var;
	long l;
	//printf("\n");
	FsmTestCase * tc = createTestCase();
	int count = 1;
	for (var = 0; var < slen; var+=3) {
		l = 0;
		l += (line[var]   - '0')*100;
		l += (line[var+1] - '0')*10;
		l += (line[var+2] - '0')*1;
		//printf("%d\t",l);
		tc->input[count-1] = l;
		tc->input = realloc(tc->input	,(count+1)*sizeof(int));
		tc->output = realloc(tc->output	,(count+1)*sizeof(int));
		count++;
	}
	tc->length = count-1;
	return tc;
}

void evaluateCoverage(FsmModel *model, FsmTestSuite *ts){
	int var,var2;
	FsmTransition *tr   = NULL;
	FsmState      *curr = NULL;
	ts->cummq = calloc(ts->noResets,sizeof(double));
	ts->cummp = calloc(ts->noResets,sizeof(double));

	for (var = 0; var < ts->noResets; ++var) {
		curr = model->init;
		//printf("\n");
		FsmTestCase *tc = ts->testCase[var];
		int tcl = tc->length;
		for (var2 = 0; var2 < tcl; ++var2) {
//			printf("\t%d",tc->input[var2]);
			tr = nextTransition(curr,tc->input[var2]);
			addStateCoveredTC(tc,tr->fr);
			addStateCoveredTC(tc,tr->to);
			addTransitionCoveredTC(tc,tr);

			addStateCoveredTS(ts,tr->fr);
			addStateCoveredTS(ts,tr->to);
			addTransitionCoveredTS(ts,tr);

			curr = tr->to;

		}
		ts->cummq[var] = (((double)ts->qTot+1)/(model->stateTot+1));
		ts->cummp[var] = (((double)ts->pTot+1)/(model->trTot+1));
	}
}

FsmTransition * nextTransition(FsmState* s0, int input){
	int var;
	FsmTransition* out=NULL;
	for (var = 0; var <= s0->outTot; ++var) {
		out = s0->out[var];
		if(out->in == input){
			return out;
		}
	}
	return NULL;
}

void addTransitionCoveredTC(FsmTestCase* tc, FsmTransition* tr){
	int var;
	for (var = 0; var <= tc->pTot; ++var) {
		if(tc->p[var] == tr){
			return;
		}
	}
	tc->pTot++;
	tc->p = (FsmTransition**)realloc(tc->p,(sizeof(FsmTransition*)*(tc->pTot+1)));
	tc->p[tc->pTot] = tr;
}

void addStateCoveredTC(FsmTestCase* tc, FsmState* s){
	int var;
	for (var = 0; var <= tc->qTot; ++var) {
		if(tc->q[var] == s){
			return;
		}
	}
	tc->qTot++;
	tc->q = (FsmState**)realloc(tc->q,(sizeof(FsmState*)*(tc->qTot+1)));
	tc->q[tc->qTot] = s;
}

void addTransitionCoveredTS(FsmTestSuite* ts, FsmTransition* tr){
	int var;
	for (var = 0; var <= ts->pTot; ++var) {
		if(ts->p[var] == tr){
			return;
		}
	}
	ts->pTot++;
	ts->p = (FsmTransition**)realloc(ts->p,(sizeof(FsmTransition*)*(ts->pTot+1)));
	ts->p[ts->pTot] = tr;
}

void addStateCoveredTS(FsmTestSuite* ts, FsmState* s){
	int var;
	for (var = 0; var <= ts->qTot; ++var) {
		if(ts->q[var] == s){
			return;
		}
	}
	ts->qTot++;
	ts->q = (FsmState**)realloc(ts->q,(sizeof(FsmState*)*(ts->qTot+1)));
	ts->q[ts->qTot] = s;
}
