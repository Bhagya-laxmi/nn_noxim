#ifndef __COSIM_H__
#define __COSIM_H__

//operating cycle period (unit: ns)
#define CYCLE_PERIOD 	1   //0.25e-9				//4GHz
////////////////HotSpot setting//////////////////////////////////////////////////////
//unit: ns
#define TEMP_REPORT_PERIOD 1e4 //1e6 //1e-4			//標準是用1e-4, 1e-6是為了加速模擬時用的設定  //1e6 should be the same with Sim_cycle_num_per_10ms_interval in NoximPower.cpp
//Initial temperature
#define INIT_TEMP	 298.15 //350.15//77 kalvin degree
#define INIT_TEMP_80 348 //353						//用來設定最上層(最熱)的溫度, 設高一點會比較快到達穩態, 節省模擬時間
#define TEMP_THRESHOULD 80//70 
//Area(Side) of Router & Tile (其實是邊長) (unit: meter)
#define ROUTER_LENGTH   0.00065
#define ROUTER_WIDTH	0.00053
#define FPMAC_LENGTH	0.002
#define FPMAC_WIDTH		0.00097
#define MEM_LENGTH		0.00135
#define MEM_WIDTH		0.00053
#define TILE_LENGTH		0.002
#define TILE_WIDTH		0.0015
//#define ROUTER_SIDE 0.000203//0.00017//0.002    
//#define Tile_SIDE 0.000609

//<Specific heat capacity in J/(m^3K)>
#define HEAT_CAP_SILICON	1.75e6
#define HEAT_CAP_THERM_IF	4e6

//<Resistivity in (m-K)/W>
#define RESISTIVITY_SILICON		0.01
#define RESISTIVITY_THERM_IF	0.25

//<Thickness in m>
#define THICKNESS_SILICON	0.00015
#define THICKNESS_THERM_IF	2.0e-05

#endif  // COSIM_H
