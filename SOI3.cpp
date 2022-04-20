#include "monitor.h"
#include <cstdlib>
#include <vector>
#include <time.h>
#include <pthread.h>
#include <iostream>

using namespace std;


vector<int> bufferFIFO;
Semaphore mutex(1), prodEvenSem(0), prodOddSem(0), consEvenSem(0), consOddSem(0);
unsigned int numOfProdEvenWaiting = 0, numOfProdOddWaiting = 0, numOfConsEvenWaiting = 0, numOfConsOddWaiting = 0;
int nrEven = 0, nrOdd = 0;

// buffer printing

void printBuffer(){
	for(int i = 0; i < bufferFIFO.size(); i++){
		cout << bufferFIFO[i] << " ";
	}
	cout << endl << endl;
}

// checkers

// checkers - even numbers

bool canPutEven(){
	return nrEven < 10;
}

bool canPopEven(){
	return (bufferFIFO.size() >= 3) && (bufferFIFO[0] % 2 == 0);
}

// checkers - odd numbers

bool canPutOdd(){
	return nrEven > nrOdd;
}

bool canPopOdd(){
	return (bufferFIFO.size() >= 7) && (bufferFIFO[0] % 2 == 1);
}

// manipulators

// manipulators - even numbers

void putEven(int evenNum){
	mutex.p();
	
	if(!canPutEven()){
		numOfProdEvenWaiting ++;
		mutex.v();
		prodEvenSem.p();
	}
	
	printf("Appending even number: [%d]\n", evenNum);
	bufferFIFO.push_back(evenNum);
	nrEven ++;
	printBuffer();
	
	if(numOfProdOddWaiting > 0 && canPutOdd()){
		numOfProdOddWaiting --;
		prodOddSem.v();
	}
	else if(numOfConsEvenWaiting > 0 && canPopEven()){
		numOfConsEvenWaiting --;
		consEvenSem.v();
	}
	else if(numOfConsOddWaiting > 0 && canPopOdd()){
		numOfConsOddWaiting --;
		consOddSem.v();
	}
	else mutex.v();
}

void popEven(){
	mutex.p();
	
	if(!canPopEven()){
		numOfConsEvenWaiting ++;
		mutex.v();
		consEvenSem.p();
	}
	
	printf("Popping even number: [%d]\n", bufferFIFO[0]);
	bufferFIFO.erase(bufferFIFO.begin());
	nrEven --;
	printBuffer();
	
	if(numOfProdEvenWaiting > 0 && canPutEven()){
		numOfProdEvenWaiting --;
		prodEvenSem.v();
	}
	else if(numOfProdOddWaiting > 0 && canPutOdd()){
		numOfProdOddWaiting --;
		prodOddSem.v();
	}
	else if(numOfConsOddWaiting > 0 && canPopOdd()){
		numOfConsOddWaiting --;
		consOddSem.v();
	}
	else mutex.v();
}

// manipulators - odd numbers

void putOdd(int oddNum){
	mutex.p();
	
	if(!canPutOdd()){
		numOfProdOddWaiting ++;
		mutex.v();
		prodOddSem.p();
	}
	
	printf("Appending odd number: [%d]\n", oddNum);
	bufferFIFO.push_back(oddNum);
	nrOdd ++;
	printBuffer();
	
	if(numOfProdEvenWaiting > 0 && canPutEven()){
		numOfProdEvenWaiting --;
		prodEvenSem.v();
	}
	else if(numOfConsEvenWaiting > 0 && canPopEven()){
		numOfConsEvenWaiting --;
		consEvenSem.v();
	}
	else if(numOfConsOddWaiting > 0 && canPopOdd()){
		numOfConsOddWaiting --;
		consOddSem.v();
	}
	else mutex.v();
}

void popOdd(){
	mutex.p();
	
	if(!canPopOdd()){
		numOfConsOddWaiting ++;
		mutex.v();
		consOddSem.p();
	}
	
	printf("Popping odd number: [%d]\n", bufferFIFO[0]);
	bufferFIFO.erase(bufferFIFO.begin());
	nrOdd --;
	printBuffer();
	
	if(numOfProdEvenWaiting > 0 && canPutEven()){
		numOfProdEvenWaiting --;
		prodEvenSem.v();
	}
	else if(numOfProdOddWaiting > 0 && canPutOdd()){
		numOfProdOddWaiting --;
		prodOddSem.v();
	}
	else if(numOfConsEvenWaiting > 0 && canPopEven()){
		numOfConsEvenWaiting --;
		consEvenSem.v();
	}
	else mutex.v();
}

// producers and consumers

// producers and consumers - even numbers

void* evenProducer(void *threadid){
	while(1){
		int evenNum = (rand() % 25) * 2;
		putEven(evenNum);
		sleep(1 + rand() % 2);
	}
}

void* evenConsumer(void *threadid){
	while(1){
		popEven();
		sleep(1 + rand() % 2);
	}
}

// producers and consumers - odd numbers

void* oddProducer(void *threadid){
	while(1){
		int oddNum = (rand() % 25) * 2 + 1;
		putOdd(oddNum);
		sleep(1 + rand() % 2);
	}
}

void* oddConsumer(void *threadid){
	while(1){
		popOdd();
		sleep(1 + rand() % 2);
	}
}

// main

int main(){
	srand(time(NULL));

   	pthread_t threads[4];
   	int rc;

   	printf("Stworzono wątek even Producer\n");
	rc = pthread_create(&threads[0], NULL, evenProducer, (void *)0);

	printf("Stworzono wątek odd Producer\n");
	rc = pthread_create(&threads[1], NULL, oddProducer, (void *)1);

	printf("Stworzono wątek even Consumer\n");
	rc = pthread_create(&threads[2], NULL, evenConsumer, (void *)2);

	printf("Stworzono wątek odd Consumer\n");
	rc = pthread_create(&threads[3], NULL, oddConsumer, (void *)3);

    	pthread_exit(NULL);
    	return 0;
}
