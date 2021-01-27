/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the Network-on-Chip
 */

#include "NoximNoC.h"
//int throttling[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y][DEFAULT_MESH_DIM_Z];		//** 2018.09.10 edit by Yueh-Chi,Yang **// //**  **//

void NoximNoC::buildMesh()
{
    

    // Check for traffic table availability
    if (NoximGlobalParams::traffic_distribution == TRAFFIC_TABLE_BASED)
	assert(gttable.load(NoximGlobalParams::traffic_table_filename));
    nnmodel.load();	//tytyty

	// Check for routing table availability
    if (NoximGlobalParams::routing_algorithm == ROUTING_TABLE_BASED)
	assert(grtable.load(NoximGlobalParams::routing_table_filename));

    // Create the mesh as a matrix of tiles
    /****************MODIFY BY HUI-SHUN********************/
	//Hui-shun add z dim for each tile
	int i,j,k;
	for ( i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
		for ( j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
			for( k = 0; k < NoximGlobalParams::mesh_dim_z; k++) {
				// Create the single Tile with a proper name
				char tile_name[20];
				//sprintf(tile_name, "Tile[%02d][%02d]", i, j);
				sprintf(tile_name, "Tile[%02d][%02d][%02d]", i, j, k);
	    t[i][j][k] = new NoximTile(tile_name);

	    // Tell to the router its coordinates
	    t[i][j][k]->r->configure(k * NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y + j*NoximGlobalParams::mesh_dim_x +i,
				  NoximGlobalParams::stats_warm_up_time,
				  NoximGlobalParams::buffer_depth,
				  grtable);
		
	    // Tell to the PE its coordinates
	    t[i][j][k]->pe->local_id = k * NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y + j*NoximGlobalParams::mesh_dim_x +i;
	    t[i][j][k]->pe->traffic_table = &gttable;	// Needed to choose destination
	    t[i][j][k]->pe->NN_Model = &nnmodel;	//tytyty
	    t[i][j][k]->pe->never_transmit = (gttable.occurrencesAsSource(t[i][j][k]->pe->local_id) == 0);
		
	    // Map clock and reset
	    t[i][j][k]->clock(clock);
	    t[i][j][k]->reset(reset);

	    // Map Rx signals
	    t[i][j][k]->req_rx[DIRECTION_NORTH] (req_to_south[i][j][k]);
	    t[i][j][k]->flit_rx[DIRECTION_NORTH] (flit_to_south[i][j][k]);
	    t[i][j][k]->ack_rx[DIRECTION_NORTH] (ack_to_north[i][j][k]);

	    t[i][j][k]->req_rx[DIRECTION_EAST] (req_to_west[i + 1][j][k]);
	    t[i][j][k]->flit_rx[DIRECTION_EAST] (flit_to_west[i + 1][j][k]);
	    t[i][j][k]->ack_rx[DIRECTION_EAST] (ack_to_east[i + 1][j][k]);

	    t[i][j][k]->req_rx[DIRECTION_SOUTH] (req_to_north[i][j + 1][k]);
	    t[i][j][k]->flit_rx[DIRECTION_SOUTH] (flit_to_north[i][j + 1][k]);
	    t[i][j][k]->ack_rx[DIRECTION_SOUTH] (ack_to_south[i][j + 1][k]);

	    t[i][j][k]->req_rx[DIRECTION_WEST] (req_to_east[i][j][k]);
	    t[i][j][k]->flit_rx[DIRECTION_WEST] (flit_to_east[i][j][k]);
	    t[i][j][k]->ack_rx[DIRECTION_WEST] (ack_to_west[i][j][k]);

		t[i][j][k]->req_rx[DIRECTION_UP](req_to_down[i][j][k]);
		t[i][j][k]->flit_rx[DIRECTION_UP](flit_to_down[i][j][k]);
		t[i][j][k]->ack_rx[DIRECTION_UP](ack_to_up[i][j][k]);

		t[i][j][k]->req_rx[DIRECTION_DOWN](req_to_up[i][j][k+1]);
		t[i][j][k]->flit_rx[DIRECTION_DOWN](flit_to_up[i][j][k+1]);
		t[i][j][k]->ack_rx[DIRECTION_DOWN](ack_to_down[i][j][k+1]);
	    // Map Tx signals
	    t[i][j][k]->req_tx[DIRECTION_NORTH] (req_to_north[i][j][k]);
	    t[i][j][k]->flit_tx[DIRECTION_NORTH] (flit_to_north[i][j][k]);
	    t[i][j][k]->ack_tx[DIRECTION_NORTH] (ack_to_south[i][j][k]);

	    t[i][j][k]->req_tx[DIRECTION_EAST] (req_to_east[i + 1][j][k]);
	    t[i][j][k]->flit_tx[DIRECTION_EAST] (flit_to_east[i + 1][j][k]);
	    t[i][j][k]->ack_tx[DIRECTION_EAST] (ack_to_west[i + 1][j][k]);

	    t[i][j][k]->req_tx[DIRECTION_SOUTH] (req_to_south[i][j + 1][k]);
	    t[i][j][k]->flit_tx[DIRECTION_SOUTH] (flit_to_south[i][j + 1][k]);
	    t[i][j][k]->ack_tx[DIRECTION_SOUTH] (ack_to_north[i][j + 1][k]);

	    t[i][j][k]->req_tx[DIRECTION_WEST] (req_to_west[i][j][k]);
	    t[i][j][k]->flit_tx[DIRECTION_WEST] (flit_to_west[i][j][k]);
	    t[i][j][k]->ack_tx[DIRECTION_WEST] (ack_to_east[i][j][k]);
		
		t[i][j][k]->req_tx[DIRECTION_UP](req_to_up[i][j][k]);
		t[i][j][k]->flit_tx[DIRECTION_UP](flit_to_up[i][j][k]);
		t[i][j][k]->ack_tx[DIRECTION_UP](ack_to_down[i][j][k]);

		t[i][j][k]->req_tx[DIRECTION_DOWN](req_to_down[i][j][k+1]);
		t[i][j][k]->flit_tx[DIRECTION_DOWN](flit_to_down[i][j][k+1]);
		t[i][j][k]->ack_tx[DIRECTION_DOWN](ack_to_up[i][j][k+1]);
	    // Map buffer level signals (analogy with req_tx/rx port mapping)
	    t[i][j][k]->free_slots[DIRECTION_NORTH] (free_slots_to_north[i][j][k]);
	    t[i][j][k]->free_slots[DIRECTION_EAST] (free_slots_to_east[i + 1][j][k]);
	    t[i][j][k]->free_slots[DIRECTION_SOUTH] (free_slots_to_south[i][j + 1][k]);
	    t[i][j][k]->free_slots[DIRECTION_WEST] (free_slots_to_west[i][j][k]);
		t[i][j][k]->free_slots[DIRECTION_UP](free_slots_to_up[i][j][k]);////
		t[i][j][k]->free_slots[DIRECTION_DOWN](free_slots_to_down[i][j][k+1]);////
		
	    t[i][j][k]->free_slots_neighbor[DIRECTION_NORTH] (free_slots_to_south[i][j][k]);
	    t[i][j][k]->free_slots_neighbor[DIRECTION_EAST] (free_slots_to_west[i + 1][j][k]);
	    t[i][j][k]->free_slots_neighbor[DIRECTION_SOUTH] (free_slots_to_north[i][j + 1][k]);
	    t[i][j][k]->free_slots_neighbor[DIRECTION_WEST] (free_slots_to_east[i][j][k]);
		t[i][j][k]->free_slots_neighbor[DIRECTION_UP](free_slots_to_down[i][j][k]);////
		t[i][j][k]->free_slots_neighbor[DIRECTION_DOWN](free_slots_to_up[i][j][k+1]);////
	    // NoP 
	    t[i][j][k]->NoP_data_out[DIRECTION_NORTH] (NoP_data_to_north[i][j][k]);
	    t[i][j][k]->NoP_data_out[DIRECTION_EAST] (NoP_data_to_east[i + 1][j][k]);
	    t[i][j][k]->NoP_data_out[DIRECTION_SOUTH] (NoP_data_to_south[i][j + 1][k]);
	    t[i][j][k]->NoP_data_out[DIRECTION_WEST] (NoP_data_to_west[i][j][k]);
		t[i][j][k]->NoP_data_out[DIRECTION_UP](NoP_data_to_up[i][j][k]);////
		t[i][j][k]->NoP_data_out[DIRECTION_DOWN](NoP_data_to_down[i][j][k+1]);////
		
	    t[i][j][k]->NoP_data_in[DIRECTION_NORTH] (NoP_data_to_south[i][j][k]);
	    t[i][j][k]->NoP_data_in[DIRECTION_EAST] (NoP_data_to_west[i + 1][j][k]);
	    t[i][j][k]->NoP_data_in[DIRECTION_SOUTH] (NoP_data_to_north[i][j + 1][k]);
	    t[i][j][k]->NoP_data_in[DIRECTION_WEST] (NoP_data_to_east[i][j][k]);
		t[i][j][k]->NoP_data_in[DIRECTION_UP](NoP_data_to_down[i][j][k]);////
		t[i][j][k]->NoP_data_in[DIRECTION_DOWN](NoP_data_to_up[i][j][k+1]);////
		/*****RCA*****/
		t[i][j][k]->monitor_out[DIRECTION_NORTH](RCA_to_north[i][j][k]);
		t[i][j][k]->monitor_out[DIRECTION_EAST](RCA_to_east[i+1][j][k]);
		t[i][j][k]->monitor_out[DIRECTION_SOUTH](RCA_to_south[i][j+1][k]);
		t[i][j][k]->monitor_out[DIRECTION_WEST](RCA_to_west[i][j][k]);
		t[i][j][k]->monitor_out[DIRECTION_UP](RCA_to_up[i][j][k]);////
		t[i][j][k]->monitor_out[DIRECTION_DOWN](RCA_to_down[i][j][k+1]);////
		t[i][j][k]->monitor_in[DIRECTION_NORTH](RCA_to_south[i][j][k]);
		t[i][j][k]->monitor_in[DIRECTION_EAST](RCA_to_west[i+1][j][k]);
		t[i][j][k]->monitor_in[DIRECTION_SOUTH](RCA_to_north[i][j+1][k]);
		t[i][j][k]->monitor_in[DIRECTION_WEST](RCA_to_east[i][j][k]);
		t[i][j][k]->monitor_in[DIRECTION_UP](RCA_to_down[i][j][k]);////
		t[i][j][k]->monitor_in[DIRECTION_DOWN](RCA_to_up[i][j][k+1]);////
		/*****RCA*****/
		/*******THROTTLING******/	
		// on/off ports in emergency mode
		t[i][j][k]->on_off[DIRECTION_NORTH](on_off_to_north[i][j][k]);
		t[i][j][k]->on_off[DIRECTION_EAST](on_off_to_east[i+1][j][k]);
		t[i][j][k]->on_off[DIRECTION_SOUTH](on_off_to_south[i][j+1][k]);
		t[i][j][k]->on_off[DIRECTION_WEST](on_off_to_west[i][j][k]);
		t[i][j][k]->on_off_neighbor[DIRECTION_NORTH](on_off_to_south[i][j][k]);
		t[i][j][k]->on_off_neighbor[DIRECTION_EAST](on_off_to_west[i+1][j][k]);
		t[i][j][k]->on_off_neighbor[DIRECTION_SOUTH](on_off_to_north[i][j+1][k]);
		t[i][j][k]->on_off_neighbor[DIRECTION_WEST](on_off_to_east[i][j][k]);
		/*******THROTTLING******/
		/*******DOWNWARD ROUTING******/
		// DW_tag ports
		t[i][j][k]->DW_tag[DIRECTION_NORTH](DW_tag_to_north[i][j][k]);
		t[i][j][k]->DW_tag[DIRECTION_EAST](DW_tag_to_east[i+1][j][k]);
		t[i][j][k]->DW_tag[DIRECTION_SOUTH](DW_tag_to_south[i][j+1][k]);
		t[i][j][k]->DW_tag[DIRECTION_WEST](DW_tag_to_west[i][j][k]);
		t[i][j][k]->DW_tag_neighbor[DIRECTION_NORTH](DW_tag_to_south[i][j][NoximGlobalParams::mesh_dim_z-1]);	//\B3\A3\B1\B5\A8\EC\B3̩\B3\BCh\B5o\A5X\AA\BADW_tag
		t[i][j][k]->DW_tag_neighbor[DIRECTION_EAST](DW_tag_to_west[i+1][j][NoximGlobalParams::mesh_dim_z-1]);
		t[i][j][k]->DW_tag_neighbor[DIRECTION_SOUTH](DW_tag_to_north[i][j+1][NoximGlobalParams::mesh_dim_z-1]);
		t[i][j][k]->DW_tag_neighbor[DIRECTION_WEST](DW_tag_to_east[i][j][NoximGlobalParams::mesh_dim_z-1]);
		/*******DOWNWARD ROUTING******/
			}
		}
    }

    // dummy NoximNoP_data structure
    NoximNoP_data tmp_NoP;

    tmp_NoP.sender_id = NOT_VALID;

    for ( i = 0; i < DIRECTIONS; i++) {
	tmp_NoP.channel_status_neighbor[i].free_slots = NOT_VALID;
	tmp_NoP.channel_status_neighbor[i].available = false;
    }
	/*******DOWNWARD ROUTING******/
	for( k=0; k<=NoximGlobalParams::mesh_dim_z-2; k++)
		for( j=0; j<=NoximGlobalParams::mesh_dim_y; j++)
			for( i=0; i<=NoximGlobalParams::mesh_dim_x; i++)
			{
			DW_tag_to_south[i][j][k].write(NOT_VALID);
			DW_tag_to_north[i][j][k].write(NOT_VALID);
			DW_tag_to_east[i][j][k].write(NOT_VALID);
			DW_tag_to_west[i][j][k].write(NOT_VALID);
    }
	/*******DOWNWARD ROUTING******/
    // Clear signals for borderline nodes
    /*	for (int i = 0; i <= NoximGlobalParams::mesh_dim_x; i++) {
	req_to_south[i][0] = 0;
	ack_to_north[i][0] = 0;
	req_to_north[i][NoximGlobalParams::mesh_dim_y] = 0;
	ack_to_south[i][NoximGlobalParams::mesh_dim_y] = 0;
	free_slots_to_south[i][0].write(NOT_VALID);
	free_slots_to_north[i][NoximGlobalParams::mesh_dim_y].write(NOT_VALID);
	NoP_data_to_south[i][0].write(tmp_NoP);
	NoP_data_to_north[i][NoximGlobalParams::mesh_dim_y].write(tmp_NoP);
    }
	for (int j = 0; j <= NoximGlobalParams::mesh_dim_y; j++) {
	req_to_east[0][j] = 0;
	ack_to_west[0][j] = 0;
	req_to_west[NoximGlobalParams::mesh_dim_x][j] = 0;
	ack_to_east[NoximGlobalParams::mesh_dim_x][j] = 0;

	free_slots_to_east[0][j].write(NOT_VALID);
	free_slots_to_west[NoximGlobalParams::mesh_dim_x][j].write(NOT_VALID);

	NoP_data_to_east[0][j].write(tmp_NoP);
	NoP_data_to_west[NoximGlobalParams::mesh_dim_x][j].write(tmp_NoP);
	}*/
	for( i=0; i<=NoximGlobalParams::mesh_dim_x; i++){
		for( k=0; k<=NoximGlobalParams::mesh_dim_z; k++){
			req_to_south[i][0][k] = 0;
			ack_to_north[i][0][k] = 0;
			req_to_north[i][NoximGlobalParams::mesh_dim_y][k] = 0;
			ack_to_south[i][NoximGlobalParams::mesh_dim_y][k]= 0;
			
			free_slots_to_south[i][0][k].write(NOT_VALID);
			free_slots_to_north[i][NoximGlobalParams::mesh_dim_y][k].write(NOT_VALID);

			RCA_to_south[i][0][k].write(0);
			RCA_to_north[i][NoximGlobalParams::mesh_dim_y][k].write(0);

			on_off_to_south[i][0][k].write(NOT_VALID);
			on_off_to_north[i][NoximGlobalParams::mesh_dim_y][k].write(NOT_VALID);
      
			DW_tag_to_south[i][0][k].write(NOT_VALID);
			DW_tag_to_north[i][NoximGlobalParams::mesh_dim_y][k].write(NOT_VALID);

			NoP_data_to_south[i][0][k].write(tmp_NoP);
			NoP_data_to_north[i][NoximGlobalParams::mesh_dim_y][k].write(tmp_NoP);
		}
    }
	for( j=0; j<=NoximGlobalParams::mesh_dim_y; j++){
		for( k=0; k<=NoximGlobalParams::mesh_dim_z; k++){
			req_to_east[0][j][k] = 0;
			ack_to_west[0][j][k] = 0;
			req_to_west[NoximGlobalParams::mesh_dim_x][j][k] = 0;
			ack_to_east[NoximGlobalParams::mesh_dim_x][j][k]= 0;

			free_slots_to_east[0][j][k].write(NOT_VALID);
			free_slots_to_west[NoximGlobalParams::mesh_dim_x][j][k].write(NOT_VALID);
	
			RCA_to_east[0][j][k].write(0);
			RCA_to_west[NoximGlobalParams::mesh_dim_x][j][k].write(0);

			on_off_to_east[0][j][k].write(NOT_VALID);
			on_off_to_west[NoximGlobalParams::mesh_dim_x][j][k].write(NOT_VALID);
      	
			DW_tag_to_east[0][j][k].write(NOT_VALID);
			DW_tag_to_west[NoximGlobalParams::mesh_dim_x][j][k].write(NOT_VALID);

			NoP_data_to_east[0][j][k].write(tmp_NoP);
			NoP_data_to_west[NoximGlobalParams::mesh_dim_x][j][k].write(tmp_NoP);
		}
	}
	for( i=0; i<=NoximGlobalParams::mesh_dim_x; i++){
		for( j=0; j<=NoximGlobalParams::mesh_dim_y; j++){
			req_to_down[i][j][0] = 0;
			ack_to_up[i][j][0] = 0;
			req_to_up[i][j][NoximGlobalParams::mesh_dim_z] = 0;
			ack_to_down[i][j][NoximGlobalParams::mesh_dim_z] = 0;

			free_slots_to_down[i][j][0].write(NOT_VALID);//
			free_slots_to_up[i][j][NoximGlobalParams::mesh_dim_z].write(NOT_VALID);//

			RCA_to_down[i][j][0].write(0);
			RCA_to_up[i][j][NoximGlobalParams::mesh_dim_z].write(0);

			NoP_data_to_down[i][j][0].write(tmp_NoP);//
			NoP_data_to_up[i][j][NoximGlobalParams::mesh_dim_z].write(tmp_NoP);//
		}
    }
    // invalidate reservation table entries for non-exhistent channels
    /*for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++) {
	t[i][0]->r->reservation_table.invalidate(DIRECTION_NORTH);
	t[i][NoximGlobalParams::mesh_dim_y - 1]->r->reservation_table.invalidate(DIRECTION_SOUTH);
    }
    for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++) {
	t[0][j]->r->reservation_table.invalidate(DIRECTION_WEST);
	t[NoximGlobalParams::mesh_dim_x - 1][j]->r->reservation_table.invalidate(DIRECTION_EAST);
    }
	*/
	for( i=0; i<NoximGlobalParams::mesh_dim_x; i++){
		for( k=0; k<NoximGlobalParams::mesh_dim_z; k++){
			t[i][0][k]->r->reservation_table.invalidate(DIRECTION_NORTH);
			t[i][NoximGlobalParams::mesh_dim_y-1][k]->r->reservation_table.invalidate(DIRECTION_SOUTH);
			}
		}
	for( j=0; j<NoximGlobalParams::mesh_dim_y; j++){
		for( k=0; k<NoximGlobalParams::mesh_dim_z; k++){
			t[0][j][k]->r->reservation_table.invalidate(DIRECTION_WEST);
			t[NoximGlobalParams::mesh_dim_x-1][j][k]->r->reservation_table.invalidate(DIRECTION_EAST);
			}   
		}
    

  
	for(int z=0; z<4; z++)
	{
		cnt_neighbor_total[z]=0;
		cnt_received_total[z]=0;
		cnt_local_total[z]=0;
		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
			{
				tot_cnt_local[x][y][z] = 0;
				tot_cnt_neighbor[x][y][z] = 0;
				tot_cnt_received[x][y][z] = 0;
			}
	}
	// Initial emergency mode to zero, initial throttle flag to zero
	for (int k=0; k<NoximGlobalParams::mesh_dim_z; k++)
		for (int j=0; j<NoximGlobalParams::mesh_dim_y; j++)
			for (int i=0; i<NoximGlobalParams::mesh_dim_x; i++)
				{
					if(NoximGlobalParams::throt_type == THROT_TEST){
						if(k < (NoximGlobalParams::mesh_dim_z - 2)){
								if(((i == 3))&&((j == 3))){
									t[i][j][k]->pe->emergency = true;
									t[i][j][k]->r->emergency = true;
									t[i][j][k]->pe->throttle_local=true;  	
							t[i][j][k]->r->throttle_neighbor=true; 
								}
								else {
								t[i][j][k]->pe->emergency = false;
								t[i][j][k]->r->emergency = false;
								t[i][j][k]->pe->throttle_local=false;  	
							t[i][j][k]->r->throttle_neighbor=false; 
								}
							}
							else {
								t[i][j][k]->pe->emergency = false;
								t[i][j][k]->r->emergency = false;
								t[i][j][k]->pe->throttle_local=false;  	
							t[i][j][k]->r->throttle_neighbor=false; 
							}
					}
					else{
							t[i][j][k]->r->emergency=false;  	
							t[i][j][k]->pe->throttle_local=false;  	
							t[i][j][k]->r->throttle_neighbor=false;  
					}

					t[i][j][k]->r->emergency_level=0;
					t[i][j][k]->r->Q_ratio=1;
					t[i][j][k]->pe->Q_ratio=1;
						
					t[i][j][k]->pe->cnt_local=0;
					t[i][j][k]->r->cnt_neighbor=0;
					t[i][j][k]->r->cnt_received=0;
					t[i][j][k]->pe->Quota_local=1000000;			//initial to big number
					t[i][j][k]->r->Quota_neighbor=1000000;
					t[i][j][k]->r->DW_tag_cur = NoximGlobalParams::down_level;					
					t[i][j][k]->r->UnavailableTemp = false;
					t[i][j][k]->pe->clean_all = false;   //Matthew
				}

   LastIsEmergency = false;



	for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
		for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++)
			for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++)
		{
			NoximGlobalParams::throttling[x][y][z]=0;
			
			if(x==3 && y==3)
			{
				NoximGlobalParams::throttling[x][y][z]=1;
			}

			if(x==3 && y==3 && (z==3 || z==2))
			{
				NoximGlobalParams::throttling[x][y][z]=0;
			}
		}
}

NoximTile *NoximNoC::searchNode(const int id) const
{
    /****************MODIFY BY HUI-SHUN********************/
	/*for (int i = 0; i < NoximGlobalParams::mesh_dim_x; i++)
	for (int j = 0; j < NoximGlobalParams::mesh_dim_y; j++)
	    if (t[i][j]->r->local_id == id)
		return t[i][j];*/
	int i,j,k;
	for ( i=0; i<NoximGlobalParams::mesh_dim_x; i++)
		for ( j=0; j<NoximGlobalParams::mesh_dim_y; j++)
			for ( k=0; k<NoximGlobalParams::mesh_dim_z; k++)
				if (t[i][j][k]->r->local_id == id)
					return t[i][j][k];
	return NULL;//false;
}

void NoximNoC::entry()  //Foster big modified - 09/11/12
{ 
	//reset power
	if (reset.read()) {
	   //in reset phase, reset power value 
	   for(int k=0; k < NoximGlobalParams::mesh_dim_z; k++) {
			for(int j=0; j < NoximGlobalParams::mesh_dim_y; j++) {	
				for(int i=0; i < NoximGlobalParams::mesh_dim_x; i++) {		
					t[i][j][k]->r->stats.power.resetPwr();
					t[i][j][k]->r->stats.power.resetTransientPwr();
				}
			}
		}
	}
	else{
	  if(( getCurrentCycleNum() % (int) (TEMP_REPORT_PERIOD) ) == 0 )
	  {
	      int x = (int)TEMP_REPORT_PERIOD;
		  //int y = POWER_INTERPOLATION_FACTOR;
		  //int a = int(TEMP_REPORT_PERIOD/POWER_INTERPOLATION_FACTOR);
	      int time =  getCurrentCycleNum();
		  
		  //accumulate steady power after warm-up time
		  if( getCurrentCycleNum() > (int)(NoximGlobalParams::stats_warm_up_time * CYCLE_PERIOD) )
		     steadyPwr2PtraceFile();
		  
		  //transient power
		  transPwr2PtraceFile();//\A7\E2\B2{\A6b\A6U\C2I\AA\BApower\C2নptrace file
	      HS_interface->Temperature_calc(instPowerTrace, TemperatureTrace);
	      setTemperature();

		  bool IsEmergency = EmergencyDecision();		//check\B2{\A6b\B7ū׬O\A7_\B6W\B9Lthermal limit, \ACO\A7_\ADn\B6i\A4Jemergency mode
		//Print information & record cnt	
		for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 
		{
		//	int avg_cnt_local=0;
		//	int avg_cnt_neighbor=0;
		//	int avg_cnt_received=0;
			for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
				for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
				{
	//{

						//cout<<"[Router:"<<t[x][y][z]->r->local_id<<"] cnt_local = "<<t[x][y][z]->pe->cnt_local<<", cnt_neighbor = "<<t[x][y][z]->r->cnt_neighbor<<", cnt_received = "<<t[x][y][z]->r->cnt_received<<endl;

//		  			avg_cnt_local+=t[x][y][z]->pe->cnt_local;
//		  			avg_cnt_neighbor+=t[x][y][z]->r->cnt_neighbor;
//		  			avg_cnt_received+=t[x][y][z]->r->cnt_received;
		  			tot_cnt_local[x][y][z]+=t[x][y][z]->pe->cnt_local;
		  			tot_cnt_neighbor[x][y][z]+=t[x][y][z]->r->cnt_neighbor;
		  			tot_cnt_received[x][y][z]+=t[x][y][z]->r->cnt_received;
		  			
		  			cnt_local_total[z] += t[x][y][z]->pe->cnt_local;
		  			cnt_neighbor_total[z] += t[x][y][z]->r->cnt_neighbor;
		  			cnt_received_total[z] += t[x][y][z]->r->cnt_received;
	//}		  			

		  			if(!LastIsEmergency)	//\A6p\B9L\A4W\AD\D3iteration\A8S\A6\B3thermal emergency, \B4N\A7\E2\B3o\A6\B8\AA\BAtraffic\A5Χ@\B7s\AA\BAtraffic quota\ADp\BA\E2
		  			{
		  				t[x][y][z]->pe->Quota_local = (int)((double)t[x][y][z]->pe->cnt_local * (float)(1-NoximGlobalParams::throt_ratio) );	//set initial quota
		  				t[x][y][z]->r->Quota_neighbor = (int)((double)t[x][y][z]->r->cnt_neighbor * (float)(1-NoximGlobalParams::throt_ratio) ); //set initial quota

		  				//cout<<"SETTING!["<<x<<"]["<<y<<"]["<<z<<"]->pe->Quota_local ="<<t[x][y][z]->pe->Quota_local <<endl;
		  				//cout<<"SETTING!["<<x<<"]["<<y<<"]["<<z<<"]->r->Quota_neighbor ="<<t[x][y][z]->r->Quota_neighbor <<endl;

		  			}
				}
//				cout<<"layer_"<<z<<"_total_cnt_local = "<<avg_cnt_local<<endl;
//				cout<<"layer_"<<z<<"_total_cnt_neighbor = "<<avg_cnt_neighbor<<endl;
//				cout<<"layer_"<<z<<"_total_cnt_received = "<<avg_cnt_received<<endl;	
				
			//	cout<<"layer_"<<z<<"_avg_cnt_local = "<<avg_cnt_local/(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y)<<endl;
			//	cout<<"layer_"<<z<<"_avg_cnt_neighbor = "<<avg_cnt_neighbor/(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y)<<endl;
			//	cout<<"layer_"<<z<<"_avg_cnt_received = "<<avg_cnt_received/(TGlobalParams::mesh_dim_x*TGlobalParams::mesh_dim_y)<<endl;	
					
//				cout<<"loacl_packets:\t"<<cnt_local_total[0]<<"\t"<<cnt_local_total[1]<<"\t"<<cnt_local_total[2]<<"\t"<<cnt_local_total[3]<<endl;
//				cout<<"neighbor_packets:\t"<<cnt_neighbor_total[0]<<"\t"<<cnt_neighbor_total[1]<<"\t"<<cnt_neighbor_total[2]<<"\t"<<cnt_neighbor_total[3]<<endl;
//				cout<<"received_packets:"<<cnt_received_total[0]<<"\t"<<cnt_received_total[1]<<"\t"<<cnt_received_total[2]<<"\t"<<cnt_received_total[3]<<endl;
			
	
		}
		LastIsEmergency = IsEmergency;


		for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 
			for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
				for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
				{							  			
		  			t[x][y][z]->pe->cnt_local = 0;//reset to zero\BCg\A6b\B3o!!\A4\A3\A5i\AE\B3\B1\BC
		  			t[x][y][z]->r->cnt_neighbor = 0;//reset to zero\BCg\A6b\B3o!!\A4\A3\A5i\AE\B3\B1\BC
		  			t[x][y][z]->r->cnt_received = 0;//reset to zero\BCg\A6b\B3o!!\A4\A3\A5i\AE\B3\B1\BC
				}
		  
	  }
	}
	
	if( getCurrentCycleNum() == NoximGlobalParams::simulation_time ) //\A6\AC\A7\C0\A1A\B0\B5steady\ADp\BA\E2
	{ 
		HS_interface->steadyTmp(t);
	}
 
      
}

//----------Modified by CMH
void NoximNoC::transPwr2PtraceFile()
{
	
    int idx = 0;	
	int m, n, o;
	/*================================Begin of collecting POWER TRACE ======================================*/
	for(o=0; o < NoximGlobalParams::mesh_dim_z; o++){
		for(n=0; n < NoximGlobalParams::mesh_dim_y; n++) {
			for(m=0; m < NoximGlobalParams::mesh_dim_x; m++) {
			
                idx = m + n*NoximGlobalParams::mesh_dim_x + o*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_x;
				
				double a = t[m][n][o]->r->stats.power.getTransientRouterPower();
				//router : offset = 0
				//instPowerTrace[3*idx] = t[m][n][o]->r->stats.power.getTransientRouterPower()/(TEMP_REPORT_PERIOD *1e-9);
				//overallPowerTrace[3*idx] += instPowerTrace[3*idx];
				instPowerTrace[3*idx] = t[m][n][o]->r->stats.power.getTransientRouterPower();
				results_log_pwr << instPowerTrace[3*idx]<<"\t";	
						
                //uP_mem : offset = 1
				//instPowerTrace[3*idx+1] = t[m][n][o]->r->stats.power.getTransientMEM()/(TEMP_REPORT_PERIOD *1e-9);
				//overallPowerTrace[3*idx+1] += instPowerTrace[3*idx+1];
				instPowerTrace[3*idx+1] = t[m][n][o]->r->stats.power.getTransientMEMPower();
				results_log_pwr << instPowerTrace[3*idx+1]<<"\t";	

				//uP_mac : offset = 2
				//instPowerTrace[3*idx+2] = t[m][n][o]->r->stats.power.getTransientFPMACPower()/(TEMP_REPORT_PERIOD *1e-9);
				//overallPowerTrace[3*idx+2] += instPowerTrace[3*idx+2];
				instPowerTrace[3*idx+2] = t[m][n][o]->r->stats.power.getTransientFPMACPower();
				results_log_pwr << instPowerTrace[3*idx+2]<<"\t";	

    			t[m][n][o]->r->stats.power.resetTransientPwr(); //\B2M\B0\A3\B3o\ACq\B4\C1\B6\A1\AA\BAPower\B2ֿn
				}
			}
	}

	/*================================End of COLLECTING Power TRACE=================================================*/
	results_log_pwr<<"\n";
}

void NoximNoC::steadyPwr2PtraceFile()
{
	
    int idx = 0;	
	int m, n, o;
	/*================================Begin of collecting POWER TRACE ======================================*/
	for(o=0; o < NoximGlobalParams::mesh_dim_z; o++){
		for(n=0; n < NoximGlobalParams::mesh_dim_y; n++) {
			for(m=0; m < NoximGlobalParams::mesh_dim_x; m++) {
			
                idx = m + n*NoximGlobalParams::mesh_dim_x + o*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_x;
				
				//router : offset = 0
				overallPowerTrace[3*idx] += t[m][n][o]->r->stats.power.getSteadyStateRouterPower();
						
                //uP_mem : offset = 1
				overallPowerTrace[3*idx+1] += t[m][n][o]->r->stats.power.getSteadyStateMEMPower();
	

				//uP_mac : offset = 2
				overallPowerTrace[3*idx+2] += t[m][n][o]->r->stats.power.getSteadyStateFPMACPower();
	
				}
			}
	}

	/*================================End of COLLECTING Power TRACE=================================================*/
	//results_log_pwr<<"\n";
}

void NoximNoC::setTemperature()
{

	int m, n, o;
    int idx = 0;
	
	for(o=0; o < NoximGlobalParams::mesh_dim_z; o++){
		for(n=0; n < NoximGlobalParams::mesh_dim_y; n++) {
			for(m=0; m < NoximGlobalParams::mesh_dim_x; m++) {
				
				idx = m + n*NoximGlobalParams::mesh_dim_x + o*NoximGlobalParams::mesh_dim_y*NoximGlobalParams::mesh_dim_x;
				//set tile temperature
				//use router temperature only ????
				t[m][n][o]->r->stats.last_temperature = t[m][n][o]->r->stats.temperature;
				t[m][n][o]->r->stats.temperature = TemperatureTrace[3*idx];
                
			}
		}
	}

}


bool NoximNoC::EmergencyDecision()
//void TNoC::EmergencyDecision()
{	
	bool isEmergency = false;
		if(NoximGlobalParams::throt_type == THROT_NORMAL)
		{
			for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 
				for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
					for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
						{
//								t[x][y][z]->pe->throttle_local = false;
//								t[x][y][z]->r->throttle_neighbor = false;
									t[x][y][z]->pe->emergency = false;
									t[x][y][z]->r->emergency = false;
						}
					
		}
		else if(NoximGlobalParams::throt_type == THROT_TEST)
		{
			for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 
				for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
					for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
						{
							if(z < (NoximGlobalParams::mesh_dim_z - 2)){
								if(((x == 3))&&((y == 3))){
									t[x][y][z]->pe->emergency = true;
									t[x][y][z]->r->emergency = true;
								}
								else {
								t[x][y][z]->pe->emergency = false;
								t[x][y][z]->r->emergency = false;
								}
							}
							else {
								t[x][y][z]->pe->emergency = false;
								t[x][y][z]->r->emergency = false;
							}
						}
		}
		else if(NoximGlobalParams::throt_type == THROT_GLOBAL)
		{
			bool emergency = false;
			for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 
				for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
					for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
						{
							if(t[x][y][z]->r->stats.temperature > TEMP_THRESHOULD) // each temperature of routers exceed temperature threshould
									emergency = true;
						}		
			if(emergency)										
			{	
				isEmergency = true;
				for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 
					for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
						for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
						{
//							t[x][y][z]->pe->Quota_local = 0	;	//set initial quota
//							t[x][y][z]->r->Quota_neighbor = 0;	//set initial quota
							t[x][y][z]->pe->emergency = true;
							t[x][y][z]->r->emergency = true;
/*			
							if(t[x][y][z]->r->cnt_local >= t[x][y][z]->pe->Quota_local)
							{	
								t[x][y][z]->pe->throttle_local=true;
								cout<<getCurrentCycleNum()<<": Local port of Router ["<<x<<"]["<<y<<"]["<<z<<"] is throttled!"<<endl;
							}
							if(t[x][y][z]->r->cnt_neighbor >= t[x][y][z]->r->Quota_neighbor)
							{	
								t[x][y][z]->r->throttle_neighbor=true;
								cout<<getCurrentCycleNum()<<": Neighbor ports of Router ["<<x<<"]["<<y<<"]["<<z<<"] are throttled!"<<endl;
							}
*/
						}
#ifdef __No_LOG__
#else
				cout<<"All routers change to therm_emergency mode!\n";
#endif
			}			
			else
			{
				for(int z=0; z < NoximGlobalParams::mesh_dim_z; z++) 		
					for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
						for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)	
						{
								t[x][y][z]->pe->emergency = false;							// normal mode		
								t[x][y][z]->r->emergency = false;							// normal mode	
//							t[x][y][z]->pe->throttle_local = false;
//							t[x][y][z]->r->throttle_neighbor = false;
						}
#ifdef __No_LOG__
#else
				cout<<"All routers change to therm_normal mode!\n";	
#endif
			}
						
		}
	else if(NoximGlobalParams::throt_type == THROT_DISTRIBUTED)
	{
		
		for(int z=NoximGlobalParams::mesh_dim_z-1; z >= 0 ; z--) //z\A5ѤU(\BEa\AA\F1heat sink)\A9\B9\A4W\C0ˬd, \A4W\AD\B1\C0ˬd\A8\EC\B9L\BC\F6\AEɤ~\A5i\A5H\B3s\A4U\AD\B1\A4@\B0_\A7令emergency
			for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
				for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
					{
						if(t[x][y][z]->r->stats.temperature > TEMP_THRESHOULD)
						{	
							isEmergency = true;
				//			t[x][y][z]->pe->Quota_local = 0	;	//set initial quota
				//			t[x][y][z]->r->Quota_neighbor = 100000;	//set initial quota
							#ifdef __Throttle_vertical__
								for(int zz = z; zz < TGlobalParams::mesh_dim_z; zz++)
								{
									t[x][y][zz]->pe->emergency = true;
									t[x][y][zz]->r->emergency = true;
									cout<<"["<<x<<"]["<<y<<"]["<<z<<"~"<<TGlobalParams::mesh_dim_z-1<<"] changes to therm_emergency mode!\n";
								}
							#else
								t[x][y][z]->pe->emergency = true;
								t[x][y][z]->r->emergency = true;				
/*								
								if(t[x][y][z]->r->cnt_local >= t[x][y][z]->pe->Quota_local)
								{	
									t[x][y][z]->pe->throttle_local=true;
									cout<<getCurrentCycleNum()<<": Local port of Router ["<<x<<"]["<<y<<"]["<<z<<"] is throttled!"<<endl;
								}
								if(t[x][y][z]->r->cnt_neighbor >= t[x][y][z]->r->Quota_neighbor)
								{	
									t[x][y][z]->r->throttle_neighbor=true;
									cout<<getCurrentCycleNum()<<": Neighbor ports of Router ["<<x<<"]["<<y<<"]["<<z<<"] are throttled!"<<endl;
								}
*/								
								cout<<"["<<x<<"]["<<y<<"]["<<z<<"] changes to therm_emergency mode!\n";
							#endif
						}
						else
						{	
								t[x][y][z]->pe->emergency = false;							// normal mode		
								t[x][y][z]->r->emergency = false;							// normal mode	
//							t[x][y][z]->pe->throttle_local = false;
//							t[x][y][z]->r->throttle_neighbor = false;
//							cout<<"["<<x<<"]["<<y<<"]["<<z<<"] changes to therm_normal mode!\n";
						}
					}
		
	}
	else if(NoximGlobalParams::throt_type == THROT_VERTICAL)
	{
		
		for(int z=NoximGlobalParams::mesh_dim_z-1; z >= 0 ; z--) //z\A5ѤU(\BEa\AA\F1heat sink)\A9\B9\A4W\C0ˬd, \A4W\AD\B1\C0ˬd\A8\EC\B9L\BC\F6\AEɤ~\A5i\A5H\B3s\A4U\AD\B1\A4@\B0_\A7令emergency
			for(int y=0; y < NoximGlobalParams::mesh_dim_y; y++) 
				for(int x=0; x < NoximGlobalParams::mesh_dim_x; x++)
					{
						if(t[x][y][z]->r->stats.temperature > TEMP_THRESHOULD)
						{	
								if( t[x][y][z]->pe->emergency_level != NoximGlobalParams::mesh_dim_z - t[x][y][z]->pe->local_id/(NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y))
								{
									t[x][y][z]->pe->emergency_level++;	
									t[x][y][z]->r->emergency_level++; 
									//cout<<"["<<x<<"]["<<y<<"]["<<z<<"] emergency_level = "<<t[x][y][z]->r->emergency_level<<endl;;
								}
#ifdef __No_LOG__
#else
								cout<<"["<<x<<"]["<<y<<"]["<<z<<"~"<<t[x][y][z]->pe->emergency_level+z-1<<"] changes to therm_emergency mode!\n";
#endif
								for(int zz = z; zz<t[x][y][z]->pe->emergency_level+z; zz++)
								{
									if(zz < NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y*(NoximGlobalParams::mesh_dim_z-1))	//\B0\B2\B3]\B3̾a\AA\F1heat sink(\A9\B3\BCh)\B3\A3\A4\A3\B7|\B6W\B9L, \ACG\A4\A3\A5ζi\A4Jemergency
									{
										t[x][y][zz]->pe->emergency = true;
										t[x][y][zz]->r->emergency = true;
										
										if(zz == t[x][y][z]->pe->emergency_level+z-1)
										{								
											t[x][y][zz]->pe->Q_ratio = 0.5;
											t[x][y][zz]->r->Q_ratio = 0.5;
#ifdef __No_LOG__
#else
											cout<<"["<<x<<"]["<<y<<"]["<<zz<<"] Q_ratio = "<<t[x][y][zz]->r->Q_ratio<<endl;
#endif
										}	
										else
										{
											t[x][y][zz]->pe->Q_ratio = 0;
											t[x][y][zz]->r->Q_ratio = 0;
#ifdef __No_LOG__
#else
											cout<<"["<<x<<"]["<<y<<"]["<<zz<<"] Q_ratio = "<<t[x][y][zz]->r->Q_ratio<<endl;
#endif
										}
									}	
								}
						}
						else
						{	
								t[x][y][z]->pe->emergency = false;							// normal mode		
								t[x][y][z]->r->emergency = false;							// normal mode	
	//						cout<<"["<<x<<"]["<<y<<"]["<<z<<"] changes to therm_normal mode!\n";
							
								t[x][y][z]->r->emergency_level=0;	
								t[x][y][z]->pe->emergency_level=0; 
								t[x][y][z]->pe->Q_ratio = 1;
								t[x][y][z]->r->Q_ratio = 1;
						}
					}
		
	}
	return isEmergency;
}

void NoximNoC::Dynamic_check()  //Dynamic mapping
{ 
	if(NoximGlobalParams::mapping_method == DYNAMIC)
	{
		int count =0;
		for(int a =0; a< nnmodel.interm_completed.size();a++)
		{
			if(nnmodel.interm_completed[a] == true)
			{
				count ++;
			}
		}
		
		if(nnmodel.should_fill == count ) //nnmodel.should_fill == count 
		{
			/*-----------Debugging--------------*/
			/*for(int a=0; a< nnmodel.interm_data_out.size();a++)
			{
				cout<<"("<<nnmodel.interm_data_out[a]<<")";
			}
			/*----------------------------------*/
			nnmodel.Dymapping();
		}
	}
	
}
