# fsmPrioritization Project


This repository contains the source code of four softwares which perform test prioritization of test suites given an FSM.
The four programs are:
- [fsmPrioritizationLMDP](https://github.com/damascenodiego/fsmPrioritization/tree/master/fsmPrioritizationLMDP): This software performs **L**ocal **M**aximum **D**istance **P**rioritization (*LMDP*).
- [fsmPrioritizationGMDP](https://github.com/damascenodiego/fsmPrioritization/tree/master/fsmPrioritizationGMDP): This software performs **G**lobal **M**aximum **D**istance **P**rioritization (*GMDP*).
- [fsmPrioritizationPLMDP](https://github.com/damascenodiego/fsmPrioritization/tree/master/fsmPrioritizationPLMDP): This software is a parallel implementation of the *LMDP* algorithm
- [fsmPrioritizationPGMDP](https://github.com/damascenodiego/fsmPrioritization/tree/master/fsmPrioritizationPGMDP): This software is a parallel implementation of the *GMDP* algorithm

The [build.sh](https://github.com/damascenodiego/fsmPrioritization/blob/master/build.sh) script generates the executable version of each algorithm.

The [conformanceTest.sh](https://github.com/damascenodiego/fsmPrioritization/blob/master/conformanceTest.sh) script runs all four algorithms over random test suites to show the conformance between the serial and parallel versions of the LMDP and GMDP algorithms.


This project was developed in the context of the course of 
[SSC5795 - Concurrent Programming](https://uspdigital.usp.br/janus/componente/catalogoDisciplinasInicial.jsf?action=3&sgldis=SSC5795) at the 
University of São Paulo [(USP)](http://usp.br/) in the Institute of Mathematical and Computer Sciences [(ICMC-USP)](icmc.usp.br/).
