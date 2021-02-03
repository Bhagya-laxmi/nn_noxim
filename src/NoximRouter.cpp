/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the router
 */

#include "NoximRouter.h"

void NoximRouter::rxProcess()
{
	int i,j,k;
    if (reset.read()) {
	// Clear outputs and indexes of receiving protocol
	for ( i = 0; i < DIRECTIONS + 1; i++) {
	    //ack_rx[i].write(0);
		ack_rx[i].write(1);
	    current_level_rx[i] = 0;
	}
		/*******DOWNWARD ROUTING******/
		/*******THROTTLING******/
		for( i=0; i<4; i++){
			//on_off[i].write(0);
			DW_tag[i].write(NoximGlobalParams::down_level);
			}
		/*******THROTTLING******/
		/*******DOWNWARD ROUTING******/
	reservation_table.clear();
	routed_flits = 0;
	local_drained = 0;
    } else {
	/*******DOWNWARD ROUTING******/
	for( i=0; i<4; i++){
		if( DW_tag_neighbor[i].read() < DW_tag_cur && DW_tag_neighbor[i].read() >= 0){
					//DW level propagation: ��ܾF�~�Φۤv��DW_tag�ȸ��p������																																								 //�����s��DW level (��j���i��|NW saturation)
//					cout<<"["<<local_id<<"]DW_tag: "<<DW_tag_cur<<"->"<<DW_tag_neighbor[i].read()<<endl;
					DW_tag_cur = DW_tag_neighbor[i].read();
					for( j=0; j<4; j++)
						DW_tag[j].write(DW_tag_cur);
				}
			}
	/*******DOWNWARD ROUTING******/
	// For each channel decide if a new flit can be accepted
	//
	// This process simply sees a flow of incoming flits. All arbitration
	// and wormhole related issues are addressed in the txProcess()

	for ( i = 0; i < DIRECTIONS + 1; i++) {
	    // To accept a new flit, the following conditions must match:
	    //
	    // 1) there is an incoming request
	    // 2) there is a free slot in the input buffer of direction i

	    //if ((req_rx[i].read() == 1 - current_level_rx[i])&& !buffer[i].IsFull()) {
		if ( (req_rx[i].read()==1) && ack_rx[i].read()==1){
		if ( (i!=DIRECTION_LOCAL && !throttle_neighbor)	||i==DIRECTION_LOCAL)		//check throttling 
//#endif
			{
		NoximFlit received_flit = flit_rx[i].read();
		
		if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
		    cout << getCurrentCycleNum() << ": Router[" << local_id << "], Input[" << i
			<< "], Received flit: " << received_flit << endl;
		}
		// Store the incoming flit in the circular buffer
		buffer[i].Push(received_flit);

		// Negate the old value for Alternating Bit Protocol (ABP)
		//current_level_rx[i] = 1 - current_level_rx[i];

		// Incoming flit
		if ( i != DIRECTION_UP && i != DIRECTION_DOWN){
			stats.power.Msint();
			stats.power.QueuesNDataPath();	
			stats.power.Clocking();
			//stats.power.Incoming();
			}
			
			
			
			}
	    }
	    //ack_rx[i].write(current_level_rx[i]);
		 ack_rx[i].write((!buffer[i].IsFull()) && (!throttle_neighbor) );
	}
    }
	if ( !throttle_neighbor)
	{
	stats.power.LeakageRouter();		
  	stats.power.LeakageFPMAC();			
	stats.power.LeakageMEM(); 
    //stats.power.Standby();
	}
}

void NoximRouter::txProcess()
{
    if (reset.read()) {
		/*--------------Debugging----------------*/
			char fileID_r[15] = "Trial_";
			char fileID_t[5];
			//cout<<"Local Id: "<<local_id<<endl;
			sprintf(fileID_t,"%d",local_id);
			//cout<<"File Id: "<<fileID_t<<endl;
			strcat(fileID_r, fileID_t);
			remove(fileID_r);
			/*------------------------------------*/
	// Clear outputs and indexes of transmitting protocol
	for (int i = 0; i < DIRECTIONS + 1; i++) {
	    req_tx[i].write(0);
	    current_level_tx[i] = 0;
	}
    } else {
	// 1st phase: Reservation
	for (int j = 0; j < DIRECTIONS + 1; j++) {
	    int i = (start_from_port + j) % (DIRECTIONS + 1);

	    if (!buffer[i].IsEmpty()) {
		NoximFlit flit = buffer[i].Front();

		if (flit.flit_type == FLIT_TYPE_HEAD) {
		    // prepare data for routing
		    NoximRouteData route_data;
		    route_data.current_id = local_id;
		    route_data.src_id = flit.src_id;
		    route_data.dst_id = flit.dst_id;
		    route_data.dir_in = i;

		    route_data.XYX_routing = flit.XYX_routing;	//tytyty

			route_data.routing = flit.routing_f;

		    int o = route(route_data);

			/*--------------Debugging----------------*/
			/*if(local_id == 20 )
			{
				cout<<"------------------------"<<endl;
				cout<<"XYX: "<<flit.XYX_routing<<endl;
				cout<<"Direc: "<<o<<" "<<flit.dst_id<<endl;
				cout<<"Default: "<<NoximGlobalParams::routing_algorithm<<endl;
			}*/

			char fileID_r[15] = "Trial_";
			char fileID_t[5];
			//cout<<"Local Id: "<<local_id<<endl;
			sprintf(fileID_t,"%d",local_id);
			//cout<<"File Id: "<<fileID_t<<endl;
			strcat(fileID_r, fileID_t);
			fstream file_t;
			file_t.open( fileID_r ,ios::out|ios::app);
			file_t << getCurrentCycleNum()
				<< ": Router[" << local_id
				<< "], Input[" << i << "] (" << buffer[i].
				Size() << " flits)" << ", reserved Output["
				<< o << "], flit: " << flit <<" XYX: "<<route_data.XYX_routing<< endl;
			/*----------------------------------------*/

		    if (reservation_table.isAvailable(o)) {
			reservation_table.reserve(i, o);
			if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
			    cout << getCurrentCycleNum()
				<< ": Router[" << local_id
				<< "], Input[" << i << "] (" << buffer[i].
				Size() << " flits)" << ", reserved Output["
				<< o << "], flit: " << flit << endl;
			}
			stats.power.ArbiterNControl();	
		    }
		}
	    }
	}
	start_from_port++;

	// 2nd phase: Forwarding
	  for(int o=0; o<DIRECTIONS+1; o++)
        if ( 1 == ack_tx[o].read())// && req_tx[o].read() == 1)
          req_tx[o].write(0);
	for (int i = 0; i < DIRECTIONS + 1; i++) {
	    if (!buffer[i].IsEmpty()) {
		NoximFlit flit = buffer[i].Front();

		int o = reservation_table.getOutputPort(i);
		if (o != NOT_RESERVED) {
		    if ( /*current_level_tx[o]*/ 1 == ack_tx[o].read()) {
			if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
			    cout << getCurrentCycleNum()
				<< ": Router[" << local_id
				<< "], Input[" << i <<
				"] forward to Output[" << o << "], flit: "
				<< flit << endl;
			}

			flit_tx[o].write(flit);
			//current_level_tx[o] = 1 - current_level_tx[o];
			//req_tx[o].write(current_level_tx[o]);
			req_tx[o].write(!buffer[i].IsEmpty());
			buffer[i].Pop();
			//------ hfsken <start>
				if( o != DIRECTION_UP && o != DIRECTION_DOWN ){
					stats.power.Crossbar();	
					stats.power.Links();	
					//stats.power.Forward();					
					}
			//------ hfsken <end>	
			
			if (flit.flit_type == FLIT_TYPE_TAIL)
			    reservation_table.release(o);

			// Update stats
			if (o == DIRECTION_LOCAL) {
			    stats.receivedFlit(getCurrentCycleNum(), flit);
				//------ hfsken <start>			
				stats.power.DualFpmacs();	
	      		stats.power.Imem();						
	      		stats.power.Dmem();				
	      		stats.power.RF();			
	      		stats.power.ClockDistribution();
				//------ hfsken <end>
			    if (NoximGlobalParams::
				max_volume_to_be_drained) {
				if (drained_volume >=
				    NoximGlobalParams::
				    max_volume_to_be_drained)
				    sc_stop();
				else {
				    drained_volume++;
				    local_drained++;
				}
			    }
			} else if (i != DIRECTION_LOCAL) {
			    // Increment routed flits counter
			    routed_flits++;
			}
		    }
		}
	    }
	}
    }				// else
    //stats.power.Standby();
}


NoximNoP_data NoximRouter::getCurrentNoPData() const
{
    NoximNoP_data NoP_data;

    for (int j = 0; j < DIRECTIONS; j++) {
	NoP_data.channel_status_neighbor[j].free_slots =
	    free_slots_neighbor[j].read();
	NoP_data.channel_status_neighbor[j].available =
	    (reservation_table.isAvailable(j));
    }

    NoP_data.sender_id = local_id;

    return NoP_data;
}

void NoximRouter::bufferMonitor()
{	
	int i;
    if (reset.read()) {
	for ( i = 0; i < DIRECTIONS + 1; i++)
	    free_slots[i].write(buffer[i].GetMaxBufferSize());
    } else {

	if (NoximGlobalParams::selection_strategy == SEL_BUFFER_LEVEL ||
	    NoximGlobalParams::selection_strategy == SEL_NOP ||
		/*******RCA******/ 
		NoximGlobalParams::selection_strategy==SEL_RCA) {

	    // update current input buffers level to neighbors
	    for ( i = 0; i < DIRECTIONS + 1; i++)
		free_slots[i].write(buffer[i].getCurrentFreeSlots());

	    // NoP selection: send neighbor info to each direction 'i'
	    NoximNoP_data current_NoP_data = getCurrentNoPData();

	    for ( i = 0; i < DIRECTIONS; i++)
		NoP_data_out[i].write(current_NoP_data);
		/***UNKNOWN***/
		if (NoximGlobalParams::verbose_mode == -57) 
			NoP_report();
		/***UNKNOWN***/
	}
    }
}

vector <
    int >NoximRouter::routingFunction(const NoximRouteData & route_data)
{
    NoximCoord position = id2Coord(route_data.current_id);
    NoximCoord src_coord = id2Coord(route_data.src_id);
    NoximCoord dst_coord = id2Coord(route_data.dst_id);
    int dir_in = route_data.dir_in;
	int routing   = route_data.routing;  //Matthew

	//cout<< "Routing alg: "<<NoximGlobalParams::routing_algorithm<<endl;
    switch (NoximGlobalParams::routing_algorithm) {
    /*case ROUTING_XY:
	return routingXY(position, dst_coord);*/
	/***ACCESS IC LAB's Routing Algorithm***/

/******************tytytytyty**************************/
	case ROUTING_XYX:
      return routingXYX(position, dst_coord,route_data.XYX_routing);
/********************************************/
	case ROUTING_XYZ:
      return routingXYZ(position, dst_coord);
      
    case ROUTING_ZXY:
      return routingZXY(position, dst_coord);
           
    case ROUTING_DOWNWARD:
    	return routingDownward(position, dst_coord, src_coord);
	
	case ROUTING_ODD_EVEN_DOWNWARD:
      return routingOddEven_Downward(position, src_coord, dst_coord, route_data);

    case ROUTING_ODD_EVEN_3D:
      return routingOddEven_3D(position, src_coord, dst_coord);

	case ROUTING_ODD_EVEN_Z:
	  return routingOddEven_Z(position, src_coord, dst_coord);
	
	case ROUTING_PROPOSED:
      return routingProposed(position, src_coord, dst_coord);

	/***ACCESS IC LAB's Routing Algorithm***/

    case ROUTING_WEST_FIRST:
	return routingWestFirst(position, dst_coord);

    case ROUTING_NORTH_LAST:
	return routingNorthLast(position, dst_coord);

    case ROUTING_NEGATIVE_FIRST:
	return routingNegativeFirst(position, dst_coord);

    case ROUTING_ODD_EVEN:
	return routingOddEven(position, src_coord, dst_coord);

    case ROUTING_DYAD:
	return routingDyAD(position, src_coord, dst_coord);

    case ROUTING_FULLY_ADAPTIVE:
	return routingFullyAdaptive(position, dst_coord);

    case ROUTING_TABLE_BASED:
	return routingTableBased(dir_in, position, dst_coord);
	
	case ROUTING_CROSS_LAYER:
    return routingCrossLayer(routing, position, dst_coord, src_coord);

	case ROUTING_DOWNWARD_CROSS_LAYER:
	return routingDownward_CrossLayer(position, dst_coord, src_coord);

	case ROUTING_DOWNWARD_CROSS_LAYER_HS:
	return routingDownward_CrossLayer_HS(position, dst_coord, src_coord);
	
    default:
	assert(false);
    }

    // something weird happened, you shouldn't be here
    return (vector < int >) (0);
}

int NoximRouter::route(const NoximRouteData & route_data)
{
    //stats.power.Routing();

    if (route_data.dst_id == local_id)
	return DIRECTION_LOCAL;

    vector < int >candidate_channels = routingFunction(route_data);
	//cout<<"candidate: "<<candidate_channels[0]<<endl;

    return selectionFunction(candidate_channels, route_data);
}

void NoximRouter::NoP_report() const
{
    NoximNoP_data NoP_tmp;
    cout << getCurrentCycleNum() << ": Router[" << local_id << "] NoP report: " << endl;

    for (int i = 0; i < DIRECTIONS; i++) {
	NoP_tmp = NoP_data_in[i].read();
	if (NoP_tmp.sender_id != NOT_VALID)
	    cout << NoP_tmp;
    }
}

void NoximRouter::RCA_Aggregation()
{
	int i,j;
	if(reset.read()){
		for(i=0; i < DIRECTIONS; i++)
			monitor_out[i].write(0);

		buffer_util = 0;

	}
	else{
		NoximCoord position = id2Coord(local_id);
		double throt_in[DIRECTIONS];
		for(int j=0; j < DIRECTIONS; j++)
			throt_in[j] = monitor_in[j].read();

		/*for(i=0; i < DIRECTIONS; i++) {
			monitor_out[i].write((stats.last_temperature - stats.temperature));

		}*/


		int self_throt;
		if(emergency)
			self_throt = 1;
		else
			self_throt = 0;
		

		double throt_out[DIRECTIONS];
		for(int j=0; j < DIRECTIONS; j++){
			if((throt_in[j] != 0)||(self_throt != 0)) //OR gate for throttling awareness
				throt_out[j] = 1;
			else
				throt_out[j] = 0;
		}

		monitor_out[DIRECTION_NORTH].write(throt_out[DIRECTION_SOUTH]);
		monitor_out[DIRECTION_SOUTH].write(throt_out[DIRECTION_NORTH]);
		monitor_out[DIRECTION_EAST].write(throt_out[DIRECTION_WEST]);
		monitor_out[DIRECTION_WEST].write(throt_out[DIRECTION_EAST]);

		//cout<<getCurrentCycleNum()<<endl;
		double total_size  = 0;
		buffer_used	= 0;
		for(int j=0; j < 4;j++)
		{
			buffer_used += buffer[j].GetMaxBufferSize() - buffer[j].getCurrentFreeSlots();
			total_size	+= buffer[j].GetMaxBufferSize();
		}
		buffer_util += buffer_used / total_size;
		//cout<<"("<<position.x<<", "<<position.y<<", "<<position.z<<"): "<<buffer_util<<endl;
	}
	

}
//---------------------------------------------------------------------------

int NoximRouter::NoPScore(const NoximNoP_data & nop_data,
			  const vector < int >&nop_channels) const
{
    int score = 0;
	/***UNKNOWN***/
	if (NoximGlobalParams::verbose_mode==-58){
		cout << nop_data;
		cout << "      On-Path channels: " << endl;
    }
	/***UNKNOWN***/
    for (unsigned int i = 0; i < nop_channels.size(); i++) {
		int available;
		if (nop_data.channel_status_neighbor[nop_channels[i]].available)
			available = 1;
		else
			available = 0;
		int free_slots = nop_data.channel_status_neighbor[nop_channels[i]].free_slots;
		/***UNKNOWN***/
		double temp_diff = nop_data.channel_status_neighbor[nop_channels[i]].temperature;
		int counter = nop_data.channel_status_neighbor[nop_channels[i]].channel_count;
		double not_throttle;
		if (NoximGlobalParams::verbose_mode==-58){
			cout << "       channel " << nop_channels[i] << " -> score: ";
			cout << " + " << available << " * (" << free_slots << ")" << endl;
		}
		if((nop_channels[i] == DIRECTION_DOWN) || (nop_channels[i] == DIRECTION_UP)){  //Throttled = 0, Not throttled = 1
			not_throttle = 1;}
		else{
			//not_throttle = !(nop_data.channel_status_neighbor[nop_channels[i]].throttle);
			int b;
			b=monitor_in[i].read();
			if(b==1)
				not_throttle = 0;
			else
				not_throttle = 1;
		}
		double    temp_decision = temp_diff;
		if(temp_diff >= 2)
			temp_decision = 2;
		else if(temp_diff <= -2)
			temp_decision = -2;
		temp_decision = temp_decision + 2;
		double tune_ratio = 0.66;
		int max_buff = buffer[nop_channels[i]].GetMaxBufferSize();
		score += not_throttle*available*free_slots; //traffic-&throttling-aware
		/***UNKNOWN***/
		//score += available * free_slots;
    }

    return score;
}
/*Selection of RCA*/
int NoximRouter::RCA_selection(const vector<int>& directions , const NoximRouteData& route_data)
{
	vector<int>  best_dirs;
	int          max_score = 0;
	/*
	//cout<< directions.size()<<endl;

	for (unsigned int i=0; i<directions.size(); i++)
    {
		if(RCA_QUADRO == 0){
			double RCA_Sel = RCA_select[directions[i]];
			bool available = reservation_table.isAvailable(directions[i]);

			if (RCA_Sel > max_score) 
				{
					max_score = RCA_Sel;
					best_dirs.clear();
					best_dirs.push_back(directions[i]);
				}
			else if (RCA_Sel == max_score)
				best_dirs.push_back(directions[i]);
		}
		else if(RCA_QUADRO == 1){
			NoximCoord current_coord = id2Coord(route_data.current_id);
			NoximCoord dst_coord = id2Coord(route_data.dst_id);

			int e0, e1;

			e0 = dst_coord.x - current_coord.x;
			e1 = dst_coord.y - current_coord.y;

			if(directions[i] == DIRECTION_NORTH) {
				if(e0 > 0) {//Go East
					if (RCA_Sel_Q2 > max_score) 
						{
							max_score = RCA_Sel_Q2;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q2 == max_score)
						best_dirs.push_back(directions[i]);
				}
				else{//Go West
					if (RCA_Sel_Q1 > max_score) 
						{
							max_score = RCA_Sel_Q1;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q1 == max_score)
						best_dirs.push_back(directions[i]);
				}
			}
			else if(directions[i] == DIRECTION_SOUTH) {
				if(e0 > 0) {//Go East
					if (RCA_Sel_Q1 > max_score) 
						{
							max_score = RCA_Sel_Q1;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q1 == max_score)
						best_dirs.push_back(directions[i]);
				}
				else{//Go West
					if (RCA_Sel_Q2 > max_score) 
						{
							max_score = RCA_Sel_Q2;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q2 == max_score)
						best_dirs.push_back(directions[i]);
				}
			}
			else if(directions[i] == DIRECTION_WEST){
				if(e1 > 0) {//Go South
					if (RCA_Sel_Q1 > max_score) 
						{
							max_score = RCA_Sel_Q1;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q1 == max_score)
						best_dirs.push_back(directions[i]);
				}
				else{//Go North
					if (RCA_Sel_Q2 > max_score) 
						{
							max_score = RCA_Sel_Q2;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q2 == max_score)
						best_dirs.push_back(directions[i]);
				}
			}
			else if(directions[i] == DIRECTION_EAST){
				if(e1 > 0) {//Go South
					if (RCA_Sel_Q2 > max_score) 
						{
							max_score = RCA_Sel_Q2;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q2 == max_score)
						best_dirs.push_back(directions[i]);
				}
				else{//Go North
					if (RCA_Sel_Q1 > max_score) 
						{
							max_score = RCA_Sel_Q1;
							best_dirs.clear();
							best_dirs.push_back(directions[i]);
						}
					else if (RCA_Sel_Q1 == max_score)
						best_dirs.push_back(directions[i]);
				}
			}
		}
    }
	
	//cout<< best_dirs.size()<<endl;

	if (best_dirs.size())
		return(best_dirs[rand() % best_dirs.size()]);
	else 
	{
		/*for (unsigned int i=0; i<directions.size(); i++)
		{
			cout<<RCA_select[directions[i]]<<endl;
		}
		return(directions[rand() % directions.size()]);
	}
	*/
	return(directions[rand() % directions.size()]);
}
int NoximRouter::selectionNoP(const vector < int >&directions,
			      const NoximRouteData & route_data)
{
    vector < int >neighbors_on_path;
    vector < int >score;
    int direction_selected = NOT_VALID;

    int current_id = route_data.current_id;
	
    for (unsigned int i = 0; i < directions.size(); i++) {
	// get id of adjacent candidate
	int candidate_id = getNeighborId(current_id, directions[i]);

	// apply routing function to the adjacent candidate node
	NoximRouteData tmp_route_data;
	tmp_route_data.current_id = candidate_id;
	tmp_route_data.src_id = route_data.src_id;
	tmp_route_data.dst_id = route_data.dst_id;
	tmp_route_data.dir_in = reflexDirection(directions[i]);


	vector < int >next_candidate_channels =
	    routingFunction(tmp_route_data);

	// select useful data from Neighbor-on-Path input 
	NoximNoP_data nop_tmp = NoP_data_in[directions[i]].read();

	// store the score of node in the direction[i]
	score.push_back(NoPScore(nop_tmp, next_candidate_channels));
    }

    // check for direction with higher score
    int max_direction = directions[0];
    int max = score[0];
	/*******DOWNWARD ROUTING******/
	int down_score = 0;
	/*******DOWNWARD ROUTING******/
    bool neighbor_throttle = 0;
	for (unsigned int i = 0; i < directions.size(); i++) {
		/*******THROTTLING******/
		if(directions[i] != DIRECTION_DOWN  && directions[i] != DIRECTION_UP)
			neighbor_throttle = neighbor_throttle || on_off_neighbor[directions[i]];
		/*******THROTTLING******/
		if (score[i] > max) {
			max_direction = directions[i];
			max = score[i];
		}
    }

    // if multiple direction have the same score = max, choose randomly.

    vector < int >equivalent_directions;

    for (unsigned int i = 0; i < directions.size(); i++)
	if (score[i] == max)
	    equivalent_directions.push_back(directions[i]);

    direction_selected =
	equivalent_directions[rand() % equivalent_directions.size()];

    return direction_selected;
}

int NoximRouter::selectionBufferLevel(const vector < int >&directions)
{
    vector < int >best_dirs;
    int max_free_slots = 0;
    for (unsigned int i = 0; i < directions.size(); i++) {
	int free_slots = free_slots_neighbor[directions[i]].read();
	bool available = reservation_table.isAvailable(directions[i]);
	if (available) {
	    if (free_slots > max_free_slots) {
		max_free_slots = free_slots;
		best_dirs.clear();
		best_dirs.push_back(directions[i]);
	    } else if (free_slots == max_free_slots)
		best_dirs.push_back(directions[i]);
	}
    }

    if (best_dirs.size())
	return (best_dirs[rand() % best_dirs.size()]);
    else
	return (directions[rand() % directions.size()]);

    //-------------------------
    // TODO: unfair if multiple directions have same buffer level
    // TODO: to check when both available
//   unsigned int max_free_slots = 0;
//   int direction_choosen = NOT_VALID;

//   for (unsigned int i=0;i<directions.size();i++)
//     {
//       int free_slots = free_slots_neighbor[directions[i]].read();
//       if ((free_slots >= max_free_slots) &&
//        (reservation_table.isAvailable(directions[i])))
//      {
//        direction_choosen = directions[i];
//        max_free_slots = free_slots;
//      }
//     }

//   // No available channel 
//   if (direction_choosen==NOT_VALID)
//     direction_choosen = directions[rand() % directions.size()]; 

//   if(NoximGlobalParams::verbose_mode>VERBOSE_OFF)
//     {
//       NoximChannelStatus tmp;

//       cout << getCurrentCycleNum() << ": Router[" << local_id << "] SELECTION between: " << endl;
//       for (unsigned int i=0;i<directions.size();i++)
//      {
//        tmp.free_slots = free_slots_neighbor[directions[i]].read();
//        tmp.available = (reservation_table.isAvailable(directions[i]));
//        cout << "    -> direction " << directions[i] << ", channel status: " << tmp << endl;
//      }
//       cout << " direction choosen: " << direction_choosen << endl;
//     }

//   assert(direction_choosen>=0);
//   return direction_choosen;
}

int NoximRouter::selectionRandom(const vector < int >&directions)
{
    return directions[rand() % directions.size()];
}

int NoximRouter::selectionFunction(const vector < int >&directions,
				   const NoximRouteData & route_data)
{
    // not so elegant but fast escape ;)
    if (directions.size() == 1)
	return directions[0];

    //stats.power.Selection();
    switch (NoximGlobalParams::selection_strategy) {
    case SEL_RANDOM:
	return selectionRandom(directions);
    case SEL_BUFFER_LEVEL:
	return selectionBufferLevel(directions);
    case SEL_NOP:
	return selectionNoP(directions, route_data);
	case SEL_RCA:
	  return RCA_selection(directions,route_data);
	default:
	assert(false);
    }

    return 0;
}
/******************tyty-XYXrouting********************************/
vector < int >NoximRouter::routingXYX(const NoximCoord & current,
				     const NoximCoord & destination,const bool XYX_routing)
{
	
    vector < int >directions;
	if(XYX_routing)
	{
		if (destination.x > current.x)
			directions.push_back(DIRECTION_EAST);
    		else if (destination.x < current.x)
			directions.push_back(DIRECTION_WEST);
    		else if (destination.y > current.y)
			directions.push_back(DIRECTION_SOUTH);
    		else if (destination.y < current.y)
			directions.push_back(DIRECTION_NORTH);
	}
	else
	{
    		if (destination.y > current.y)
			directions.push_back(DIRECTION_SOUTH);
    		else if (destination.y < current.y)
			directions.push_back(DIRECTION_NORTH);
		else if (destination.x > current.x)
			directions.push_back(DIRECTION_EAST);
    		else if (destination.x < current.x)
			directions.push_back(DIRECTION_WEST);
	}

    return directions;
}
/*****************************************************/
vector < int >NoximRouter::routingXYZ(const NoximCoord & current,
				     const NoximCoord & destination)
{
    vector < int >directions;
	if (destination.x > current.x)
	directions.push_back(DIRECTION_EAST);
    else if (destination.x < current.x)
	directions.push_back(DIRECTION_WEST);
    else if (destination.y > current.y)
	directions.push_back(DIRECTION_SOUTH);
    else if (destination.y < current.y)
	directions.push_back(DIRECTION_NORTH);
	else if (destination.z > current.z)
	directions.push_back(DIRECTION_DOWN);
    else if (destination.z < current.z)
	directions.push_back(DIRECTION_UP);

    return directions;
}
vector<int> NoximRouter::routingZXY(const NoximCoord& current, const NoximCoord& destination)
{
  vector<int> directions;
  if (destination.z > current.z)
    directions.push_back(DIRECTION_DOWN);
  else if (destination.z < current.z)
    directions.push_back(DIRECTION_UP);
  else if (destination.x > current.x)
    directions.push_back(DIRECTION_EAST);
  else if (destination.x < current.x)
    directions.push_back(DIRECTION_WEST);
  else if (destination.y > current.y)
    directions.push_back(DIRECTION_SOUTH);
  else if (destination.y < current.y)
    directions.push_back(DIRECTION_NORTH);
  

  return directions;
}

vector<int> NoximRouter::routingDownward(const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source)
{

	int down_level = DW_tag_cur;	
	//if( down_level == 0)
	//cout<< "Now the level is selected as 0!!!"<<endl;
	vector<int> directions;
	int layer;	//means which layer that packet should be transmitted

	if( (source.z + down_level)> NoximGlobalParams::mesh_dim_z-1)
		layer = NoximGlobalParams::mesh_dim_z-1;
	else 
		layer = source.z + down_level;

	if(current.z < layer && (current.x != destination.x || current.y != destination.y) )  
		{

			if(current.z == destination.z && ( (current.x-destination.x== 1 && current.y==destination.y)	//while the dest. is one hop to source, don't downward and transmit directly
																			 ||(current.x-destination.x==-1 && current.y==destination.y) 
																			 ||(current.y-destination.y== 1 && current.x==destination.x) 
																			 ||(current.y-destination.y==-1 && current.x==destination.x) ))
			{
				directions = routingXYZ(current, destination);	 
			}
			else 
			{
				directions.push_back(DIRECTION_DOWN);
			}
		}
		else if(current.z >=layer && (current.x != destination.x || current.y != destination.y) )	//forward by xyz routing
		{
			 directions = routingXYZ(current, destination);	 
		}
		else if((current.x == destination.x && current.y == destination.y) && current.z > destination.z)	//same (x,y), forward to up
		{	
			directions.push_back(DIRECTION_UP);
		}	
		else if((current.x == destination.x && current.y == destination.y) && current.z < destination.z)  //same (x,y), forward to down
		{	
			directions.push_back(DIRECTION_DOWN);
		}	
		else 
		{ 
			cout<<"ERROR! Out of condition!?!?"<<endl;
			cout<<"current: ("<<current.x<<","<<current.y<<","<<current.z<<")"<<endl;
			cout<<"destination: ("<<destination.x<<","<<destination.y<<","<<destination.z<<")"<<endl;
			cout<<"layer: "<<layer<<endl;
			exit(1);			
		}

  return directions;
}

vector < int >NoximRouter::routingWestFirst(const NoximCoord & current,
					    const NoximCoord & destination)
{
    vector < int >directions;

    if (destination.x <= current.x || destination.y == current.y)
	return routingXYZ(current, destination);

    if (destination.y < current.y) {
	directions.push_back(DIRECTION_NORTH);
	directions.push_back(DIRECTION_EAST);
    } else {
	directions.push_back(DIRECTION_SOUTH);
	directions.push_back(DIRECTION_EAST);
    }

    return directions;
}

vector < int >NoximRouter::routingNorthLast(const NoximCoord & current,
					    const NoximCoord & destination)
{
    vector < int >directions;

    if (destination.x == current.x || destination.y <= current.y)
	return routingXYZ(current, destination);

    if (destination.x < current.x) {
	directions.push_back(DIRECTION_SOUTH);
	directions.push_back(DIRECTION_WEST);
    } else {
	directions.push_back(DIRECTION_SOUTH);
	directions.push_back(DIRECTION_EAST);
    }

    return directions;
}

vector < int >NoximRouter::routingNegativeFirst(const NoximCoord & current,
						const NoximCoord &
						destination)
{
    vector < int >directions;

    if ((destination.x <= current.x && destination.y <= current.y) ||
	(destination.x >= current.x && destination.y >= current.y))
	return routingXYZ(current, destination);

    if (destination.x > current.x && destination.y < current.y) {
	directions.push_back(DIRECTION_NORTH);
	directions.push_back(DIRECTION_EAST);
    } else {
	directions.push_back(DIRECTION_SOUTH);
	directions.push_back(DIRECTION_WEST);
    }

    return directions;
}

vector < int >NoximRouter::routingOddEven(const NoximCoord & current,
					  const NoximCoord & source,
					  const NoximCoord & destination)
{
    vector < int >directions;

    int c0 = current.x;
    int c1 = current.y;
    int s0 = source.x;
    //  int s1 = source.y;
    int d0 = destination.x;
    int d1 = destination.y;
    int e0, e1;

    e0 = d0 - c0;
    e1 = -(d1 - c1);

    if (e0 == 0) {
	if (e1 > 0)
	    directions.push_back(DIRECTION_NORTH);
	else
	    directions.push_back(DIRECTION_SOUTH);
    } else {
	if (e0 > 0) {
	    if (e1 == 0)
		directions.push_back(DIRECTION_EAST);
	    else {
		if ((c0 % 2 == 1) || (c0 == s0)) {
		    if (e1 > 0)
			directions.push_back(DIRECTION_NORTH);
		    else
			directions.push_back(DIRECTION_SOUTH);
		}
		if ((d0 % 2 == 1) || (e0 != 1))
		    directions.push_back(DIRECTION_EAST);
	    }
	} else {
	    directions.push_back(DIRECTION_WEST);
	    if (c0 % 2 == 0) {
		if (e1 > 0)
		    directions.push_back(DIRECTION_NORTH);
		if (e1 < 0)
		    directions.push_back(DIRECTION_SOUTH);
	    }
	}
    }

    if (!(directions.size() > 0 && directions.size() <= 2)) {
	cout << "\n PICCININI, CECCONI & ... :";	// STAMPACCHIA
	cout << source << endl;
	cout << destination << endl;
	cout << current << endl;

    }
    assert(directions.size() > 0 && directions.size() <= 2);

    return directions;
}
vector<int> NoximRouter::routingProposed(const NoximCoord& current, 
				    const NoximCoord& source, const NoximCoord& destination)
{
  vector<int> directions;

  int c0 = current.x;
  int c1 = current.y;
  //int s0 = source.x;
  //  int s1 = source.y;
  int d0 = destination.x;
  int d1 = destination.y;
  int e0, e1;

  e0 = d0 - c0;
  e1 = -(d1 - c1);

  if (e0 == 0)
    {
      if (e1 > 0)
	directions.push_back(DIRECTION_NORTH);
      else
	directions.push_back(DIRECTION_SOUTH);
    }
  else
    {
      if (e0 > 0)
	{
	  if (e1 == 0)
	    directions.push_back(DIRECTION_EAST);
	  else
	    {
	      if ( (c0 % 2 == 1) ) //|| (c0 == s0)
		{
		  if (e1 > 0)
		    directions.push_back(DIRECTION_NORTH);
		  else
		    directions.push_back(DIRECTION_SOUTH);
		}
	      if ( (d0 % 2 == 1) || (e0 != 1) )
		directions.push_back(DIRECTION_EAST);
	    }
	}
      else
	{
	  directions.push_back(DIRECTION_WEST);
	  if (c0 % 2 == 0)
	    {
	      if (e1 > 0)
		directions.push_back(DIRECTION_NORTH);
	      if (e1 < 0) 
		directions.push_back(DIRECTION_SOUTH);
	    }
	}
    }
  
  if (!(directions.size() > 0 && directions.size() <= 2))
  {
      cout << "\n STAMPACCHIO :";
      cout << source << endl;
      cout << destination << endl;
      cout << current << endl;

  }
  assert(directions.size() > 0 && directions.size() <= 2);
  
  return directions;
}
/*=============================Odd Even-3D==========================*/
vector<int> NoximRouter::routingOddEven_for_3D(const NoximCoord& current, 
				    const NoximCoord& source, const NoximCoord& destination)
{
  vector<int> directions;

  int c0 = current.y;
  int c1 = current.z;
  int s0 = source.y;
  //  int s1 = source.y;
  int d0 = destination.y;
  int d1 = destination.z;
  int e0, e1;

  e0 = d0 - c0;
  e1 = d1 - c1;

  if (e0 == 0)
    {
      if (e1 > 0)
		directions.push_back(DIRECTION_DOWN);
      else if (e1 < 0)
		directions.push_back(DIRECTION_UP);
    }
  else
    {
      if (e0 > 0)
	  {
		if (e1 == 0)
			directions.push_back(DIRECTION_SOUTH);
		else
			{
			if ( (c0 % 2 == 1) || (c0 == s0) ) //Odd
			{
			if (e1 > 0)
				directions.push_back(DIRECTION_DOWN);
			else if (e1 < 0)
				directions.push_back(DIRECTION_UP);
			}
			if ( (d0 % 2 == 1) || (e0 != 1) )
			directions.push_back(DIRECTION_SOUTH);
			}
	  }
      else
	  {
		directions.push_back(DIRECTION_NORTH);
		if (c0 % 2 == 0)
			{
			if (e1 > 0)
			directions.push_back(DIRECTION_DOWN);
			if (e1 < 0) 
			directions.push_back(DIRECTION_UP);
			}
	  }
    }
  
  /*if (!(directions.size() > 0 && directions.size() <= 2))
  {
      cout << "\n STAMPACCHIO :";
      cout << source << endl;
      cout << destination << endl;
      cout << current << endl;

  }
  assert(directions.size() > 0 && directions.size() <= 2);*/
  
  return directions;
}
vector<int> NoximRouter::routingOddEven_3D (const NoximCoord& current, 
				    const NoximCoord& source, const NoximCoord& destination)
{
  vector<int> directions;

  int c0 = current.x;
  int c1 = current.y;
  int c2 = current.z;
  int s0 = source.x;
  int s1 = source.y;
  int s2 = source.z;
  int d0 = destination.x;
  int d1 = destination.y;
  int d2 = destination.z; //z = 0, which is far from heat sink
  int e0, e1, e2;

  e0 = d0 - c0;
  e1 = -(d1 - c1); //positive: North, negative: South
  e2 = d2 - c2; //positive: Down, negative: Up

  if (e0 == 0)
  {
		directions = routingOddEven_for_3D(current, source, destination);
  }
  else 
  {
	if (e0 < 0) //x-
	{
		
		if ( (c0 % 2 == 0) )
		{
			directions = routingOddEven_for_3D(current, source, destination);
		}
		directions.push_back(DIRECTION_WEST);
		
		
	}
	else //e0 > 0, x+
	{
		if( (e1 == 0) && (e2 == 0) )
			directions.push_back(DIRECTION_EAST);	
		else
		{
			if ( (d0 % 2 == 1) || (e0 != 1) )
			{
				directions.push_back(DIRECTION_EAST);
			}
			if ( (c0 % 2 == 1) || (c0 == s0) )
			{
				directions = routingOddEven_for_3D(current, source, destination);
			}
		}
	}
  }
	
  /*    cout << source << endl;
      cout << destination << endl;
      cout << current << endl;
  //Output the directions
  vector<int>::iterator it;
	
  for ( it=directions.begin() ; it < directions.end(); it++ )
	  cout << " " << *it << endl;*/
  
 

  if (!(directions.size() > 0 && directions.size() <= 3))
  {
      cout << "\n STAMPACCHIO :";
      cout << source << endl;
      cout << destination << endl;
      cout << current << endl;

  }
  assert(directions.size() > 0 && directions.size() <= 3);
  
  return directions;
}
//=============================Odd Even + Downward==========================
//�ª��A�Y���쪺Downward tag

vector<int> NoximRouter::routingOddEven_Downward (const NoximCoord& current, 
				    const NoximCoord& source, const NoximCoord& destination, const NoximRouteData& route_data)
{
 int down_level = DW_tag_cur;	
	
	vector<int> directions;
	int layer;	//���ܦ�packet���Ӧb����layer��XY�ǻ�
	if( (source.z + down_level)> NoximGlobalParams::mesh_dim_z-1)
		layer = NoximGlobalParams::mesh_dim_z-1;
	else 
		layer = source.z + down_level;
	if(current.z < layer && (current.x != destination.x || current.y != destination.y) )  
	{

		if(current.z == destination.z && ( (current.x-destination.x== 1 && current.y==destination.y)	//X or Y��V�W�u�۶Z�@��ɴN�����L�h,��DW
										||(current.x-destination.x==-1 && current.y==destination.y) 
										||(current.y-destination.y== 1 && current.x==destination.x) 
										||(current.y-destination.y==-1 && current.x==destination.x) ))
		{
			directions = routingOddEven(current, source, destination);	 
		}
		else 
		{
					directions.push_back(DIRECTION_DOWN);
		}
	}
	else if(current.z >=layer && (current.x != destination.x || current.y != destination.y) )	//�b�����Hxy routing��
	{
		// route_data.flag_downward =1;
			directions = routingOddEven(current, source, destination);
	}
	else if((current.x == destination.x && current.y == destination.y) && current.z > destination.z)	//xy�ۦP, z��V���W��
	{	
		directions.push_back(DIRECTION_UP);
	}	
	else if((current.x == destination.x && current.y == destination.y) && current.z < destination.z)  //xy�ۦP, z��V���U��
	{	
		directions.push_back(DIRECTION_DOWN);
	}	
	else 
	{ 
		cout<<"ERROR! Out of condition!?!?"<<endl;
		cout<<"current: ("<<current.x<<","<<current.y<<","<<current.z<<")"<<endl;
		cout<<"destination: ("<<destination.x<<","<<destination.y<<","<<destination.z<<")"<<endl;
		cout<<"layer: "<<layer<<endl;
		exit(1);			
	}

  assert(directions.size() > 0 && directions.size() <= 3);
  
  return directions;
}
vector<int> NoximRouter::routingOddEven_Z (const NoximCoord& current, 
				    const NoximCoord& source, const NoximCoord& destination)
{
	vector<int> directions;

	if(current.x == destination.x && current.y == destination.y)
	{
		if(current.z < destination.z)
			directions.push_back(DIRECTION_DOWN);
		else
			directions.push_back(DIRECTION_UP);
	}
	else
		directions = routingOddEven(current, source, destination);	
	
	if (!(directions.size() > 0 && directions.size() <= 3))
	{
      cout << "\n STAMPACCHIO :";
      cout << source << endl;
      cout << destination << endl;
      cout << current << endl;

	}
	assert(directions.size() > 0 && directions.size() <= 3);

	return directions;	
}

vector < int >NoximRouter::routingDyAD(const NoximCoord & current,
				       const NoximCoord & source,
				       const NoximCoord & destination)
{
    vector < int >directions;

    directions = routingOddEven(current, source, destination);

    if (!inCongestion())
	directions.resize(1);

    return directions;
}

vector < int >NoximRouter::routingFullyAdaptive(const NoximCoord & current,
						const NoximCoord &
						destination)
{
    vector < int >directions;

    if (destination.x == current.x || destination.y == current.y)
	return routingXYZ(current, destination);

    if (destination.x > current.x && destination.y < current.y) {
	directions.push_back(DIRECTION_NORTH);
	directions.push_back(DIRECTION_EAST);
    } else if (destination.x > current.x && destination.y > current.y) {
	directions.push_back(DIRECTION_SOUTH);
	directions.push_back(DIRECTION_EAST);
    } else if (destination.x < current.x && destination.y > current.y) {
	directions.push_back(DIRECTION_SOUTH);
	directions.push_back(DIRECTION_WEST);
    } else {
	directions.push_back(DIRECTION_NORTH);
	directions.push_back(DIRECTION_WEST);
    }

    return directions;
}

vector < int >NoximRouter::routingTableBased(const int dir_in,
					     const NoximCoord & current,
					     const NoximCoord &
					     destination)
{
    NoximAdmissibleOutputs ao =
	routing_table.getAdmissibleOutputs(dir_in, coord2Id(destination));

    if (ao.size() == 0) {
	cout << "dir: " << dir_in << ", (" << current.x << "," << current.
	    y << ") --> " << "(" << destination.x << "," << destination.
	    y << ")" << endl << coord2Id(current) << "->" <<
	    coord2Id(destination) << endl;
    }
	//cout<<"ao size: "<<ao.size()<<endl;
    assert(ao.size() > 0);

    //-----
    /*
       vector<int> aov = admissibleOutputsSet2Vector(ao);
       cout << "dir: " << dir_in << ", (" << current.x << "," << current.y << ") --> "
       << "(" << destination.x << "," << destination.y << "), outputs: ";
       for (int i=0; i<aov.size(); i++)
       cout << aov[i] << ", ";
       cout << endl;
     */
    //-----

    return admissibleOutputsSet2Vector(ao);
}

vector<int> NoximRouter::routingCrossLayer(const int select_routing, const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source)
{
	if(select_routing == ROUTING_XYZ)
		return routingXYZ(current, destination);
	else if(select_routing == ROUTING_WEST_FIRST)
	    return routingWestFirst(current, destination);
	else if(select_routing == ROUTING_DOWNWARD_CROSS_LAYER)
        return routingDownward_CrossLayer(current, destination, source);
	else
		cout<<"Wrong with Cross-Layer!"<<endl;

}

//---------------------------------------------------------------------------

vector<int> NoximRouter::routingDownward_CrossLayer(const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source)
{


	
	vector<int> directions;
	int layer = 3;	//���ܦ�packet���Ӧb����layer��XY�ǻ�



	   if(current.z < layer && (current.x != destination.x || current.y != destination.y) )  
		{
            directions.push_back(DIRECTION_DOWN);
		}
		else if(current.z >=layer && (current.x != destination.x || current.y != destination.y) )	//�b�����Hxy routing��
		{
			 directions = routingXYZ(current, destination);	 //Matthew:  directions = routingXYZ(current, destination)		                                                         //�쥻downward routing��XY �o�̥�westfirst
		}
		else if((current.x == destination.x && current.y == destination.y) && current.z > destination.z)	//xy�ۦP, z��V���W��
		{	
			directions.push_back(DIRECTION_UP);
		}	
		else if((current.x == destination.x && current.y == destination.y) && current.z < destination.z)  //xy�ۦP, z��V���U��
		{	
			directions.push_back(DIRECTION_DOWN);
		}	
		else 
		{ 
			cout<<"ERROR! Out of condition!?!?"<<endl;
			cout<<"current: ("<<current.x<<","<<current.y<<","<<current.z<<")"<<endl;
			cout<<"destination: ("<<destination.x<<","<<destination.y<<","<<destination.z<<")"<<endl;
			cout<<"layer: "<<layer<<endl;
			exit(1);			
		}

  return directions;
}
vector<int> NoximRouter::routingCrossLayer_HS(const int select_routing, const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source)
{
	if(select_routing == ROUTING_XYZ)
		return routingXYZ(current, destination);
	else if(select_routing == ROUTING_WEST_FIRST)
	    return routingWestFirst(current, destination);
	else if(select_routing == ROUTING_DOWNWARD_CROSS_LAYER)
        return routingDownward_CrossLayer_HS(current, destination, source);
	else
		cout<<"Wrong with Cross-Layer!"<<endl;

}

//---------------------------------------------------------------------------

vector<int> NoximRouter::routingDownward_CrossLayer_HS(const NoximCoord& current, const NoximCoord& destination, const NoximCoord& source)
{


	
	vector<int> directions;
	int layer = 3;	//���ܦ�packet���Ӧb����layer��XY�ǻ�



	   if(current.z < layer && (current.x != destination.x || current.y != destination.y) )  
		{
            directions.push_back(DIRECTION_DOWN);
		}
		else if(current.z >=layer && (current.x != destination.x || current.y != destination.y) )	//�b�����Hxy routing��
		{
			 directions = routingXYZ(current, destination);	 //Matthew:  directions = routingXYZ(current, destination)		                                                         //�쥻downward routing��XY �o�̥�westfirst
		}
		else if((current.x == destination.x && current.y == destination.y) && current.z > destination.z)	//xy�ۦP, z��V���W��
		{	
			directions.push_back(DIRECTION_UP);
		}	
		else if((current.x == destination.x && current.y == destination.y) && current.z < destination.z)  //xy�ۦP, z��V���U��
		{	
			directions.push_back(DIRECTION_DOWN);
		}	
		else 
		{ 
			cout<<"ERROR! Out of condition!?!?"<<endl;
			cout<<"current: ("<<current.x<<","<<current.y<<","<<current.z<<")"<<endl;
			cout<<"destination: ("<<destination.x<<","<<destination.y<<","<<destination.z<<")"<<endl;
			cout<<"layer: "<<layer<<endl;
			exit(1);			
		}

  return directions;
}	

void NoximRouter::configure(const int _id,
			    const double _warm_up_time,
			    const unsigned int _max_buffer_size,
			    NoximGlobalRoutingTable & grt)
{
    local_id = _id;
    stats.configure(_id, _warm_up_time);

    start_from_port = DIRECTION_LOCAL;

    if (grt.isValid())
	routing_table.configure(grt, _id);

    for (int i = 0; i < DIRECTIONS + 1; i++)
	buffer[i].SetMaxBufferSize(_max_buffer_size);
}

unsigned long NoximRouter::getRoutedFlits()
{
    return routed_flits;
}

unsigned int NoximRouter::getFlitsCount()
{
    unsigned count = 0;

    for (int i = 0; i < DIRECTIONS + 1; i++)
	count += buffer[i].Size();

    return count;
}

double NoximRouter::getPower()
{
    //return stats.power.getPower();
    //return stats.power.getTransientRouterPower();
    return stats.power.getSteadyStateRouterPower();
}

int NoximRouter::reflexDirection(int direction) const
{
    if (direction == DIRECTION_NORTH)
	return DIRECTION_SOUTH;
    if (direction == DIRECTION_EAST)
	return DIRECTION_WEST;
    if (direction == DIRECTION_WEST)
	return DIRECTION_EAST;
    if (direction == DIRECTION_SOUTH)
	return DIRECTION_NORTH;
	/****************MODIFY BY HUI-SHUN********************/
	if (direction == DIRECTION_UP) 
	return DIRECTION_DOWN; ////
    if (direction == DIRECTION_DOWN) 
	return DIRECTION_UP;////
    /****************MODIFY BY HUI-SHUN********************/
	// you shouldn't be here
    assert(false);
    return NOT_VALID;
}

int NoximRouter::getNeighborId(int _id, int direction) const
{
    NoximCoord my_coord = id2Coord(_id);

    switch (direction) {
    case DIRECTION_NORTH:
	if (my_coord.y == 0)
	    return NOT_VALID;
	my_coord.y--;
	break;
    case DIRECTION_SOUTH:
	if (my_coord.y == NoximGlobalParams::mesh_dim_y - 1)
	    return NOT_VALID;
	my_coord.y++;
	break;
    case DIRECTION_EAST:
	if (my_coord.x == NoximGlobalParams::mesh_dim_x - 1)
	    return NOT_VALID;
	my_coord.x++;
	break;
    case DIRECTION_WEST:
	if (my_coord.x == 0)
	    return NOT_VALID;
	my_coord.x--;
	break;
	/****************MODIFY BY HUI-SHUN********************/
	case DIRECTION_UP:
		if (my_coord.z==0) return NOT_VALID;
	    my_coord.z--;
	    break;
	case DIRECTION_DOWN:
	    if (my_coord.z==NoximGlobalParams::mesh_dim_z-1) return NOT_VALID;
	    my_coord.z++;
	    break;
	/****************MODIFY BY HUI-SHUN********************/
    default:
	cout << "direction not valid : " << direction;
	assert(false);
    }

    int neighbor_id = coord2Id(my_coord);

    return neighbor_id;
}
/****************MODIFY BY HUI-SHUN********************/
int NoximRouter::getNeighborId_downward(int _id, int direction, int dst_id) const
{
    NoximCoord my_coord = id2Coord(_id);
	NoximCoord dst_coord = id2Coord(dst_id);

    switch (direction)
    {
	case DIRECTION_NORTH:
	    if (my_coord.y==0) return NOT_VALID;
	    my_coord.y--;
	    break;
	case DIRECTION_SOUTH:
	    if (my_coord.y==NoximGlobalParams::mesh_dim_y-1) return NOT_VALID;
	    my_coord.y++;
	    break;
	case DIRECTION_EAST:
	    if (my_coord.x==NoximGlobalParams::mesh_dim_x-1) return NOT_VALID;
	    my_coord.x++;
	    break;
	case DIRECTION_WEST:
	    if (my_coord.x==0) return NOT_VALID;
	    my_coord.x--;
	    break;
	case DIRECTION_UP:
	    if (my_coord.z==0) return NOT_VALID;
	    my_coord.z--;
	    break;
	case DIRECTION_DOWN:
	    if (my_coord.z==NoximGlobalParams::mesh_dim_z-1) return NOT_VALID;
	    my_coord.z = dst_coord.z;
	    break;
	default:
	    cout << "direction not valid : " << direction;
	    assert(false);
    }

    int neighbor_id = coord2Id(my_coord);

  return neighbor_id;
}

void NoximRouter::TraffThrottlingProcess()	//�Y�bemergency mode, �Btraffic�W�Ltraffic quota, �hthrottle
{
	if( reset.read() ){
		for(int i=0; i<4; i++){
			on_off[i].write(0);
		}
	}
	else if(NoximGlobalParams::throt_type == THROT_NORMAL)
		throttle_neighbor = false;
	else if(NoximGlobalParams::throt_type == THROT_TEST)
	{	
		if(!emergency){
			throttle_neighbor = false;
			for(int i=0; i<4; i++)
					on_off[i].write(0);
		}
		else //emergency mode
		{		
				throttle_neighbor=true;
				for(int i=0; i<4; i++)
					on_off[i].write(1);
		}		
	}
	else if(NoximGlobalParams::throt_type == THROT_VERTICAL)
	{	
		if(!emergency)
		{	
			throttle_neighbor = false;
			for(int i=0; i<4; i++)
					on_off[i].write(0);
		}	
		else //emergency mode
		{		
			if(cnt_neighbor >= Quota_neighbor*Q_ratio )
			{
				throttle_neighbor=true;
				for(int i=0; i<4; i++)
					on_off[i].write(1);
//				cout<<getCurrentCycleNum()<<": Local port of Router "<<local_id<<" are throttled!"<<endl;
			}
			else
			{
				throttle_neighbor=false;
				for(int i=0; i<4; i++)
					on_off[i].write(0);
			}
		}		
	}
	else
	{	
		if(!emergency)
		{	
			throttle_neighbor = false;
			for(int i=0; i<4; i++)
					on_off[i].write(0);
		}	
		else //emergency mode
		{		
			if(cnt_neighbor >= Quota_neighbor)
			{	
				throttle_neighbor=true;
				for(int i=0; i<4; i++)
					on_off[i].write(1);
//				cout<<getCurrentCycleNum()<<": Neighbor ports of Router "<<local_id<<" are throttled!"<<endl;
			}
			else
			{
				throttle_neighbor=false;
				for(int i=0; i<4; i++)
					on_off[i].write(0);
			}
	  }	
	}
}
/****************MODIFY BY HUI-SHUN********************/
bool NoximRouter::inCongestion()
{
    for (int i = 0; i < DIRECTIONS; i++) {
	int flits =
	    NoximGlobalParams::buffer_depth - free_slots_neighbor[i];
	if (flits >
	    (int) (NoximGlobalParams::buffer_depth *
		   NoximGlobalParams::dyad_threshold))
	    return true;
    }

    return false;
}
