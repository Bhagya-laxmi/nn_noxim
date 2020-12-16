/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */
/*
 * NN-Noxim - the NoC-based ANN Simulator
 *
 * (C) 2018 by National Sun Yat-sen University in Taiwan
 *
 * This file contains the implementation of loading NN model
 */

#ifndef __NOXIMPROCESSINGELEMENT_H__
#define __NOXIMPROCESSINGELEMENT_H__
#define _USE_MATH_DEFINES	//tytyty
#include <queue>
#include <deque>		//tytyty
#include <systemc.h>
#include <cmath>
#include <cassert>
#include "NoximMain.h"
#include "NoximGlobalTrafficTable.h"
#include "NNModel.h"		//tytyty
using namespace std;

SC_MODULE(NoximProcessingElement)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the PE
    sc_in < bool > reset;	// The reset signal for the PE

    sc_in < NoximFlit > flit_rx;	// The input channel
    sc_in < bool > req_rx;	// The request associated with the input channel
    sc_out < bool > ack_rx;	// The outgoing ack signal associated with the input channel

    sc_out < NoximFlit > flit_tx;	// The output channel
    sc_out < bool > req_tx;	// The request associated with the output channel
    sc_in < bool > ack_tx;	// The outgoing ack signal associated with the output channel
	/***MODIFY BY HUI-SHUN***/
	sc_out <int >free_slots;
	/***MODIFY BY HUI-SHUN***/
    sc_in < int >free_slots_neighbor;

    // Registers
    int local_id;		// Unique identification number
    bool current_level_rx;	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx;	// Current level for Alternating Bit Protocol (ABP)
    queue < NoximPacket > packet_queue;	// Local queue of packets
    bool transmittedAtPreviousCycle;	// Used for distributions with memory

//********************************************************************************************
	bool PE_enable;
	bool flag_p;
	bool flag_f;
	int ID_layer;
	char Type_layer;
	int ID_group;
	int in_data;
	int Use_Neu;
	deque<int> Use_Neu_ID;
	int receive;
	int should_receive;
	deque<int> receive_Neu_ID;
	deque<float> receive_data;
	int trans;
	deque<int> trans_PE_ID;
	int should_trans;
	deque<deque< float> > my_data_in;
	deque< float> res;
	deque< deque< float> > PE_Weight;
	deque< NeuInformation > PE_table;
	int computation_time;
	int temp_computation_time;

	/*Convolution and Pooling layers*/
	//deque<int> coord_xyz;
	deque<int> trans_PE_ID_conv;
	deque<deque<int>> trans_PE_ID_pool;
	deque <deque<int>> receive_neu_ID_conv;
	deque <deque<int>> receive_neu_ID_pool;
	deque <int> trans_conv;
	deque <int> trans_pool;
	deque <int> receive_conv;
	deque <int> receive_pool;
	int start_index;
	int curr_XYXrouting; //0: YX routing; 1: XY routing-----------Intermittent XY routing
	bool flag_complete;
	deque<int> Neu_complete;

	deque<int> curr_trans_pe_id;
	//deque <int> packet_size;
	//deque <deque<int>> curr_src_neu_id;
	//deque<deque<float>> curr_data;
//*********************************************************************************************
	
	// Functions
    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process  
//**********tytyty********************************************
	bool frequcnce(); 
float fixed_sim(double long d);
//**********^^^^^^************************************************
	bool canShot(NoximPacket & packet);	// True when the packet must be shot
    /***MODIFY BY HUI-SHUN***/
	NoximPacket trafficRandom_Tvar();
	/***MODIFY BY HUI-SHUN***/
	//NoximFlit nextFlit();	// Take the next flit of the current packet
	  NoximFlit nextFlit(const int ID_layer, const int in_data);	// Take the next flit of the current packet
  //NoximFlit nextFlit(const int ID_layer);	// Take the next flit of the current packet
    NoximPacket trafficRandom();	// Random destination distribution
    NoximPacket trafficTranspose1();	// Transpose 1 destination distribution
    NoximPacket trafficTranspose2();	// Transpose 2 destination distribution
    NoximPacket trafficBitReversal();	// Bit-reversal destination distribution
    NoximPacket trafficShuffle();	// Shuffle destination distribution
    NoximPacket trafficButterfly();	// Butterfly destination distribution
	
	
	int transmit;//Matthew
	int not_transmit;//Matthew
	int adaptive_transmit;
	int dor_transmit;
	int dw_transmit;
	bool emergency;
	int emergency_level;
	double Q_ratio;
	bool throttle_local;	
	int cnt_local;	
	int	Quota_local;
	bool clean_all;
	
    NoximGlobalTrafficTable *traffic_table;	// Reference to the Global traffic Table
    NNModel *NN_Model;				//tytyty
    bool never_transmit;	// true if the PE does not transmit any packet 
    //  (valid only for the table based traffic)

    void fixRanges(const NoximCoord, NoximCoord &);	// Fix the ranges of the destination
    int randInt(int min, int max);	// Extracts a random integer number between min and max
    int getRandomSize();	// Returns a random size in flits for the packet
    void setBit(int &x, int w, int v);
    int getBit(int x, int w);
    double log2ceil(double x);

	void								 TraffThrottlingProcess();
	
    // Constructor
    SC_CTOR(NoximProcessingElement) {
		//cout<< "PE executing"<<endl;
	/*SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();
	
	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();*/

	SC_METHOD(TraffThrottlingProcess);
    sensitive << reset;
    sensitive << clock.pos();
	
    }

};

#endif
