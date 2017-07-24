#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Progress.H>
#include "calculations.h"

char** names = NULL;

FILE* input = NULL;
Fl_Box** labels = NULL;
Fl_Float_Input** minf = NULL;
Fl_Float_Input** maxf = NULL;
Fl_Float_Input** stepf = NULL;
Fl_Window* win = NULL;
void createWindow();


Fl_Button* calculate = NULL;
Fl_Button* cancel = NULL;
pthread_t calculationThread;
//Callbacks
Fl_Progress* bar;

void showResults();

void cb_updateTimer(void* null); 
void cb_updateTimer(void* null){
	bar->value(completion);
	if(done){
		bar->hide();
		cancel->hide();
		calculate->show();
		win->redraw();
		showResults();
		return;
	}
	Fl::repeat_timeout(0.5, cb_updateTimer);
}

void cb_cancelCalc(Fl_Widget* o, void* null){
	Fl::remove_timeout(cb_updateTimer);
	cancel->hide();
	bar->hide();
	win->redraw();
	pthread_cancel(calculationThread);
	pthread_join(calculationThread, nullptr);
	calculate->show();
	win->redraw();
}
void cb_calculate(Fl_Widget* o, void* field){
	o->hide();
	win->redraw();
	best = (long int**)calloc(indepCount, sizeof(long int*));
	for(int temp = 0; temp < indepCount; temp++){
		min[temp] = atof(minf[temp]->value());
		max[temp] = atof(maxf[temp]->value());
		step[temp] = atof(stepf[temp]->value());
		best[temp] = (long int*)calloc((max[temp]-min[temp])/step[temp]+1, sizeof(long int));
		for(int idx = 0; idx < (max[temp]-min[temp])/step[temp]+1; idx++){
			best[temp][idx] = 9223372036854775807;//FIXME nasty
		}
	}
	pthread_create(&calculationThread, NULL, calculateMain, NULL);
	bar->value(0);
	bar->show();
	cancel->show();
	Fl::add_timeout(0.5, cb_updateTimer);
	win->redraw();
}


void cb_load(Fl_Widget* o, void* field){
	if(input != NULL){
		fclose(input);
	}
	input = fopen(((Fl_File_Input*)field)->value(), "r");
	if(input == NULL){
		o->label("Failed to Load File");
		return;
	}
	o->label("File Loaded");
	calculate->show();
	for(int temp = 0; temp < indepCount; temp++){
		Fl::delete_widget(labels[temp]);
		Fl::delete_widget(minf[temp]);
		Fl::delete_widget(maxf[temp]);
		Fl::delete_widget(stepf[temp]);
		free(names[temp]);
	}
	free(names);
	free(min);
	free(max);
	free(step);
	free(minf);
	free(maxf);
	free(stepf);
	free(labels);
	fscanf(input, "%d", &indepCount);
	fscanf(input, "%d", &records);
	names = (char**)calloc(indepCount, sizeof(char*));
	labels = (Fl_Box**)calloc(indepCount, sizeof(Fl_Box*));
	minf = (Fl_Float_Input**)calloc(indepCount, sizeof(Fl_Float_Input*));
	maxf = (Fl_Float_Input**)calloc(indepCount, sizeof(Fl_Float_Input*));
	stepf = (Fl_Float_Input**)calloc(indepCount, sizeof(Fl_Float_Input*));
	min = (double*)calloc(indepCount, sizeof(double));
	max = (double*)calloc(indepCount, sizeof(double));
	step = (double*)calloc(indepCount, sizeof(double));
	win->begin();
	for(int temp = 0; temp < indepCount; temp++){
		names[temp] = (char*)calloc(100, sizeof(char));
		fscanf(input, "%s", names[temp]);
		int row = 100+temp*(570/indepCount);//FIXME make 500 dependent on height -200
		int height = 570/indepCount;//FIXME
		labels[temp] = new Fl_Box(25, row, 150, height, names[temp]);
		minf[temp] = new Fl_Float_Input(175, row, 150, height);
		maxf[temp] = new Fl_Float_Input(325, row, 150, height);
		stepf[temp] = new Fl_Float_Input(475, row, 150, height);
	}
	
	win->end();
	data = (double**)calloc(records, sizeof(double*));
	for(int temp = 0; temp < records; temp++){
		data[temp] = (double*)calloc(indepCount, sizeof(double));
		for(int field = 0; field < indepCount; field++){
			fscanf(input, "%lf", &(data[temp][field]));
		}
	}
	o->label("Load");
	win->redraw();
}

void cb_closeWindow(Fl_Widget* o, void* window){
	((Fl_Window*)window)->hide();
}

void reset(){
	calculate->show();
	win->redraw();
}

void showResults(){
	reset();
}

int main(int argc, char *argv[]){
	createWindow();
	return Fl::run();
}

void createWindow(){
	win = new Fl_Window(650, 700, "SoftFit v1.0");
	win->begin();
	Fl_File_Input* filename = new Fl_File_Input(75, 10, 500, 35, "Input File");
	Fl_Button* load = new Fl_Button(25, 45, 550, 35, "Load");
	calculate = new Fl_Button(550, 670, 95, 20, "Calculate");
	cancel = new Fl_Button(550, 670, 95, 20, "Cancel");
	new Fl_Box(25, 80, 100, 20, "NAME");
	new Fl_Box(175, 80, 100, 20, "MIN");
	new Fl_Box(325, 80, 100, 20, "MAX");
	new Fl_Box(475, 80, 100, 20, "STEP");
	bar = new Fl_Progress(10, 670, 540, 20, "Calculating");
	win->end();
	bar->minimum(0);
	bar->maximum(1);
	bar->hide();
	cancel->hide();
	calculate->hide();
	load->callback(cb_load, filename);
	calculate->callback(cb_calculate, NULL);
	cancel->callback(cb_cancelCalc, NULL);
	win->show();
}

