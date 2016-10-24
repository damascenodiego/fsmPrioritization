/*
 * fsmLib.h
 *
 *  Created on: 21 de out de 2016
 *      Author: damasceno
 */

#ifndef LIB_FSMLIB_H_
#define LIB_FSMLIB_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct _FsmState{
	int id;
	struct _FsmTransition** in;
	struct _FsmTransition** out;
	int inTot;
	int outTot;
} FsmState;

typedef struct _FsmTransition{
	struct _FsmState* fr;
	int in;
	int out;
	struct _FsmState* to;
} FsmTransition;


typedef struct _FsmModel{
	struct _FsmState** fsmState;
	struct _FsmTransition** fsmTr;
	struct _FsmState* init;
	int** fsmIn;
	int** fsmOut;

	int stateTot;
	int trTot;
	int inTot;
	int outTot;

} FsmModel;


typedef struct _FsmTestSuite{
	struct _FsmTestCase **testCase;
	int noResets;
	int length;
	double avgLength;
	struct _FsmState** q;
	struct _FsmTransition** p;
	int qTot;
	int pTot;
	double *cummq;
	double *cummp;

} FsmTestSuite;

typedef struct _FsmTestCase{
	int* input;
	int* output;
	struct _FsmState** q;
	struct _FsmTransition** p;
	int length;
	int qTot;
	int pTot;
} FsmTestCase;



char* readLine(FILE *f);
FsmModel* loadFsm(FILE* f);
FsmTestSuite* loadTest(FILE* f);
FsmState* addState(FsmModel* m, int id);
FsmModel* createFsm(void);
FsmTestSuite* createTestSuite(void);
FsmTestCase* createTestCase(void);
FsmState* createState(int id);
FsmTransition* createTransition(int i, int o);
FsmState** incrementState(FsmModel * m);
FsmTransition** incrementTr(FsmModel * m);
FsmTestCase* addTestCase(FsmTestSuite *t,char *line);
void evaluateCoverage(FsmModel *model, FsmTestSuite *test);
FsmTransition * nextTransition(FsmState* s0, int input);
void addTransitionCoveredTC(FsmTestCase* tc, FsmTransition* tr);
void addStateCoveredTC(FsmTestCase* tc, FsmState* s);
void addTransitionCoveredTS(FsmTestSuite* ts, FsmTransition* tr);
void addStateCoveredTS(FsmTestSuite* ts, FsmState* s);

#endif /* LIB_FSMLIB_H_ */
