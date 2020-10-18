/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */
/*
 * NN-Noxim - the NoC-based ANN Simulator
 *
 * (C) 2018 by National Sun Yat-sen University in Taiwan
 *
 * This file contains the implementation of loading NN model
 */

#include "NoximCmdLineParser.h"

void showHelp(char selfname[])
{
    cout << "Usage: " << selfname <<
	" [options]\nwhere [options] is one or more of the following ones:"
	<< endl;
    cout << "\t-help\t\tShow this help and exit" << endl;
    cout <<
	"\t-verbose N\tVerbosity level (1=low, 2=medium, 3=high, default off)"
	<< endl;
    cout <<
	"\t-trace FILENAME\tTrace signals to a VCD file named 'FILENAME.vcd' (default off)"
	<< endl;
    cout <<
	"\t-dimx N\t\tSet the mesh X dimension to the specified integer value (default "
	<< DEFAULT_MESH_DIM_X << ")" << endl;
    cout <<
	"\t-dimy N\t\tSet the mesh Y dimension to the specified integer value (default "
	<< DEFAULT_MESH_DIM_Y << ")" << endl;
	//** 2018.09.01 edit by Yueh-Chi,Yang **//
    cout <<	
	"\t-groupsize N\tSet the number of neuron in each group to the specified integer value (default "
	<< DEFAULT_GROUP_NEU_NUM << ")" << endl;
	//**************************************//
	//** 2018.09.02 edit by Yueh-Chi,Yang **//
    cout <<	
	"\t-NNmodel  FILENAME	Neural Network Model with table in the specified file named 'FILENAME.txt' (default: " 
	<< DEFAULT_NNMODEL_FILENAME << ")" << endl;
    cout <<	
	"\t-NNweight  FILENAME	Neural Network Weight with table in the specified file named 'FILENAME.txt' (default: " 
	<< DEFAULT_NNWEIGHT_FILENAME << ")" << endl;
    cout <<	
	"\t-mapping_table  FILENAME	Mapping table in the specified file named 'FILENAME.txt' (No default)" << endl;
    cout <<	
	"\t-mapping  algorithm	Mapping algorithm name (default: " 
	<< DEFAULT_MAPPING_ALGORITHM << ")" << endl;
    cout <<	
	"\t-NNinput  FILENAME	Mapping algorithm with table in the specified file named 'FILENAME.txt' (default: " 
	<< DEFAULT_NNINPUT_FILENAME << ")" << endl;
	//**************************************//
    cout <<
	"\t-buffer N\tSet the buffer depth of each channel of the router to the specified integer value [flits] (default "
	<< DEFAULT_BUFFER_DEPTH << ")" << endl;
    cout <<
	"\t-size Nmin Nmax\tSet the minimum and maximum packet size to the specified integer values [flits] (default min="
	<< DEFAULT_MIN_PACKET_SIZE << ", max=" << DEFAULT_MAX_PACKET_SIZE
	<< ")" << endl;
    cout <<
	"\t-routing TYPE\tSet the routing algorithm to TYPE where TYPE is one of the following (default "
	<< DEFAULT_ROUTING_ALGORITHM << "):" << endl;
    cout << "\t\txy\t\tXY routing algorithm" << endl;
    cout << "\t\twestfirst\tWest-First routing algorithm" << endl;
    cout << "\t\tnorthlast\tNorth-Last routing algorithm" << endl;
    cout << "\t\tnegativefirst\tNegative-First routing algorithm" << endl;
    cout << "\t\toddeven\t\tOdd-Even routing algorithm" << endl;
    cout << "\t\tdyad T\t\tDyAD routing algorithm with threshold T" <<
	endl;
    cout << "\t\tfullyadaptive\tFully-Adaptive routing algorithm" << endl;
    cout <<
	"\t\ttable FILENAME\tRouting Table Based routing algorithm with table in the specified file"
	<< endl << endl;
    cout <<
	"\t-sel TYPE\tSet the selection strategy to TYPE where TYPE is one of the following (default "
	<< DEFAULT_SELECTION_STRATEGY << "):" << endl;
    cout << "\t\trandom\t\tRandom selection strategy" << endl;
    cout << "\t\tbufferlevel\tBuffer-Level Based selection strategy" <<
	endl;
    cout << "\t\tnop\t\tNeighbors-on-Path selection strategy" << endl << endl;
    cout <<
	"\t-pir R TYPE\t\tSet the packet injection rate to the specified real value [0..1] (default "
	<< DEFAULT_PACKET_INJECTION_RATE <<
	") and the time distribution \n\t\t\t\tof traffic to TYPE where TYPE is one of the following:"
	<< endl;
    cout << "\t\tpoisson\t\tMemory-less Poisson distribution (default)" <<
	endl;
    cout << "\t\tburst R\t\tBurst distribution with given real burstness"
	<< endl;
    cout <<
	"\t\tpareto on off r\tSelf-similar Pareto distribution with given real parameters (alfa-on alfa-off r)"
	<< endl;
    cout <<
	"\t\tcustom R\tCustom distribution with given real probability of retransmission"
	<< endl << endl;
    cout <<
	"\t-traffic TYPE\tSet the spatial distribution of traffic to TYPE where TYPE is one of the following (default "
	<< DEFAULT_TRAFFIC_DISTRIBUTION << "'):" << endl;
    cout << "\t\trandom\t\tRandom traffic distribution" << endl;
    cout << "\t\ttranspose1\tTranspose matrix 1 traffic distribution" <<
	endl;
    cout << "\t\ttranspose2\tTranspose matrix 2 traffic distribution" <<
	endl;
    cout << "\t\tbitreversal\tBit-reversal traffic distribution" << endl;
    cout << "\t\tbutterfly\tButterfly traffic distribution" << endl;
    cout << "\t\tshuffle\t\tShuffle traffic distribution" << endl;
    cout <<
	"\t\ttable FILENAME\tTraffic Table Based traffic distribution with table in the specified file"
	<< endl << endl;
    cout <<
	"\t-hs ID P\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)"
	<< endl;
    cout <<
	"\t-warmup N\tStart to collect statistics after N cycles (default "
	<< DEFAULT_STATS_WARM_UP_TIME << ")" << endl;
    cout <<
	"\t-seed N\t\tSet the seed of the random generator (default time())"
	<< endl;
    cout << "\t-detailed\tShow detailed statistics" << endl;
    cout <<
	"\t-volume N\tStop the simulation when either the maximum number of cycles has been reached or N flits have been delivered"
	<< endl;
    cout <<
	"\t-sim N\t\tRun for the specified simulation time [cycles] (default "
	<< DEFAULT_SIMULATION_TIME << ")" << endl << endl;
    cout <<
	"If you find this program useful please don't forget to mention in your paper Maurizio Palesi <mpalesi@diit.unict.it>"
	<< endl;
    cout <<
	"If you find this program useless please feel free to complain with Davide Patti <dpatti@diit.unict.it>"
	<< endl;
    cout <<
	"And if you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>"
	<< endl;
}

void showConfig()
{
    cout << "Using the following configuration: " << endl;
    cout << "- verbose_mode = " << NoximGlobalParams::verbose_mode << endl;
    cout << "- trace_mode = " << NoximGlobalParams::trace_mode << endl;
    //  cout << "- trace_filename = " << NoximGlobalParams::trace_filename << endl;
    cout << "- mesh_dim_x = " << NoximGlobalParams::mesh_dim_x << endl;
    cout << "- mesh_dim_y = " << NoximGlobalParams::mesh_dim_y << endl;
    /***3D***/
	cout << "- mesh_dim_z = " << NoximGlobalParams::mesh_dim_z << endl; ////
	cout << "- group_neu_num = " << NoximGlobalParams::group_neu_num << endl; //** 2018.09.01 edit by Yueh-Chi,Yang **//
	//** 2018.09.02 edit by Yueh-Chi,Yang **//
	cout << "- NNmodel = " << NoximGlobalParams::NNmodel_filename << endl;
	cout << "- NNweight = " << NoximGlobalParams::NNweight_filename << endl;
	cout << "- mapping table = " << NoximGlobalParams::mapping_table_filename << endl;
	cout << "- mapping = " << NoximGlobalParams::mapping_algorithm << endl;
	cout << "- NNinput = " << NoximGlobalParams::NNinput_filename << endl;
	//**************************************//
	cout << "- buffer_depth = " << NoximGlobalParams::buffer_depth << endl;
    cout << "- max_packet_size = " << NoximGlobalParams::
	max_packet_size << endl;
    cout << "- routing_algorithm = " << NoximGlobalParams::
	routing_algorithm << endl;
    //  cout << "- routing_table_filename = " << NoximGlobalParams::routing_table_filename << endl;
    cout << "- selection_strategy = " << NoximGlobalParams::
	selection_strategy << endl;
    cout << "- packet_injection_rate = " << NoximGlobalParams::
	packet_injection_rate << endl;
    cout << "- probability_of_retransmission = " << NoximGlobalParams::
	probability_of_retransmission << endl;
    cout << "- traffic_distribution = " << NoximGlobalParams::
	traffic_distribution << endl;
    cout << "- simulation_time = " << NoximGlobalParams::
	simulation_time << endl;
    cout << "- stats_warm_up_time = " << NoximGlobalParams::
	stats_warm_up_time << endl;
    cout << "- rnd_generator_seed = " << NoximGlobalParams::
	rnd_generator_seed << endl;
	/***THROTTLING***/
	cout << "- throttling_type = " << NoximGlobalParams::throt_type << endl;
}

void checkInputParameters()
{
    if (NoximGlobalParams::mesh_dim_x <= 1) {
	cerr << "Error: dimx must be greater than 1" << endl;
	exit(1);
    }

    if (NoximGlobalParams::mesh_dim_y <= 1) {
	cerr << "Error: dimy must be greater than 1" << endl;
	exit(1);
    }
	
	if(NoximGlobalParams::group_neu_num < 1) {	//** 2018.09.01 edit by Yueh-Chi,Yang **//
		cerr << "Error: groupsize must be >= 1" << endl;
		exit(1);
	}
	
	//** 2018.09.02 edit by Yueh-Chi,Yang **//
	if(strlen(NoximGlobalParams::NNmodel_filename) <= 4) {
		cerr << "Error: model file name is invalid" << endl;
		exit(1);
	}
	if(strlen(NoximGlobalParams::NNweight_filename) <= 4) {
		cerr << "Error: weight file name is invalid" << endl;
		exit(1);
	}
	if(strlen(NoximGlobalParams::mapping_table_filename) <= 4){
		if(strlen(NoximGlobalParams::mapping_algorithm) <=3) {
			cerr << "Error: mapping algorithm is invalid" << endl;
			exit(1);
		}
	}

	//**************************************//
	
    if (NoximGlobalParams::buffer_depth < 1) {
	cerr << "Error: buffer must be >= 1" << endl;
	exit(1);
    }

    if (NoximGlobalParams::min_packet_size < 2 ||
	NoximGlobalParams::max_packet_size < 2) {
	cerr << "Error: packet size must be >= 2" << endl;
	exit(1);
    }

    if (NoximGlobalParams::min_packet_size >
	NoximGlobalParams::max_packet_size) {
	cerr << "Error: min packet size must be less than max packet size"
	    << endl;
	exit(1);
    }

    if (NoximGlobalParams::routing_algorithm == INVALID_ROUTING) {
	cerr << "Error: invalid routing algorithm" << endl;
	exit(1);
    }

    if (NoximGlobalParams::selection_strategy == INVALID_SELECTION) {
	cerr << "Error: invalid selection policy" << endl;
	exit(1);
    }

    if (NoximGlobalParams::packet_injection_rate <= 0.0 ||
	NoximGlobalParams::packet_injection_rate > 1.0) {
	cerr <<
	    "Error: packet injection rate mmust be in the interval [0,1]"
	    << endl;
	exit(1);
    }

    if (NoximGlobalParams::traffic_distribution == INVALID_TRAFFIC) {
	cerr << "Error: invalid traffic" << endl;
	exit(1);
    }

    for (unsigned int i = 0; i < NoximGlobalParams::hotspots.size(); i++) {
	if (NoximGlobalParams::hotspots[i].first >=
	    NoximGlobalParams::mesh_dim_x *
	    NoximGlobalParams::mesh_dim_y) {
	    cerr << "Error: hotspot node " << NoximGlobalParams::
		hotspots[i].first << " is invalid (out of range)" << endl;
	    exit(1);
	}
	if (NoximGlobalParams::hotspots[i].second < 0.0
	    && NoximGlobalParams::hotspots[i].second > 1.0) {
	    cerr <<
		"Error: hotspot percentage must be in the interval [0,1]"
		<< endl;
	    exit(1);
	}
    }

    if (NoximGlobalParams::stats_warm_up_time < 0) {
	cerr << "Error: warm-up time must be positive" << endl;
	exit(1);
    }

    if (NoximGlobalParams::simulation_time < 0) {
	cerr << "Error: simulation time must be positive" << endl;
	exit(1);
    }

    if (NoximGlobalParams::stats_warm_up_time >
	NoximGlobalParams::simulation_time) {
	cerr << "Error: warmup time must be less than simulation time" <<
	    endl;
	exit(1);
    }
}

//---------------------------------------------------------------------------

void parseCmdLine(int arg_num, char *arg_vet[])
{
    if (arg_num == 1)
	cout <<
	    "Running with default parameters (use '-help' option to see how to override them)"
	    << endl;
    else {
	for (int i = 1; i < arg_num; i++) {
	    if (!strcmp(arg_vet[i], "-help")) {
		showHelp(arg_vet[0]);
		exit(0);
	    } else if (!strcmp(arg_vet[i], "-verbose"))
			NoximGlobalParams::verbose_mode = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-trace")) {
			NoximGlobalParams::trace_mode = true;
			strcpy(NoximGlobalParams::trace_filename, arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-dimx"))
			NoximGlobalParams::mesh_dim_x = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-dimy"))
			NoximGlobalParams::mesh_dim_y = atoi(arg_vet[++i]);
		/***3D***/
		else if (!strcmp(arg_vet[i], "-dimz"))
			NoximGlobalParams::mesh_dim_z = atoi(arg_vet[++i]);
		/***3D***/
		
		//************ 2018.09.01 edit by Yueh-Chi,Yang ************//
		else if (!strcmp(arg_vet[i], "-groupsize"))				
			NoximGlobalParams::group_neu_num = atoi(arg_vet[++i]);
		//**********************************************************//
		//************ 2018.09.01 edit by Yueh-Chi,Yang ************//
		else if (!strcmp(arg_vet[i], "-NNmodel")) 
			strcpy(NoximGlobalParams::NNmodel_filename, arg_vet[++i]); 
		else if (!strcmp(arg_vet[i], "-NNweight"))
			strcpy(NoximGlobalParams::NNweight_filename, arg_vet[++i]); 
		else if (!strcmp(arg_vet[i], "-mapping")){
			strcpy(NoximGlobalParams::mapping_algorithm, arg_vet[++i]); 
			if (!strcmp(NoximGlobalParams::mapping_algorithm, "dir_x")) 
				cout<<"mapping_algorithm => dir_x"<<endl;
			else if (!strcmp(NoximGlobalParams::mapping_algorithm, "dir_y")) 
				cout<<"mapping_algorithm => dir_y"<<endl;
			else if (!strcmp(NoximGlobalParams::mapping_algorithm, "random")) 
				cout<<"mapping_algorithm => random"<<endl;
			else if (!strcmp(NoximGlobalParams::mapping_algorithm, "table")) 
				strcpy(NoximGlobalParams::mapping_table_filename, arg_vet[++i]);
			else{
				strcpy(NoximGlobalParams::mapping_algorithm, "INVALID_ROUTING");
				cout<<"mapping_algorithm => INVALID!!!"<<endl;
			}
		}
		else if (!strcmp(arg_vet[i], "-NNinput"))
			strcpy(NoximGlobalParams::NNinput_filename, arg_vet[++i]);
		else if (!strcmp(arg_vet[i], "-PEcomptime"))
			NoximGlobalParams::PE_computation_time = atoi(arg_vet[++i]);
		//**********************************************************//
		
	    else if (!strcmp(arg_vet[i], "-buffer"))
			NoximGlobalParams::buffer_depth = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-size")) {
			NoximGlobalParams::min_packet_size = atoi(arg_vet[++i]);
			NoximGlobalParams::max_packet_size = atoi(arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-routing")) {
		char *routing = arg_vet[++i];
		/****************MODIFY BY HUI-SHUN********************/
		//if (!strcmp(routing, "xy"))
		//    NoximGlobalParams::routing_algorithm = ROUTING_XY;
//****************tyty*********************//
		if (!strcmp(routing, "xyx"))
		    NoximGlobalParams::routing_algorithm = 
			ROUTING_XYX;
//*****************************************//
		else if (!strcmp(routing, "xyz"))
		    NoximGlobalParams::routing_algorithm = 
			ROUTING_XYZ;
		else if (!strcmp(routing, "zxy"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_ZXY;
		else if(!strcmp(arg_vet[i+1],"downward")) 
			NoximGlobalParams::routing_algorithm = 
			ROUTING_DOWNWARD;
		else if(!strcmp(arg_vet[i+1],"oe_downward"))
			NoximGlobalParams::routing_algorithm = 
			ROUTING_ODD_EVEN_DOWNWARD;
		else if(!strcmp(arg_vet[i+1],"oe_3d"))
			NoximGlobalParams::routing_algorithm = 
			ROUTING_ODD_EVEN_3D;
		else if(!strcmp(arg_vet[i+1],"oe_z")) 
			NoximGlobalParams::routing_algorithm = 
			ROUTING_ODD_EVEN_Z;
		else if(!strcmp(arg_vet[i+1],"proposed")) 
			NoximGlobalParams::routing_algorithm = 
			ROUTING_PROPOSED;
			
        /****************MODIFY BY HUI-SHUN********************/		
		else if (!strcmp(routing, "westfirst"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_WEST_FIRST;
		else if (!strcmp(routing, "northlast"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_NORTH_LAST;
		else if (!strcmp(routing, "negativefirst"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_NEGATIVE_FIRST;
		else if (!strcmp(routing, "oddeven"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_ODD_EVEN;
		else if (!strcmp(routing, "dyad")) {
		    NoximGlobalParams::routing_algorithm = ROUTING_DYAD;
		    NoximGlobalParams::dyad_threshold = atof(arg_vet[++i]);
		}
		else if (!strcmp(routing, "fullyadaptive"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_FULLY_ADAPTIVE;
		else if (!strcmp(routing, "DLADR"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_DOWNWARD_CROSS_LAYER;
		else if (!strcmp(routing, "DLADR_HS"))
		    NoximGlobalParams::routing_algorithm =
			ROUTING_DOWNWARD_CROSS_LAYER_HS;	
		else if (!strcmp(routing, "table")) {
		    NoximGlobalParams::routing_algorithm =
			ROUTING_TABLE_BASED;
		    strcpy(NoximGlobalParams::routing_table_filename, arg_vet[++i]);
		    NoximGlobalParams::packet_injection_rate = 0;	// ??? why ???
		} else
		    NoximGlobalParams::routing_algorithm = INVALID_ROUTING;
	    } else if (!strcmp(arg_vet[i], "-sel")) {
		char *selection = arg_vet[++i];
		if (!strcmp(selection, "random"))
		    NoximGlobalParams::selection_strategy = SEL_RANDOM;
		else if (!strcmp(selection, "bufferlevel"))
		    NoximGlobalParams::selection_strategy =
			SEL_BUFFER_LEVEL;
		else if (!strcmp(selection, "nop"))
		    NoximGlobalParams::selection_strategy = SEL_NOP;
		else if(!strcmp(arg_vet[i+1],"rca")) 
			NoximGlobalParams::selection_strategy = 
			SEL_RCA;
		else
		    NoximGlobalParams::selection_strategy =
			INVALID_SELECTION;
	    } else if (!strcmp(arg_vet[i], "-pir")) {
			NoximGlobalParams::packet_injection_rate =
		    atof(arg_vet[++i]);
			char *distribution = arg_vet[++i];
			if (!strcmp(distribution, "poisson"))
		    NoximGlobalParams::probability_of_retransmission =
			NoximGlobalParams::packet_injection_rate;
		else if (!strcmp(distribution, "burst")) {
		    float burstness = atof(arg_vet[++i]);
		    NoximGlobalParams::probability_of_retransmission =
			NoximGlobalParams::packet_injection_rate / (1 - burstness);
		} else if (!strcmp(distribution, "pareto")) {
		    float Aon = atof(arg_vet[++i]);
		    float Aoff = atof(arg_vet[++i]);
		    float r = atof(arg_vet[++i]);
		    NoximGlobalParams::probability_of_retransmission =
			NoximGlobalParams::packet_injection_rate *
			pow((1 - r), (1 / Aoff - 1 / Aon));
		} else if (!strcmp(distribution, "custom"))
		    NoximGlobalParams::probability_of_retransmission =
			atof(arg_vet[++i]);
	    } else if (!strcmp(arg_vet[i], "-traffic")) {
		char *traffic = arg_vet[++i];
		if (!strcmp(traffic, "random"))
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_RANDOM;
		/****************MODIFY BY HUI-SHUN********************/		
		else if(!strcmp(arg_vet[i+1],"random_tvar")) 
			NoximGlobalParams::traffic_distribution = 
			TRAFFIC_RANDOM_TVAR;
	 	else if(!strcmp(arg_vet[i+1],"random_2")) 
			NoximGlobalParams::traffic_distribution = 
			TRAFFIC_RANDOM_2;	
		/****************MODIFY BY HUI-SHUN********************/		
		else if (!strcmp(traffic, "transpose1"))
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_TRANSPOSE1;
		else if (!strcmp(traffic, "transpose2"))
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_TRANSPOSE2;
		else if (!strcmp(traffic, "bitreversal"))
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_BIT_REVERSAL;
		else if (!strcmp(traffic, "butterfly"))
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_BUTTERFLY;
		else if (!strcmp(traffic, "shuffle"))
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_SHUFFLE;
		else if (!strcmp(traffic, "table")) {
		    NoximGlobalParams::traffic_distribution =
			TRAFFIC_TABLE_BASED;
		    strcpy(NoximGlobalParams::traffic_table_filename,
			   arg_vet[++i]);
		} else
		    NoximGlobalParams::traffic_distribution =
			INVALID_TRAFFIC;
	    } else if (!strcmp(arg_vet[i], "-hs")) {
			int node = atoi(arg_vet[++i]);
			double percentage = atof(arg_vet[++i]);
			pair < int, double >t(node, percentage);
			NoximGlobalParams::hotspots.push_back(t);
	    } else if (!strcmp(arg_vet[i], "-warmup"))
			NoximGlobalParams::stats_warm_up_time = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-seed"))
			NoximGlobalParams::rnd_generator_seed = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-detailed"))
			NoximGlobalParams::detailed = true;
	    else if (!strcmp(arg_vet[i], "-volume"))
			NoximGlobalParams::max_volume_to_be_drained =
		    atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i], "-sim"))
			NoximGlobalParams::simulation_time = atoi(arg_vet[++i]);
	    else if (!strcmp(arg_vet[i++],"-throt")) {// traffic throttling
  			if (!strcmp(arg_vet[i],"normal")) 
				NoximGlobalParams::throt_type = THROT_NORMAL;
  			else if (!strcmp(arg_vet[i],"global"))
				NoximGlobalParams::throt_type = THROT_GLOBAL;
			else if (!strcmp(arg_vet[i],"distributed")) 
				NoximGlobalParams::throt_type = THROT_DISTRIBUTED;
 			else if (!strcmp(arg_vet[i],"vertical")) 
				NoximGlobalParams::throt_type = THROT_VERTICAL;
  			else 
				NoximGlobalParams::throt_type = NOT_VALID;	
      	}// end traffic throttling
		
		
		
		
		
		else {
		cerr << "Error: Invalid option: " << arg_vet[i] << endl;
		exit(1);
	    }
	}
}

    checkInputParameters();

    // Show configuration
    if (NoximGlobalParams::verbose_mode > VERBOSE_OFF)
	showConfig();
}
