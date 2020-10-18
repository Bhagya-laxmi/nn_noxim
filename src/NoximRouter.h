/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the router
 */

#ifndef __NOXIMROUTER_H__
#define __NOXIMROUTER_H__

#include <systemc.h>
#include "NoximMain.h"
#include "NoximBuffer.h"
#include "NoximStats.h"
#include "NoximGlobalRoutingTable.h"
#include "NoximLocalRoutingTable.h"
#include "NoximReservationTable.h"
using namespace std;

extern unsigned int drained_volume;

SC_MODULE(NoximRouter)
{

    // I/O Ports
    sc_in_clk clock;		                  // The input clock for the router
    sc_in <bool> reset;                           // The reset signal for the router

    sc_in <NoximFlit> flit_rx[DIRECTIONS + 1];	  // The input channels (including local one)
    sc_in <bool> req_rx[DIRECTIONS + 1];	  // The requests associated with the input channels
    sc_out <bool> ack_rx[DIRECTIONS + 1];	  // The outgoing ack signals associated with the input channels

    sc_out <NoximFlit> flit_tx[DIRECTIONS + 1];   // The output channels (including local one)
    sc_out <bool> req_tx[DIRECTIONS + 1];	  // The requests associated with the output channels
    sc_in <bool> ack_tx[DIRECTIONS + 1];	  // The outgoing ack signals associated with the output channels

    sc_out <int> free_slots[DIRECTIONS + 1];
    sc_in <int> free_slots_neighbor[DIRECTIONS + 1];
	/*******THROTTLING******/
	sc_out<bool>	 on_off[4];				  //告知鄰近router自己是否有被throttle neighbor
	sc_in<bool>		 on_off_neighbor[4];	  //判斷鄰近router是否被throttle neighbor, 來決定是否繞開
	/*******THROTTLING******/
	/*******DOWNWARD ROUTING******/
	sc_out<int>		 DW_tag[4];				  //根據此pillar的traffic情況所決定出可以使用的DW level
	sc_in<int>		 DW_tag_neighbor[4]; 	  //鄰近router可以使用的DW level
	/*******DOWNWARD ROUTING******/
    /*******RCA******/
	sc_out<double>	 monitor_out[DIRECTIONS];
	sc_in<double>	 monitor_in[DIRECTIONS];
	/*******RCA******/
	// Neighbor-on-Path related I/O
    sc_out < NoximNoP_data > NoP_data_out[DIRECTIONS];
    sc_in < NoximNoP_data > NoP_data_in[DIRECTIONS];

    // Registers

    /*
       NoximCoord position;                     // Router position inside the mesh
     */
    int local_id;		                // Unique ID
    int routing_type;		                // Type of routing algorithm
    int selection_type;
    NoximBuffer buffer[DIRECTIONS + 1];	        // Buffer for each input channel 
    bool current_level_rx[DIRECTIONS + 1];	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx[DIRECTIONS + 1];	// Current level for Alternating Bit Protocol (ABP)
    NoximStats stats;		                // Statistics
    NoximLocalRoutingTable routing_table;	// Routing table
    NoximReservationTable reservation_table;	// Switch reservation table
    int start_from_port;	                // Port from which to start the reservation cycle
    unsigned long routed_flits;
	/*******THROTTLING******/
	bool 	emergency;									// emergency mode
	int 	emergency_level;							// emergency level
	double	Q_ratio;									// traffic quota ratio
	bool	throttle_neighbor;							// throttle port from neighbor routers
	/*******THROTTLING******/
	/*******DOWNWARD ROUTING******/
	int 	Quota_neighbor;
	int		incre_;										//increse when temperature > last_temperature
	int		DW_tag_cur;	                                //DW_tag of current router
	/*******DOWNWARD ROUTING******/
	/*******RCA******/
	double	RCA_select[DIRECTIONS];                     //For RCA selection and next propagation
	/*******RCA******/
	/****UNKNOWN****/
	int		cnt_neighbor;								// counter for packets from neighbor routers		
	int		cnt_received;
	int		thermal_counter[DIRECTIONS];
	bool	UnavailableTemp;
	int		rx_count;
	double  buffer_util;
	double  buffer_used;
	/****UNKNOWN****/
    // Functions

    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process
    void bufferMonitor();
    void configure(const int _id, const double _warm_up_time,
		   const unsigned int _max_buffer_size,
		   NoximGlobalRoutingTable & grt);

    unsigned long getRoutedFlits();	// Returns the number of routed flits 
    unsigned int getFlitsCount();	// Returns the number of flits into the router
    double getPower();		        // Returns the total power dissipated by the router
	
	/*******THROTTLING******/
	void TraffThrottlingProcess();
	/*******THROTTLING******/
    // Constructor

    SC_CTOR(NoximRouter) {
	SC_METHOD(rxProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(txProcess);
	sensitive << reset;
	sensitive << clock.pos();

	SC_METHOD(bufferMonitor);
	sensitive << reset;
	sensitive << clock.pos();
	/*******RCA******/
	SC_METHOD(RCA_Aggregation);
	sensitive << reset;
	sensitive << clock.pos();
    /*******RCA******/
	/*******THROTTLING******/
    SC_METHOD(TraffThrottlingProcess);
    sensitive << reset;
    sensitive << clock.pos();
	/*******THROTTLING******/
    }

  private:

    // performs actual routing + selection
    int route(const NoximRouteData & route_data);

    // wrappers
    int selectionFunction(const vector <int> &directions,
			  const NoximRouteData & route_data);
    vector < int >routingFunction(const NoximRouteData & route_data);

    // selection strategies
    int selectionRandom(const vector <int> & directions);
    int selectionBufferLevel(const vector <int> & directions);
    int selectionNoP(const vector <int> & directions,
		     const NoximRouteData & route_data);
	/*******RCA******/
	int RCA_selection(const vector<int>& directions, const NoximRouteData& route_data);
	/*******RCA******/
    // routing functions
	//****************tytyty*******************************//
	vector<int> routingXYX(const NoximCoord& current, const NoximCoord& destination, const bool XYX_routing);
	/***ACCESS IC LAB's Routing Algorithm***/
	vector<int> routingXYZ(const NoximCoord& current, const NoximCoord& destination);
	vector<int> routingZXY(const NoximCoord& current, const NoximCoord& destination);
	/***Using DW_tag***/
	vector<int> routingDownward(const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source);
	vector<int> routingOddEven_Downward (const NoximCoord& current, const NoximCoord& source, const NoximCoord& destination, const NoximRouteData& route_data); //Foster modified
	/***Using DW_tag***/
	vector<int> routingOddEven_for_3D (const NoximCoord& current, const NoximCoord& source, const NoximCoord& destination); //Foster modified
	vector<int> routingOddEven_3D (const NoximCoord& current, const NoximCoord& source, const NoximCoord& destination); //Foster modified
	
	vector<int> routingOddEven_Z (const NoximCoord& current, const NoximCoord& source, const NoximCoord& destination);
	vector<int> routingProposed(const NoximCoord& current, const NoximCoord& source, const NoximCoord& destination);
	/***ACCESS IC LAB's Routing Algorithm***/
	vector < int >routingWestFirst(const NoximCoord & current,const NoximCoord & destination);
    vector < int >routingNorthLast(const NoximCoord & current,const NoximCoord & destination);
    vector < int >routingNegativeFirst(const NoximCoord & current,const NoximCoord & destination);
    vector < int >routingOddEven(const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
    vector < int >routingDyAD(const NoximCoord & current,const NoximCoord & source,const NoximCoord & destination);
    vector < int >routingLookAhead(const NoximCoord & current,const NoximCoord & destination);
    vector < int >routingFullyAdaptive(const NoximCoord & current,const NoximCoord & destination);
    vector < int >routingTableBased(const int dir_in,const NoximCoord & current,const NoximCoord & destination);
	/***Under Transport Layer Assisted Routing Scheme***/
	vector<int> routingCrossLayer(const int select_routing, const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source);  //Matthew
	vector<int> routingDownward_CrossLayer(const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source); //Matthew
	vector<int> routingCrossLayer_HS(const int select_routing, const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source); //Hui-shun
	vector<int> routingDownward_CrossLayer_HS(const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source); //Hui-shun
	/***Under Transport Layer Assisted Routing Scheme***/
    NoximNoP_data getCurrentNoPData() const;
    void NoP_report() const;
    int NoPScore(const NoximNoP_data & nop_data, const vector <int> & nop_channels) const;
    int reflexDirection(int direction) const;
    int getNeighborId(int _id, int direction) const;
    bool inCongestion();
	/*******DOWNWARD ROUTING******/
	int getNeighborId_downward(int _id, int direction, int dst_z) const;
	/*******DOWNWARD ROUTING******/
	/*******RCA******/
	void RCA_Aggregation();
	/*******RCA******/
  public:

    unsigned int local_drained;

};

#endif
