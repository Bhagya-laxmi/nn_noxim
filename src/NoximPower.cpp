/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include "NoximPower.h"
using namespace std;

NoximPower::NoximPower()
{
	// Set time scale for Noxim, Hostpot, and Interface
	Total_Simulation_Time_in_Sec     = 10   ; // total simulated virtual time: 10 second
	Accumulation_Interval_in_Sec     = 0.01 ; // 0.01 second, interval of transient temp. est. in HotSpot
	Real_cycle_num_per_10ms_interval = 1e7  ; // 1GHz router, 0.01 second interval = 10^7 cycle 
	Sim_cycle_num_per_10ms_interval  = 1e4  ; // Sim. 10^5 cycle for each real 10^7 cycle interval
	
	// Set this veariable for shorten the simulation cycle number
	Energy_Extrapolation_Factor = (double)Real_cycle_num_per_10ms_interval/Sim_cycle_num_per_10ms_interval;
	
	// Set this variable for scaling estimated power
	// fitting target: steady state avergate temperature = 80-core paper's avg temp. (要再重新驗證!!)
	Power_Scaling_Factor = 0.25; 
	
	// Transient_Power = accEnergy_Transient * accEnergy_to_Transient_Power_coeff;
	accEnergy_to_Transient_Power_coeff = (Energy_Extrapolation_Factor/Accumulation_Interval_in_Sec)
	                                     *Power_Scaling_Factor;
	                                     
	// SteadyState_Power = accEnergy_SteadyState * accEnergy_to_SteadyState_Power_coeff;
	accEnergy_to_SteadyState_Power_coeff = (Energy_Extrapolation_Factor/Total_Simulation_Time_in_Sec)
	                                     *Power_Scaling_Factor;
	
	clearAllAccEnergy();
}	

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void   NoximPower::clearTransientAccEnergy()  // call each interval 
{
    // power and accumulated energy for transient and steady state temperature computation
    accEnergy_Transient_Router   = 0;
    accEnergy_Transient_FPMAC    = 0;
    accEnergy_Transient_MEM      = 0;
}

// ---------------------------------------------------------------------------

void   NoximPower::clearAllAccEnergy() // call at simulation setup
{
    accEnergy_SteadyState_Router = 0;
    accEnergy_SteadyState_FPMAC  = 0;
    accEnergy_SteadyState_MEM    = 0;
    
    clearTransientAccEnergy();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void NoximPower::QueuesNDataPath()
{
  accEnergy_SteadyState_Router += ENERGY_QUEUES_DATA_PATH;
  accEnergy_Transient_Router   += ENERGY_QUEUES_DATA_PATH;
}

// ---------------------------------------------------------------------------

void NoximPower::Msint()
{
  accEnergy_SteadyState_Router += ENERGY_MSINT;
  accEnergy_Transient_Router   += ENERGY_MSINT;
}

// ---------------------------------------------------------------------------

void NoximPower::ArbiterNControl()
{
  accEnergy_SteadyState_Router += ENERGY_ARBITER_CONTROL;
  accEnergy_Transient_Router   += ENERGY_ARBITER_CONTROL;
}

// ---------------------------------------------------------------------------

void NoximPower::Crossbar()
{
  accEnergy_SteadyState_Router += ENERGY_CROSSBAR;
  accEnergy_Transient_Router   += ENERGY_CROSSBAR;
}

// ---------------------------------------------------------------------------

void NoximPower::Links()
{
  accEnergy_SteadyState_Router += ENERGY_LINKS;
  accEnergy_Transient_Router   += ENERGY_LINKS;
}

// ---------------------------------------------------------------------------

void NoximPower::Clocking()
{
  accEnergy_SteadyState_Router += ENERGY_CLOCKING;
  accEnergy_Transient_Router   += ENERGY_CLOCKING;
}

// ---------------------------------------------------------------------------

void NoximPower::DualFpmacs()
{
  accEnergy_SteadyState_FPMAC += ENERGY_DUAL_FPMACS;
  accEnergy_Transient_FPMAC   += ENERGY_DUAL_FPMACS;
}

// ---------------------------------------------------------------------------
	
void NoximPower::RF()
{
  accEnergy_SteadyState_FPMAC += ENERGY_RF;
  accEnergy_Transient_FPMAC   += ENERGY_RF;
}

// ---------------------------------------------------------------------------
	
void NoximPower::Imem()
{
  accEnergy_SteadyState_MEM += ENERGY_IMEM;
  accEnergy_Transient_MEM   += ENERGY_IMEM;
}

// ---------------------------------------------------------------------------
	
void NoximPower::Dmem()
{
  accEnergy_SteadyState_MEM += ENERGY_DMEM;
  accEnergy_Transient_MEM   += ENERGY_DMEM;
}

// ---------------------------------------------------------------------------
	
void NoximPower::ClockDistribution()
{
  accEnergy_SteadyState_Router += ENERGY_CLOCK_DISTRIBUTION * 0.3;
  accEnergy_SteadyState_FPMAC  += ENERGY_CLOCK_DISTRIBUTION * 0.4;
  accEnergy_SteadyState_MEM    += ENERGY_CLOCK_DISTRIBUTION * 0.3;
  
  accEnergy_Transient_Router   += ENERGY_CLOCK_DISTRIBUTION * 0.3;
  accEnergy_Transient_FPMAC    += ENERGY_CLOCK_DISTRIBUTION * 0.4;
  accEnergy_Transient_MEM      += ENERGY_CLOCK_DISTRIBUTION * 0.3;
}

// ---------------------------------------------------------------------------

void NoximPower::LeakageRouter()
{
  accEnergy_SteadyState_Router += ENERGY_LEAKAGE_ROUTER;
  accEnergy_Transient_Router   += ENERGY_LEAKAGE_ROUTER;
}

// ---------------------------------------------------------------------------
	
void NoximPower::LeakageFPMAC()
{
  accEnergy_SteadyState_FPMAC += ENERGY_LEAKAGE_FPMAC + ENERGY_LEAKAGE_RF;
  accEnergy_Transient_FPMAC   += ENERGY_LEAKAGE_FPMAC + ENERGY_LEAKAGE_RF;
}

// ---------------------------------------------------------------------------
	
void NoximPower::LeakageMEM()
{
  accEnergy_SteadyState_MEM += ENERGY_LEAKAGE_IMEM + ENERGY_LEAKAGE_DMEM;
  accEnergy_Transient_MEM   += ENERGY_LEAKAGE_IMEM + ENERGY_LEAKAGE_DMEM;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

double NoximPower::getTransientRouterPower()   
{
	pwr_Transient_Router = accEnergy_Transient_Router * accEnergy_to_Transient_Power_coeff;
	return pwr_Transient_Router;
}

// ---------------------------------------------------------------------------

double NoximPower::getTransientFPMACPower()    
{
	pwr_Transient_FPMAC = accEnergy_Transient_FPMAC * accEnergy_to_Transient_Power_coeff;
	return pwr_Transient_FPMAC;
}

// ---------------------------------------------------------------------------

double NoximPower::getTransientMEMPower()      
{
	pwr_Transient_MEM = accEnergy_Transient_MEM * accEnergy_to_Transient_Power_coeff;
	return pwr_Transient_MEM;	
}

// ---------------------------------------------------------------------------

double NoximPower::getSteadyStateRouterPower() 
{
	pwr_SteadyState_Router = accEnergy_SteadyState_Router * accEnergy_to_SteadyState_Power_coeff;
	return pwr_SteadyState_Router;
}

// ---------------------------------------------------------------------------

double NoximPower::getSteadyStateFPMACPower()  
{
	pwr_SteadyState_FPMAC = accEnergy_SteadyState_FPMAC * accEnergy_to_SteadyState_Power_coeff;
	return pwr_SteadyState_FPMAC;
}

// ---------------------------------------------------------------------------

double NoximPower::getSteadyStateMEMPower()    
{
	pwr_SteadyState_MEM = accEnergy_SteadyState_MEM * accEnergy_to_SteadyState_Power_coeff;
	return pwr_SteadyState_MEM;	
}

// ---------------------------------------------------------------------------

   


  
/*	// ------ Noxim Original Power Model <start> 
	pwr = 0.0;

    pwr_standby = PWR_STANDBY;
    pwr_forward = PWR_FORWARD_FLIT;
    pwr_incoming = PWR_INCOMING;

    if (NoximGlobalParams::routing_algorithm == ROUTING_XYZ)
	pwr_routing = PWR_ROUTING_XY;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_ZXY)
	pwr_routing = PWR_ROUTING_XY;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_WEST_FIRST)
	pwr_routing = PWR_ROUTING_WEST_FIRST;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_NORTH_LAST)
	pwr_routing = PWR_ROUTING_NORTH_LAST;
    else if (NoximGlobalParams::routing_algorithm ==
	     ROUTING_NEGATIVE_FIRST)
	pwr_routing = PWR_ROUTING_NEGATIVE_FIRST;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_ODD_EVEN)
	pwr_routing = PWR_ROUTING_ODD_EVEN;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_DYAD)
	pwr_routing = PWR_ROUTING_DYAD;
    else if (NoximGlobalParams::routing_algorithm ==
	     ROUTING_FULLY_ADAPTIVE)
	pwr_routing = PWR_ROUTING_FULLY_ADAPTIVE;
    else if (NoximGlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
	pwr_routing = PWR_ROUTING_TABLE_BASED;
    else
	assert(false);

    if (NoximGlobalParams::selection_strategy == SEL_RANDOM)
	pwr_selection = PWR_SEL_RANDOM;
    else if (NoximGlobalParams::selection_strategy == SEL_BUFFER_LEVEL)
	pwr_selection = PWR_SEL_BUFFER_LEVEL;
    else if (NoximGlobalParams::selection_strategy == SEL_NOP)
	pwr_selection = PWR_SEL_NOP;
    else
	assert(false);
}

void NoximPower::Routing()
{
    pwr += pwr_routing;
}

void NoximPower::Selection()
{
    pwr += pwr_selection;
}

void NoximPower::Standby()
{
    pwr += pwr_standby;
}

void NoximPower::Forward()
{
    pwr += pwr_forward;
}

void NoximPower::Incoming()
{
    pwr += pwr_incoming;
}
// ------ Noxim Original Power Model <end> */