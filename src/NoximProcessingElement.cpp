/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */
/*
 * NN-Noxim - the NoC-based ANN Simulator
 *
 * (C) 2018 by National Sun Yat-sen University in Taiwan
 *
 * This file contains the implementation of loading NN model
 */

//Dynamic Mapping
#include <iomanip>
#include "fixedp.h"
#include "NoximProcessingElement.h"
//extern int throttling[8][8][4];
//extern int throttling[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y][DEFAULT_MESH_DIM_Z];
//Local memory implementation

int NoximProcessingElement::randInt(int min, int max)
{
	return min +
		(int) ((double) (max - min + 1) * rand() / (RAND_MAX + 1.0));
}

void NoximProcessingElement::rxProcess()
{	
	if (reset.read() ) {
		//cout<<endl;

		//cout<<"PE Rx Reset process "<<reset.read()<<endl;
		//ack_rx.write(0);
		//current_level_rx = 0;
		ack_rx.write(1);
//***************NN-Noxim********************************reset_1

   		if(reset.read())
		{
			char fileID_r[10];
			sprintf(fileID_r,"%d",local_id);
			char file_name_r[10] = "PE_R_";
			strcat(file_name_r, fileID_r);
			remove(file_name_r);
		}
        //cout<<"PE RX Reset end Process"<<endl;
//**********************^^^^^^^^^^^^^^**************************
    	} 
	else {
		//cout<<"PE Rx process"<<local_id<<endl;
		//if (req_rx.read() == 1 - current_level_rx) {
		if(req_rx.read()==1){

			NoximFlit flit_tmp = flit_rx.read();
			
	    		//if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
	    		if (1) {
				char fileID_r[10];
				sprintf(fileID_r,"%d",local_id);
				char file_name_r[10] = "PE_R_";
				strcat(file_name_r, fileID_r);
				fstream file_r;
				file_r.open( file_name_r ,ios::out|ios::app);
				file_r << sc_simulation_time() << ": ProcessingElement[" <<local_id << "] RECEIVING " << flit_tmp << endl;
				//cout << sc_simulation_time() << ": ProcessingElement[" <<local_id << "] RECEIVING " << flit_tmp << endl;
			}
			// current_level_rx = 1 - current_level_rx;		// Negate the old value for Alternating Bit Protocol (ABP)
			
//***************NN-Noxim*************************receive & compute
			if (flit_tmp.flit_type == FLIT_TYPE_BODY)
			{
				int point_receive_Neu_ID = -1;
				for (int i = 0 ; i<receive ; i++)
				{
					if (receive_Neu_ID[i] == flit_tmp.src_Neu_id)
					{
						point_receive_Neu_ID = i;
						should_receive--;
						break;
					}
				}
				
				receive_data[point_receive_Neu_ID]=flit_tmp.data;
				

				if (should_receive == 0)
				{
					cout<<sc_simulation_time()<<": (PE_"<<local_id<<") Now layer "<<ID_layer<<" start computing..."<<endl;
					//*****************************computing*******************************		

					//int start_ID_last_layer = receive_Neu_ID[0];
					//int x_size_last_layer = NN_Model->all_leyer_size[ID_layer-1][1];
					//int y_size_last_layer = NN_Model->all_leyer_size[ID_layer-1][2];
					//int x_size_layer = NN_Model->all_leyer_size[ID_layer][1];
					//int n_size_layer = NN_Model->all_leyer_size[ID_layer][3];					
					if(Type_layer == 'f')
					{
						LayerFCComp(receive_data);
						
					}else if(Type_layer == 'c')
					{
						
						deque <float> deq_data;
						
						float value;
						for(int bg=0 ; bg < Use_Neu; bg++)
						{
							//Idea: First accumulate the prev layer data in the order in a temp variable
							//then accumulate kernel values in order, consider index
							//multiply these two temp deques element-wise
							//Repeat for each neuron
							deq_data.clear();
							
							for(int bh=0; bh< receive_neu_ID_conv[bg].size(); bh++)
							{
								for(int bi=0; bi< receive_Neu_ID.size();bi++)
								{
									if(receive_neu_ID_conv[bg][bh] == receive_Neu_ID[bi] )
									{
										deq_data.push_back(receive_data[bi]);
										break;
									}
								}
							}
							
							/*---------------------------------------*/
							value =0.0;
							int size_conv = NN_Model->all_leyer_size[ID_layer][4]* NN_Model->all_leyer_size[ID_layer][5];
							int conv_z= NN_Model->all_leyer_size[ID_layer][6];
							int denominator = NN_Model->all_leyer_size[ID_layer][1]* NN_Model->all_leyer_size[ID_layer][2];
							
							for(int bl =0; bl< conv_z; bl++)
							{
								for(int fg=0;fg< size_conv;fg++)
								{
									value = value + deq_data[bl*size_conv +fg]* NN_Model->all_conv_weight[PE_table[bg].ID_conv][PE_table[bg].ID_In_layer/denominator][bl][fg];
								}
								
								
							}
							
							//Add bias
							value = value + NN_Model->all_conv_bias[PE_table[bg].ID_conv][PE_table[bg].ID_In_layer/denominator];
							
							//Activation function
							if ( NN_Model->all_leyer_size[ID_layer].back() == RELU )//relu
							{
								if (value <= 0) 
									res[bg]=0;
								else
								res[bg] =value; 				
							}
							
						}
						
					}else if(Type_layer =='p')
					{
						int index;
						float value =0.0;
						for(int bc =0; bc< Use_Neu; bc++)
						{
							value =0;
							for(int bd =0; bd< receive_neu_ID_pool[bc].size();bd++)
							{
								for(int be=0; be< receive_Neu_ID.size(); be++)
								{
									if(receive_neu_ID_pool[bc][bd] == receive_Neu_ID[be] )
									{
										index = be;
										break;
									}
								}
								if ( NN_Model->all_leyer_size[ID_layer].back() == AVERAGE )//average
								{
									value = value + receive_data[index];
									/*-----------Debugging--------------*/
									//if(ID_group == 76 && bc == 16)
									//{
									//	cout<<receive_data[index]<<"--";
									//}
									/*if(ID_group == 50)
									{
										cout<<"Receive data: "<<receive_data[index]<<"--";	
									}*/
									/*----------------------------------*/	 					
								}else if(NN_Model->all_leyer_size[ID_layer].back() == MAXIMUM)
								{
									if(value < receive_data[index])
									{
										value = receive_data[index];
									}
								}
								
							} 
							if(NN_Model->all_leyer_size[ID_layer].back() == AVERAGE)
							{
								value = value / (NN_Model->all_leyer_size[ID_layer][4]*NN_Model->all_leyer_size[ID_layer][5]);
							}
							res[bc] = value;
						}
						/*---------------Debugging----------------*/
						//if(ID_group == 76 && bc == 16)
						//{
							//cout<<"Pooling data for group "<<ID_group<<"::(";
							//for(int vv =0; vv< res.size(); vv++)
							//{
							//	cout<<vv<<"**" <<res[vv]<<")--(";
							//}
							
						//}
						/*----------------------------------------*/
					}
					
					if(flag_transmit == true)
					{
						flag_p = 1; 
						flag_f = 1; 
					}else
					{
						flag_p = 0; 
						flag_f = 0;
						for(int gf=0; gf< Use_Neu; gf++)
						{
							NN_Model->interm_data_out[PE_table[gf].ID_In_layer] = res[gf];
							NN_Model->interm_completed[PE_table[gf].ID_In_layer] = true;
						}
					}
					 
					temp_computation_time = sc_simulation_time();
				}
			}
//**********************^^^^^^^^^^^^^^**************************
		}
		//ack_rx.write(current_level_rx);
		ack_rx.write(1);
		//cout<<"PE RX end Process"<<endl;
    	}
}

void NoximProcessingElement::txProcess()
{
if (reset.read() ) {
	//cout<<"PE Tx Reset process"<<endl;
		req_tx.write(0);
		current_level_tx = 0;
		transmittedAtPreviousCycle = false;
		not_transmit=0;
		transmit=0;
		adaptive_transmit=0;
		dor_transmit=0;
		dw_transmit =0;
//********************NN-Noxim*****************tytyty****************reset_2
		
		PE_enable = 0;	
		ID_layer = -1;
		ID_group = 0;	//** 2018.09.17 edit by Yueh-Chi,Yang **//

		res.clear();

		receive = 0;
		receive_Neu_ID.clear();
		
		Use_Neu	= 0;
		Use_Neu_ID.clear();
		trans = 0;
		trans_PE_ID.clear();

		my_data_in.clear();
		PE_Weight.clear();
   
   		if(!reset.read()){
			cout<<"Step 1"<<endl;
			in_data++;
		   }
		else
		{
			//cout<<"Step2"<<endl;
			char fileID_t[10];
			//cout<<"Local Id: "<<local_id<<endl;
			sprintf(fileID_t,"%d",local_id);
			//cout<<"File Id: "<<fileID_t<<endl;
			char file_name_t[10] = "PE_T_";
			strcat(file_name_t, fileID_t);
			//cout<<"File Name: "<<file_name_t<<endl;
			remove(file_name_t);
			//cout<<"File Name: "<<file_name_t<<endl;
			char output_file[11];
			sprintf(output_file,"output.txt");
			//cout<<"Output Name: "<<output_file<<endl;
			remove(output_file);
			in_data = 0;
			//cout<<"Step 2 Done"<<endl;
		}

		PreprocessingProcess();	
		
		}
	else {
		//cout<<"PE Tx  process"<<endl;
		
		NoximPacket packet;
//********************NN-Noxim*****************tytyty****************trans
		if(clean_all == false){
			if(!throttle_local){

				if(PE_enable && ID_layer != NN_Model->all_leyer_size.size()-1 && flag_p ){
					//**** 2018.09.17 edit by Yueh-Chi,Yang ****//
					cout<<sc_simulation_time()<<": (PE_"<<local_id<<") Now layer "<<ID_layer<<" start sending..."<<endl;
					
					if( Type_layer == 'f' || NN_Model -> all_leyer_type[ID_layer+1]=='f')
					{
						for(int i = 0; i<trans ; i++)
						{
						packet.make(local_id, trans_PE_ID[i], getCurrentCycleNum(), Use_Neu+2);
						packet_queue.push(packet);
						}

						/*--------------Debugging-----------*/

						/*----------------------------------*/
						
					}else
					{
						if( Type_layer == 'c')
						{
							for(int ar =0; ar< trans_PE_ID.size(); ar++)
							{
								packet.make(local_id, trans_PE_ID[ar], getCurrentCycleNum(), trans_conv[ar]+2);
								packet_queue.push(packet);

								/*--------------Debugging-----------*/
								/*if(ID_group == 26 )
								{
									cout<<"Packet Checking....";
									cout<<"Src id: "<<packet.src_id<<" Dst Id: "<<packet.dst_id<<" Size: "<<packet.size<<endl;
								}
								/*if(ID_group == 60)
								{
									cout<<"Src id: "<<packet.src_id<<" Dst Id: "<<packet.dst_id<<" Size: "<<packet.size<<endl;
								}*/
								/*----------------------------------*/
							}
							/*---------Debugging---------------*/
							//cout<<"Packet Queue Size: "<<packet_queue.size();
							/*---------------------------------*/
							
						}
						else if(Type_layer == 'p')
						{
							for(int ar =0; ar< trans_PE_ID.size(); ar++)
							{
								packet.make(local_id, trans_PE_ID[ar], getCurrentCycleNum(), trans_pool[ar]+2);
								packet_queue.push(packet);
								/*--------------Debugging-----------*/
								/*cout<<"Pooling layer, Local id:  "<<local_id<<endl;
								if(ID_group == 26)
								{
									//cout<<"Packet Checking....";
									cout<<"Src id: "<<packet.src_id<<" Dst Id: "<<packet.dst_id<<" Size: "<<packet.size<<endl;
									//cout<<"Packet queue size: "<<packet_queue.size()<<endl;
								}
								/*----------------------------------*/
							}
							
						}	
					}
					
					
					flag_p = 0;
					transmittedAtPreviousCycle = true;
					if((ID_group -(NN_Model->all_leyer_ID_Group[ID_layer-1][0] )) < (NN_Model->all_leyer_ID_Group[ID_layer-1].size()/2))
					{
						curr_XYXrouting = 0; //Initialization so that first packet starts with XY routing
					}else
					{
						curr_XYXrouting = 1; //Initialization so that first packet starts with YX routing  1
					}
					
				} 
				else
					transmittedAtPreviousCycle = false;
			}
		}
		/*---------------Debugging---------------*/
		/*if(ID_group ==0)
		{
			cout<<"("<<sc_simulation_time()<<"--";
			cout<< temp_computation_time<<"--"<<computation_time<<")";
		}
		/*---------------------------------------*/
		if(ack_tx.read() == 1  && flag_f && PE_enable && ID_layer != NN_Model->all_leyer_size.size()-1 && (sc_simulation_time() >= temp_computation_time + computation_time||ID_layer==0) ){
			if (!packet_queue.empty()) {
				//NoximFlit flit = nextFlit();	// Generate a new flit
				
				/*------------Debugging----------------*/
				/*if(ID_group ==0)
				{
					cout<<"Packet: "<<packet.src_id<<"-"<<packet.size<<endl;
					cout<<in_data<<endl;
				}
				/*-------------------------------------*/
				
				NoximFlit flit = nextFlit(ID_layer, in_data);	// tytyty Generate a new flit
        			//NoximFlit flit = nextFlit(ID_layer);	// tytyty Generate a new flit
				//if (NoximGlobalParams::verbose_mode > VERBOSE_OFF) {
				if (1) {
					char fileID_t[10];
					sprintf(fileID_t,"%d",local_id);
					char file_name_t[10] = "PE_T_";
					strcat(file_name_t, fileID_t);
    					fstream file_t;
					file_t.open( file_name_t ,ios::out|ios::app);
					file_t<< getCurrentCycleNum() << ": ProcessingElement[" << local_id <<"] SENDING " << flit << endl;
				}
				
				flit_tx->write(flit);	// Send the generated flit
				//current_level_tx = 1 - current_level_tx;	// Negate the old value for Alternating Bit Protocol (ABP)
				//req_tx.write(current_level_tx);
				req_tx.write(1);
				if (flit.flit_type==FLIT_TYPE_HEAD) 
					cnt_local++;
			}
			else{
				req_tx.write(0);
				flag_f = 0;
			}
			
		}
		/*-------Debugging--------*/
		//if(ID_group ==0)
		//{
		//	cout<<"Ack tx signal: "<<ack_tx<<"--";
		//}
		/*------------------------*/
		//cout<<"PE TX end Process"<<endl;	
//**************************^^^^^^^^^^^^^^^^^^^^^**************************
}
}

//NoximFlit NoximProcessingElement::nextFlit()
NoximFlit NoximProcessingElement::nextFlit(const int ID_layer, const int in_data)  //************tyty*****
//NoximFlit NoximProcessingElement::nextFlit(const int ID_layer)  //************tyty*****
{
	
    NoximFlit flit;
    NoximPacket packet = packet_queue.front();
	
    flit.src_id = packet.src_id;
    flit.dst_id = packet.dst_id;
    flit.timestamp = packet.timestamp;
    flit.sequence_no = packet.size - packet.flit_left;
    flit.hop_no = 0;
	flit.routing_f   = packet.routing;
	/*----------debugging-------------*/
	/*if(ID_group ==0)
	{
		cout<<"Src id: "<<packet.src_id<<" Dst Id: "<<packet.dst_id<<" Size: "<<packet.size<<" Seq no: "<<flit.sequence_no<<" Start Index: "<<start_index <<endl;
		//cout<<Type_layer<<endl;
	}
	/*--------------------------------*/
//************Intermittent XY routing********************
	if(flit.sequence_no == 0)
	{
		curr_XYXrouting = abs(curr_XYXrouting -1);  
		flit.XYX_routing   = curr_XYXrouting ; 
		/*------------Debugging---------------*/
		/*if(local_id == 0  )
		{
			cout<<"(Local ID: "<<local_id<<" Packet source id: "<<packet.src_id<<" Dst:"<<packet.dst_id<<" Routing:"<<flit.XYX_routing<<" Seq no: "<<flit.sequence_no<<endl;
		}
		/*------------------------------------*/
	}		
	else
	{
		flit.XYX_routing   = curr_XYXrouting;
		/*------------Debugging---------------*/
		/*if(local_id == 48)
		{
			cout<<"(Local ID: "<<local_id<<" Packet source id: "<<packet.src_id<<" Dst:"<<packet.dst_id<<" Routing:"<<flit.XYX_routing<<" Seq no: "<<flit.sequence_no<<")--";
		}*/
		/*------------------------------------*/
		
	}
		 
//*******************************************************
    //  flit.payload     = DEFAULT_PAYLOAD;

    if (packet.size == packet.flit_left) {
		flit.flit_type = FLIT_TYPE_HEAD;
		flit.src_Neu_id = 0;
		flit.data = 0;
		start_index =0;
	}
	
    else if (packet.flit_left == 1) {
		flit.flit_type = FLIT_TYPE_TAIL;
		flit.src_Neu_id = 0;
		flit.data = 0;
	}
	
//************************NN-Noxim*****************tytyty**************setflit_data
    else{
	flit.flit_type = FLIT_TYPE_BODY;
		if(Type_layer == 'f' || NN_Model -> all_leyer_type[ID_layer+1]=='f')
		{
			flit.src_Neu_id = Use_Neu_ID[flit.sequence_no-1];
			//if(Type_layer=='i')
				//flit.data = my_data_in[in_data][flit.sequence_no-1];
				//flit.data = my_data_in[flit.sequence_no-1];
			//else
			flit.data = res[flit.sequence_no-1];
		}else if( Type_layer =='c')
		{
			int pe_id = packet.dst_id;
			for( int ag = start_index; ag< trans_PE_ID_conv.size(); ag++)
			{
				if(trans_PE_ID_conv[ag] == pe_id){
					start_index = ag+1;
					break;
				}
			}
			flit.src_Neu_id = Use_Neu_ID[start_index-1];
			flit.data = res[start_index-1];
			/*--------------Debugging-----------------*/
			/*if(ID_group == 26)
			{
				cout<<"("<<flit.src_Neu_id<<"--"<<start_index<<")";
			}
			/*----------------------------------------*/
		}else
		{
			int pe_id = packet.dst_id;
			int done;
			for( int ag = start_index; ag< trans_PE_ID_pool.size(); ag++)
			{
				done =0;
				for(int ah=0; ah<trans_PE_ID_pool[ag].size(); ah++)
				{
					if(trans_PE_ID_pool[ag][ah] == pe_id)
					{
						start_index = ag+1;
						done =1;
						break;
					}
				}if(done == 1) break;
				
			}
			flit.src_Neu_id = Use_Neu_ID[start_index-1];
			flit.data = res[start_index-1];
			/*--------------Debugging-----------------*/
			/*if(ID_group == 0)
			{
				cout<<"("<<flit.src_Neu_id<<")--("<<start_index<<")--";
			}
			/*----------------------------------------*/
			
		}
		
    }
//************************************^^^^^^^^^^^^^^^^^^^^^^^^*********************
	
    packet_queue.front().flit_left--;
    if (packet_queue.front().flit_left == 0)
	packet_queue.pop();

	/*------------Debugging--------------*/
	/*if(ID_group == 0)
	{
		cout<<"(flit Type: "<<flit.flit_type<<"--";
		cout<<"Source Neu ID: "<<flit.src_Neu_id<<"--";
		cout<<"Sequence No: "<<flit.sequence_no<<"--";
		cout<<"Packet size: "<<packet.size<<"--";
		cout<<"Packet flit left: "<<packet.flit_left<<"--";
		cout<<"Start Index: "<<start_index<<")--";
	}
	/*-----------------------------------*/

    return flit;
}

bool NoximProcessingElement::canShot(NoximPacket & packet)
{
    bool shot;
    double threshold;

    if (NoximGlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED) 
	{
	if (!transmittedAtPreviousCycle)
		threshold = NoximGlobalParams::packet_injection_rate;
	else
	    threshold = NoximGlobalParams::probability_of_retransmission;
	
	shot = (((double) rand()) / RAND_MAX < threshold);
	if (shot) {
	    switch (NoximGlobalParams::traffic_distribution) {
	    case TRAFFIC_RANDOM:
		packet = trafficRandom();
		break;

	    case TRAFFIC_TRANSPOSE1:
		packet = trafficTranspose1();
		break;

	    case TRAFFIC_TRANSPOSE2:
		packet = trafficTranspose2();
		break;

	    case TRAFFIC_BIT_REVERSAL:
		packet = trafficBitReversal();
		break;

	    case TRAFFIC_SHUFFLE:
		packet = trafficShuffle();
		break;

	    case TRAFFIC_BUTTERFLY:
		packet = trafficButterfly();
		break;

	    default:
		assert(false);
	    }
	}
    }else 
	{			// Table based communication traffic
		if (never_transmit)
			return false;

		int now = getCurrentCycleNum() ;
		bool use_pir = (transmittedAtPreviousCycle == false);
		vector < pair < int, double > > dst_prob;
		double threshold =
			traffic_table->getCumulativePirPor(local_id, now,
							   use_pir, dst_prob);

		double prob = (double) rand() / RAND_MAX;
		shot = (prob < threshold);
		if (shot) 
		{
			for (unsigned int i = 0; i < dst_prob.size(); i++) {
			if (prob < dst_prob[i].second) {
				packet.make(local_id, dst_prob[i].first, now,
					getRandomSize());
				break;
			}
			}
		}
    }

    return shot;
}


NoximPacket NoximProcessingElement::trafficRandom()
{
    int max_id = (NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_z) - 1;
	NoximPacket p;
    p.src_id = local_id;
    double rnd = rand() / (double) RAND_MAX;
    double range_start = 0.0;

    //cout << "\n " << getCurrentCycleNum() << " PE " << local_id << " rnd = " << rnd << endl;

    
	 int re_transmit=1; //1

    // Random destination distribution
    do {
	transmit++;
	p.dst_id = randInt(0, max_id);

	// check for hotspot destination
	for (unsigned int i = 0; i < NoximGlobalParams::hotspots.size(); i++) {
	    //cout << getCurrentCycleNum() << " PE " << local_id << " Checking node " << NoximGlobalParams::hotspots[i].first << " with P = " << NoximGlobalParams::hotspots[i].second << endl;

	    if (rnd >= range_start
		&& rnd <
		range_start + NoximGlobalParams::hotspots[i].second) {
		if (local_id != NoximGlobalParams::hotspots[i].first) {
		    //cout << getCurrentCycleNum() << " PE " << local_id <<" That is ! " << endl;
		    p.dst_id = NoximGlobalParams::hotspots[i].first;
		}
		break;
	    } else
		range_start += NoximGlobalParams::hotspots[i].second;	// try next
	}
	
	/////////////////////////////Matthew: Cross Layer Solution////////////////////////////
 
 re_transmit=1;  //1

 NoximCoord current = id2Coord(local_id);
 NoximCoord destination = id2Coord(p.dst_id);

 int x_diff=destination.x-current.x;
 int y_diff=destination.y-current.y;
 int z_diff=destination.z-current.z;

 int n_x;
 int n_y;
 int n_z;
 int a_x_search;
 int a_y_search;
 int a_z_search;
 
 int in_xy = 0;  //0
 int c=0;
 int xy_fail=0;
 int adaptive_not_ok=0;  //0
 int routing =ROUTING_XYZ;
 int destination_throttle=1;
 int dw =0;

 if(NoximGlobalParams::throttling[destination.x][destination.y][destination.z] == 0)
 {
   destination_throttle =0;
    
    if(x_diff>=0 && y_diff>=0)
		 {
		    int x_a_diff = x_diff;
			int y_a_diff = y_diff;
		    for(int y_a=0;y_a<y_a_diff+1;y_a++)
			{
			  for(int x_a=0;x_a<x_a_diff+1;x_a++)
			  {
			    a_x_search=current.x+x_a;
				a_y_search=current.y+y_a;
				if(NoximGlobalParams::throttling[a_x_search][a_y_search][current.z]==1)
				   adaptive_not_ok++;
			  }
			}
		 }
		 else if(x_diff>=0 && y_diff<0)
		 {
		    int x_a_diff = x_diff;
			int y_a_diff = -y_diff;
		    for(int y_a=0;y_a<y_a_diff+1;y_a++)
			{
			  for(int x_a=0;x_a<x_a_diff+1;x_a++)
			  {
			    a_x_search=current.x+x_a;
				a_y_search=current.y-y_a;
				if(NoximGlobalParams::throttling[a_x_search][a_y_search][current.z]==1)
				   adaptive_not_ok++;
			  }
			}
		 }
		 else if(x_diff<0 && y_diff>=0)
		 {
		    int x_a_diff = -x_diff;
			int y_a_diff = y_diff;
		    for(int y_a=0;y_a<y_a_diff+1;y_a++)
			{
			  for(int x_a=0;x_a<x_a_diff+1;x_a++)
			  {
			    a_x_search=current.x-x_a;
				a_y_search=current.y+y_a;
				if(NoximGlobalParams::throttling[a_x_search][a_y_search][current.z]==1)
				   adaptive_not_ok++;
			  }
			}
		 }
		 else //(x_diff<0 && y_diff<0)
		 {
		    int x_a_diff = -x_diff;
			int y_a_diff = -y_diff;
		    for(int y_a=0;y_a<y_a_diff+1;y_a++)
			{
			  for(int x_a=0;x_a<x_a_diff+1;x_a++)
			  {
			    a_x_search=current.x-x_a;
				a_y_search=current.y-y_a;
				if(NoximGlobalParams::throttling[a_x_search][a_y_search][current.z]==1)
				   adaptive_not_ok++;
			  }
			}
		 }

		 if(z_diff>=0)
		 {
            for(int zt=1;zt<z_diff+1;zt++)
		    {
				a_z_search=current.z+zt;
				if(NoximGlobalParams::throttling[destination.x][destination.y][a_z_search]==1)
					adaptive_not_ok++;
		    }
		 }
		 else
		 {
			 int z_diff_tt = -z_diff;
			 for(int zt=1;zt<z_diff_tt+1;zt++)
		    {
				a_z_search=current.z-zt;
				if(NoximGlobalParams::throttling[destination.x][destination.y][a_z_search]==1)
					adaptive_not_ok++;
		    }
		 }


		 
		 
		 if(adaptive_not_ok >= 1)
		   in_xy=1;
		 else
		   in_xy=0;

	 ////////////////////////�i�JXY Routing//////////////
	if(in_xy==1)
	{
		 if(x_diff>=0)
		 {
            for(int xt=1;xt<x_diff+1;xt++)
		    {
				n_x=current.x+xt;
				if(NoximGlobalParams::throttling[n_x][current.y][current.z]==1)
					c++;
		    }
		 }
		 else
		 {
			 int x_diff_t=-x_diff;
			 for(int xt=1;xt<x_diff_t+1;xt++)
		    {
				n_x=current.x-xt;
				if(NoximGlobalParams::throttling[n_x][current.y][current.z]==1)
					c++;
		    }
		 }

		 if(y_diff>=0)
		 {
            for(int yt=1;yt<y_diff+1;yt++)
		    {
				n_y=current.y+yt;
				if(NoximGlobalParams::throttling[destination.x][n_y][current.z]==1)
					c++;
		    }
		 }
		 else
		 {
			 int y_diff_t = -y_diff;
			 for(int yt=1;yt<y_diff_t+1;yt++)
		    {
				n_y=current.y-yt;
				if(NoximGlobalParams::throttling[destination.x][n_y][current.z]==1)
					c++;
		    }
		 }

		 if(z_diff>=0)
		 {
            for(int zt=1;zt<z_diff+1;zt++)
		    {
				n_z=current.z+zt;
				if(NoximGlobalParams::throttling[destination.x][destination.y][n_z]==1)
					c++;
		    }
		 }
		 else
		 {
			 int z_diff_t = -z_diff;
			 for(int zt=1;zt<z_diff_t+1;zt++)
		    {
				n_z=current.z-zt;
				if(NoximGlobalParams::throttling[destination.x][destination.y][n_z]==1)
					c++;
		    }
		 }

		 if(c>=1)
			 xy_fail=1;
		 else
			 xy_fail=0;


	 }


     ////////////////////////�i�JDownward Routing//////////////
	 
	 if(xy_fail>=1)
	 {
	    int z_diff_dw_s = (NoximGlobalParams::mesh_dim_z-1) - current.z;
	      for(int zzt=1;zzt<z_diff_dw_s+1;zzt++)
		    {
				n_z=current.z+zzt;
				if(NoximGlobalParams::throttling[current.x][current.y][n_z]==1)
					dw++;
		    }
			
		int z_diff_dw_d = (NoximGlobalParams::mesh_dim_z-1) - destination.z;
	      for(int zzt=1;zzt<z_diff_dw_d+1;zzt++)
		    {
				n_z=destination.z+zzt;
				if(NoximGlobalParams::throttling[destination.x][destination.y][n_z]==1)
					dw++;
		    }
	 
	 }
	 
	 
	 if(adaptive_not_ok==0)
		{
		   re_transmit=0;
		   routing =ROUTING_WEST_FIRST; //ROUTING_WEST_FIRST
		   adaptive_transmit++;
		}
		else if(adaptive_not_ok >=1 && xy_fail==0)
		{
		   re_transmit=0;//0
		   routing =ROUTING_XYZ;
		   dor_transmit++;
		}
		else if(adaptive_not_ok >=1 && xy_fail >=1 && dw==0)
		{
		   re_transmit=0;
		   routing =ROUTING_DOWNWARD_CROSS_LAYER;
		   dw_transmit++;
		}
		else if(adaptive_not_ok >=1 && xy_fail >=1 && dw>=1)
		{
		   re_transmit=1;
		   routing =ROUTING_DOWNWARD_CROSS_LAYER;
		}
		 
		p.routing=routing;
		
     }//if(throttling[destination.x][destination.y] == 0)


       if(re_transmit==1)
		 not_transmit++;
	
	
    } while ((p.dst_id == p.src_id) || (re_transmit));

    p.timestamp = getCurrentCycleNum() ;
    p.size = p.flit_left = getRandomSize();

    return p;
}



NoximPacket NoximProcessingElement::trafficTranspose1()
{
    NoximPacket p;
    p.src_id = local_id;
    NoximCoord src, dst;

    // Transpose 1 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    src.z = id2Coord(p.src_id).z;
	dst.x = NoximGlobalParams::mesh_dim_x - 1 - src.y;
    dst.y = NoximGlobalParams::mesh_dim_y - 1 - src.x;
    dst.z = NoximGlobalParams::mesh_dim_z-1-src.z;
	fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = getCurrentCycleNum() ;
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficTranspose2()
{
    NoximPacket p;
    p.src_id = local_id;
    NoximCoord src, dst;

    // Transpose 2 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = src.y;
    dst.y = src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = getCurrentCycleNum();
    p.size = p.flit_left = getRandomSize();

    return p;
}

void NoximProcessingElement::setBit(int &x, int w, int v)
{
    int mask = 1 << w;

    if (v == 1)
	x = x | mask;
    else if (v == 0)
	x = x & ~mask;
    else
	assert(false);
}

int NoximProcessingElement::getBit(int x, int w)
{
    return (x >> w) & 1;
}

inline double NoximProcessingElement::log2ceil(double x)
{
    return ceil(log(x) / log(2.0));
}

NoximPacket NoximProcessingElement::trafficBitReversal()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits; i++)
	setBit(dnode, i, getBit(local_id, nbits - i - 1));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = getCurrentCycleNum();
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficShuffle()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits - 1; i++)
	setBit(dnode, i + 1, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = getCurrentCycleNum();
    p.size = p.flit_left = getRandomSize();

    return p;
}

NoximPacket NoximProcessingElement::trafficButterfly()
{

    int nbits =
	(int)
	log2ceil((double)
		 (NoximGlobalParams::mesh_dim_x *
		  NoximGlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 1; i < nbits - 1; i++)
	setBit(dnode, i, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));
    setBit(dnode, nbits - 1, getBit(local_id, 0));

    NoximPacket p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = getCurrentCycleNum() ;
    p.size = p.flit_left = getRandomSize();

    return p;
}

void NoximProcessingElement::fixRanges(const NoximCoord src,
				       NoximCoord & dst)
{
    // Fix ranges
    if (dst.x < 0)
	dst.x = 0;
    if (dst.y < 0)
	dst.y = 0;
    if (dst.x >= NoximGlobalParams::mesh_dim_x)
	dst.x = NoximGlobalParams::mesh_dim_x - 1;
    if (dst.y >= NoximGlobalParams::mesh_dim_y)
	dst.y = NoximGlobalParams::mesh_dim_y - 1;
	if (dst.z >= NoximGlobalParams::mesh_dim_z)
	dst.z = NoximGlobalParams::mesh_dim_z - 1;
}

int NoximProcessingElement::getRandomSize()
{
    return randInt(NoximGlobalParams::min_packet_size,
		   NoximGlobalParams::max_packet_size);
}
/***MODIFY BY HUI-SHUN***/
NoximPacket NoximProcessingElement::trafficRandom_Tvar()
{
  int max_id = (NoximGlobalParams::mesh_dim_x * NoximGlobalParams::mesh_dim_y * NoximGlobalParams::mesh_dim_z)-1;////
  NoximPacket p;
  p.src_id = local_id;//randInt(0, max_id);//
  double rnd = rand()/(double)RAND_MAX;
  double range_start = 0.0;

  //cout << "\n " << getCurrentCycleNum() << " PE " << local_id << " rnd = " << rnd << endl;

   

  // Random destination distribution
  do
  {
		p.dst_id = randInt(0, max_id);

    // check for hotspot destination
    for (unsigned int i = 0; i<NoximGlobalParams::hotspots.size(); i++)
    {
	//cout << getCurrentCycleNum() << " PE " << local_id << " Checking node " << TGlobalParams::hotspots[i].first << " with P = " << TGlobalParams::hotspots[i].second << endl;

	if (rnd>=range_start && rnd < range_start + NoximGlobalParams::hotspots[i].second)
	{
	    if (local_id != NoximGlobalParams::hotspots[i].first)
	    {
		//cout << getCurrentCycleNum() << " PE " << local_id <<" That is ! " << endl;
		p.dst_id = NoximGlobalParams::hotspots[i].first;
	    }
	    break;
	}
	else 
	    range_start+=NoximGlobalParams::hotspots[i].second; // try next
    }
  } while(p.dst_id==p.src_id);

  p.timestamp = getCurrentCycleNum();
  p.size = p.flit_left = getRandomSize();

  return p;
}


void NoximProcessingElement::TraffThrottlingProcess()
{
	if(NoximGlobalParams::throt_type == THROT_NORMAL)
		throttle_local = false;
	else if(NoximGlobalParams::throt_type == THROT_TEST)
	{	
		if(!emergency)
			throttle_local = false;
		else //emergency mode
		{		
				throttle_local=true;
		}		
	}
	else if(NoximGlobalParams::throt_type == THROT_VERTICAL)
	{	
		if(!emergency)
			throttle_local = false;
		else //emergency mode
		{		
			if(cnt_local >= Quota_local*Q_ratio )
			{
				throttle_local=true;
//				cout<<getCurrentCycleNum()<<": Local port of Router "<<local_id<<" are throttled!"<<endl;
			}
			else
				throttle_local=false;
		}		
	}
	else
	{	
		if(!emergency)
			throttle_local = false;
		else //emergency mode
		{		
			if(cnt_local >= Quota_local )
			{
				throttle_local=true;
//				cout<<getCurrentCycleNum()<<": Local port of Router "<<local_id<<" are throttled!"<<endl;
			}
			else
				throttle_local=false;
		}		
	}
}

void NoximProcessingElement::PreprocessingProcess()
{
	Use_Neu_ID.clear();
	res.clear();
	//cout<<"Prep: "<<local_id<<endl;
	for( int k = 0 ; k<NN_Model->mapping_table.size() ; k++ )
	{
		//cout<<"Loop 1"<<endl;
		if(NN_Model->mapping_table[k]==local_id)
		{
			ID_group = k;
			if(ID_group<NN_Model->Group_table.size())
			{
				PE_enable = 1;

				PE_table = NN_Model->Group_table[ID_group];
				
				ID_layer = PE_table[0].ID_layer;
				Type_layer = PE_table[0].Type_layer;
				Use_Neu = PE_table.size();
				res.assign( Use_Neu, 0 );

				for(int i = 0 ; i<Use_Neu ; i++)
				{
					Use_Neu_ID.push_back(PE_table[i].ID_Neu);
				}
				

				if( Type_layer == 'f')
				{	
					if(NoximGlobalParams::mapping_method == STATIC)
					{
						FCtrans();
						//receive ids
						FCreceive(false);
						flag_p =0;
						flag_f =0;
						flag_transmit = true;
					}else if (NoximGlobalParams::mapping_method == DYNAMIC)
					{
						if(NN_Model ->active_layers.size() == 1) // Only 1 layer is mapped
						{
							FCreceive(true);
							//Perform computation and store it
							LayerFCComp(NN_Model->interm_data_in);
							for(int gf=0; gf< Use_Neu; gf++)
							{
								NN_Model->interm_data_out[PE_table[gf].ID_In_layer] = res[gf];
								NN_Model->interm_completed[PE_table[gf].ID_In_layer] = true;

								/*--------------Debugging------------------*/
								//if(PE_table[gf].ID_In_layer >= 0 && PE_table[gf].ID_In_layer < 784)
								//{
								//	cout<< "(Interm: "<<PE_table[gf].ID_In_layer<<"--"<<NN_Model->interm_data_out[PE_table[gf].ID_In_layer/ denominator][PE_table[gf].ID_In_layer% denominator]<<")-";
								//}	
								/*-----------------------------------------*/
							}
							flag_p =0;
							flag_f=0;
							flag_transmit = false;

						}else
						{
							if(ID_layer == NN_Model->active_layers.front()) //
							{
								/*---------------Debugging--------------*/
								//cout<<local_id<<endl;
								/*--------------------------------------*/
								FCreceive(true);
								FCtrans();
								//Perform computation and send it
								LayerFCComp(NN_Model->interm_data_in);
								//cout<<sc_simulation_time()<<": (PE_"<<local_id<<") Now layer "<<ID_layer<<" start computing..."<<endl;
								flag_p = 1; 
								flag_f = 1; 
								temp_computation_time = sc_simulation_time();
								flag_transmit = true;

							}else if(ID_layer == NN_Model->active_layers.back())
							{
								FCreceive(false);
								flag_transmit = false;
								flag_p =0;
								flag_f=0;
							}else
							{
								FCreceive(false);
								FCtrans();
								flag_transmit = true;
								flag_p =0;
								flag_f=0;
							}
							
						}
					}
					

				}else if(Type_layer == 'c')
				{
					if(NoximGlobalParams::mapping_method == STATIC)
					{
						//Layer is convolution
						Convtrans();
						//Step2: Receive neuron ids from NNModel

						Convreceive(false);

						//Step3: If layer is conv 1, take data from memory and perform convolution and send data
						if(ID_layer == 1)
						{
							
							LayerConvComp(NN_Model-> all_data_in[in_data]);
							/*--------------Debugging-------------------*/
							/*if(ID_group ==1)
							{
								for(int ff =0; ff< Use_Neu; ff++)
								{
									cout<<"("<< res[ff]<<")--";
								}
								cout<<endl<<res.size()<<endl;;
							}*/
							/*------------------------------------------*/
							//cout<<sc_simulation_time()<<": (PE_"<<local_id<<") Now layer "<<ID_layer<<" start computing..."<<endl;
							flag_p=1;
							flag_f=1;
							temp_computation_time = sc_simulation_time();
						}else
						{
							flag_p=0;
							flag_f=0;
						}
						flag_transmit = true;
					}else if(NoximGlobalParams::mapping_method == DYNAMIC)
					{
						if(NN_Model ->active_layers.size() == 1) // Only 1 layer is mapped
						{
							Convreceive(true);
							if(ID_layer == 1)
							{
								LayerConvComp(NN_Model-> all_data_in[in_data]);
								int denominator = NN_Model->all_leyer_size[ID_layer][1]* NN_Model->all_leyer_size[ID_layer][2];
								for(int gf=0; gf< Use_Neu; gf++)
								{
									NN_Model->interm_data_out[PE_table[gf].ID_In_layer] = res[gf];
									NN_Model->interm_completed[PE_table[gf].ID_In_layer] = true;

									/*--------------Debugging------------------*/
									//if(PE_table[gf].ID_In_layer >= 0 && PE_table[gf].ID_In_layer < 784)
									//{
									//	cout<< "(Interm: "<<PE_table[gf].ID_In_layer<<"--"<<NN_Model->interm_data_out[PE_table[gf].ID_In_layer/ denominator][PE_table[gf].ID_In_layer% denominator]<<")-";
									//}	
									/*-----------------------------------------*/
								}
								
							}
							flag_transmit = false;
							flag_p =0;
							flag_f =0;
						}else
						{
							//cout<<"Conv.."<<local_id<<endl;
							if(ID_layer == NN_Model->active_layers.front()) //
							{
								//cout<<"Checking.."<<local_id<<endl;
								Convreceive(true);
								
								if(ID_layer == 1)
								{
									LayerConvComp(NN_Model-> all_data_in[in_data]);
								}else
								{
									LayerConvComp(NN_Model->interm_data_in);
									
								}
								//cout<<"Checking.."<<local_id<<endl;
								/*----------Debugging---------------*/
								/*for(int ff=0; ff< Use_Neu; ff++)
								{
									if(PE_table[ff].ID_In_layer >=0 && PE_table[ff].ID_In_layer <= 100)
									{
										cout<<"(Resultant: "<<res[ff]<<")-";
									}
								}*/
								//cout<<"Conv First Layer "<<endl;
								/*----------------------------------*/
								Convtrans();
								flag_transmit = true;
								temp_computation_time = sc_simulation_time();
								//cout<<sc_simulation_time()<<": (PE_"<<local_id<<") Now layer "<<ID_layer<<" start computing..."<<endl;
								flag_p = 1;
								flag_f = 1;
								//cout<<"Checking.."<<local_id<<endl;
							}else if(ID_layer == NN_Model->active_layers.back())
							{
								/*---------------Debugging--------------*/
								//cout<<"Conv End Layer "<<endl;
								/*--------------------------------------*/
								Convreceive(false);
								flag_transmit = false;
								flag_f=0;
								flag_p=0;

							}else
							{
								/*---------------Debugging--------------*/
								//cout<<"Conv Middle Layer "<<local_id<<endl;
								/*--------------------------------------*/
								Convreceive(false);
								
								Convtrans();
								flag_transmit = true;
								flag_f=0;
								flag_p=0;
								
							}
						}
					}	
				}else if(Type_layer=='p')
				{
					//Layer is pooling
					if(NoximGlobalParams::mapping_method == STATIC)
					{
						//Step1: Transmitting PE ids if next layer is conv
						Pooltrans();
						
						//Step2: Receive ids
						Poolreceive(false);
						flag_transmit = true;
					}
					else if (NoximGlobalParams::mapping_method == DYNAMIC)
					{
						//cout<<"Pool.."<<local_id<<endl;
						if(NN_Model ->active_layers.size() == 1) // Only 1 layer is mapped
						{
							Poolreceive(true);
							//Perform computation and store it
							LayerPoolComp(NN_Model->interm_data_in);
							for(int gf=0; gf< Use_Neu; gf++)
							{
								NN_Model->interm_data_out[PE_table[gf].ID_In_layer] = res[gf];
								NN_Model->interm_completed[PE_table[gf].ID_In_layer] = true;

								/*--------------Debugging------------------*/
								//if(PE_table[gf].ID_In_layer >= 0 && PE_table[gf].ID_In_layer < 784)
								//{
								//	cout<< "(Interm: "<<PE_table[gf].ID_In_layer<<"--"<<NN_Model->interm_data_out[PE_table[gf].ID_In_layer/ denominator][PE_table[gf].ID_In_layer% denominator]<<")-";
								//}	
								/*-----------------------------------------*/
							}
							flag_p =0;
							flag_f=0;
							flag_transmit = false;

						}else
						{
							if(ID_layer == NN_Model->active_layers.front()) //
							{
								/*---------------Debugging--------------*/
								//cout<<local_id<<endl;
								/*--------------------------------------*/
								Poolreceive(true);
								Pooltrans();
								//Perform computation and send it
							
								LayerPoolComp(NN_Model->interm_data_in);
								
								flag_p = 1; 
								flag_f = 1; 
								//cout<<sc_simulation_time()<<": (PE_"<<local_id<<") Now layer "<<ID_layer<<" start computing..."<<endl;
								temp_computation_time = sc_simulation_time();
								flag_transmit = true;

							}else if(ID_layer == NN_Model->active_layers.back())
							{
								Poolreceive(false);
								flag_transmit = false;
								flag_p =0;
								flag_f=0;
							}else
							{
								Poolreceive(false);
								Pooltrans();
								flag_transmit = true;
								flag_p =0;
								flag_f=0;
							}
							
						}	
					}
				
				}
			}
			break;
		}
	}
	computation_time = NoximGlobalParams::PE_computation_time;
}

void NoximProcessingElement::FCtrans()
{
	if(ID_layer !=NN_Model->all_leyer_size.size()-1)
	{
		trans_PE_ID.clear();
		//trans ids	
		int i;
		for(i = 0 ; i<NN_Model->all_leyer_ID_Group[ID_layer].size() ; i++)
		{
			int temp_Group = NN_Model->all_leyer_ID_Group[ID_layer][i];
			trans_PE_ID.push_back(NN_Model-> mapping_table[temp_Group]);
		}
		trans = i;
		should_trans = trans;
		/*-------Debugging------*/
		//cout<<"Size of next layer: "<<NN_Model->all_leyer_ID_Group[ID_layer].size()<<endl;
		//cout<<ID_layer<<"-"<<ID_group<<"-"<< should_trans<<"-"<<trans_PE_ID[0]<<endl;
		/*----------------------*/
	}
}

void NoximProcessingElement::FCreceive(bool SingleOrFirstLayer)
{
	receive = NN_Model-> all_leyer_size[ID_layer-1][0];
	should_receive = receive;
	receive_Neu_ID.clear();
	receive_data.clear();
	receive_data.assign(receive , 0 );
	
	if(!SingleOrFirstLayer)
	{
		int temp_receive_start_ID = Use_Neu_ID[0] - PE_table[0].ID_In_layer - receive;
	
		for(int i = 0 ; i<receive ; i++)
		{
			receive_Neu_ID.push_back(temp_receive_start_ID+i);
		}
	}else
	{
		for(int i = 0 ; i<receive ; i++)
		{
			receive_Neu_ID.push_back(i);
		}
	}
	
	
}

void NoximProcessingElement::Convtrans()
{
	deque< NeuInformation > PE_table_nxtlayer;
	deque< NeuInformation > PE_table_nxtlayer_neuron;
	int done=0;
	//Step1: Transmitting PE ids for each neuron
	trans_PE_ID_conv.clear();
	if(NN_Model->all_leyer_type[ID_layer+1] == 'p')
	{
		int temp_nxtgrp_neuron = NN_Model->all_leyer_ID_Group[ID_layer][0];
		PE_table_nxtlayer_neuron=NN_Model->Group_table[temp_nxtgrp_neuron];
		for(int aa =0; aa<Use_Neu; aa++)
		{
			done = 0;
			for(int ab =0; ab< NN_Model->all_pool_coord[PE_table_nxtlayer_neuron[0].ID_pool].size();ab++ )
			{
				for(int ac=0; ac<NN_Model->all_pool_coord[PE_table_nxtlayer_neuron[0].ID_pool][ab].size(); ac++ )
				{
					/*----------Debugging----------*/
					/*if(ID_group == 7 && aa == 85)
					{
						cout<<(PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))<<endl;
					}*/
						/*-----------------------------*/
					if( (PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))== NN_Model->all_pool_coord[PE_table_nxtlayer_neuron[0].ID_pool][ab][ac])
					{
						
						for(int ad =0; ad< NN_Model->all_leyer_ID_Group[ID_layer].size(); ad++)
						{
							int temp_Group = NN_Model->all_leyer_ID_Group[ID_layer][ad];
							PE_table_nxtlayer = NN_Model->Group_table[temp_Group];
							for(int ae=0; ae<PE_table_nxtlayer.size();ae++)
							{
								/*------------------Debugging-----------------*/
									/*if(ID_group == 7 && aa == 85)
									{
										cout<<ab<<"--"<<(NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2])<<"--"<<(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))<<"--";
										cout<<(PE_table_nxtlayer[ae].ID_In_layer )<<"--";
										cout<<(NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2])*(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))<<endl;	
									}*/
									/*--------------------------------------------*/
								if((ab+ (NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2])*(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))) == (PE_table_nxtlayer[ae].ID_In_layer))
								{
									
									trans_PE_ID_conv.push_back(NN_Model-> mapping_table[temp_Group]);
									done =1;
									
									break;
								}
							}if(done ==1 ){break;}
							
						}if(done ==1 ){break;}
					}
					
				}if(done ==1 ){break;}
				
			}
		}

		/*-------------------Debugging------------------*/
		/*if(local_id == 26)
		{	
			//cout<<Use_Neu<<endl;
			for(int za=0; za<trans_PE_ID_conv.size();za++)
			{
				cout<<"("<<trans_PE_ID_conv[za]<<")--";
			}
			cout<<endl<<"Size: "<<trans_PE_ID_conv.size()<<endl;
			//cout<<NN_Model->all_pool_coord[PE_table_nxtlayer_neuron[0].ID_pool].size()<<endl;
			//cout<<(NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])<<endl;
		}
		
		/*----------------------------------------------*/

		

		/*--------------Debugging-----------------*/
		/*if(ID_group == 60)
		{
			cout<<"Transmitting: ";
			for(int zz =0; zz<trans_PE_ID.size();zz++ )
			{
				cout<<"("<<trans_PE_ID[zz]<<"--"<<trans_conv[zz]<<")";
			}
			cout<<endl;
		}
		/*----------------------------------------*/
	}else if(NN_Model->all_leyer_type[ID_layer+1]== 'c')
	{
		int temp_nxtgrp_neuron = NN_Model->all_leyer_ID_Group[ID_layer][0];
		PE_table_nxtlayer_neuron=NN_Model->Group_table[temp_nxtgrp_neuron];
		for(int aa =0; aa<Use_Neu; aa++)
		{
			done = 0;
			for(int ab =0; ab< NN_Model->all_conv_coord[PE_table_nxtlayer_neuron[0].ID_conv].size();ab++ )
			{
				for(int ac=0; ac<NN_Model->all_conv_coord[PE_table_nxtlayer_neuron[0].ID_conv][ab].size(); ac++ )
				{
					/*----------Debugging----------*/
					/*if(ID_group == 7 && aa == 85)
					{
						cout<<(PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))<<endl;
					}*/
						/*-----------------------------*/
					if( (PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))== NN_Model->all_conv_coord[PE_table_nxtlayer_neuron[0].ID_conv][ab][ac])
					{
						
						for(int ad =0; ad< NN_Model->all_leyer_ID_Group[ID_layer].size(); ad++)
						{
							int temp_Group = NN_Model->all_leyer_ID_Group[ID_layer][ad];
							PE_table_nxtlayer = NN_Model->Group_table[temp_Group];
							for(int ae=0; ae<PE_table_nxtlayer.size();ae++)
							{
								/*------------------Debugging-----------------*/
									/*if(ID_group == 7 && aa == 85)
									{
										cout<<ab<<"--"<<(NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2])<<"--"<<(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))<<"--";
										cout<<(PE_table_nxtlayer[ae].ID_In_layer )<<"--";
										cout<<(NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2])*(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))<<endl;	
									}*/
									/*--------------------------------------------*/
								if((ab+ (NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2])*(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2]))) == (PE_table_nxtlayer[ae].ID_In_layer))
								{
									
									trans_PE_ID_conv.push_back(NN_Model-> mapping_table[temp_Group]);
									done =1;
									
									break;
								}
							}if(done ==1 ){break;}
							
						}if(done ==1 ){break;}
					}
					
				}if(done ==1 ){break;}
				
			}
		}
	}

	trans_PE_ID.clear();
	trans_PE_ID.push_back(trans_PE_ID_conv[0]);
	trans_conv.clear();
	int needed=0;
	for(int ag =0; ag<trans_PE_ID_conv.size(); ag++)
	{
		needed =0;
		for(int ah=0;ah<trans_PE_ID.size(); ah++)
		{
			if(trans_PE_ID_conv[ag] == trans_PE_ID[ah])
			{
				needed =0;
				break;
			}else{
				needed =1;
			}
		}
		if(needed == 1){trans_PE_ID.push_back(trans_PE_ID_conv[ag]);}
	}

	
	//counts per PE for packet size
	int count;
	for(int au=0;au< trans_PE_ID.size();au++)
	{
		count =0;
		for(int av=0; av<trans_PE_ID_conv.size(); av++ )
		{
			if(trans_PE_ID[au] == trans_PE_ID_conv[av])
			{
				count = count+1;
			}
		}trans_conv.push_back(count);
	}

	/*-------------------Debugging------------------*/
		/*if(local_id == 26)
		{	
			//cout<<Use_Neu<<endl;
			for(int za=0; za<trans_PE_ID.size();za++)
			{
				cout<<"("<<trans_PE_ID[za]<<"--"<<trans_conv[za]<<")--";
			}
			//cout<<NN_Model->all_pool_coord[PE_table_nxtlayer_neuron[0].ID_pool].size()<<endl;
			//cout<<(NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])<<endl;
		}
		
		/*----------------------------------------------*/

}

void NoximProcessingElement::Convreceive(bool SingleOrFirstLayer)
{
	/*--------------Debugging-------------*/
	/*if(ID_group == 30 && NN_Model->PEMappingCount.size() >1)
	{
		cout<<"ConvReceive "<<ID_group<<"-"<<ID_layer<<endl;
		
	}


	/*------------------------------------*/
	receive_neu_ID_conv.clear();
	deque<int> temp_receive_neu_id_conv;
	deque< NeuInformation > PE_table_Prevlayer;
	int done =0;	
	for(int aa=0; aa<Use_Neu;aa++)
	{
		for(int ab =0; ab< NN_Model->all_conv_coord[PE_table[aa].ID_conv][PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])].size(); ab++)
		{
			int id_in_layer = NN_Model->all_conv_coord[PE_table[aa].ID_conv][PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])][ab];
			//cout<<"("<<id_in_layer<<"-"<<SingleOrFirstLayer<<"--"<<ID_layer<<")";
			done =0;
			if(ID_layer != 1 && !SingleOrFirstLayer)
			{
				for(int ac=0; ac< NN_Model->all_leyer_ID_Group[ID_layer-2].size();ac++)
				{
					int temp_Prevgrp = NN_Model->all_leyer_ID_Group[ID_layer-2][ac];
					PE_table_Prevlayer=NN_Model->Group_table[temp_Prevgrp];
					for(int ad =0; ad<PE_table_Prevlayer.size(); ad++)
					{
						if(id_in_layer == PE_table_Prevlayer[ad].ID_In_layer)
						{
							temp_receive_neu_id_conv.push_back(PE_table_Prevlayer[ad].ID_Neu);
							done =1;
							break;
						}
					}if(done == 1)break;
				}
			}else
			{
				temp_receive_neu_id_conv.push_back(id_in_layer);
			}
			
			
		}
		receive_neu_ID_conv.push_back(temp_receive_neu_id_conv);
		temp_receive_neu_id_conv.clear();
	}

	if(ID_layer != 1 && !SingleOrFirstLayer)
	{
		int needed=0;
		receive_Neu_ID.clear();
		receive_Neu_ID.push_back(receive_neu_ID_conv[0][0]);
		for( int ba = 0; ba <receive_neu_ID_conv.size() ; ba++)
		{
			for(int bb =0; bb< receive_neu_ID_conv[ba].size(); bb++)
			{
				needed = 0;
				for(int bc =0; bc< receive_Neu_ID.size(); bc++)
				{
					if(receive_neu_ID_conv[ba][bb] == receive_Neu_ID[bc])
					{
						needed = 0;
						break;
					}else
					{
						needed=1;
					}
					
				}if(needed ==1) receive_Neu_ID.push_back(receive_neu_ID_conv[ba][bb]);
			}
		}
		
	}
	receive = receive_Neu_ID.size();
	should_receive = receive;
	receive_data.clear();
	receive_data.assign(receive , 0 );
	

	/*----------------Debugging-----------------*/
	/*if(ID_group == 24)
	{
		cout<<receive_Neu_ID.size()<<endl;
	}
	/*------------------------------------------*/
	
}

void NoximProcessingElement::Pooltrans()
{
	if(NN_Model->all_leyer_type[ID_layer +1] == 'c')
	{
		deque< NeuInformation > PE_table_nxtlayer;
		deque< NeuInformation > PE_table_nxtlayer_neuron;
		trans_PE_ID_pool.clear();
		int temp_nxtgrp_neuron = NN_Model->all_leyer_ID_Group[ID_layer][0];
		PE_table_nxtlayer_neuron=NN_Model->Group_table[temp_nxtgrp_neuron];
		deque< int> temp_trans_pool;
		int needed =0;
		for(int aa=0; aa< Use_Neu; aa++)
		{
			for(int ab =0; ab < NN_Model->all_conv_coord[PE_table_nxtlayer_neuron[0].ID_conv].size(); ab++)
			{
				for(int ac =0; ac <NN_Model->all_conv_coord[PE_table_nxtlayer_neuron[0].ID_conv][ab].size(); ac++ )
				{
					if(NN_Model->all_conv_coord[PE_table_nxtlayer_neuron[0].ID_conv][ab][ac] == (PE_table[aa].ID_In_layer%(NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])))
					{
						for(int af =0; af< NN_Model->all_leyer_size[ID_layer+1][3] ; af++)
						{
							for(int ad =0; ad< NN_Model->all_leyer_ID_Group[ID_layer].size(); ad++)
							{
								int temp_Group = NN_Model->all_leyer_ID_Group[ID_layer][ad];
								PE_table_nxtlayer = NN_Model->Group_table[temp_Group];
								for(int ae=0; ae<PE_table_nxtlayer.size();ae++)
								{
									if((ab + af*(NN_Model->all_leyer_size[ID_layer+1][1] *NN_Model->all_leyer_size[ID_layer+1][2] )) == (PE_table_nxtlayer[ae].ID_In_layer))
									{
										needed =1;
										for(int am =0; am<temp_trans_pool.size(); am++ )
										{
											if(NN_Model-> mapping_table[temp_Group] == temp_trans_pool[am])
											{
												needed =0;
												break;
											}else{
												needed =1;
											}
										} if(needed == 1){temp_trans_pool.push_back(NN_Model-> mapping_table[temp_Group]);}	
										
									}
								}
							}
						}
					}
				}
			}
			trans_PE_ID_pool.push_back(temp_trans_pool);
			temp_trans_pool.clear();
		}
		/*-------------Debugging--------------------*/
		/*if(ID_group == 0)
		{
			//cout<<"("<<temp_nxtgrp_neuron<<"--"<< PE_table_nxtlayer_neuron[0].ID_conv<<")"<<endl;
			for(int ff=0; ff< trans_PE_ID_pool[0].size(); ff++)
			{
				cout<<trans_PE_ID_pool[0][ff]<<"--";
			}
			cout<<endl;
			cout<<endl<<"Size: "<< trans_PE_ID_pool[0].size()<<endl;;
		}
		/*------------------------------------------*/

		trans_PE_ID.clear();
		trans_PE_ID.push_back(trans_PE_ID_pool[0][0]);
		needed =0;
		for(int an =0; an<trans_PE_ID_pool.size();an++)
		{
			for(int ao=0;ao<trans_PE_ID_pool[an].size();ao++)
			{
				for(int ap=0; ap<trans_PE_ID.size();ap++ )
				{
					if(trans_PE_ID_pool[an][ao] == trans_PE_ID[ap])
					{
						needed =0;
						break;
					}else
					{
						needed =1;
					}
				}if(needed == 1){trans_PE_ID.push_back(trans_PE_ID_pool[an][ao]);}
			}
		}
		
		trans_pool.clear();
		int count;
		for(int au=0;au< trans_PE_ID.size();au++)
		{
			count =0;
			for(int av=0; av<trans_PE_ID_pool.size(); av++ )
			{
				for(int aw =0; aw<trans_PE_ID_pool[av].size();aw++)
				{
					if(trans_PE_ID[au] == trans_PE_ID_pool[av][aw])
					{
						count = count+1;
					}
				}
				
			}trans_pool.push_back(count);
		}
		/*----------------Debugging----------------*/
		/*if(ID_group == 0)
		{
			cout<<"Size of trans pe id: ("<<trans_PE_ID.size()<<"-";
			for(int a=0; a< trans_PE_ID.size(); a++)
			{
				cout<<trans_PE_ID[a]<<"-"<<trans_pool[a]<<"-";
			}
			cout<<")"<<endl;
		}
		/*-----------------------------------------*/

	}else if(NN_Model->all_leyer_type[ID_layer +1] == 'f')
	{
		//(Step3: if next layer is FC)
		trans_PE_ID.clear();
		int as;
		for( as = 0 ; as<NN_Model->all_leyer_ID_Group[ID_layer].size() ; as++)
		{
			int temp_Group = NN_Model->all_leyer_ID_Group[ID_layer][as];
			trans_PE_ID.push_back(NN_Model-> mapping_table[temp_Group]);
		}
		trans = as;
		should_trans = trans;

		flag_p =0;
		flag_f =0;
		/*---------------Debugging----------------*/
		//cout<<"Current layer is pooling and next is fully connected.....";
		//cout<<"("<<ID_group<<")-("<<ID_layer<<")-("<<trans<<")-("<<trans_PE_ID[0]<<")-("<<trans_PE_ID[1]<<endl;
		/*----------------------------------------*/

	}
}

void NoximProcessingElement::Poolreceive(bool SingleOrFirstLayer)
{
	receive_neu_ID_pool.clear();
	deque<int> temp_receive_neu_id_pool;
	deque< NeuInformation > PE_table_Prevlayer;
	int done =0;
	for(int aa=0; aa<Use_Neu;aa++)
	{
		for(int ab =0; ab< NN_Model->all_pool_coord[PE_table[aa].ID_pool][PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])].size(); ab++)
		{
			int term = (NN_Model->all_leyer_size[ID_layer-1][1]* NN_Model->all_leyer_size[ID_layer-1][2])*(PE_table[aa].ID_In_layer / (NN_Model->all_leyer_size[ID_layer][1]* NN_Model->all_leyer_size[ID_layer][2]));
			int id_in_layer = NN_Model->all_pool_coord[PE_table[aa].ID_pool][PE_table[aa].ID_In_layer % (NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2])][ab]+term;
			done =0;
			if( !SingleOrFirstLayer )
			{
				for(int ac =0; ac< NN_Model->all_leyer_ID_Group[ID_layer-2].size(); ac++)
				{
					int temp_Prevgrp = NN_Model->all_leyer_ID_Group[ID_layer-2][ac];
					PE_table_Prevlayer=NN_Model->Group_table[temp_Prevgrp];
					for(int ad =0; ad<PE_table_Prevlayer.size(); ad++)
					{
						if(id_in_layer == PE_table_Prevlayer[ad].ID_In_layer )
						{
							temp_receive_neu_id_pool.push_back(PE_table_Prevlayer[ad].ID_Neu);
							done =1;
							break;
						}
					}if(done == 1)break;
				}
			}
			else
			{
				temp_receive_neu_id_pool.push_back(id_in_layer);
			}
			
			
			
			/*--------------------Debugging--------------------*/
			//if(ID_group == 0)
			//{
			//	cout<<"("<< aa<<"--"<<id_in_layer<<")";
			//}
			/*------------------------------------------------*/
		}
		receive_neu_ID_pool.push_back(temp_receive_neu_id_pool);
		temp_receive_neu_id_pool.clear();
	}
	

	if(!SingleOrFirstLayer)
	{
		int needed=0;
		receive_Neu_ID.clear();
		receive_Neu_ID.push_back(receive_neu_ID_pool[0][0]);
		for( int ba = 0; ba <receive_neu_ID_pool.size() ; ba++)
		{
			for(int bb =0; bb< receive_neu_ID_pool[ba].size(); bb++)
			{
				needed =0;
				for(int bc =0; bc< receive_Neu_ID.size(); bc++)
				{
					if(receive_neu_ID_pool[ba][bb] == receive_Neu_ID[bc])
					{
						needed =0;
						break;
					}else
					{
						needed =1;
					}
					
				}if(needed ==1) receive_Neu_ID.push_back(receive_neu_ID_pool[ba][bb]);
			}
		}

		receive = receive_Neu_ID.size();
		should_receive = receive;
		receive_data.assign(receive , 0 );
	}
	/*--------------------Debugging--------------------*/
	/*if(ID_group == 48)
	{
		for(int u=0; u<receive_Neu_ID.size(); u++)
		{
			cout<<receive_Neu_ID[u]<<"--";
		}
		cout<<endl;
	}
	/*------------------------------------------------*/
}

void NoximProcessingElement::DynamicMappingDone()
{
	//cout<<NN_Model->Mapping_done<<endl;
	if(trig_mapping.read())//NN_Model->Mapping_done == true)
	{
		//cout<<local_id<<endl;
		//cout<<"Mapping status: "<<NN_Model->Mapping_done<<endl;
		
		//cout<<"Mapping status: "<<NN_Model->Mapping_done<<endl;
		PreprocessingProcess();	
		NN_Model->Mapping_done = false;
	}
}

void NoximProcessingElement::LayerConvComp(deque<float> &data_deq)
{
	float value=0.0;
	int kernel_size = NN_Model->all_leyer_size[ID_layer][4]*NN_Model->all_leyer_size[ID_layer][5];
	int kernel_z= NN_Model->all_leyer_size[ID_layer][6];
	int denominator = NN_Model->all_leyer_size[ID_layer][1]*NN_Model->all_leyer_size[ID_layer][2];
	for(int aa =0; aa<Use_Neu; aa++)
	{
		value =0.0;
		for(int ab=0; ab< kernel_z ;ab++)
		{
			for(int ac =0; ac< kernel_size;ac++)
			{
				value += NN_Model->all_conv_weight[PE_table[aa].ID_conv][PE_table[aa].ID_In_layer / denominator][ab][ac] * data_deq[receive_neu_ID_conv[aa][ac+ab*kernel_size]];//NN_Model-> all_data_in[in_data][receive_neu_ID_conv[aa][ac+ab*kernel_size]];
			}
			
		}
		//Adding bias
		value += NN_Model ->all_conv_bias[PE_table[aa].ID_conv][PE_table[aa].ID_In_layer / denominator];

		//Activation function
		if ( NN_Model->all_leyer_size[ID_layer].back() == RELU )//relu
		{
			if (value <= 0) 
				res[aa]=0;
			else
				res[aa] = value;				
		}
	}	
}

void NoximProcessingElement::LayerPoolComp(deque<float> &data_deq)
{
	float value;
	for(int r=0; r< Use_Neu; r++)
	{
		value =0.0;
		for(int s=0; s< receive_neu_ID_pool[r].size(); s++)
		{
			if ( NN_Model->all_leyer_size[ID_layer].back() == AVERAGE )//average
			{

				value = value + data_deq[receive_neu_ID_pool[r][s]];

			}else if(NN_Model->all_leyer_size[ID_layer].back() == MAXIMUM)
			{
				if(value < data_deq[receive_neu_ID_pool[r][s]])
				{
					value = data_deq[receive_neu_ID_pool[r][s]];
				}

			}
	
		}
		if(NN_Model->all_leyer_size[ID_layer].back() == AVERAGE)
			{
				/*-------------debugging---------------*/
				/*if(ID_group == 0)
				{
					cout<<"("<< value<<"--";
					cout<< value/4<<")";
				}
				/*-------------------------------------*/
				value = value / (NN_Model->all_leyer_size[ID_layer][4]*NN_Model->all_leyer_size[ID_layer][5]);
			}
			res[r] = value;
	}

	/*---------------Debugging---------------*/
	/*if(ID_group ==0)
	{
		int val = 25;
		cout<<endl<<"Pool layer "<<ID_layer<<":(";
		for(int af=0; af< receive_neu_ID_pool[val].size();af++)
		{
			cout<<receive_neu_ID_pool[val][af]<<"-"<<data_deq[receive_neu_ID_pool[val][af]]<<"--";
			//cout<<res[af]<<"-";
		}
		cout<<res[val]<<")"<<endl;
		//cout<<receive_neu_ID_pool[0].size();
	}
	/*---------------------------------------*/
}

void NoximProcessingElement::LayerFCComp(deque<float> &data_deq)
{
	float denominator_value =0.0;
	for (int j = 0 ; j<receive ; j++)  //receive
	{					
		for (int i = 0 ; i<Use_Neu ; i++)  //Use_Neu
		{
			//********************fully connected********************** //
			
			float weight_tmp = PE_table[i].weight[j]; 
			
			res[i] += data_deq[j] * weight_tmp;  
			/*---------------------------Debugging----------------------------*/
			/*if(ID_group == 83 && i ==2)
			{
				cout<<"("<<res[i]<<")";
			}*/
			/*----------------------------------------------------------------*/

			if (j==receive-1) //(j==receive-1)
			{
				float bias_tmp = PE_table[i].weight.back();  

				res[i] += bias_tmp;				// act fun & compute complete 
				/*---------------------------Debugging----------------------------*/
				/*if(ID_group == 83)
				{
					cout<<"Neuron "<<i<<": "<<res[i]<<endl;
				}*/
				/*----------------------------------------------------------------*/
				if ( NN_Model->all_leyer_size[ID_layer].back() == RELU )//relu
				{
					if (res[i] <= 0) 
						res[i]=0;				
				}
				else if ( NN_Model->all_leyer_size[ID_layer].back() == TANH )//tanh
				{
					res[i]= 2/(1+exp(-2*res[i]))-1;		
				}
				else if ( NN_Model->all_leyer_size[ID_layer].back() == SIGMOID )//sigmoid
				{
					//cout<<"Sigmoid"<<endl;
					res[i]= 1/(1+exp(-1*res[i]));	//res[i]= 1/(1+exp(-1*res[i]));
				}
				else if ( NN_Model->all_leyer_size[ID_layer].back() == SOFTMAX )//softmax
				{											
					res[i] = exp(res[i]);
					
					if(ID_layer == NN_Model->all_leyer_size.size()-1 && i == 9)
					{
						for(int fd =0; fd< NN_Model->all_leyer_size[ID_layer].front(); fd++)
						{
							denominator_value += res[fd];
							/*----------------Debugging------------*/
							
							/*if( fd== 9)
							{
								cout<< "Denominator: "<< denominator_value<<endl;
							}
							cout<<res[fd]<<"-"<<exp(res[fd])<<endl;*/
							/*-------------------------------------*/
						}
					}
				}

				if (ID_layer == NN_Model->all_leyer_size.size()-1 && NN_Model->all_leyer_size[ID_layer].back() != SOFTMAX)
				{
					//cout << sc_simulation_time() + computation_time <<": The prediction result of item "<< PE_table[i].ID_In_layer << " : " << res[i] << endl;
					char output_file[11];
					sprintf(output_file,"output.txt");
					fstream file_o;
					file_o.open( output_file ,ios::out|ios::app);
					file_o << "No." << i << " output neuron result: ";
					file_o << res[i] << endl; //file_o << res[i] << endl;
				}
			}
			
		}
	}
	if (ID_layer == NN_Model->all_leyer_size.size()-1 && NN_Model->all_leyer_size[ID_layer].back() == SOFTMAX)
	{
		//cout << sc_simulation_time() + computation_time <<": The prediction result of item "<< PE_table[i].ID_In_layer << " : " << res[i] << endl;
		for(int ff=0; ff< NN_Model->all_leyer_size[ID_layer].front(); ff++)
		{
			res[ff] = res[ff]/denominator_value;
			
			char output_file[11];
			sprintf(output_file,"output.txt");
			fstream file_o;
			file_o.open( output_file ,ios::out|ios::app);
			file_o << "No." << ff << " output neuron result: ";
			file_o << res[ff] << endl; //file_o << res[i] << endl;
		}
		
	}
}