#include "thermal_IF.h"

void Thermal_IF::fpGenerator()
{
    
    // Generate floorplan file of each layer for HotSpot (.flp)
	for(int k=0; k<mesh_dim_z; k++)
	{
		ofstream flp_file;
		ostringstream k_str;
		k_str<<k;
		string flpFileName = "mesh3d_"+k_str.str()+".flp";
		flp_file.open(flpFileName.c_str());
		if(!flp_file.is_open())
			cout<<"Cannot open "<<flpFileName.c_str()<<endl;
		else
		{
		
			for (int j=0; j<mesh_dim_y; j++)
				for (int i=0; i<mesh_dim_x; i++)
				{
					flp_file<<"router["<<i<<"]["<<j<<"]["<<k<<"]\t"<<ROUTER_WIDTH<<"\t"<<ROUTER_LENGTH<<"\t"<<TILE_WIDTH*i<<"\t"<<TILE_LENGTH*j<<"\n";
					flp_file<<"uP_mem["<<i<<"]["<<j<<"]["<<k<<"]\t"<<MEM_WIDTH<<"\t"<<MEM_LENGTH<<"\t"<<TILE_WIDTH*i<<"\t"<<TILE_LENGTH*j+ROUTER_LENGTH<<"\n";
					flp_file<<"uP_mac["<<i<<"]["<<j<<"]["<<k<<"]\t"<<FPMAC_WIDTH<<"\t"<<FPMAC_LENGTH<<"\t"<<TILE_WIDTH*i+ROUTER_WIDTH<<"\t"<<TILE_LENGTH*j<<"\n";
				}
			
			/*Τring blockセ
			for (int j=0; j<TGlobalParams::mesh_dim_y; j++)
				for (int i=0; i<TGlobalParams::mesh_dim_x; i++)
				{
					flp_file<<"router["<<i<<"]["<<j<<"]["<<k<<"]\t"<<ROUTER_WIDTH<<"\t"<<ROUTER_LENGTH<<"\t"<<TILE_WIDTH+TILE_WIDTH*i<<"\t"<<TILE_LENGTH+TILE_LENGTH*j<<"\n";
					flp_file<<"uP_mem["<<i<<"]["<<j<<"]["<<k<<"]\t"<<MEM_WIDTH<<"\t"<<MEM_LENGTH<<"\t"<<TILE_WIDTH+TILE_WIDTH*i<<"\t"<<TILE_LENGTH+TILE_LENGTH*j+ROUTER_LENGTH<<"\n";
					flp_file<<"uP_mac["<<i<<"]["<<j<<"]["<<k<<"]\t"<<FPMAC_WIDTH<<"\t"<<FPMAC_LENGTH<<"\t"<<TILE_WIDTH+TILE_WIDTH*i+ROUTER_WIDTH<<"\t"<<TILE_LENGTH+TILE_LENGTH*j<<"\n";
				}				
				flp_file<<"Null_block1["<<k<<"]\t"<<TILE_WIDTH*(TGlobalParams::mesh_dim_x+1)<<"\t"<<TILE_LENGTH<<"\t"<<0<<"\t"<<0<<"\n";
				flp_file<<"Null_block2["<<k<<"]\t"<<TILE_WIDTH<<"\t"<<TILE_LENGTH*(TGlobalParams::mesh_dim_y+1)<<"\t"<<0<<"\t"<<TILE_LENGTH<<"\n";	
				flp_file<<"Null_block3["<<k<<"]\t"<<TILE_WIDTH*(TGlobalParams::mesh_dim_x+1)<<"\t"<<TILE_LENGTH<<"\t"<<TILE_WIDTH<<"\t"<<TILE_LENGTH*(TGlobalParams::mesh_dim_y+1)<<"\n";
				flp_file<<"Null_block4["<<k<<"]\t"<<TILE_WIDTH<<"\t"<<TILE_LENGTH*(TGlobalParams::mesh_dim_y+1)<<"\t"<<TILE_WIDTH*(TGlobalParams::mesh_dim_x+1)<<"\t"<<0<<"\n";	
			*/
			flp_file.close();
		}
	}
	
	// Generate grid_layer_file for HotSpot (.lcf)
	ofstream lcf_file;
	lcf_file.open("mesh3d.lcf");
	if(!lcf_file.is_open())
		cout<<"Cannot open mesh3d.lcf"<<endl;
	else
	{
		for(int k=0; k<mesh_dim_z; k++)
		{
			lcf_file<<"# Layer "<<k*2<<": Silicon\n";
			lcf_file<<k*2<<"\nY\nY\n"<<HEAT_CAP_SILICON<<"\n"<<RESISTIVITY_SILICON<<"\n"<<THICKNESS_SILICON<<"\nmesh3d_"<<k<<".flp\n\n";
			lcf_file<<"# Layer "<<k*2+1<<": thermal interface material (TIM)\n";
			lcf_file<<k*2+1<<"\nY\nN\n"<<HEAT_CAP_THERM_IF<<"\n"<<RESISTIVITY_THERM_IF<<"\n"<<THICKNESS_THERM_IF<<"\nmesh3d_"<<k<<".flp\n\n";
		}
		lcf_file.close();
	}
	// Generate initial temperature file for HotSpot (.init)
	/*ofstream init_temp_file;
	init_temp_file.open("mesh3d.init");
	if(!init_temp_file.is_open())
		cout<<"Cannot open mesh3d.init"<<endl;
	else
	{
  	for (int k=0; k<mesh_dim_z; k++){	  			
		  for (int j=0; j<mesh_dim_y; j++)
			  for (int i=0; i<mesh_dim_x; i++)		  
				{	
					if(k==0 || k==1)
					{
						init_temp_file<<"layer_"<<k*2<<"_router["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP_80<<endl;
						init_temp_file<<"layer_"<<k*2<<"_uP_mem["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP_80<<endl;
						init_temp_file<<"layer_"<<k*2<<"_uP_mac["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP_80<<endl;
					}
					else
					{
						init_temp_file<<"layer_"<<k*2<<"_router["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP<<endl;
						init_temp_file<<"layer_"<<k*2<<"_uP_mem["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP<<endl;
						init_temp_file<<"layer_"<<k*2<<"_uP_mac["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP<<endl;
					}
				}
			init_temp_file<<"layer_"<<k*2<<"_Null_block1["<<k<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"layer_"<<k*2<<"_Null_block2["<<k<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"layer_"<<k*2<<"_Null_block3["<<k<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"layer_"<<k*2<<"_Null_block4["<<k<<"]\t"<<INIT_TEMP<<endl;	
			for (int j=0; j<mesh_dim_y; j++)
			  for (int i=0; i<mesh_dim_x; i++)	
				{
					if(k==0 || k==1)
					{
						init_temp_file<<"layer_"<<k*2+1<<"_router["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP_80<<endl;
						init_temp_file<<"layer_"<<k*2+1<<"_uP_mem["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP_80<<endl;
						init_temp_file<<"layer_"<<k*2+1<<"_uP_mac["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP_80<<endl;
					}
					else
					{
						init_temp_file<<"layer_"<<k*2+1<<"_router["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP<<endl;
						init_temp_file<<"layer_"<<k*2+1<<"_uP_mem["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP<<endl;
						init_temp_file<<"layer_"<<k*2+1<<"_uP_mac["<<i<<"]["<<j<<"]["<<k<<"]\t"<<INIT_TEMP<<endl;
					}
				}
			init_temp_file<<"layer_"<<k*2+1<<"_Null_block1["<<k<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"layer_"<<k*2+1<<"_Null_block2["<<k<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"layer_"<<k*2+1<<"_Null_block3["<<k<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"layer_"<<k*2+1<<"_Null_block4["<<k<<"]\t"<<INIT_TEMP<<endl;	
			}
			for(int y=0; y < mesh_dim_y; y++) 
				for(int x=0; x < mesh_dim_x; x++)
				{
					init_temp_file<<"hsp_router["<<x<<"]["<<y<<"]["<< mesh_dim_z-1<<"]\t";//router
					init_temp_file<<INIT_TEMP<<"\n";
					init_temp_file<<"hsp_uP_mem["<<x<<"]["<<y<<"]["<< mesh_dim_z-1<<"]\t";//up_mem
					init_temp_file<<INIT_TEMP<<"\n";
					init_temp_file<<"hsp_uP_mac["<<x<<"]["<<y<<"]["<< mesh_dim_z-1<<"]\t";//up_mac
					init_temp_file<<INIT_TEMP<<"\n";
				}
			init_temp_file<<"hsp_Null_block1["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"hsp_Null_block2["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"hsp_Null_block3["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"hsp_Null_block4["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			for(int y=0; y < mesh_dim_y; y++) 
				for(int x=0; x < mesh_dim_x; x++)
				{
					init_temp_file<<"hsink_router["<<x<<"]["<<y<<"]["<< mesh_dim_z-1<<"]\t";//router
					init_temp_file<<INIT_TEMP<<"\n";
					init_temp_file<<"hsink_uP_mem["<<x<<"]["<<y<<"]["<< mesh_dim_z-1<<"]\t";//up_mem
					init_temp_file<<INIT_TEMP<<"\n";
					init_temp_file<<"hsink_uP_mac["<<x<<"]["<<y<<"]["<< mesh_dim_z-1<<"]\t";//up_mac
					init_temp_file<<INIT_TEMP<<"\n";
				}
			init_temp_file<<"hsink_Null_block1["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"hsink_Null_block2["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"hsink_Null_block3["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
			init_temp_file<<"hsink_Null_block4["<< mesh_dim_z-1<<"]\t"<<INIT_TEMP<<endl;	
		for(int i=0; i<12; i++)
			init_temp_file<<"inode_"<<i<<"\t"<<INIT_TEMP<<endl;
		init_temp_file.close();
	}*/
}


/*=====================================HotSpot initialization=====================*/
void Thermal_IF::hs_init()
{
	/* initialize flp, get adjacency matrix */
	hs_flp = read_flp("mesh3d_0.flp", FALSE); //Redundant file, overwrite by lcf file latter

	/* 
	 * configure thermal model parameters. default_thermal_config 
	 * returns a set of default parameters. only those configuration
	 * parameters (config.*) that need to be changed are set explicitly. 
	 */
	thermal_config_t hs_config = default_thermal_config();

	/*strcpy(config.init_file, init_file);
	strcpy(config.steady_file, steady_file);*/

	/* default_thermal_config selects block model as the default.
	 * in case grid model is needed, select it explicitly and
	 * set the grid model parameters (grid_rows, grid_cols, 
	 * grid_steady_file etc.) appropriately. for e.g., in the
	 * following commented line, we just choose the grid model 
	 * and let everything else to be the default. 
	 * NOTE: for modeling 3-D chips, it is essential to set
	 * the layer configuration file (grid_layer_file) parameter.
	 */
	strcpy(hs_config.model_type, GRID_MODEL_STR);
	strcpy(hs_config.grid_layer_file, "mesh3d.lcf");

	/* allocate and initialize the RC model	*/
	hs_model = alloc_RC_model(&hs_config, hs_flp);
	populate_R_model(hs_model, hs_flp);
	populate_C_model(hs_model, hs_flp);

	/* allocate the temp and power arrays	*/
	/* using hotspot_vector to internally allocate any extra nodes needed	*/
	hs_inst_temp = hotspot_vector(hs_model);
	hs_inst_power = hotspot_vector(hs_model);
	hs_steady_temp = hotspot_vector(hs_model);
	hs_overall_power = hotspot_vector(hs_model);
	
	/* set up initial instantaneous temperatures */
	//if (strcmp(model->config->init_file, NULLFILE)) {
	//	if (!model->config->dtm_used)	/* initial T = steady T for no DTM	*/
	//		read_temp(model, temp, model->config->init_file, FALSE);
	//	else	/* initial T = clipped steady T with DTM	*/
	//		read_temp(model, temp, model->config->init_file, TRUE);
	//}
	//else	/* no input file - use init_temp as the common temperature	*/
	
	set_temp(hs_model, hs_inst_temp, hs_model->config->init_temp);

	hs_first_call = TRUE;
}
/*===================================End of HS initialization=====================*/

/*===================================Compute Steady Temperature===================*/
/*void Thermal_IF::hs_steady()
{
	int i, j, base,idx;
	int hs_simulation_time;

	string file_name ("results/STEADY/SteadyTemp");
	char temperal [20];
	char * cstr;

	hs_simulation_time = int ( getCurrentCycleNum() / TEMP_REPORT_PERIOD ); //家览Ω计ぃ琌丁┪cylce计
	
	//find the average power dissipated in the elapsed time 
	for(i=0, base=0; i < hs_model->grid->n_layers; i++) {
		if(hs_model->grid->layers[i].has_power)
			for(j=0; j < hs_model->grid->layers[i].flp->n_units; j++){
				hs_overall_power[base+j] /= hs_simulation_time;
			}
			base += hs_model->grid->layers[i].flp->n_units;
		}
    //Create the file name for each simulation.
	//  Simulations with the same configuration will be recorded in the same file. 2009/12/8-Foster
	//sprintf( temperal, "%d", NoximGlobalParams::routing_algorithm);
	//file_name = file_name + "_routing-" + temperal;
	//sprintf( temperal, "%d", NoximGlobalParams::selection_strategy);
	//file_name = file_name + "_selection-" + temperal;
	sprintf( temperal, "%f", NoximGlobalParams::packet_injection_rate);
	file_name = file_name + "_pir-" + temperal;
	sprintf( temperal, "%d", NoximGlobalParams::traffic_distribution);
	file_name = file_name + "_traffic-" + temperal + ".txt";

	cstr = new char [file_name.size()+1];
	strcpy (cstr, file_name.c_str());	
	// get steady state temperatures 
	
	steady_state_temp(hs_model, hs_overall_power, hs_steady_temp);
	//dump_overallpwr();
	dump_temp(hs_model, hs_steady_temp, cstr);
	dump_steady_temp_grid(hs_model->grid, "grid_steady_temp");
}*/
void Thermal_IF::hs_steady( NoximTile *t[MAX_STATIC_DIM][MAX_STATIC_DIM][MAX_STATIC_DIM] )
{
	int i, j, base,idx;
	int hs_simulation_time;

	string file_name ("results/STEADY/SteadyTemp");
	char temperal [20];
	char * cstr;

	hs_simulation_time = int ( getCurrentCycleNum() / TEMP_REPORT_PERIOD ); //家览Ω计ぃ琌丁┪cylce计
	
	/* find the average power dissipated in the elapsed time */
	/*for(i=0, base=0; i < hs_model->grid->n_layers; i++) {
		if(hs_model->grid->layers[i].has_power)
			for(j=0; j < hs_model->grid->layers[i].flp->n_units; j++){
				hs_overall_power[base+j] /= hs_simulation_time;
			}
			base += hs_model->grid->layers[i].flp->n_units;
		}
	*/
	for(int z = 0; z < mesh_dim_z; z++){
		for(int y = 0; y < mesh_dim_y; y++){
			for(int x = 0; x < mesh_dim_x; x++){
				idx = 3*(x + y*mesh_dim_y + z*mesh_dim_y*mesh_dim_x);
				hs_overall_power[idx    ] = t[x][y][z]->r->stats.power.getSteadyStateRouterPower();
				hs_overall_power[idx + 1] = t[x][y][z]->r->stats.power.getSteadyStateMEMPower()   ;
				hs_overall_power[idx + 2] = t[x][y][z]->r->stats.power.getSteadyStateFPMACPower() ;
			}
		}
	}
    /*Create the file name for each simulation.
	  Simulations with the same configuration will be recorded in the same file. 2009/12/8-Foster*/
	//sprintf( temperal, "%d", NoximGlobalParams::routing_algorithm);
	//file_name = file_name + "_routing-" + temperal;
	//sprintf( temperal, "%d", NoximGlobalParams::selection_strategy);
	//file_name = file_name + "_selection-" + temperal;
	sprintf( temperal, "%f", NoximGlobalParams::packet_injection_rate);
	file_name = file_name + "_pir-" + temperal;
	sprintf( temperal, "%d", NoximGlobalParams::traffic_distribution);
	file_name = file_name + "_traffic-" + temperal + ".txt";

	cstr = new char [file_name.size()+1];
	strcpy (cstr, file_name.c_str());	
	/* get steady state temperatures */
	
	steady_state_temp(hs_model, hs_overall_power, hs_steady_temp);
	//dump_overallpwr();
	dump_temp(hs_model, hs_steady_temp, cstr);
	dump_steady_temp_grid(hs_model->grid, "grid_steady_temp");
}
/*============================End of Compute Steady Temperature===================*/

void Thermal_IF::logFile_init()
{
	//Transient temperature tracefile
	string TEMP_filename = string("results/TEMP/TEMP");
	char temperal [20];
	//sprintf( temperal, "%d", NoximGlobalParams::routing_algorithm);
	//TEMP_filename = TEMP_filename  + "_routing-" + temperal;
	//sprintf( temperal, "%d", NoximGlobalParams::selection_strategy);
	//TEMP_filename  = TEMP_filename + "_selection-" + temperal;
	sprintf( temperal, "%f", NoximGlobalParams::packet_injection_rate);
	TEMP_filename = TEMP_filename  + "_pir-" + temperal;
	sprintf( temperal, "%d", NoximGlobalParams::traffic_distribution);
	TEMP_filename = TEMP_filename + "_traffic-" + temperal + +".txt";
	results_log_temp.open( TEMP_filename.c_str(), ios::out );
	if(!results_log_temp.is_open())
		cout<<"Cannot open "<< TEMP_filename.c_str() <<endl;
		
	for(int z = 0; z < mesh_dim_z; z++){
		for(int y = 0; y < mesh_dim_y; y++){
			for(int x = 0; x < mesh_dim_x; x++){

				results_log_temp<<"router["<<x<<"]["<<y<<"]["<<z<<"]\t";
				results_log_temp<<"uP_mem["<<x<<"]["<<y<<"]["<<z<<"]\t";
				results_log_temp<<"uP_mac["<<x<<"]["<<y<<"]["<<z<<"]\t";
			}
		}
	}
	results_log_temp<<"\n";
}

void Thermal_IF::hs_finish()
{
    delete_RC_model(hs_model);
	free_flp(hs_flp, FALSE);
	free_dvector(hs_inst_temp);
	free_dvector(hs_inst_power);
	free_dvector(hs_steady_temp);
	free_dvector(hs_overall_power);
}

void Thermal_IF::hs_temperature(vector<double>& instPwr)
{
   //convert powerTrace from Network Model to internal power represent format of HotSpot  
   char router_name[20], mem_name[20], mac_name[20], nb_name[20]; //tile计ぃ禬筁计  XD
   int r_nm, mem_nm, mac_nm, nb_nm;
   int m, n, o, hs_lyr, hs_base;
   int hs_idx;

   int router_chk, mem_chk, mac_chk;

   router_chk = 0;
   mem_chk = 0;
   mac_chk = 0;
   
   int idx = 0;
   
   for(hs_lyr=0, hs_base=0; hs_lyr < hs_model->grid->n_layers; hs_lyr++) { //Τ礚種竡癹伴禣硂
		if(hs_model->grid->layers[hs_lyr].has_power){
			for(o=0; o < mesh_dim_z; o++){
				for(n=0; n < mesh_dim_y; n++) {
					for(m=0; m < mesh_dim_x; m++) {
						r_nm = sprintf(router_name, "router[%d][%d][%d]", m, n, o);
						mem_nm = sprintf(mem_name, "uP_mem[%d][%d][%d]", m, n, o);
						mac_nm = sprintf(mac_name, "uP_mac[%d][%d][%d]", m, n, o);

						idx = 3*(m + n*mesh_dim_y + o*mesh_dim_y*mesh_dim_x);
						//router
						hs_idx = get_blk_index(hs_model->grid->layers[hs_lyr].flp, router_name);
						if(hs_idx != -1){
							hs_inst_power[hs_base+hs_idx] = instPwr[idx];
							hs_overall_power[hs_base+hs_idx] += hs_inst_power[hs_base+hs_idx];
						}

						//mem
						hs_idx = get_blk_index(hs_model->grid->layers[hs_lyr].flp, mem_name);
						if(hs_idx != -1){
							hs_inst_power[hs_base+hs_idx] = instPwr[idx+1];//uP_mem
							hs_overall_power[hs_base+hs_idx] += hs_inst_power[hs_base+hs_idx];
						}

						//mac
						hs_idx = get_blk_index(hs_model->grid->layers[hs_lyr].flp, mac_name);
						if(hs_idx != -1){
							hs_inst_power[hs_base+hs_idx] = instPwr[idx+2];//uP_mac
							hs_overall_power[hs_base+hs_idx] += hs_inst_power[hs_base+hs_idx];
						}
						
				    }
			    }
		    }
			
		}
		hs_base += hs_model->grid->layers[hs_lyr].flp->n_units;
	}

	
   if (hs_first_call)
		compute_temp(hs_model, hs_inst_power, hs_inst_temp, hs_model->config->sampling_intvl);
   else
		compute_temp(hs_model, hs_inst_power, NULL, hs_model->config->sampling_intvl);
	
	/* make sure to record the first call	*/
	hs_first_call = FALSE;

}

void Thermal_IF::Tmp2TtraceFile(vector<double>& temperature)
{
	
	char router_name[30], mem_name[21], mac_name[21], nb_name[21];
	int r_nm, mem_nm, mac_nm, nb_nm;
	int m, n, o, hs_lyr, hs_base;
	int hs_idx;
	int idx = 0;

	/*================================Temperature output from HotSpot model==============================*/
	for(hs_lyr=0, hs_base=0; hs_lyr < hs_model->grid->n_layers; hs_lyr++) {
		if(hs_model->grid->layers[hs_lyr].has_power){
			for(o=0; o < mesh_dim_z; o++){
				for(n=0; n < mesh_dim_y; n++) {
					for(m=0; m < mesh_dim_x; m++) {
						r_nm = sprintf(router_name, "router[%d][%d][%d]", m, n, o);
						mem_nm = sprintf(mem_name, "uP_mem[%d][%d][%d]", m, n, o);
						mac_nm = sprintf(mac_name, "uP_mac[%d][%d][%d]", m, n, o);
						
						idx = 3*(m + n*mesh_dim_y + o*mesh_dim_y*mesh_dim_x);
						
						//router
						hs_idx = get_blk_index(hs_model->grid->layers[hs_lyr].flp, router_name);
						if(hs_idx != -1)
						{
							results_log_temp << hs_inst_temp[hs_base+hs_idx] - 273.15 <<"\t";
							temperature[idx] = hs_inst_temp[hs_base+hs_idx] - 273.15;
                            
							//max_temperature
							/*if((hs_inst_temp[hs_base+hs_idx] - 273.15) > TGlobalParams::max_temp){
								TGlobalParams::max_temp = hs_inst_temp[hs_base+hs_idx] - 273.15;
								strcpy(TGlobalParams::max_temp_r, router_name); 						
							}*/
						}
						
						//mem
						hs_idx = get_blk_index(hs_model->grid->layers[hs_lyr].flp, mem_name);
						if(hs_idx != -1)
						{
							results_log_temp << hs_inst_temp[hs_base+hs_idx] - 273.15 <<"\t";
							temperature[idx+1] = hs_inst_temp[hs_base+hs_idx] - 273.15;

							/*if((hs_inst_temp[hs_base+hs_idx] - 273.15) > TGlobalParams::max_temp){
								TGlobalParams::max_temp = hs_inst_temp[hs_base+hs_idx] - 273.15;
								strcpy(TGlobalParams::max_temp_r, mem_name); 						
							}*/
						}

						//mac
						hs_idx = get_blk_index(hs_model->grid->layers[hs_lyr].flp, mac_name);
						if(hs_idx != -1){
							results_log_temp << hs_inst_temp[hs_base+hs_idx] - 273.15 <<"\t";
                            temperature[idx+2] = hs_inst_temp[hs_base+hs_idx] - 273.15;							

							/*if((hs_inst_temp[hs_base+hs_idx] - 273.15) > TGlobalParams::max_temp){
								TGlobalParams::max_temp = hs_inst_temp[hs_base+hs_idx] - 273.15;
								strcpy(TGlobalParams::max_temp_r, mac_name);  						
							}*/
						}
					}
				}

			}

		}
		hs_base += hs_model->grid->layers[hs_lyr].flp->n_units;
	}
	results_log_temp<<"\n";
}
