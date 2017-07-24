#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void insertionSort(double* list, int len){//A really bad sort
	double output[len];
	for(int pass = 0; pass < len; pass++){
		int minidx = 0;
		for(int idx = 1; idx < len-pass; idx++){
			if(list[idx] < list[minidx]){
				minidx = idx;
			}
		}
		output[pass] = list[minidx];
		list[minidx] = list[len-pass-1];
	}
	memcpy(list, output, sizeof(double)*len);
}
