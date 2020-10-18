/*
 * NN-Noxim - the NoC-based ANN Simulator
 *
 * (C) 2018 by National Sun Yat-sen University in Taiwan
 *
 * This file contains the implementation of loading NN model
 */


#ifndef __NNMODEL_H__
#define __NNMODEL_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>		//tytyty
#include <iostream>
#include <sstream>
#include <iterator> 
#include <algorithm> 
#include <stdexcept> 
#include <cassert>
#include <fstream>
#include "NoximMain.h"
using namespace std;

#define SOFTMAX                	0
#define RELU             	1
#define TANH              	2
#define SIGMOID                	3

// Structure used to store information into the table
struct NeuInformation {
	int ID_Neu;			// ID of the Neuron in software
    	int ID_layer;			// Layer Number of the Neuron
	char Type_layer;			// Type of the layer
	deque< float> weight;		// Weight of the Neu
	int ID_Group;			// ID of the Group Neuron
	int ID_In_Group;		// ID of the Neuron in the Group
	int ID_In_layer;		// ID of the Neuron in the layer
	//int ID_PE;			// ID of the PE
	int local_x;
	int local_y;
	int local_n;	
	int sta_x; 
	int end_x; 
	int sta_y; 
	int end_y; 
	int ID_conv;
};

/*struct Neu_table {
	deque < NeuInformation >	Neu_table
};*/

class NNModel {

  	public:
    		NNModel();

    	bool load();

	deque< char > all_leyer_type;
	deque< deque< int > > all_leyer_size;
	//deque< int > all_leyer_size_Group;
	deque< deque< int > > all_leyer_ID_Group;
	deque< deque< float > > all_data_in;
        deque < deque< NeuInformation > > Group_table;
	deque < int > mapping_table;
	deque< deque< float > > all_conv_weight;
        //deque < deque< NeuInformation > > PE_table;
        //deque < NeuInformation > Neu_table;
        //deque < NeuInformation > Neu_table_4G;
};

#endif
