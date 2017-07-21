#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
char** names = NULL;
int indepCount = 0;

FILE* input = NULL;
Fl_Box** labels = NULL;
Fl_Input** minf = NULL;
double* min = NULL;
Fl_Input** maxf = NULL;
double* max = NULL;
Fl_Input** stepf = NULL;
double* step = NULL;
Fl_Window* win = NULL;
void createWindow();

//Callbacks
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
	names = (char**)calloc(indepCount, sizeof(char*));
	labels = (Fl_Box**)calloc(indepCount, sizeof(Fl_Box*));
	minf = (Fl_Input**)calloc(indepCount, sizeof(Fl_Input*));
	maxf = (Fl_Input**)calloc(indepCount, sizeof(Fl_Input*));
	stepf = (Fl_Input**)calloc(indepCount, sizeof(Fl_Input*));
	min = (double*)calloc(indepCount, sizeof(double));
	max = (double*)calloc(indepCount, sizeof(double));
	step = (double*)calloc(indepCount, sizeof(double));
	win->begin();
	for(int temp = 0; temp < indepCount; temp++){
		names[temp] = (char*)calloc(100, sizeof(char));
		fscanf(input, "%s", names[temp]);
		int row = 100+temp*(500/indepCount);//FIXME make 500 dependent on height -200
		int height = 500/indepCount;//FIXME
		labels[temp] = new Fl_Box(25, row, 100, height, names[temp]);
		minf[temp] = new Fl_Input(125, row, 100, height);
		maxf[temp] = new Fl_Input(225, row, 100, height);
		stepf[temp] = new Fl_Input(325, row, 100, height);
	}
	o->label("Load");
	win->redraw();
}

void cb_closeWindow(Fl_Widget* o, void* window){
	((Fl_Window*)window)->hide();
}

int main(int argc, char *argv[]){
	createWindow();
	return Fl::run();
}

void createWindow(){
	win = new Fl_Window(600, 700, "SoftFit v1.0");
	win->begin();
	Fl_File_Input* filename = new Fl_File_Input(75, 10, 500, 35, "Input File");
	Fl_Button* load = new Fl_Button(25, 45, 550, 35, "Load");
	new Fl_Box(25, 80, 100, 20, "NAME");
	new Fl_Box(125, 80, 100, 20, "MIN");
	new Fl_Box(225, 80, 100, 20, "MAX");
	new Fl_Box(325, 80, 100, 20, "STEP");
	win->end();
	load->callback(cb_load, filename);
	win->show();
}

