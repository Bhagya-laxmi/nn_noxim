//Design by CMH, Access Lab, NTU

#ifndef __THERMAL_IF_H__
#define __THERMAL_IF_H__

#include "NoximMain.h"
#include "co-sim.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

//HotSpot source code
extern "C"{ //Foster:HotSpot��C�g���ANoxim�OC++�A�[����ޥ�C�y���{��
#include "temperature.h"
#include "temperature_grid.h"	/* for dump_steady_temp_grid	*/
#include "temperature_block.h"	/* for dump_steady_temp_grid	*/
#include "flp.h"
#include "util.h"
#include "npe.h"
#include "shape.h"
}
#include <stdio.h>
#include "NoximTile.h"
using namespace std;



class Thermal_IF{
public:
    Thermal_IF();
	Thermal_IF(int x,int y, int z) :mesh_dim_x(x), mesh_dim_y(y), mesh_dim_z(z) 
	{ fpGenerator(); hs_init(); logFile_init();}
	~Thermal_IF() 
	{ if(results_log_temp.is_open()) results_log_temp.close(); }
	
	void initial() { fpGenerator(); }
	void Temperature_calc(vector<double>& pwr, vector<double>&temp)
	{ 
	  hs_temperature(pwr); 
	  Tmp2TtraceFile(temp); 
	}
	void fpGenerator();  //floorplan Generator
	void steadyTmp( NoximTile* t[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM]){ hs_steady(t); }
	void finish() { hs_finish(); }
	
private:
    int mesh_dim_x, mesh_dim_y, mesh_dim_z;

    /*=================================Variable for HotSpot======================*/
	/* floorplan	*/
	flp_t *hs_flp;
	/* hotspot temperature model	*/
	RC_model_t *hs_model;
	/* instantaneous temperature and power values	*/
	double *hs_inst_temp, *hs_inst_power;
	/* steady state temperature and power values	*/
	double *hs_overall_power, *hs_steady_temp;
	int hs_first_call;

    //Thermal model function of HotSpot
    void hs_init();
	//void hs_steady();
	void hs_steady( NoximTile *t[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM]);
	void hs_finish();
	void hs_temperature(vector<double>& pwr);
	void Tmp2TtraceFile(vector<double>& temp);
	
	//logFile
	ofstream results_log_temp;
	void logFile_init();
	
	

};
#endif
