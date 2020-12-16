/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file represents the top-level testbench
 */

#ifndef __NOXIMNOC_H__
#define __NOXIMNOC_H__

#include <systemc.h>
#include "NoximTile.h"
#include "NoximGlobalRoutingTable.h"
#include "NoximGlobalTrafficTable.h"
#include "NNModel.h"	//tytyty
#include "thermal_IF.h"
using namespace std;

extern ofstream results_log_pwr;

SC_MODULE(NoximNoC)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the NoC
    sc_in < bool > reset;	// The reset signal for the NoC

    // Signals
	/****************MODIFY BY HUI-SHUN********************/
    //Hui-shun add the third dim in the end
	//and add up/down signals
	sc_signal <bool> req_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> req_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
	sc_signal <bool> req_to_up[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1]; 
	sc_signal <bool> req_to_down[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	
    sc_signal <bool> ack_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <bool> ack_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
	sc_signal <bool> ack_to_up[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1]; 
	sc_signal <bool> ack_to_down[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	
    sc_signal <NoximFlit> flit_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM+1];
    sc_signal <NoximFlit> flit_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM+1];
    sc_signal <NoximFlit> flit_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM+1];
    sc_signal <NoximFlit> flit_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM+1];
	sc_signal <NoximFlit> flit_to_up[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1]; ////
    sc_signal <NoximFlit> flit_to_down[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];////

    sc_signal <int> free_slots_to_east[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_west[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_south[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
    sc_signal <int> free_slots_to_north[MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1][MAX_STATIC_DIM + 1];
	sc_signal <int> free_slots_to_up[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1]; ////
	sc_signal <int> free_slots_to_down[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];////
    // NoP (�n�T�{�n���n�[�@)
    sc_signal <NoximNoP_data> NoP_data_to_east[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_west[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_south[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];
    sc_signal <NoximNoP_data> NoP_data_to_north[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];
	sc_signal <NoximNoP_data> NoP_data_to_up[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];////
    sc_signal <NoximNoP_data> NoP_data_to_down[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];////
    /*******RCA******/
	// RCA Monitor Network
	sc_signal<double>    RCA_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<double>    RCA_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<double>    RCA_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<double>    RCA_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<double>    RCA_to_up[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];////
	sc_signal<double>    RCA_to_down[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];////
	
	/*******RCA******/
	/*******THROTTLING******/
	sc_signal<bool>	on_off_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<bool>	on_off_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<bool>	on_off_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<bool>	on_off_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	/*******THROTTLING******/
	/*******DOWNWARD ROUTING******/
	sc_signal<int>	DW_tag_to_east[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<int>	DW_tag_to_west[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<int>	DW_tag_to_south[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	sc_signal<int>	DW_tag_to_north[MAX_STATIC_DIM+1][MAX_STATIC_DIM+1][MAX_STATIC_DIM+1];
	/*******DOWNWARD ROUTING******/
	// Matrix of tiles
    NoximTile *t[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM];
	/****************MODIFY BY HUI-SHUN********************/
    // Global tables
    NoximGlobalRoutingTable grtable;
    NoximGlobalTrafficTable gttable;
    NNModel nnmodel;	//tytyty


	int	cnt_neighbor_total[4];
	int cnt_received_total[4];
	int	cnt_local_total[4];
	int	tot_cnt_local[4][4][4];
	int tot_cnt_neighbor[4][4][4];
	int tot_cnt_received[4][4][4];

    //---------- Mau experiment <start>
    void flitsMonitor() {
	
	if (!reset.read()) {
	    //      if ((int)sc_simulation_time() % 5)
	    //        return;
	/****************MODIFY BY HUI-SHUN********************/
	    unsigned int count = 0;
	    /*for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++)
		for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++)
		    count += t[i][j]->r->getFlitsCount();*/
		int i,j,k;
		for( i=0; i<NoximGlobalParams::mesh_dim_x; i++)
			for( j=0; j<NoximGlobalParams::mesh_dim_y; j++)
				for( k=0; k<NoximGlobalParams::mesh_dim_z; k++) ////
					count += t[i][j][k]->r->getFlitsCount();////
	    cout << count << endl;
	}
    }
    //---------- Mau experiment <stop>

    // Constructor

    SC_CTOR(NoximNoC) {

	//---------- Mau experiment <start>
	/*
	   SC_METHOD(flitsMonitor);
	   sensitive(reset);
	   sensitive_pos(clock);
	 */
	//---------- Mau experiment <stop>
	// Build the Mesh
	buildMesh();
	//---------- Hot spot interface BY CMH <start>
	
	//HS_initial();
	HS_interface = new Thermal_IF(NoximGlobalParams::mesh_dim_x,NoximGlobalParams::mesh_dim_y, NoximGlobalParams::mesh_dim_z);
	instPowerTrace.resize(3*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_z, 0);
	overallPowerTrace.resize(3*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_z, 0);
	TemperatureTrace.resize(3*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_z, 0);
	
	SC_METHOD(entry);
	sensitive << reset;
	sensitive << clock.pos();
	
	//---------- Hot spot interface BY CMH <end>
	
	/*--------Dynamic mapping-------------*/
	SC_METHOD(Dynamic_check);
	sensitive << clock.pos();

    }
    ~NoximNoC() 
	{ 
	  delete HS_interface; 
	};
	// Support methods
    NoximTile *searchNode(const int id) const;
	bool EmergencyDecision();

  private:

	    void buildMesh();
		void entry();
		void Dynamic_check();
	    Thermal_IF* HS_interface;
		 bool LastIsEmergency;
    
        //Power trace	
		vector<double> instPowerTrace;
		vector<double> overallPowerTrace;
		//get transient power in one sample period
		void transPwr2PtraceFile();
		void steadyPwr2PtraceFile();
		
		//Temperature trace 
		vector<double> TemperatureTrace;
        void setTemperature();
		
		

};

#endif
