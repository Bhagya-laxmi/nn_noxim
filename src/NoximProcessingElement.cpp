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

#include <iomanip>
#include "fixedp.h"
#include "NoximProcessingElement.h"
//extern int throttling[8][8][4];
//extern int throttling[DEFAULT_MESH_DIM_X][DEFAULT_MESH_DIM_Y][DEFAULT_MESH_DIM_Z];

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

	//Conversion of receive_neu_ID_conv and receive_neu_ID_pool into receive_Neu_ID
	if(Type_layer == 'c' && ID_layer != 1)
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

	}else if(Type_layer == 'p')
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
	}
	receive = receive_Neu_ID.size();
	should_receive = receive;
	receive_data.assign(receive , 0 );
	/*----------------Debugging---------------*/
	//if(ID_group == 48)
	//{
	//	cout<<"Receive neuron ids for Group "<<ID_group<<"--";
	//	for(int i=0; i<receive_Neu_ID.size(); i++ )
	//	{
	//		cout<<"("<<receive_Neu_ID[i]<<")--";
	//	}
	//cout<<"Size: "<<receive_Neu_ID.size()<<endl;
	//}
	/*----------------------------------------*/
    	} 
	else {
		//cout<<"PE Rx process"<<endl;
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
					float denominator_value =0.0;
					if(Type_layer == 'f')
					{
						for (int j = 0 ; j<receive ; j++)  //receive
						{					
							for (int i = 0 ; i<Use_Neu ; i++)  //Use_Neu
							{
								//********************fully connected********************** //
								if(Type_layer =='f')
								{
									float weight_tmp = PE_table[i].weight[j]; 
									
									res[i] += receive_data[j] * weight_tmp;  
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
					}else if(Type_layer == 'c')
					{
						int offset_conv=0;
						for(int bf=0; bf<ID_layer; bf++)
						{
							if(NN_Model->all_leyer_type[bf] =='c')
							{
								offset_conv = offset_conv + NN_Model->all_leyer_size[bf][3];
							}
						}
						deque <float> deq_data;
						deque <float>deq_kernel;
						float value;
						for(int bg=0 ; bg < Use_Neu; bg++)
						{
							//Idea: First accumulate the prev layer data in the order in a temp variable
							//then accumulate kernel values in order, consider index
							//multiply these two temp deques element-wise
							//Repeat for each neuron
							deq_data.clear();
							deq_kernel.clear();
							int layer_neu = coord_xyz[bg]%100;
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
							
							for(int bj=0; bj<NN_Model->all_leyer_size[ID_layer-1][3]; bj++)
							{
								for(int bk=0; bk< NN_Model->all_leyer_size[ID_layer][4]*NN_Model->all_leyer_size[ID_layer][5]; bk++)
								{
									deq_kernel.push_back(NN_Model->all_conv_weight[bj+offset_conv + (coord_xyz[bg]%100)*NN_Model->all_leyer_size[ID_layer][6]][bk]);
								}
								 
							}

							
							value =0.0;
							for(int bl =0; bl<deq_kernel.size() ; bl++)
							{
								value = value + deq_kernel[bl] *deq_data[bl];
								/*--------------Debugging---------------*/
								/*if(ID_group == 75 && bg== 0)
								{
									cout<<"("<<deq_kernel[bl]<<":"<<deq_data[bl]<<":"<<value<<")";
								}*/
								/*--------------------------------------*/
							}
							//Add bias
							value = value + NN_Model->all_conv_bias[offset_conv + (coord_xyz[bg]%100)];
							/*--------Debugging--------------*/
							/*if(ID_group == 75 && bg ==0)
							{
								cout<<endl<<"Bias: "<< NN_Model->all_conv_bias[offset_conv + (coord_xyz[bg]%100)]<<endl;
								cout<< "Final Value: "<<value<<endl;
							}*/
							/*-------------------------------*/
							if (value <= 0) 
								res[bg]=0;
							else
								res[bg] =value; 

							
							/*--------------Debugging----------------*/
							//if(ID_group == 60 && bg ==0)
							//{
							//	cout<<"Conv data is: ";
							//	for(int gg =0; gg< 25; gg++)
							//	{
							//		cout<<deq_data[gg]<<")--(";
							//	}
							//	cout<<endl;
							//}
							/*---------------------------------------*/
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
								if ( NN_Model->all_leyer_size[ID_layer].back() == AVERAGE )//relu
								{
									value = value + receive_data[index];
									/*-----------Debugging--------------*/
									//if(ID_group == 76 && bc == 16)
									//{
									//	cout<<receive_data[index]<<"--";
									//}
									/*----------------------------------*/	 					
								}
								
							} 
							value = value / (NN_Model->all_leyer_size[ID_layer][4]*NN_Model->all_leyer_size[ID_layer][5]);
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
					
					flag_p = 1; 
					flag_f = 1; 
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

		//cout<< NN_Model->all_leyer_ID_Group.size()<<endl;
		//cout<<endl<<"starting"<<endl;
		
		for( int k = 0 ; k<NN_Model->mapping_table.size() ; k++ )
		{
			//cout<<"Loop 1"<<endl;
			if(NN_Model->mapping_table[k]==local_id)
			{
				//cout<<"Loop 2"<<endl;
				ID_group = k;
				if(ID_group<NN_Model->Group_table.size())
				{
					
					//cout<<"Loop 3"<<endl;
					PE_enable = 1;

					PE_table = NN_Model->Group_table[ID_group];
					//deque<NeuInformation>().swap(NN_Model->Group_table[ID_group]);
					
					ID_layer = PE_table[0].ID_layer;
					Type_layer = PE_table[0].Type_layer;
					Use_Neu = PE_table.size();
					res.assign( Use_Neu, 0 );
					
					/*-------Debugging-------*/
					//if(ID_layer == 1&& ID_group == 0)
					//{
					//	cout<<"Step "<<local_id<<endl;
					//}
					/*------------------------*/
					for(int i = 0 ; i<Use_Neu ; i++)
					{
						//cout<<"Loop 4"<<endl;
						Use_Neu_ID.push_back(PE_table[i].ID_Neu);
					}

					/*-------Debugging------*/
					//cout<<"Local id: "<< local_id<<endl;
					//cout<<"Use Neuron ID: "<<Use_Neu_ID.back()<<endl;
					//cout<<"Use Neuron ID: "<<Use_Neu_ID.front()<<endl;
					/*----------------------*/
					if( Type_layer == 'f')
					{
						
						if(ID_layer !=NN_Model->all_leyer_size.size()-1)
						{
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

						//receive ids
						receive = NN_Model-> all_leyer_size[ID_layer-1][0];
						should_receive = receive;
						receive_Neu_ID.clear();
						receive_data.assign(receive , 0 );
						
						int temp_receive_start_ID = Use_Neu_ID[0] - PE_table[0].ID_In_layer - receive;
						
						for(int i = 0 ; i<receive ; i++)
						{
							receive_Neu_ID.push_back(temp_receive_start_ID+i);
						}
						flag_p =0;
						flag_f =0;
						/*-------Debugging------*/
						//cout<<"Size of previous layer: "<<receive<<endl;
						/*----------------------*/
					}else
					{
						//cout<<"Conv or pool layer"<<endl;
						//Convert the id_in_layer into 3d coordinate system
					    int layer_x = NN_Model->all_leyer_size[ID_layer][1];
						int layer_y = NN_Model->all_leyer_size[ID_layer][2];
						int layer_z = NN_Model->all_leyer_size[ID_layer][3];
						int done = 0;
						int temp_coord;
						coord_xyz.clear();
						
						for(int i=0; i<Use_Neu; i++)
						{
							done =0;
							for(int j=0; j< layer_z; j++)
							{
								for(int m =0; m< layer_x; m++)
								{
									for(int l =0; l< layer_y; l++)
									{
										if((j*layer_x*layer_y + layer_y*m + l)==PE_table[i].ID_In_layer)
										{
											temp_coord= m*10000 + l*100 + j;
											coord_xyz.push_back(temp_coord);
											temp_coord =0;
											/*-------Debugging--------*/
											
											//if((ID_layer == 3) && (PE_table[i].ID_In_layer == 1599)/*&& (PE_table[i].ID_In_layer < 77)*/){
											//	cout<<"Layer id: "<<ID_layer<<":::";
											//	cout<<endl<<"Neuron count: "<<Use_Neu<<"::";
											//	cout<< "ID in layer: "<<PE_table[i].ID_In_layer<<"--";
											//	cout<<"Coord: "<< coord_xyz[i]<<"--";
											//	cout<<"x: "<<m<<"--y: "<<l<<"--z: "<<j<<endl;
											//	cout<<layer_x<<"--"<<layer_y<<"--"<<layer_z<<endl;
											//}
											
											/*------------------------*/
											done = 1;
											break;
										}
									}
									if( done ==1) break;

								}if(done ==1) break;

							}
						}
						//cout<<"coord dequeu size: "<<coord_xyz.size()<<endl;
						//if(ID_layer == 1){cout<<coord_xyz.front()<<"--"<<coord_xyz.back()<<endl;}
						
						if(Type_layer == 'c')
						{
							deque<int> temp_coord_needed_nxtLayer;
							deque<deque<int>> coord_needed_nxtLayer;
							int horizontal=0;
							int vertical=0;
							//cout<<"Conv"<<endl;
							//trans PE ids are noted down
							if(NN_Model->all_leyer_type[ID_layer+1] == 'p') //must consider stride
							{
								//output size of next layer(14*14 or 5*5)
								int pool_x =NN_Model->all_leyer_size[ID_layer+1][1];
								int pool_y=NN_Model->all_leyer_size[ID_layer+1][2];
								int pool_z =NN_Model->all_leyer_size[ID_layer+1][3];
								int kernelP_x= NN_Model->all_leyer_size[ID_layer+1][4];
								int kernelP_y= NN_Model->all_leyer_size[ID_layer+1][5];
								int stride = NN_Model->all_leyer_size[ID_layer+1][6];

								for(int a =0; a< pool_x; a++)
								{
									for(int b=0; b< pool_y;b++)
									{ 
										if(a>0){ vertical =1;}
										if(b>0) {horizontal =1;}
										if(horizontal ==0 && vertical ==0) //0,0
										{
											//TODO
											temp_coord_needed_nxtLayer.push_back(a*100+b);
											temp_coord_needed_nxtLayer.push_back(a*100+(b+1));
											temp_coord_needed_nxtLayer.push_back((a+1)*100+b);
											temp_coord_needed_nxtLayer.push_back((a+1)*100+(b+1));

										}else if(horizontal ==0 && vertical ==1)
										{
											
											temp_coord_needed_nxtLayer.push_back((stride*a)*100+b);
											temp_coord_needed_nxtLayer.push_back((stride*a)*100+(b+1));
											temp_coord_needed_nxtLayer.push_back(((stride*a)+1)*100+b);
											temp_coord_needed_nxtLayer.push_back(((stride*a)+1)*100+(b+1));											
										}else if(horizontal ==1 && vertical == 0)
										{
											
											temp_coord_needed_nxtLayer.push_back(a*100+b*stride);
											temp_coord_needed_nxtLayer.push_back(a*100+(b*stride+1));
											temp_coord_needed_nxtLayer.push_back((a+1)*100+b*stride);
											temp_coord_needed_nxtLayer.push_back((a+1)*100+(b*stride+1));
										}else if(horizontal ==1 && vertical == 1)
										{
											
											temp_coord_needed_nxtLayer.push_back((stride*a)*100+b*stride);
											temp_coord_needed_nxtLayer.push_back((stride*a)*100+(b*stride+1));
											temp_coord_needed_nxtLayer.push_back(((stride*a)+1)*100+b*stride);
											temp_coord_needed_nxtLayer.push_back(((stride*a)+1)*100+(b*stride+1));
										}
											/*-------Debugging--------*/
											
											//if((ID_layer == 3) && (a ==4)&& (b==4)){
											//	cout<<endl<<"Pool: "<<pool_x<<"-"<<pool_y<<"-"<<pool_z<<"--";
												//cout<<horizontal<<"--"<<vertical<<"--";
											//	cout<< temp_coord_needed_nxtLayer[0]<<"--";
											//	cout<< temp_coord_needed_nxtLayer[1]<<"--";
											//	cout<< temp_coord_needed_nxtLayer[2]<<"--";
											//	cout<< temp_coord_needed_nxtLayer[3]<<endl;
											//}
											
											/*------------------------*/
										coord_needed_nxtLayer.push_back(temp_coord_needed_nxtLayer);
										temp_coord_needed_nxtLayer.clear();
										horizontal =0;
										vertical =0;
									}
								}
								/*------Debugging------*/
								//if(ID_group ==3){
								//	cout<<"Coor nedded next layer size: "<<coord_needed_nxtLayer.size()<<endl;
								//	for(int aa =0; aa<coord_needed_nxtLayer.size(); aa++ )
								//	{
								//		cout<<"(";
								//		for(int ab =0 ; ab<coord_needed_nxtLayer[aa].size(); ab++ )
								//		{
								//			cout<<coord_needed_nxtLayer[aa][ab]<<"--";
								//		}
								//		cout<<")";
								//	}	
								//}
								//cout<<"Use neurons: "<<Use_Neu<<endl;
								/*---------------------*/
								int curr_id;
								int tmp;
								done =0;
								//int done_conv;
								//deque <int> temp_check;
								deque<int> trans_id_in_layer;
								//deque <int> trans_xyz;
								for(int h=0; h< Use_Neu; h++)
								{
									done=0;
									curr_id = (coord_xyz[h]/10000)* 100 + (coord_xyz[h]%10000)/100; //xy only
									for(int f=0; f<coord_needed_nxtLayer.size();f++)
									{
										
										if((curr_id == coord_needed_nxtLayer[f][0])||
										(curr_id == coord_needed_nxtLayer[f][1])||
										(curr_id == coord_needed_nxtLayer[f][2])||
										(curr_id == coord_needed_nxtLayer[f][3]))
										{
											trans_id_in_layer.push_back(f+coord_xyz[h]%100*pool_x*pool_y); //Need to save id including xyz
											/*------Debugging------*/
											//Converting into 2d coord
											//for(int g=0; g<pool_x;g++)
											//{
											//	done_conv =0;
											//	for(int q=0; q< pool_y;q++)
											//	{
											//		if(f == (pool_y*g + q))
											//		{
											//			tmp = g*10000 +q*100+coord_xyz[h]%100;
											//			done_conv=1;
											//			break;
											//		}
											//	}if(done_conv == 1) break;
											//}
											//temp_check.push_back(f);
											//trans_xyz.push_back(tmp);
											
											//if((ID_layer ==3)&&(PE_table[h].ID_In_layer == 1599))
											//{
											//	cout<<"Curr: "<<curr_id<<"--"<<f<<"--"<<trans_id_in_layer[h]<<"-"<<coord_xyz[h]<<endl;
												//cout<<"Curr: "<<curr_id<<"-"<<f<<"-"<<trans_PE_ID_conv[h]<<"-"<<trans_xyz[h]<<"-"<<"-"<<temp_check[h]<<"-"<<coord_xyz[h]<<endl;
											//}
											/*--------------------*/

											done=1;break;
										}
									}

								}

								/*---------Debugging-------*/
								//if(ID_group ==3)
								//{
								//	cout<<"Trans id in layer: "<<trans_id_in_layer.size()<<". Trans id in layer for gp 0 and layer 1: "<<endl;
								//	for(int ab =0; ab< trans_id_in_layer.size(); ab++)
								//	{
								//		cout<<"("<< trans_id_in_layer[ab]<<")"<<"--";
								//	}
								//	cout<<".......";
								//}
								//if((ID_group ==3))
								//{
								//	for(int az =0; az<coord_xyz.size();az++)
								//	{
								//		cout<<coord_xyz[az]<<"--";
								//	}
									
									//cout<<"Size: "<<temp_trans_xyz[1].size()<<"--"<<temp_trans_xyz[1][0]<<endl;

								//}
								/*------------------------*/
							 //Converting the ID-in_layer to PE id
							 deque< NeuInformation > PE_table_nxtlayer;
							 trans_PE_ID_conv.clear();
							 trans_conv.clear();
							 //trans_conv.assign(Use_Neu,1); //transmitting count per neuron
								for(int w=0; w< Use_Neu; w++)
								{
									done =0;
									for(int i = 0 ; i<NN_Model->all_leyer_ID_Group[ID_layer].size() ; i++)
									{
										int temp_Group = NN_Model->all_leyer_ID_Group[ID_layer][i];
										PE_table_nxtlayer = NN_Model->Group_table[temp_Group];
										for(int e=0; e<PE_table_nxtlayer.size();e++)
										{
											if(trans_id_in_layer[w] == PE_table_nxtlayer[e].ID_In_layer)
											{
												trans_PE_ID_conv.push_back(NN_Model-> mapping_table[temp_Group]);
												done =1;

												/*---------Debugging---------*/
												//if(ID_group ==3)
												//{
												//	cout<<"-"<<PE_table_nxtlayer[e].ID_In_layer;
												//}
												/*---------------------------*/
												break;
											}
										}if(done == 1) break;
										
									}
								}
								/*---------Debugging---------*/
								//if(ID_group ==3)
								//{
									//cout<<"Trans PE ID conv: "<<trans_PE_ID_conv.size()<<". Trans PE id conv for layer 1, gp 0 "<<endl;
									
								//	for(int ab =0; ab <trans_PE_ID_conv.size();ab++)
								//	{
								//		cout<<trans_PE_ID_conv[ab]<<"-";
								//	}
								//}
								/*---------------------------*/
								//all the PE ids for this group
								trans_PE_ID.clear();
								trans_PE_ID.push_back(trans_PE_ID_conv[0]);
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

								int kernel_x= NN_Model->all_leyer_size[ID_layer][4];
								int kernel_y= NN_Model->all_leyer_size[ID_layer][5];
								
								int prev_x =NN_Model->all_leyer_size[ID_layer-1][1] ;
								int prev_y=NN_Model->all_leyer_size[ID_layer-1][2];
								int prev_z =NN_Model->all_leyer_size[ID_layer-1][3];
								//distinction btw 1st convolution layer and others
								deque <int> temp_receive_id;
								int curr_id_x;
								int curr_id_y;
								//deque <int> check;
								deque <int> temp_receive_neu_id;
								deque< NeuInformation > PE_table_prev;
								receive_neu_ID_conv.clear();
								for(int u=0;u<Use_Neu; u++)
								{
									curr_id_x =0;
									curr_id_y =0;
									curr_id_x = (coord_xyz[u]/10000); 
									curr_id_y= (coord_xyz[u]%10000)/100; //xy only

									for( int a=0;a< prev_z; a++)
									{
										for(int b=0; b<kernel_x; b++)
										{
											for(int c=0; c< kernel_y; c++)
											{
												//check.push_back((curr_id_x +b)*10000+(curr_id_y+c)*100+a);
												temp_receive_id.push_back((curr_id_x +b)*prev_y+(curr_id_y+c)+a*prev_y*prev_x);
												
											}
											
										}	

									}
									int PE_count =0;
									int PE_id_curr;
									if(ID_layer != 1)
									{
										for(int d =0; d<temp_receive_id.size(); d++ )
										{
											done =0;
											for(int e=0; e< NN_Model->all_leyer_ID_Group[ID_layer-2].size(); e++)
											{
												int temp_group = NN_Model->all_leyer_ID_Group[ID_layer-2][e];
												PE_table_prev = NN_Model->Group_table[temp_group];
												for(int f=0; f< PE_table_prev.size(); f++)
												{
													if(temp_receive_id[d] == PE_table_prev[f].ID_In_layer)
													{
														temp_receive_neu_id.push_back(PE_table_prev[f].ID_Neu);
														done =1;
														break;
													}
												}if(done ==1 )break;
											}
										}
										receive_neu_ID_conv.push_back(temp_receive_neu_id); 
										//PE_id_curr = temp_receive_neu_id[0];
										//for(int g =0; g<temp_receive_neu_id.size();g++)
										//{											
										//	if(PE_id_curr != temp_receive_neu_id[g+1])
										//	{
										//		PE_count = PE_count+1;
										//		PE_id_curr = temp_receive_neu_id[g+1];
										//	}

										//}
										
										
									}else
									{
										receive_neu_ID_conv.push_back(temp_receive_id);
									}
									
									//check.clear();
									/*------Debugging-------*/
									//if(ID_group == 2)
									//{
									//	cout<<coord_xyz[u]<<"--"<<curr_id_x<<"--"<<curr_id_y<<endl;
									//	for(int ch =0; ch< receive_neu_ID_conv[u].size(); ch++)
									//	{
									//		cout<<receive_neu_ID_conv[u][ch]<<"-";
											//cout<< check[ch]<<"-";
											
									//	}	//cout<<receive_conv[u]<<"--";
									//}
									//if((ID_layer ==3)&&(PE_table[u].ID_In_layer == 0))
									//{
									//	cout<<ID_layer<<"-"<<temp_receive_neu_id.size()<<"-"<<NN_Model->all_leyer_ID_Group[ID_layer-2].size()<<endl;
									//	for(int ch =0; ch< temp_receive_id.size(); ch++)
									//	{
											//cout<<temp_receive_id[ch]<<"-";
									//		cout<<receive_neu_ID_conv[u][ch]<<"-";
											//cout<< check[ch]<<"-";
											
									//	}	//cout<<receive_conv[u]<<"--";
									//}
									/*---------------------*/	
									temp_receive_id.clear();
									temp_receive_neu_id.clear();
									//check.clear();
								}
								receive_conv.clear();
								if(ID_layer == 1)
								{
									//Take data from memory and prepare the data
									deque <int> channels_in_group;
									int chan_init = coord_xyz[0]%100;
									channels_in_group.push_back(chan_init);
									for(int g=0; g< Use_Neu; g++)
									{
										int chan_curr =coord_xyz[g]%100;
										if(chan_curr != chan_init)
										{
											chan_init = chan_curr;
											channels_in_group.push_back(chan_curr);
										}
									}
									/*-------Debugging------*/
									//if(ID_group == 47)
									//{
									//	for(int ch =0; ch< channels_in_group.size(); ch++)
									//	{
									//		cout<<channels_in_group[ch]<<"-";
									//	}
									//}
									/*--------------------*/

									//deque<deque <float>> conv_weight;
									//deque<float> conv_bias;
									//for(int h=0; h<channels_in_group.size(); h++)
									//{
									//	conv_weight.push_back(NN_Model -> all_conv_weight[channels_in_group[h]]);
									//	conv_bias.push_back(NN_Model ->all_conv_bias[channels_in_group[h]]);
									//}
									/*-------------Debugging--------*/
									//if(ID_group == 47)
									//{
									//	for(int ch =0; ch< conv_weight.size(); ch++)
									//	{
									//		for(int dh =0; dh<conv_weight[ch].size(); dh++ )
									//		{
									//			cout<< conv_weight[ch][dh]<<"--";
									//		}
									//		cout<<"#"<<conv_bias[ch]<<endl;
									//	}
									//}
									/*------------------------------*/
									
									//Perfomr MAC operation, add bias and apply activation function
									deque <float> temp_conv_weight;
									//float temp_conv_bias;
									for(int i =0; i< Use_Neu ; i++)
									{
										int lay_neu = coord_xyz[i]%100;
										
										//for(int l=0; l<NN_Model ->all_conv_weight[lay_neu].size(); l++)
										//{
										//	temp_conv_weight.push_back(conv_weight[lay_neu][l]);
										//}
										
										//temp_conv_bias = conv_bias[i];
										float value=0.0;
										
										for(int j =0; j< kernel_x *kernel_y; j++)
										{
											float x = NN_Model ->all_conv_weight[lay_neu][j];
											float y =  NN_Model-> all_data_in[in_data][receive_neu_ID_conv[i][j]];
											//cout<< "conv weight: "<<x<<endl;
											//cout<<" all data in: "<<y<<endl;
											value = value + x*y; 
											/*------Debugging--------*/
											//if(ID_group == 2 && i == 98)
											//{
											//	cout<<"("<<x<<"--"<<y<<"--"<<receive_neu_ID_conv[i][j]<<")--";
											//}
											/*-----------------------*/
										    
										}
										
										value = value + NN_Model ->all_conv_bias[lay_neu];

										//Using relu function
										if (value <= 0) 
											res[i]=0;
										else
											res[i] = value;
										/*---------Debugging----------*/
										//cout<<"value: "<<value<<endl;
										
										/*---------------------------*/
										
									}
									flag_p=1;
									flag_f=1;
								}else
								{
									// note down the receive ids
									receive_conv.assign(Use_Neu,NN_Model->all_leyer_size[ID_layer][4]* NN_Model->all_leyer_size[ID_layer][5]*NN_Model->all_leyer_size[ID_layer-1][3]);
									/*---------Debugging--------*/
									//if((ID_layer ==3))
									//{
										//cout<< receive_conv[0];
									//	cout<< receive_conv.size();
									//}
									
									/*--------------------------*/
									flag_p =0;
									flag_f =0;
								}
							}	
						}else
						{
							//Layer is pooling
							//(Step1: receive neuron ids)
							receive_neu_ID_pool.clear();
							receive_pool.clear();
							deque <int> temp_receive_pool;
							int horizontal =0;
							int vertical =0;
							int stride = NN_Model->all_leyer_size[ID_layer][6] ;
							for(int aa =0; aa<Use_Neu; aa++)
							{
								//coordinates of the neurons in the current pooling group
								int aa_x = coord_xyz[aa]/10000;
								int aa_y = coord_xyz[aa]/100 -((coord_xyz[aa]/10000)*100);
								int aa_z = coord_xyz[aa]%100;
								if(aa_y > 0) horizontal = 1;
								if(aa_x > 0) vertical = 1;
								int done =0;
								deque <NeuInformation> PE_table_prev;
								temp_receive_pool.clear();
								if(horizontal == 0 && vertical ==0) //00
								{
									for(int ab=0; ab <NN_Model->all_leyer_size[ID_layer][4]; ab++ ) //kernel of the pooling layer
									{
										for(int ac =0; ac< NN_Model->all_leyer_size[ID_layer][5];ac++)
										{
											int id_in_layer = (aa_x+ab)*NN_Model->all_leyer_size[ID_layer-1][2] + (aa_y+ac) + aa_z*NN_Model->all_leyer_size[ID_layer-1][1]*NN_Model->all_leyer_size[ID_layer-1][2];
										  	done =0;
											for(int ad=0; ad< NN_Model->all_leyer_ID_Group[ID_layer-2].size();ad++)
											{
												int temp_group = NN_Model->all_leyer_ID_Group[ID_layer-2][ad];
												PE_table_prev = NN_Model->Group_table[temp_group];
												for(int ae=0; ae< PE_table_prev.size(); ae++)
												{
													if(id_in_layer== PE_table_prev[ae].ID_In_layer)
													{
														temp_receive_pool.push_back(PE_table_prev[ae].ID_Neu);
														done =1;
														break;
													}
												}if(done ==1 )break;
											}
											
										}
									}
								}else if(horizontal == 0 && vertical == 1)
								{
									for(int ab=0; ab <NN_Model->all_leyer_size[ID_layer][4]; ab++ ) //kernel of the pooling layer
									{
										for(int ac =0; ac< NN_Model->all_leyer_size[ID_layer][5];ac++)
										{
											int id_in_layer = (aa_x*stride+ab)*NN_Model->all_leyer_size[ID_layer-1][2] + (aa_y+ac) + aa_z*NN_Model->all_leyer_size[ID_layer-1][1]*NN_Model->all_leyer_size[ID_layer-1][2];
										  	done =0;
											for(int ad=0; ad< NN_Model->all_leyer_ID_Group[ID_layer-2].size();ad++)
											{
												int temp_group = NN_Model->all_leyer_ID_Group[ID_layer-2][ad];
												PE_table_prev = NN_Model->Group_table[temp_group];
												for(int ae=0; ae< PE_table_prev.size(); ae++)
												{
													if(id_in_layer== PE_table_prev[ae].ID_In_layer)
													{
														temp_receive_pool.push_back(PE_table_prev[ae].ID_Neu);
														done =1;
														break;
													}
												}if(done ==1 )break;
											}
											
										}
									}
								}else if(horizontal == 1 && vertical == 0)
								{
									for(int ab=0; ab <NN_Model->all_leyer_size[ID_layer][4]; ab++ ) //kernel of the pooling layer
									{
										for(int ac =0; ac< NN_Model->all_leyer_size[ID_layer][5];ac++)
										{
											int id_in_layer = (aa_x+ab)*NN_Model->all_leyer_size[ID_layer-1][2] + (aa_y*stride+ac) + aa_z*NN_Model->all_leyer_size[ID_layer-1][1]*NN_Model->all_leyer_size[ID_layer-1][2];
										  	done =0;
											for(int ad=0; ad< NN_Model->all_leyer_ID_Group[ID_layer-2].size();ad++)
											{
												int temp_group = NN_Model->all_leyer_ID_Group[ID_layer-2][ad];
												PE_table_prev = NN_Model->Group_table[temp_group];
												for(int ae=0; ae< PE_table_prev.size(); ae++)
												{
													if(id_in_layer== PE_table_prev[ae].ID_In_layer)
													{
														temp_receive_pool.push_back(PE_table_prev[ae].ID_Neu);
														done =1;
														break;
													}
												}if(done ==1 )break;
											}
											
										}
									}
								}else if( horizontal == 1 && vertical == 1)
								{
									for(int ab=0; ab <NN_Model->all_leyer_size[ID_layer][4]; ab++ ) //kernel of the pooling layer
									{
										for(int ac =0; ac< NN_Model->all_leyer_size[ID_layer][5];ac++)
										{
											int id_in_layer = (aa_x*stride+ab)*NN_Model->all_leyer_size[ID_layer-1][2] + (aa_y*stride+ac) + aa_z*NN_Model->all_leyer_size[ID_layer-1][1]*NN_Model->all_leyer_size[ID_layer-1][2];
										  	done =0;
											for(int ad=0; ad< NN_Model->all_leyer_ID_Group[ID_layer-2].size();ad++)
											{
												int temp_group = NN_Model->all_leyer_ID_Group[ID_layer-2][ad];
												PE_table_prev = NN_Model->Group_table[temp_group];
												for(int ae=0; ae< PE_table_prev.size(); ae++)
												{
													if(id_in_layer== PE_table_prev[ae].ID_In_layer)
													{
														temp_receive_pool.push_back(PE_table_prev[ae].ID_Neu);
														done =1;
														break;
													}
												}if(done ==1 )break;
											}
											
										}
									}
								}
								receive_neu_ID_pool.push_back(temp_receive_pool);	
							}
							/*--------------Debugging-------------*/
								//if(ID_group == 76)
								//{
								//	cout<<"Receive neu id for group " << ID_group<<" neuron "<<"25"<<"..";
								//    for(int aa=0; aa<receive_neu_ID_pool[25].size();aa++)
								//	{
								//		cout<<"("<<receive_neu_ID_pool[25][aa]<<")-";
								//	}
								//cout<<endl;
								//}
								
								/*------------------------------------*/
							
							
							if(NN_Model->all_leyer_type[ID_layer +1] == 'c')
							{
								deque <int> temp_trans_pool;
								deque <deque<int>> temp_pool_nxtlayer; 
								//(Step2: if next layer is Conv)
								for(int ac=0;ac<NN_Model->all_leyer_size[ID_layer+1][1];ac++)
								{
									for(int ad =0; ad< NN_Model->all_leyer_size[ID_layer+1][2]; ad++)
									{
										temp_trans_pool.clear();
										for(int ae =0; ae<NN_Model->all_leyer_size[ID_layer+1][4];ae++)
										{
											for(int af =0; af<NN_Model->all_leyer_size[ID_layer+1][5];af++)
											{
												temp_trans_pool.push_back((ac+ae)*100+(ad+af));
											}
										}
										temp_pool_nxtlayer.push_back(temp_trans_pool);
									}
								}

								/*-------Debugging --------*/
								//if(ID_group == 48)
								//{
									//cout<<"Size: "<< temp_pool_nxtlayer.size()<<".....";
								//	for(int af=0; af<temp_pool_nxtlayer[1].size(); af++)
								//	{
								//		cout<<"("<<temp_pool_nxtlayer[1][af]<<")-";
								//	}	
								//}
								/*-------------------------*/
								temp_trans_pool.clear();
								trans_PE_ID_pool.clear();
								deque <NeuInformation> PE_table_nxtLayer;
								int needed =0;
								for(int ag =0; ag<Use_Neu ;ag++)
								{
									temp_trans_pool.clear();
									for(int ah =0; ah<temp_pool_nxtlayer.size();ah++)
									{
										for(int ai =0; ai<temp_pool_nxtlayer[ah].size(); ai++ )
										{
											if(coord_xyz[ag]/100 == temp_pool_nxtlayer[ah][ai])
											{
												for(int aj =0; aj <NN_Model->all_leyer_size[ID_layer+1][3];aj++ )
												{
													int temp_id = ah + aj*NN_Model->all_leyer_size[ID_layer+1][1]*NN_Model->all_leyer_size[ID_layer+1][2];
													for(int ak =0; ak<NN_Model->all_leyer_ID_Group[ID_layer].size();ak++)
													{
														int temp_group = NN_Model -> all_leyer_ID_Group[ID_layer][ak];
														PE_table_nxtLayer = NN_Model->Group_table[temp_group];
														for(int al =0; al < PE_table_nxtLayer.size();al++)
														{
															if( temp_id== PE_table_nxtLayer[al].ID_In_layer)
															{
																needed =1;
																for(int am =0; am<temp_trans_pool.size(); am++ )
																{
																	if(NN_Model-> mapping_table[temp_group] == temp_trans_pool[am])
																	{
																		needed =0;
																		break;
																	}else{
																		needed =1;
																	}
																} if(needed == 1){temp_trans_pool.push_back(NN_Model-> mapping_table[temp_group]);}
															 	
															}
														}
													}
												}
											}
										}
									}
									trans_PE_ID_pool.push_back(temp_trans_pool);
								}
								//transmitting PE ids from the group
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

								flag_f =0;
								flag_p =0;
								/*--------------Debugging-----------------*/
								/*if(ID_group == 48)
								{
									cout<<endl<<"Trans PE ids for pool layer id 2 with group id "<<ID_group<<" and neuron id 15"<<"...";
									for(int aa=0; aa<trans_PE_ID_pool[15].size();aa++)
									{
										cout<<"("<<trans_PE_ID_pool[15][aa]<<")-";
									}

									cout<<endl<<"Final trans ids: "<<"....";
									for(int ay=0; ay<trans_PE_ID.size();ay++)
									{
										cout<<"("<<trans_PE_ID[ay]<<"-"<<trans_pool[ay]<<")....";
									}
									
								}*/
								/*----------------------------------------*/

							}else if( NN_Model -> all_leyer_type[ID_layer+1]=='f')
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
						
					}
					
				}
				break;
			}
		}
		computation_time = NoximGlobalParams::PE_computation_time;
        //cout<<"PE TX Reset end Process"<<endl;
//**************************^^^^^^^^^^^^^^^^^^^^^**************************
    	/*-------------Debugging---------------*/
		//if( ID_group < 84)
		//{
		  //cout<<"(Local id: "<<local_id<<")- Layer: "<<ID_layer<<" Neuron ids: "<<PE_table[0].ID_Neu<<" (Id in layer: "<<PE_table[0].ID_In_layer<<")--"<<PE_table[PE_table.size()-1].ID_Neu<<" (Id in layer: "<<PE_table[PE_table.size()-1].ID_In_layer<<")"<<endl;	
		//}
		
		/*if(ID_group == 48  ) //Layer1:0,7,47 ;Layer3: 60
		{
			cout<<endl<<"trans PE id for layer "<<ID_layer<<", group " <<ID_group<<": ";
			for(int ab=0; ab< trans_PE_ID_pool.size(); ab++)
			{
				cout<<")--(";
				for(int cc =0; cc<trans_PE_ID_pool[ab].size();cc++)
				{
					cout<<trans_PE_ID_pool[ab][cc]<<"--";
				}
				
			}
			//cout<<"Size of group:("<<trans_PE_ID_conv.size()<<")"<<endl;

			cout<<"Final Trans PE ids: "<<".....";
			for(int ap=0;ap<trans_PE_ID.size();ap++)
			{
				cout<<"("<<trans_PE_ID[ap]<<"-"<<trans_pool[ap]<<")..";
			}
			//cout<<endl<< "Receive neuron id for layer 1, group " <<ID_group<<": ";
			//for(int ab =0; ab <receive_neu_ID_conv[85].size(); ab++)
			//{

			//	cout<<receive_neu_ID_conv[85][ab]<<"--";
			//}
			//cout<<"Size of group:("<<receive_neu_ID_conv[0].size()<<")"<<endl;
			//cout<<"...........";
		} */

		//if(ID_group ==60){

		//	cout<< receive_conv.size()<<"("<<receive_conv[0]<<")"<<endl;
		//}
		/*-------------------------------------*/
		
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
								//if(ID_group >= 0 && ID_group <= 47)
								//{
								//	cout<<"Packet Checking....";
								//	cout<<"Src id: "<<packet.src_id<<" Dst Id: "<<packet.dst_id<<" Size: "<<packet.size<<endl;
								//}
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
							}
							/*--------------Debugging-----------*/
							//cout<<"Pooling layer, Local id:  "<<local_id<<endl;
							//if(ID_group == 48)
							//{
							//	cout<<"Packet Checking....";
							//	cout<<"Src id: "<<packet.src_id<<" Dst Id: "<<packet.dst_id<<" Size: "<<packet.size<<endl;
							//}
							/*----------------------------------*/
						}	
					}
					
					
					flag_p = 0;
					transmittedAtPreviousCycle = true;
				} 
				else
					transmittedAtPreviousCycle = false;
			}
		}

		if(ack_tx.read() == 1  && flag_f && PE_enable && ID_layer != NN_Model->all_leyer_size.size()-1 && (sc_simulation_time() >= temp_computation_time + computation_time||ID_layer==0) ){
			if (!packet_queue.empty()) {
				//NoximFlit flit = nextFlit();	// Generate a new flit
				
				/*------------Debugging----------------*/
				//cout<<"Packet Size: "<<packet_queue.size()<<endl;
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
//**********************tytyty***************************
	if(ID_layer%2)
		flit.XYX_routing   = 0;	//YX_routing
	else
		flit.XYX_routing   = 1; //XY_routing
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
			//if(ID_group == 3 && pe_id == 49)
			//{
			//	cout<<"("<<flit.src_Neu_id<<")--("<<start_index<<")--";
			//}
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
		}
		
    }
//************************************^^^^^^^^^^^^^^^^^^^^^^^^*********************
	
    packet_queue.front().flit_left--;
    if (packet_queue.front().flit_left == 0)
	packet_queue.pop();

	/*------------Debugging--------------*/
	//if(ID_group == 0)
	//{
		//cout<<"(flit Type: "<<flit.flit_type<<"--";
		//cout<<"Source Neu ID: "<<flit.src_Neu_id<<"--";
		//cout<<"Sequence No: "<<flit.sequence_no<<"--";
		//cout<<"Packet size: "<<packet.size<<"--";
		//cout<<"Packet flit left: "<<packet.flit_left<<"--";
		//cout<<"Start Index: "<<start_index<<")--";
	//}
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
