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
double keep = 1;
double power = 2;
long int **best = NULL;
double *min, *max, *step;
double *actualValues;
int indepCount;
int records;
double calcScore(double* test);
void printInput(){
	printf("keep %lf, power %lf\n", keep, power);
	for(int x = 0; x < records; x++){
		printf("%lf ", actualValues[x]);
		for(int temp = 0; temp < indepCount; temp++){
			printf("%lf ", data[x][temp]);
		}
		printf("\n");
	}
}

void* calculateMain(void *null){
	done = false;
	printInput();
	int bestIndex[indepCount];
	memset(bestIndex, 0, sizeof(int)*indepCount);
	double test[indepCount];
	memcpy(test, min, sizeof(double)*indepCount);

	long int totalCalcs = 1;
	for(int x = 0; x < indepCount; x++){
		totalCalcs*=((max[x]-min[x])/step[x])+1;
	}
	printf("total Calcs %ld\n", totalCalcs);
	long int calcs = 0;
	while(test[indepCount-1] <= max[indepCount-1]){
		calcs++;
		completion = (double)calcs/(double)totalCalcs;
		if(calcs %10000 == 0){
			pthread_testcancel();//this allows the cancel button to function
		}
		double thisScore = calcScore(test);
		for(int attr = 0; attr < indepCount; attr++){
			if(thisScore < best[attr][bestIndex[attr]]){
				best[attr][bestIndex[attr]] = thisScore;
			}
		}
		double adjTest[indepCount];
		memcpy(adjTest, test, sizeof(double)*indepCount);
		bool isPeak = true;
		for(int attr = 0; attr < indepCount; attr++){
			//plus
			adjTest[attr]+=step[attr];
			if(calcScore(adjTest)<thisScore){
				isPeak = false;
				break;
			}
			//neg
			adjTest[attr]-=2*step[attr];
			if(calcScore(adjTest)<thisScore){
				isPeak = false;
				break;
			}
			//return to same
			adjTest[attr] = test[attr];
		}
		if(isPeak){
			printf("peak: ");
			for(int attr = 0; attr < indepCount; attr++){
				printf("%s %lf ", names[attr], test[attr]);
			}
			printf("\n");
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
double calcScore(double* test){
		double indivScores[records];
		for(int thisRecord = 0; thisRecord < records; thisRecord++){
			double calculatedValue = 0;
			for(int temp = 0; temp < indepCount; temp++){
				calculatedValue+=test[temp]*data[thisRecord][temp];
			}
			indivScores[thisRecord] = pow(abs(calculatedValue-actualValues[thisRecord]),power);
		}
		insertionSort(indivScores, records);
		double thisScore = 0;
		for(int scoreIdx = 0; scoreIdx < records*keep; scoreIdx++){
			thisScore+=indivScores[scoreIdx];
		}
		return thisScore;
}
