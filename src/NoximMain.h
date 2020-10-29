/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
 */

#ifndef __NOXIMMAIN_H__
#define __NOXIMMAIN_H__

#include <cassert>
#include <systemc.h>
#include <vector>
#include <deque>

/* parameter definition of traffic-thermal co-sim */
//#include "define.h"
#include "co-sim.h"

using namespace std;

// Define the directions as numbers
/****************MODIFY BY HUI-SHUN********************/
//#define DIRECTIONS             4
#define DIRECTIONS             6
#define DIRECTION_NORTH        0
#define DIRECTION_EAST         1
#define DIRECTION_SOUTH        2
#define DIRECTION_WEST         3
//#define DIRECTION_LOCAL        4
#define DIRECTION_UP           4 
#define DIRECTION_DOWN         5 
#define DIRECTION_LOCAL        6 
/****************MODIFY BY HUI-SHUN********************/
/****************MODIFY BY HUI-SHUN********************/
/********************RCA PARAMETER*********************/
#define RCA_LOCAL_RATIO 0.3 //Traffic information based on local or remote
							//0.7 for OE3D-version RCA
							//0.8 optimization
#define RCA_1D			1   //1D mode = 0, Fanin mode = 1
#define RCA_QUADRO      0
/****************MODIFY BY HUI-SHUN********************/
// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Routing algorithms
/****************MODIFY BY HUI-SHUN********************/
//#define ROUTING_XY             0
#define ROUTING_XYZ             0
/****************MODIFY BY HUI-SHUN********************/
#define ROUTING_WEST_FIRST     1
#define ROUTING_NORTH_LAST     2
#define ROUTING_NEGATIVE_FIRST 3
#define ROUTING_ODD_EVEN       4
#define ROUTING_DYAD           5
#define ROUTING_FULLY_ADAPTIVE 8
#define ROUTING_TABLE_BASED    9
/****************MODIFY BY HUI-SHUN********************/
#define ROUTING_PROPOSED       6
#define ROUTING_ZXY            10
#define ROUTING_DOWNWARD	   11
#define ROUTING_ODD_EVEN_DOWNWARD 12
#define ROUTING_ODD_EVEN_3D    13
#define ROUTING_ODD_EVEN_Z     14
#define ROUTING_CROSS_LAYER    15
#define ROUTING_DOWNWARD_CROSS_LAYER 16
#define ROUTING_DOWNWARD_CROSS_LAYER_HS 17
/****************MODIFY BY HUI-SHUN********************/
#define ROUTING_XYX 18 		//tytyty
#define INVALID_ROUTING       -1

// Selection strategies
#define SEL_RANDOM             0
#define SEL_BUFFER_LEVEL       1
#define SEL_NOP                2
/****************MODIFY BY HUI-SHUN********************/
#define SEL_RCA                3
#define SEL_AXY                4
#define SEL_AXY_NOP            5
#define SEL_AXY_TURN           6
/****************MODIFY BY HUI-SHUN********************/
#define INVALID_SELECTION     -1

// Traffic distribution
#define TRAFFIC_RANDOM         0
#define TRAFFIC_TRANSPOSE1     1
#define TRAFFIC_TRANSPOSE2     2
#define TRAFFIC_HOTSPOT        3
#define TRAFFIC_TABLE_BASED    4
#define TRAFFIC_BIT_REVERSAL   5
#define TRAFFIC_SHUFFLE        6
#define TRAFFIC_BUTTERFLY      7
/****************MODIFY BY HUI-SHUN********************/
#define TRAFFIC_RANDOM_TVAR	   8
#define TRAFFIC_RANDOM_2	   9
/****************MODIFY BY HUI-SHUN********************/
#define INVALID_TRAFFIC       -1
// Input Selection
#define INSEL_RANDOM           0
#define INSEL_CS               1
// Verbosity levels
#define VERBOSE_OFF            0
#define VERBOSE_LOW            1
#define VERBOSE_MEDIUM         2
#define VERBOSE_HIGH           3

/****************MODIFY BY HUI-SHUN********************/
/***************THROTTLING MECHANISM*******************/
#define THROT_NORMAL		   0
#define THROT_GLOBAL		   1
#define THROT_DISTRIBUTED	   2
#define THROT_VERTICAL		   3
#define THROT_TEST			   4

//Buffer allocation Uniform:1,2,2,11.
#define BUFFER_LAYER_0			1 
#define BUFFER_LAYER_1			2
#define BUFFER_LAYER_2			2
#define BUFFER_LAYER_3			11

#define BUFFER_ALLOC			0

#define THROT_X1			2 
#define THROT_Y1			2 
#define THROT_Z1			0

#define THROT_X2			3 
#define THROT_Y2			3 
#define THROT_Z2			0

#define THROT_X3			4 
#define THROT_Y3			4 
#define THROT_Z3			0
/****************MODIFY BY HUI-SHUN********************/

// Default configuration can be overridden with command-line arguments
#define DEFAULT_VERBOSE_MODE               VERBOSE_OFF
#define DEFAULT_TRACE_MODE                       false
#define DEFAULT_TRACE_FILENAME                      ""
#define DEFAULT_MESH_DIM_X                          128
#define DEFAULT_MESH_DIM_Y                          128
/****************MODIFY BY HUI-SHUN********************/
#define DEFAULT_MESH_DIM_Z                           4
#define DEFAULT_GROUP_NEU_NUM                        1	//tytyty //** 2018.09.01 edit by Yueh-Chi,Yang **//
//********* 2018.09.10 edit by Yueh-Chi,Yang **********//
//*****************************************************//
/****************MODIFY BY HUI-SHUN********************/
#define DEFAULT_BUFFER_DEPTH                        16
#define DEFAULT_MAX_PACKET_SIZE                      6
#define DEFAULT_MIN_PACKET_SIZE                      6
#define DEFAULT_ROUTING_ALGORITHM          ROUTING_XYZ //tytyty
#define DEFAULT_ROUTING_TABLE_FILENAME              ""
#define DEFAULT_SELECTION_STRATEGY          SEL_RANDOM
#define DEFAULT_PACKET_INJECTION_RATE             0.01
#define DEFAULT_PROBABILITY_OF_RETRANSMISSION     0.01
#define DEFAULT_TRAFFIC_DISTRIBUTION   TRAFFIC_RANDOM
#define DEFAULT_TRAFFIC_TABLE_FILENAME              ""
#define DEFAULT_RESET_TIME                           1 //tytyty
#define DEFAULT_SIMULATION_TIME                  50000  
#define DEFAULT_STATS_WARM_UP_TIME  DEFAULT_RESET_TIME
#define DEFAULT_DETAILED                         false
#define DEFAULT_DYAD_THRESHOLD                     0.6
#define DEFAULT_MAX_VOLUME_TO_BE_DRAINED             0
/****************MODIFY BY HUI-SHUN********************/
#define DEFAULT_PIR_IS_LOCAL_RANDOM		         false
#define DEFAULT_PIR_LOCAL_min					     0
#define DEFAULT_PIR_LOCAL_max					  0.04	
#define DEFAULT_PIR_P_ON			           0.00025 
#define	DEFAULT_PIR_P_OFF			           0.00025													
#define DEFAULT_DOWN_LEVEL				             3
#define DEFAULT_THROTTLING_TYPE		      THROT_TEST
#define DEFAULT_THROTTLING_RATIO			         0

// Packet Type
#define PKT_NORMAL             0
#define PKT_SHARE              1
#define PKT_REQ                2
#define PKT_ACK                3
#define PKT_NAK                4
#define PKT_RETRANS            5
//reorder selection
#define REORDER_OFF            0
#define REORDER_ON             1
#define DEFAULT_RENTS_HIER                           3
#define DEFAULT_RENTS_BETA                        0.74
#define DEFAULT_BURST_LENGTH                         1
#define DEFAULT_SCRATCH_SIZE                        -1
#define DEFAULT_MAX_SCRATCH_SIZE                    -1
#define DEFAULT_AVG_SCRATCH_SIZE                    -1
//latency histogram
#define HIST_OFF               0
#define HIST_ON                1

/****************MODIFY BY HUI-SHUN********************/
// TODO by Fafa - this MUST be removed!!! Use only STL vectors instead!!!
#define MAX_STATIC_DIM 20

//** 2018.09.02 edit by Yueh-Chi,Yang **//
#define DEFAULT_NNMODEL_FILENAME	"model.txt"
#define DEFAULT_NNWEIGHT_FILENAME	"weight.txt"
#define DEFAULT_MAPPING_ALGORITHM	"dir_x"
#define DEFAULT_MAPPING_TABLE_FILENAME	""
#define DEFAULT_NNINPUT_FILENAME	"input.txt"
#define DEFAULT_PE_COMPUTATION_TIME	10
//**************************************//

// NoximGlobalParams -- used to forward configuration to every sub-block
struct NoximGlobalParams {
    static int verbose_mode;
    static int trace_mode;
    static char trace_filename[128];
    static int mesh_dim_x;
    static int mesh_dim_y;
	/****************MODIFY BY HUI-SHUN********************/
	static int mesh_dim_z;
	/****************MODIFY BY HUI-SHUN********************/
    static int group_neu_num;	//** 2018.09.01 edit by Yueh-Chi,Yang **//
    static int buffer_depth;
    static int min_packet_size;
    static int max_packet_size;
    static int routing_algorithm;
    static char routing_table_filename[128];
    static int selection_strategy;
    static float packet_injection_rate;
    static float probability_of_retransmission;
    static int traffic_distribution;
    static char traffic_table_filename[128];
    static int simulation_time;
    static int stats_warm_up_time;
    static int rnd_generator_seed;
    static bool detailed;
    static vector <pair <int, double> > hotspots;
    static float dyad_threshold;
    static unsigned int max_volume_to_be_drained;
	/****************MODIFY BY HUI-SHUN********************/
	static int burst_length;
	static int down_level;
	static int throt_type;
	static float throt_ratio;
	static double max_temp;
	static char max_temp_r[40];
	static bool pir_is_local_random;
	/****************MODIFY BY HUI-SHUN********************/
	//** 2018.09.02 edit by Yueh-Chi,Yang **//
	static char NNmodel_filename[128];
	static char NNweight_filename[128];
	static char mapping_algorithm[128];
	static char mapping_table_filename[128];
	static char NNinput_filename[128];
	static int PE_computation_time;
	//**************************************//
	//** 2018.09.12 edit by Yueh-Chi, Yang **//
	//static deque < deque < deque <int> > > throttling;
	static int throttling[128][128][1];
	//***************************************//
};

// NoximCoord -- XY coordinates type of the Tile inside the Mesh
class NoximCoord {
  public:
    int x;			// X coordinate
    int y;			// Y coordinate
	/****************MODIFY BY HUI-SHUN********************/
	int z;          // Z coordinate
	/****************MODIFY BY HUI-SHUN********************/

    inline bool operator ==(const NoximCoord & coord) const {
	/****************MODIFY BY HUI-SHUN********************/
	//return (coord.x == x && coord.y == y);
	return (coord.x == x && coord.y == y && coord.z == z);
	/****************MODIFY BY HUI-SHUN********************/
}};

// NoximFlitType -- Flit type enumeration
enum NoximFlitType {
    FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL
};

// NoximPayload -- Payload definition
struct NoximPayload {
    sc_uint<32> data;	// Bus for the data to be exchanged

    inline bool operator ==(const NoximPayload & payload) const {
	return (payload.data == data);
}};

// NoximPacket -- Packet definition
struct NoximPacket {
    int src_id;
    int dst_id;
    double timestamp;		// SC timestamp at packet generation
    int size;
    int flit_left;		// Number of remaining flits inside the packet
	int                routing;

    // Constructors
    NoximPacket() { }
    NoximPacket(const int s, const int d, const double ts, const int sz) {
	make(s, d, ts, sz);
    }

    void make(const int s, const int d, const double ts, const int sz) {
	src_id = s;
	dst_id = d;
	timestamp = ts;
	size = sz;
	flit_left = sz;
    }
};

// NoximRouteData -- data required to perform routing
struct NoximRouteData {
    int current_id;
    int src_id;
    int dst_id;
    int dir_in;			// direction from which the packet comes from
	bool XYX_routing;	//tytytyty
	int routing;
};

struct NoximChannelStatus {
    int free_slots;		// occupied buffer slots
    bool available;		// 
	/****************MODIFY BY HUI-SHUN********************/
	bool throttle; //Foster
	double temperature; //Foster modified for spreading temperature info.
	int  channel_count;
	/****************MODIFY BY HUI-SHUN********************/
    inline bool operator ==(const NoximChannelStatus & bs) const {
	return (free_slots == bs.free_slots && available == bs.available);
    };
};

// NoximNoP_data -- NoP Data definition
struct NoximNoP_data {
    int sender_id;
    NoximChannelStatus channel_status_neighbor[DIRECTIONS];

    inline bool operator ==(const NoximNoP_data & nop_data) const {
	return (sender_id == nop_data.sender_id &&
		nop_data.channel_status_neighbor[0] ==
		channel_status_neighbor[0]
		&& nop_data.channel_status_neighbor[1] ==
		channel_status_neighbor[1]
		&& nop_data.channel_status_neighbor[2] ==
		channel_status_neighbor[2]
		&& nop_data.channel_status_neighbor[3] ==
		channel_status_neighbor[3]);
    };
};

// NoximFlit -- Flit definition
struct NoximFlit {
    int src_id;
    int dst_id;
    NoximFlitType flit_type;	// The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
    int sequence_no;		// The sequence number of the flit inside the packet
    NoximPayload payload;	// Optional payload
    double timestamp;		// Unix timestamp at packet generation
    int hop_no;			// Current number of hops from source to destination
	int                routing_f;

    float data;                //tytyty
	bool	XYX_routing;		//tytyty-XYXrouting
    int src_Neu_id;
	
	inline bool operator ==(const NoximFlit & flit) const {
	return (flit.src_id == src_id && flit.dst_id == dst_id
		&& flit.flit_type == flit_type
		&& flit.sequence_no == sequence_no
		&& flit.payload == payload && flit.timestamp == timestamp
		&& flit.hop_no == hop_no);
}};

//
inline int getCurrentCycleNum(){
	return (int)(sc_time_stamp().to_double()/1000/CYCLE_PERIOD);
};

// Output overloading

inline ostream & operator <<(ostream & os, const NoximFlit & flit)
{

    if (NoximGlobalParams::verbose_mode == VERBOSE_HIGH) {

	os << "### FLIT ###" << endl;
	os << "Source Tile[" << flit.src_id << "]" << endl;
	os << "Destination Tile[" << flit.dst_id << "]" << endl;
	switch (flit.flit_type) {
	case FLIT_TYPE_HEAD:
	    os << "Flit Type is HEAD" << endl;
	    break;
	case FLIT_TYPE_BODY:
	    os << "Flit Type is BODY" << endl;
	    break;
	case FLIT_TYPE_TAIL:
	    os << "Flit Type is TAIL" << endl;
	    break;
	}
	os << "Sequence no. " << flit.sequence_no << endl;
	os << "Payload printing not implemented (yet)." << endl;
	os << "Unix timestamp at packet generation " << flit.
	    timestamp << endl;
	os << "Total number of hops from source to destination is " <<
	    flit.hop_no << endl;
    } else {
	os << "[type: ";
	switch (flit.flit_type) {
	case FLIT_TYPE_HEAD:
	    os << "H";
	    break;
	case FLIT_TYPE_BODY:
	    os << "B";
	    break;
	case FLIT_TYPE_TAIL:
	    os << "T";
	    break;
	}

	os << ", seq: " << flit.sequence_no << ", " << flit.src_id << "-->" << flit.dst_id << "]";
    }
//**************************NN-Noxim*********************************

    os << ", src_Neu_id = " << flit.src_Neu_id;//tytyty
    os << ", data = " << flit.data;//tytyty

//**************************^^^^^^^^^^^^*****************************
    return os;
}

inline ostream & operator <<(ostream & os,
			     const NoximChannelStatus & status)
{
    char msg;
    if (status.available)
	msg = 'A';
    else
	msg = 'N';
    os << msg << "(" << status.free_slots << ")";
    return os;
}

inline ostream & operator <<(ostream & os, const NoximNoP_data & NoP_data)
{
    os << "      NoP data from [" << NoP_data.sender_id << "] [ ";

    for (int j = 0; j < DIRECTIONS; j++)
	os << NoP_data.channel_status_neighbor[j] << " ";

    cout << "]" << endl;
    return os;
}

inline ostream & operator <<(ostream & os, const NoximCoord & coord)
{
    /****************MODIFY BY HUI-SHUN********************/
    //os << "(" << coord.x << "," << coord.y << ")";
	os << "(" << coord.x << "," << coord.y << "," << coord.z <<")";
    /****************MODIFY BY HUI-SHUN********************/
    
    return os;
}

// Trace overloading

inline void sc_trace(sc_trace_file * &tf, const NoximFlit & flit, string & name)
{
    sc_trace(tf, flit.src_id, name + ".src_id");
    sc_trace(tf, flit.dst_id, name + ".dst_id");
    sc_trace(tf, flit.sequence_no, name + ".sequence_no");
    sc_trace(tf, flit.timestamp, name + ".timestamp");
    sc_trace(tf, flit.hop_no, name + ".hop_no");
}

inline void sc_trace(sc_trace_file * &tf, const NoximNoP_data & NoP_data, string & name)
{
    sc_trace(tf, NoP_data.sender_id, name + ".sender_id");
}

inline void sc_trace(sc_trace_file * &tf, const NoximChannelStatus & bs, string & name)
{
    sc_trace(tf, bs.free_slots, name + ".free_slots");
    sc_trace(tf, bs.available, name + ".available");
}

// Misc common functions

inline NoximCoord id2Coord(int id)
{
    NoximCoord coord;
/****************MODIFY BY HUI-SHUN********************/
    //coord.x = id % NoximGlobalParams::mesh_dim_x;
    //coord.y = id / NoximGlobalParams::mesh_dim_x;
    coord.z = id / (NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y);////
    coord.y = (id-coord.z*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y) /NoximGlobalParams::mesh_dim_x;
    coord.x = (id-coord.z*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y)  % NoximGlobalParams::mesh_dim_x;

    assert(coord.x < NoximGlobalParams::mesh_dim_x);
    assert(coord.y < NoximGlobalParams::mesh_dim_y);
	assert(coord.z < NoximGlobalParams::mesh_dim_z);
/****************MODIFY BY HUI-SHUN********************/
    return coord;
}

inline int coord2Id(const NoximCoord & coord)
{
	/****************MODIFY BY HUI-SHUN********************/
    //int id = (coord.y * NoximGlobalParams::mesh_dim_x) + coord.x;
    //assert(id < NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y);
	int id = coord.z*NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y + (coord.y * NoximGlobalParams::mesh_dim_x) + coord.x; ////
    assert(id < NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y * NoximGlobalParams::mesh_dim_z); ////
    /****************MODIFY BY HUI-SHUN********************/
	
    return id;
}


#endif
