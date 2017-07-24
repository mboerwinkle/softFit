#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "insertionSort.h"
#include "calculations.h"
float completion = 0;
bool done = false;
double **data;
long int **best = NULL;
double *peak = NULL;
double *min, *max, *step;
double *actualValues;
int indepCount;
int records;
void* calculateMain(void *null){
	done = false;
	int bestIndex[indepCount];
	memset(bestIndex, 0, sizeof(int)*indepCount);
	peak = (double*)realloc(peak, sizeof(double)*indepCount);
	double test[indepCount];
	memcpy(test, min, sizeof(double)*indepCount);

	long int totalCalcs = 1;
	for(int x = 0; x < indepCount; x++){
		totalCalcs*=((max[x]-min[x])/step[x])+1;
	}
	printf("total Calcs %ld\n", totalCalcs);

	long int topscore = 9223372036854775807;
	long int calcs = 0;
	while(test[indepCount-1] <= max[indepCount-1]){
		long int thisScore = 0;
		calcs++;
		completion = (double)calcs/(double)totalCalcs;
		if(calcs %1000 == 0){
			pthread_testcancel();//this allows the cancel button to function
		}
		double indivScores[records];
		for(int thisRecord = 0; thisRecord < records; thisRecord++){
			double calculatedValue = 0;
			for(int temp = 0; temp < indepCount; temp++){
				calculatedValue+=test[temp]*data[thisRecord][temp];
			}
			indivScores[thisRecord] = pow(calculatedValue-actualValues[thisRecord],2)/100000;
		}
//		insertionSort(indivScores, records);//FIXME do not require on complete tests
		for(int scoreIdx = 0; scoreIdx < records*1; scoreIdx++){
			thisScore+=indivScores[scoreIdx];
		}
		for(int attr = 0; attr < indepCount; attr++){
			if(thisScore < best[attr][bestIndex[attr]]){
				best[attr][bestIndex[attr]] = thisScore;
			}
		}
		if(thisScore < topscore){
			topscore = thisScore;
			for(int attr = 0; attr < indepCount; attr++){
				peak[attr] = test[attr];
			}
		}
		test[0]+=step[0];
		bestIndex[0]++;
		for(int curr = 1; curr < indepCount; curr++){
			if(test[curr-1] > max[curr-1]){
				test[curr-1] = min[curr-1];
				bestIndex[curr-1] = 0;
				test[curr]+=step[curr];
				bestIndex[curr]++;
			}
		}
	}
	done = true;
	return 0;
}
