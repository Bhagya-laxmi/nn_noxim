/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
 */

#include "NoximGlobalStats.h"
using namespace std;

NoximGlobalStats::NoximGlobalStats(const NoximNoC * _noc)
{
    noc = _noc;

#ifdef TESTING
    drained_total = 0;
#endif
}

double NoximGlobalStats::getAverageDelay()
{
    unsigned int total_packets = 0;
    double avg_delay = 0.0;
    /****************MODIFY BY HUI-SHUN********************/
    /*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    unsigned int received_packets =
		noc->t[x][y]->r->stats.getReceivedPackets();
	    if (received_packets) {
		avg_delay +=
		    received_packets *
		    noc->t[x][y]->r->stats.getAverageDelay();
		total_packets += received_packets;
	    }
	}
    avg_delay /= (double) total_packets;*/
	int x,y,z;
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)  ////
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++)
				{
					unsigned int received_packets = noc->t[x][y][z]->r->stats.getReceivedPackets(); ////

					if (received_packets)
						{
							avg_delay += received_packets * noc->t[x][y][z]->r->stats.getAverageDelay();////
							total_packets += received_packets;
						}
				}
	/****************MODIFY BY HUI-SHUN********************/
	
    return avg_delay/(double)total_packets;
}
/****************MODIFY BY HUI-SHUN********************/
vector<double>  NoximGlobalStats::getLayerAverageDelay()
{ 

  vector<double>   avg_delay;
  vector<double>   received_packets;
  vector<double>  total_packets;
	int x,y,z;
	avg_delay.resize(NoximGlobalParams::mesh_dim_z); 
	received_packets.resize(NoximGlobalParams::mesh_dim_z); 
	total_packets.resize(NoximGlobalParams::mesh_dim_z); 
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)  
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++){
				received_packets[z] = noc->t[x][y][z]->r->stats.getReceivedPackets(); ////
				if (received_packets[z]){
					avg_delay[z] += received_packets[z] * noc->t[x][y][z]->r->stats.getAverageDelay();////
					total_packets[z] += received_packets[z];
				}
			}
	for ( z = 0 ; z<NoximGlobalParams::mesh_dim_z; z++)  
		avg_delay[z] /= (double)total_packets[z];

return avg_delay;  
}
/****************MODIFY BY HUI-SHUN********************/

double NoximGlobalStats::getAverageDelay(const int src_id,
					 const int dst_id)
{
    NoximTile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageDelay(src_id);
}

double NoximGlobalStats::getMaxDelay()
{
    double maxd = -1.0;
	int node_id,x,y;
	double d;
	/****************MODIFY BY HUI-SHUN********************/
    /*for ( y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
		for ( x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    NoximCoord coord;
	    coord.x = x;
	    coord.y = y;
	    node_id = coord2Id(coord);
	    d = getMaxDelay(node_id);
	    if (d > maxd)
		maxd = d;
	}*/
	for (int z=0; z<NoximGlobalParams::mesh_dim_z; z++)
		for (int y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for (int x=0; x<NoximGlobalParams::mesh_dim_x; x++){
				NoximCoord coord;
				coord.x = x;
				coord.y = y;
				coord.z = z; 
				node_id = coord2Id(coord);
				d = getMaxDelay(node_id);
				if (d > maxd)
				maxd = d;
				}
	/****************MODIFY BY HUI-SHUN********************/
    return maxd;
}

double NoximGlobalStats::getMaxDelay(const int node_id)
{
    NoximCoord coord = id2Coord(node_id);
	/****************MODIFY BY HUI-SHUN********************/
	//unsigned int received_packets = noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();
	unsigned int received_packets = noc->t[coord.x][coord.y][coord.z]->r->stats.getReceivedPackets();
    if (received_packets)
		return noc->t[coord.x][coord.y][coord.z]->r->stats.getMaxDelay();
	//return noc->t[coord.x][coord.y]->r->stats.getMaxDelay();
    else
		return -1.0;
	/****************MODIFY BY HUI-SHUN********************/
}

double NoximGlobalStats::getMaxDelay(const int src_id, const int dst_id)
{
    NoximTile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getMaxDelay(src_id);
}
/****************MODIFY BY HUI-SHUN********************/
/*vector < vector < double > > NoximGlobalStats::getMaxDelayMtx()
{
    vector < vector < double > > mtx;
    mtx.resize(NoximGlobalParams::mesh_dim_y);
    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	mtx[y].resize(NoximGlobalParams::mesh_dim_x);
    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    NoximCoord coord;
	    coord.x = x;
	    coord.y = y;
	    int id = coord2Id(coord);
	    mtx[y][x] = getMaxDelay(id);
	}
    return mtx;}*/
vector<vector<vector<double> > > NoximGlobalStats::getMaxDelayMtx()
{
	vector<vector<vector<double> > > mtx;
	int x,y,z,id;
	mtx.resize(NoximGlobalParams::mesh_dim_z); ////
	for ( z = 0 ; z<NoximGlobalParams::mesh_dim_z; z++){	  mtx[z].resize(NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);////
       for ( y = 0 ; y<NoximGlobalParams::mesh_dim_y; y++)
        mtx[z][y].resize(NoximGlobalParams::mesh_dim_x);////
		}
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++){
				NoximCoord coord;
				coord.x = x;
				coord.y = y;
				coord.z = z;////
				id = coord2Id(coord);
				mtx[z][y][x] = getMaxDelay(id);////
			}
  return mtx;
}
/****************MODIFY BY HUI-SHUN********************/
double NoximGlobalStats::getAverageThroughput(const int src_id,
					      const int dst_id)
{
    NoximTile *tile = noc->searchNode(dst_id);

    assert(tile != NULL);

    return tile->r->stats.getAverageThroughput(src_id);
}

double NoximGlobalStats::getAverageThroughput()
{
    unsigned int total_comms = 0;
    double avg_throughput = 0.0;
/****************MODIFY BY HUI-SHUN********************/
	int x,y,z;
	unsigned int ncomms;
    /*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    unsigned int ncomms =
		noc->t[x][y]->r->stats.getTotalCommunications();
	    if (ncomms) {
		avg_throughput +=
		    ncomms * noc->t[x][y]->r->stats.getAverageThroughput();
		total_comms += ncomms;
	    }
	}
	*/
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++){
				ncomms = noc->t[x][y][z]->r->stats.getTotalCommunications(); ////

			if (ncomms){
				avg_throughput += ncomms * noc->t[x][y][z]->r->stats.getAverageThroughput();////
				total_comms += ncomms;
				}
			}
    avg_throughput /= (double) total_comms;
/****************MODIFY BY HUI-SHUN********************/
    return avg_throughput;
}

unsigned int NoximGlobalStats::getReceivedPackets()
{
    unsigned int n = 0;
	/****************MODIFY BY HUI-SHUN********************/
	int x,y,z;
	/*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
	    n += noc->t[x][y]->r->stats.getReceivedPackets();*/
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++)
				n += noc->t[x][y][z]->r->stats.getReceivedPackets();////
	/****************MODIFY BY HUI-SHUN********************/
    return n;
}

unsigned int NoximGlobalStats::getReceivedFlits()
{
    unsigned int n = 0;
	/****************MODIFY BY HUI-SHUN********************/
    int x,y,z;
	/*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    n += noc->t[x][y]->r->stats.getReceivedFlits();*/
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++){
				n += noc->t[x][y][z]->r->stats.getReceivedFlits();////
#ifdef TESTING
	    //drained_total += noc->t[x][y]->r->local_drained;
		drained_total+= noc->t[x][y][z]->r->local_drained;
#endif
	/****************MODIFY BY HUI-SHUN********************/
	}

    return n;
}

double NoximGlobalStats::getThroughput()
{
    int total_cycles =
	NoximGlobalParams::simulation_time -
	NoximGlobalParams::stats_warm_up_time;

    //  int number_of_ip = NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y;
    //  return (double)getReceivedFlits()/(double)(total_cycles * number_of_ip);

    unsigned int n = 0;
    unsigned int trf = 0;
	/****************MODIFY BY HUI-SHUN********************/
    int x,y,z;
	unsigned int rf;
	/*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++) {
	    unsigned int rf = noc->t[x][y]->r->stats.getReceivedFlits();*/
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++){
				rf = noc->t[x][y][z]->r->stats.getReceivedFlits();////
				if (rf != 0)n++;
				trf += rf;
			}
	/****************MODIFY BY HUI-SHUN********************/
    return (double) trf / (double) (total_cycles * n);

}

vector <vector < vector < unsigned long > > >NoximGlobalStats::getRoutedFlitsMtx()
{

    vector < vector < vector < unsigned long > > > mtx;
	/****************MODIFY BY HUI-SHUN********************/
    int x,y,z;
	/*mtx.resize(NoximGlobalParams::mesh_dim_y);
    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	mtx[y].resize(NoximGlobalParams::mesh_dim_x);

    for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
	    mtx[y][x] = noc->t[x][y]->r->getRoutedFlits();*/
	mtx.resize( NoximGlobalParams::mesh_dim_z); ////
	for ( z=0; z< NoximGlobalParams::mesh_dim_z; z++){
		mtx[z].resize( NoximGlobalParams::mesh_dim_x *  NoximGlobalParams::mesh_dim_y);////
		for ( y=0; y<  NoximGlobalParams::mesh_dim_y; y++)
			mtx[z][y].resize( NoximGlobalParams::mesh_dim_x);////
	}
	for( z=0; z< NoximGlobalParams::mesh_dim_z; z++)////
		for( y=0; y< NoximGlobalParams::mesh_dim_y; y++)
			for( x=0; x<NoximGlobalParams::mesh_dim_x; x++)
				mtx[z][y][x] = noc->t[x][y][z]->r->getRoutedFlits();////
    /****************MODIFY BY HUI-SHUN********************/
	return mtx;
}

double NoximGlobalStats::getPower()
{
    double power = 0.0;
	/****************MODIFY BY HUI-SHUN********************/
    /*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
	    power += noc->t[x][y]->r->getPower();*/
	int x,y,z;
	for( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for( x=0; x<NoximGlobalParams::mesh_dim_x; x++)
				power += noc->t[x][y][z]->r->getPower();  ////	
	/****************MODIFY BY HUI-SHUN********************/
    return power;
}
/****************MODIFY BY HUI-SHUN********************/
vector<double>  NoximGlobalStats::getLayerPower()
{ 
	vector<double>   power;  
	int x,y,z;
	power.resize(NoximGlobalParams::mesh_dim_z); 
	for ( z=0; z<NoximGlobalParams::mesh_dim_z; z++)  
		for ( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for ( x=0; x<NoximGlobalParams::mesh_dim_x; x++)
				power[z] += noc->t[x][y][z]->r->getPower();  
	return power;  
}
/****************MODIFY BY HUI-SHUN********************/
void NoximGlobalStats::showStats(std::ostream & out, bool detailed)
{
    out << "% Total received packets: " << getReceivedPackets() << endl;
    out << "% Total received flits: " << getReceivedFlits() << endl;
    out << "% Global average delay (cycles): " << getAverageDelay() <<
	endl;
    out << "% Global average throughput (flits/cycle): " <<
	getAverageThroughput() << endl;
    out << "% Throughput (flits/cycle/IP): " << getThroughput() << endl;
    out << "% Max delay (cycles): " << getMaxDelay() << endl;
    out << "% Total energy (J): " << getPower() << endl;
	/****************MODIFY BY HUI-SHUN********************/
	out	<< "% Avg power (J/cycle) : "<<	getPower()/NoximGlobalParams::simulation_time<< endl;
	out	<< "% Avg power per router (J/cycle) : "<<	getPower()/NoximGlobalParams::simulation_time/(NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_z) << endl;
	out << "% Layer average delay (cycles): " ;
	vector<double> delay_mtx = getLayerAverageDelay(); 
	unsigned int z,x,y;
	for ( z=0; z<delay_mtx.size(); z++)
		out << setw(6) <<delay_mtx[z]<<"  ";
	out<<endl;
	out << "% Layer energy (J): " ;
    vector<double> power_mtx = getLayerPower(); 
	for ( z=0; z<power_mtx.size(); z++)
		out << setw(6) <<power_mtx[z]<<"  ";
	out<<endl;
	/****************MODIFY BY HUI-SHUN********************/
    if (detailed) {
	out << endl << "detailed = [" << endl;
	/****************MODIFY BY HUI-SHUN********************/
	/*for (int y = 0; y < NoximGlobalParams::mesh_dim_y; y++)
	    for (int x = 0; x < NoximGlobalParams::mesh_dim_x; x++)
		noc->t[x][y]->r->stats.showStats(y *
						 NoximGlobalParams::
						 mesh_dim_x + x, out,
						 true);*/
	for( z=0; z<NoximGlobalParams::mesh_dim_z; z++)////
		for( y=0; y<NoximGlobalParams::mesh_dim_y; y++)
			for( x=0; x<NoximGlobalParams::mesh_dim_x; x++)
				noc->t[x][y][z]->r->stats.showStats(z*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y + y*NoximGlobalParams::mesh_dim_x+x,out,true);
	/****************MODIFY BY HUI-SHUN********************/
	out << "];" << endl;

	// show MaxDelay matrix
	vector <vector < vector < double > > > md_mtx = getMaxDelayMtx();

	out << endl << "max_delay = [" << endl;
	/****************MODIFY BY HUI-SHUN********************/
	/*for (unsigned int y = 0; y < md_mtx.size(); y++) {
	    out << "   ";
	    for (unsigned int x = 0; x < md_mtx[y].size(); x++)
		out << setw(6) << md_mtx[y][x];
	    out << endl;
	}*/
	for( z=0; z<md_mtx.size(); z++)
		for( y=0; y<md_mtx[z].size(); y++){
			out << "   ";
			for ( x=0; x<md_mtx[z][y].size(); x++)
				out << setw(6) << md_mtx[z][y][x];
			out << endl;
		}
	/****************MODIFY BY HUI-SHUN********************/
	out << "];" << endl;

	// show RoutedFlits matrix
	vector <vector < vector < unsigned long > > > rf_mtx = getRoutedFlitsMtx();

	out << endl << "routed_flits = [" << endl;
	/****************MODIFY BY HUI-SHUN********************/
	/*for (unsigned int y = 0; y < rf_mtx.size(); y++) {
	    out << "   ";
	    for (unsigned int x = 0; x < rf_mtx[y].size(); x++)
		out << setw(10) << rf_mtx[y][x];
	    out << endl;	}*/
	for ( z=0; z<rf_mtx.size(); z++)
		for ( y=0; y<rf_mtx[z].size(); y++){
			out << "   ";
			for ( x=0; x<rf_mtx[z][y].size(); x++)
				out << setw(10) << rf_mtx[z][y][x];
			out << endl;
		}
	 /****************MODIFY BY HUI-SHUN********************/ 
	out << "];" << endl;    }
	
	
}