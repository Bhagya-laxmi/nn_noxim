/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the power model
 */

#ifndef __NOXIMPOWER_H__
#define __NOXIMPOWER_H__

#include <cassert>
#include "NoximMain.h"
using namespace std;

/*

The average energy dissipated by a flit for a hop switch was estimated
as being 0.151nJ, 0.178nJ, 0.182nJ and 0.189nJ for XY, Odd-Even, DyAD,
and NoP-OE respectively

We assumed the tile size to be 2mm x 2mm and that the tiles were
arranged in a regular fashion on the floorplan. The load wire
capacitance was set to 0.50fF per micron, so considering an average of
25% switching activity the amount of energy consumed by a flit for a
hop interconnect is 0.384nJ.

*/
/*// ------ Noxim Original Power Model <start> 

    #define PWR_ROUTING_XY             0.151e-9
    #define PWR_ROUTING_WEST_FIRST     0.155e-9
    #define PWR_ROUTING_NORTH_LAST     0.155e-9
    #define PWR_ROUTING_NEGATIVE_FIRST 0.155e-9
    #define PWR_ROUTING_ODD_EVEN       0.178e-9
    #define PWR_ROUTING_DYAD           0.182e-9
    #define PWR_ROUTING_FULLY_ADAPTIVE 0.0
    #define PWR_ROUTING_TABLE_BASED    0.185e-9

    #define PWR_SEL_RANDOM             0.002e-9
    #define PWR_SEL_BUFFER_LEVEL       0.006e-9
    #define PWR_SEL_NOP                0.012e-9

    #define PWR_FORWARD_FLIT           0.384e-9
    #define PWR_INCOMING               0.002e-9
    #define PWR_STANDBY                0.0001e-9/2.0

// ------ Noxim Original Power Model <end> */

// ------- Intel 80-cores Power Model <start> 
//���: Watt
//�ϥήɷ|�A���W�ɯ߶g��(�ާ@�W�v�˼�), �ƭȥN��@�g���Ү��Ӥ���q(E = P/f)
//�ƭȬ��Ѧ�intel 80 core���ƾ�, �᭱��scaling factor�ΥH�վ�ܷūײŦX��paper�ҭz��110��
//ENERGY_SCALING_FACTOR ��ֿn��energy�@scaling �[�ּ����t��
/*
    #define PWR_QUEUES_DATA_PATH            203.28e-3 *CYCLE_PERIOD*1e-9*2*2 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_MSINT                       55.44e-3  *CYCLE_PERIOD*1e-9*2*2 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_ARBITER_CONTROL             64.68e-3  *CYCLE_PERIOD*1e-9*2*2 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_CROSSBAR                    138.6e-3  *CYCLE_PERIOD*1e-9*2*2 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_LINKS                       157.08e-3 *CYCLE_PERIOD*1e-9*2*2 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_CLOCKING                    304.92e-3 *CYCLE_PERIOD*1e-9*2*2 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_LEAKAGE_ROUTER              70e-3     *CYCLE_PERIOD*1e-9     *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_DUAL_FPMACS                 1188e-3   *CYCLE_PERIOD*1e-9*2*6 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_IMEM                        363e-3    *CYCLE_PERIOD*1e-9*2*6 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_DMEM                        330e-3    *CYCLE_PERIOD*1e-9*2*6 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_RF                          132e-3    *CYCLE_PERIOD*1e-9*2*6 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_CLOCK_DISTRIBUTION          363e-3    *CYCLE_PERIOD*1e-9*2*6 *POWER_SCALING_FACTOR *ENERGY_SCALING_FACTOR
    #define PWR_LEAKAGE_FPMAC               40e-3     *CYCLE_PERIOD*1e-9  
    #define PWR_LEAKAGE_IMEM                21e-3     *CYCLE_PERIOD*1e-9  
    #define PWR_LEAKAGE_DMEM                8e-3      *CYCLE_PERIOD*1e-9  
    #define PWR_LEAKAGE_RF                  7.5e-3    *CYCLE_PERIOD*1e-9  
// ------- Intel 80-cores Power Model <end> 
*/

// ------- Intel 80-core Energy Table, modified by Chihhao <begin>
                                            // �᭱�o�� *2 *2 �P *2 *6�n�A�T�{�O�ƻ�

    #define ENERGY_QUEUES_DATA_PATH    0.20328 *1e-9 *2 *2
    #define ENERGY_MSINT               0.05544 *1e-9 *2 *2
    #define ENERGY_ARBITER_CONTROL     0.06468 *1e-9 *2 *2
    #define ENERGY_CROSSBAR            0.13860 *1e-9 *2 *2
    #define ENERGY_LINKS               0.15708 *1e-9 *2 *2
    #define ENERGY_CLOCKING            0.30492 *1e-9 *2 *2
    #define ENERGY_LEAKAGE_ROUTER      0.07000 *1e-9      
    #define ENERGY_DUAL_FPMACS         1.18800 *1e-9 *2 *6
    #define ENERGY_RF                  0.13200 *1e-9 *2 *6
    #define ENERGY_IMEM                0.36300 *1e-9 *2 *6
    #define ENERGY_DMEM                0.33000 *1e-9 *2 *6
    #define ENERGY_CLOCK_DISTRIBUTION  0.36300 *1e-9 *2 *6
    #define ENERGY_LEAKAGE_FPMAC       0.04000 *1e-9  
    #define ENERGY_LEAKAGE_IMEM        0.02100 *1e-9  
    #define ENERGY_LEAKAGE_DMEM        0.00800 *1e-9  
    #define ENERGY_LEAKAGE_RF          0.00750 *1e-9  
// ------- Intel 80-core Energy Model, modified by Chihhao <end>

class NoximPower {

  public:

    NoximPower();
    
    // Accumulation functions called in Router
    void QueuesNDataPath();
    void Msint();
    void ArbiterNControl();
    void Crossbar();
    void Links();
    void Clocking();
    void LeakageRouter();
    
    // Accumulation functions called in tile
    void DualFpmacs();
    void Imem();
    void Dmem();
    void RF();
    void ClockDistribution();
    void LeakageFPMAC();
    void LeakageMEM();
    
    // Methods for getting short-term average power, in NoximNoC::entry(), before calling Hotspot
    double getTransientRouterPower()   ;
    double getTransientFPMACPower()    ;
    double getTransientMEMPower()      ;
    
    // Methods for getting steadystate average power, before calling Hotspot 
    double getSteadyStateRouterPower() ;
    double getSteadyStateFPMACPower()  ;
    double getSteadyStateMEMPower()    ;
    
    // Methods for resetting energy accumulation    
    void   resetPwr()          { clearAllAccEnergy();       } // redundant, remove later
    void   resetTransientPwr() { clearTransientAccEnergy(); } // redundant, remove later  
    void   clearAllAccEnergy();        // call at simulation setup (NoximNoC::entry() reset phase)
    void   clearTransientAccEnergy();  // call each interval in NoximNoC::entry()
    
    // Get energy for per-flit operation
    double getFlitEnergyQueuesNDataPath()   { return ENERGY_QUEUES_DATA_PATH    ; }
    double getFlitEnergyMsint()             { return ENERGY_MSINT               ; }
    double getFlitEnergyArbiterNControl()   { return ENERGY_ARBITER_CONTROL     ; }
    double getFlitEnergyCrossbar()          { return ENERGY_CROSSBAR            ; }
    double getFlitEnergyLinks()             { return ENERGY_LINKS               ; }
    double getFlitEnergyClocking()          { return ENERGY_CLOCKING            ; }
    double getFlitEnergyLeakageRouter()     { return ENERGY_LEAKAGE_ROUTER      ; }
    double getFlitEnergyLeakageFPMAC()      { return ENERGY_LEAKAGE_FPMAC + ENERGY_LEAKAGE_RF  ; }
    double getFlitEnergyLeakageMEM()        { return ENERGY_LEAKAGE_IMEM + ENERGY_LEAKAGE_DMEM ; }             
 
 
  private:
	// parameters to convert energy and power, setting in NoximPower();
    int	   Real_cycle_num_per_10ms_interval;
	int	   Sim_cycle_num_per_10ms_interval;
	double Power_Scaling_Factor;
	double Energy_Extrapolation_Factor;
	double Accumulation_Interval_in_Sec;
	double accEnergy_to_Transient_Power_coeff;
	double accEnergy_to_SteadyState_Power_coeff;
	double Total_Simulation_Time_in_Sec;

    // power and accumulated energy for transient and steady state temperature computation
    double pwr_Transient_Router   , accEnergy_Transient_Router   ;
    double pwr_Transient_FPMAC    , accEnergy_Transient_FPMAC    ;
    double pwr_Transient_MEM      , accEnergy_Transient_MEM      ;
    double pwr_SteadyState_Router , accEnergy_SteadyState_Router ;
    double pwr_SteadyState_FPMAC  , accEnergy_SteadyState_FPMAC  ;
    double pwr_SteadyState_MEM    , accEnergy_SteadyState_MEM    ;
	/*
	// member data for power and energy computation of router
    double pwr_QueuesNDataPath	 , accEnergy_QueuesNDataPath	;
    double pwr_Msint             , accEnergy_Msint              ;
    double pwr_ArbiterNControl   , accEnergy_ArbiterNControl    ;
    double pwr_Crossbar          , accEnergy_Crossbar           ;
    double pwr_Links             , accEnergy_Links              ;
    double pwr_Clocking          , accEnergy_Clocking           ;
    double pwr_leakage_Router    , accEnergy_leakage_Router     ;    
    // member data for power and energy computation of FPMAC and MEM and ClockDistribution   
    double pwr_DualFpmacs        , accEnergy_DualFpmacs         ;
    double pwr_Imem              , accEnergy_Imem               ;
    double pwr_Dmem              , accEnergy_Dmem               ;
    double pwr_RF                , accEnergy_RF                 ;
    double pwr_ClockDistribution , accEnergy_ClockDistribution  ;
    double pwr_leakage_FPMAC     , accEnergy_leakage_FPMAC      ;
    double pwr_leakage_MEM       , accEnergy_leakage_MEM        ;
    */

                
    /*// ------ Noxim Original Power Model <start> 
    void Routing();
    void Selection();
    void Standby();
    void Forward();
    void Incoming();

    double getPower() {
    return pwr;
    } double getPwrRouting() {
    return pwr_routing;
    }
    double getPwrSelection() {
    return pwr_selection;
    }
    double getPwrForward() {
    return pwr_forward;
    }
    double getPwrStandBy() {
    return pwr_standby;
    }
    double getPwrIncoming() {
    return pwr_incoming;
    }

  private:

    double pwr_routing;
    double pwr_selection;
    double pwr_forward;
    double pwr_standby;
    double pwr_incoming;

    double pwr;
    // ------ Noxim Original Power Model <end> */
};

#endif
