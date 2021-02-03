/*
 * NN-Noxim - the NoC-based ANN Simulator
 *
 * (C) 2018 by National Sun Yat-sen University in Taiwan
 *
 * This file contains the implementation of loading NN model
 */

#include "NNModel.h"
#include <iomanip>
#include <math.h>
#include <ctime>
#include <string>

NNModel::NNModel()
{
}

bool NNModel::load()//M_fname Useless tytyty
{
	cout<<"model file loading (filename: " << NoximGlobalParams::NNmodel_filename << ")..."<< endl;		//** 2018.09.02 edit by Yueh-Chi,Yang **//
	ifstream fin(NoximGlobalParams::NNmodel_filename, ios::in); 						//** 2018.09.02 edit by Yueh-Chi,Yang **//
	char temp_type[20], temp_sv_pad[20], temp_actfun[10];
	int temp;
	int temp_c_x, temp_c_y, temp_z, temp_num, temp_std, temp_x, temp_y, temp_pad, temp_channels, temp_c_z;
	int input_size, output_size;
	deque< deque< int > > conv;
	deque< deque< int > > pool;
	int all_Nue=0;
	// *****************all layer Neu_num setting*******************
	cout<<endl;
	cout<<"layer_ID |    type | Neu_num |       X |       Y | channel |   C/P_X |   C/P_Y |    C/P_Z |  stride | padding | act_fun |"<<endl;
	cout<<"-------------------------------------------------------------------------------------------------------------------------"<<endl;

	while(fin >> temp_type){


		if (!strcmp( temp_type, "Dense"))
		{
			all_leyer_type.push_back('f');
			char line[256];
			fin.getline(line, sizeof(line) - 1);
			sscanf(line, "%d %s", &temp, temp_actfun);
			deque< int > temp_leyer_size;
			temp_leyer_size.push_back(temp);
			if(!strcmp( temp_actfun, "relu"))
				temp_leyer_size.push_back(RELU);
			else if(!strcmp( temp_actfun, "tanh"))
				temp_leyer_size.push_back(TANH);
			else if(!strcmp( temp_actfun, "sigmoid"))
				temp_leyer_size.push_back(SIGMOID);
			else if(!strcmp( temp_actfun, "softmax"))
				temp_leyer_size.push_back(SOFTMAX);

			all_leyer_size.push_back(temp_leyer_size);
			all_Nue+=temp;
			cout<<setw(8)<<all_leyer_type.size()-1<<" |"<<    setw(8)<<"Fully"<<" |"<<setw(8)<<temp_leyer_size[0]<<" |"
				<<                   setw(10)<<" |"<<            setw(10)<<" |"<<                   setw(10)<<" |"
				<<                   setw(10)<<" |"<<            setw(10)<<" |"<<            setw(10)<<" |"
				<<                   setw(10)<<" |"
				<<                   setw(10)<<" |"<<setw(8)<<temp_actfun<<" |"<<endl;
		}
		else if (!strcmp( temp_type, "Input"))
		{
			all_leyer_type.push_back('i');
			char line[256];
			fin.getline(line, sizeof(line) - 1);
			sscanf(line, "%d %d %d", &temp_x, &temp_y, &temp_z);
			temp = temp_x * temp_y*temp_z;
			deque< int > temp_leyer_size;
			temp_leyer_size.push_back(temp);
			temp_leyer_size.push_back(temp_x);
			temp_leyer_size.push_back(temp_y);
			temp_leyer_size.push_back(temp_z);

			all_leyer_size.push_back(temp_leyer_size);
			//all_Nue+=temp;
			cout<<setw(8)<<all_leyer_type.size()-1<<" |"<<           setw(8)<<"Input"<<" |"<<setw(10)<<" |"
				<<setw(8)<<temp_leyer_size[1]<<" |"<<setw(8)<<temp_leyer_size[2]<<" |"<<setw(8)<<temp_leyer_size[3]<<" |"
				<<                   setw(10)<<" |"<<                   setw(10)<<" |"<<                   setw(10)<<" |"
				<<                   setw(10)<<" |"<<                   setw(10)<<" |"<<            setw(10)<<" |"<<endl;
		}
		else if (!strcmp( temp_type, "Convolution"))
		{
			all_leyer_type.push_back('c');
			char line[256];
			fin.getline(line, sizeof(line) - 1);
			sscanf(line, "%d %d %d %d %d %d %d %d %s",&temp_x,&temp_y, &temp_channels, &temp_c_x, &temp_c_y,&temp_c_z, &temp_std, &temp_pad, temp_actfun);
			deque< int > temp_leyer_size;
			temp = temp_x *temp_y*temp_channels;
			temp_leyer_size.push_back(temp);  //The size of the convolution layer
			temp_leyer_size.push_back(temp_x);
			temp_leyer_size.push_back(temp_y);
			temp_leyer_size.push_back(temp_channels);
			temp_leyer_size.push_back(temp_c_x);
			temp_leyer_size.push_back(temp_c_y);
			temp_leyer_size.push_back(temp_c_z);
			temp_leyer_size.push_back(temp_std);
			temp_leyer_size.push_back(temp_pad);
			if(!strcmp( temp_actfun, "relu"))
				temp_leyer_size.push_back(RELU);
			else if(!strcmp( temp_actfun, "tanh"))
				temp_leyer_size.push_back(TANH);
			else if(!strcmp( temp_actfun, "sigmoid"))
				temp_leyer_size.push_back(SIGMOID);
			else if(!strcmp( temp_actfun, "softmax"))
				temp_leyer_size.push_back(SOFTMAX);

			all_leyer_size.push_back(temp_leyer_size);
			all_Nue+=temp;
			cout<<setw(8)<<all_leyer_type.size()-1<<" |"<<     setw(8)<<"Convol"<<" |"<<setw(8)<<temp_leyer_size[0]<<" |"
			<<      setw(8)<<temp_leyer_size[1]<<" |"<<   setw(8)<<temp_leyer_size[2]<<" |"<<setw(8)<<temp_leyer_size[3]<<" |"
			<<      setw(8)<<temp_leyer_size[4]<<" |"<<   setw(8)<<temp_leyer_size[5]<<" |"<<setw(8)<<temp_leyer_size[6]<<" |"	
			<<      setw(8)<<temp_leyer_size[7]<<" |"<<   setw(8)<<temp_leyer_size[8]<<" |"<<setw(8)<<temp_actfun<<" |"<<endl;
		}else if (!strcmp( temp_type, "Pooling"))
		{
			all_leyer_type.push_back('p');
			char line[256];
			fin.getline(line, sizeof(line) - 1);
			sscanf(line, "%d %d %d %d %d %d %s", &temp_x,&temp_y, &temp_channels, &temp_c_x, &temp_c_y, &temp_std, temp_actfun);
			deque< int > temp_leyer_size;
			temp = temp_x *temp_y*temp_channels;
			temp_leyer_size.push_back(temp);//The size of the pooling layer
			temp_leyer_size.push_back(temp_x);
			temp_leyer_size.push_back(temp_y);
			temp_leyer_size.push_back(temp_channels);
			temp_leyer_size.push_back(temp_c_x);
			temp_leyer_size.push_back(temp_c_y);
			temp_leyer_size.push_back(temp_std);
			
			if(!strcmp( temp_actfun, "average"))
				temp_leyer_size.push_back(AVERAGE);
			else if(!strcmp( temp_actfun, "maximum"))
				temp_leyer_size.push_back(MAXIMUM);

			all_leyer_size.push_back(temp_leyer_size);
			all_Nue+=temp;
			cout<<setw(8)<<all_leyer_type.size()-1<<" |"<<     setw(8)<<"Pooling"<<" |"<<setw(8)<<temp_leyer_size[0]<<" |"
			<<      setw(8)<<temp_leyer_size[1]<<" |"<<   setw(8)<<temp_leyer_size[2]<<" |"<<setw(8)<<temp_leyer_size[3]<<" |"
			<<      setw(8)<<temp_leyer_size[4]<<" |"<<   setw(8)<<temp_leyer_size[5]<<" |"<<            setw(10)<<" |"<<setw(8)<<temp_leyer_size[6]<<" |"	
			<<                   setw(10)<<" |"<<         setw(8)<<temp_actfun<<" |"<<endl;
		}
		else if (!strcmp( temp_type, "%"))
		{
		
			char line[256];
			fin.getline(line, sizeof(line) - 1);
		}
		else
		{
			cout<<"!!Error model format: "<<temp_type<<" !!"<<endl;
			char line[256];
			fin.getline(line, sizeof(line) - 1);
		}

	}
	//for( int i=0; i<5;i++){
	//  cout<<all_leyer_type[i]<<"-------";
	//}

	cout<<"model all_leyer complete"<<endl;
	cout<<"all neu:"<<all_Nue<<endl;
	fin.close();
	input_size=all_leyer_size.front()[0];
	output_size=all_leyer_size.back()[0];

	//******************mapping information prepare************************
	cout<<"Mapping Algorithm=> "<< NoximGlobalParams::mapping_algorithm<<endl;
	cout<<"Mapping Method=> STATIC"<<endl;
	if(NoximGlobalParams::faulty_mode != INACTIVE)
	{
		cout<<"Routing Algorithm=> Table based routing"<<endl;
	}else
	{
		cout<<"Routing Algorithm=> XYX routing"<<endl;
	}
	
	
	mapping_table.clear();
	mapping_table.assign( NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y, -1 );
	
	
	if(!strcmp(NoximGlobalParams::mapping_algorithm, "random") )
	{
		srand( time(NULL) );
		for( int i = 0; i < mapping_table.size() ; i++ )
		{
			while(1)
			{
				int map_point = rand() % mapping_table.size();
				if( mapping_table[map_point]==-1 )
				{
					mapping_table[map_point] = i;
					break;
				}

			}
		}
	}
	else if(!strcmp(NoximGlobalParams::mapping_algorithm, "dir_x") )
		for( int i = 0; i < mapping_table.size() ; i++ )	//dir_x mapping
			mapping_table[i]=i;
	else if(!strcmp(NoximGlobalParams::mapping_algorithm, "dir_y") )	
		for( int i = 0; i < mapping_table.size() ; i++ )	//dir_y mapping
			mapping_table[i]= (i%NoximGlobalParams::mesh_dim_x)*NoximGlobalParams::mesh_dim_x + (i/NoximGlobalParams::mesh_dim_x);	
	else if(!strcmp(NoximGlobalParams::mapping_algorithm, "table") )	
	{
		ifstream fin_m(NoximGlobalParams::mapping_table_filename, ios::in);						//** 2018.09.02 edit by Yueh-Chi,Yang **
		cout<<"mapping file loading (filename: " << NoximGlobalParams::mapping_table_filename << ")..."<< endl;		//** 2018.09.02 edit by Yueh-Chi,Yang **
		while(!fin_m.eof()){
			char line[256];
			fin_m.getline(line, sizeof(line) - 1);
			if (line[0] != '\0') {
					if (line[0] != '%') {
					int ID_Group, ID_PE;
					sscanf(line, "%d %d", &ID_Group, &ID_PE);
					mapping_table[ID_Group] = ID_PE;
					}
			}
		}
	}

	else
	{
		cout<<"Error mapping algorithm!!"<<endl;
		exit(1);
	}
	
	/*------Debugging--------*/
	//cout<<"Mapping Table"<<endl;
	//for( int i =0; i< mapping_table.size();i++){
	//	cout<< mapping_table.at(i)<<"--";
	//}
	//cout<<endl;
	/*-----------------------*/
	cout<<"Model mapping table: "<<mapping_table.size()<<endl;
	cout<<"maping complete"<<endl;
	
	// ******************temp_Group_table setting**********************
	int temp_ID_Neu=0;
	int temp_layer=1; //Layer id starts from layer 1 which is convolution layer
	int temp_ID_In_layer=0;
	int temp_ID_Group=0;
	int temp_ID_In_Group=0;
	float temp_w;
	int temp_ID_conv = 0;
	int temp_ID_pool =-1;

	deque <deque<float>> temp_conv_weight;
	deque<deque<deque<float>>> temp_conv_weight_layer;
	int temp_layer_maxID = all_leyer_size[temp_layer][0]; //Starting with layer 1
	//cout<<"Max id: "<<temp_layer_maxID<<endl;
	Group_table.clear();
	deque < NeuInformation > temp_Group_table;
	temp_Group_table.clear();
	deque < int > temp_leyer_ID_Group;
	temp_leyer_ID_Group.clear();
	
	//int prevLayer_size;
	int kernel_size;
	deque <float> temp_bias;
	ifstream fin_w(NoximGlobalParams::NNweight_filename, ios::in);	
	cout<<"weight file loading (filename: " << NoximGlobalParams::NNweight_filename << ")..."<<endl;	//** 2018.09.02 edit by Yueh-Chi,Yang **//
    
	//Save convolution weights and bias for each filter
	deque<float> temp_weights;
	deque<float> temp_bias_conv;
	for(int i =0; i< all_leyer_type.size(); i++)
	{
       if(all_leyer_type[i] == 'c')
	   {
		   kernel_size = all_leyer_size[i][4] * all_leyer_size[i][5];
		   temp_conv_weight.clear();
		   temp_bias_conv.clear();
		   temp_conv_weight_layer.clear();
		   for(int j=0; j< all_leyer_size[i][3];j++)
		   {
				
				for(int q=0;q<all_leyer_size[i][6];q++)
				{
					temp_weights.clear();
					for(int l=0; l<kernel_size;l++)
					{
						fin_w >> temp_w;
						temp_weights.push_back(temp_w);
					}
					temp_conv_weight.push_back(temp_weights);//Convolution kernel weights
				}
			   temp_conv_weight_layer.push_back(temp_conv_weight);
			   temp_conv_weight.clear();
		   }
			all_conv_weight.push_back(temp_conv_weight_layer);
			temp_conv_weight_layer.clear();
		   for( int m =0; m < all_leyer_size[i][3];m++)
		   {
			   fin_w >> temp_w;
			   temp_bias_conv.push_back(temp_w);   //Bias for each filter   
		   }
		   all_conv_bias.push_back(temp_bias_conv);
	   }

	}

	/*--------------Debugging---------------------------*/
    /*cout<< all_conv_bias.size()<<"--"<<all_conv_bias[1].size()<<endl;
    for( int p=0; p< all_conv_bias[1].size();p++)
	{
		cout<<all_conv_bias[1][p]<<"-----";
	}*/
	/*cout<<endl;
	cout<<all_conv_weight.size()<<endl;
	
	cout<<all_conv_weight[0].size()<<endl;
	cout<<all_conv_weight[1].size()<<endl;
	cout<<all_conv_weight[0][0].size()<<endl;
	cout<<all_conv_weight[1][0].size()<<endl;
	cout<<all_conv_weight[0][0][0].size()<<endl;
	cout<<all_conv_weight[1][0][0].size()<<endl;
	for( int p=0; p< all_conv_weight[1][0].size();p++)
	{
		for(int q=0; q<all_conv_weight[1][0][p].size();q++)
		{
			cout<<all_conv_weight[1][0][p][q]<<"---";
		}
		
	}
	cout<<endl;*/
	/*--------------------------------------------------*/
    
	
	while(1)
	{
		NeuInformation NeuInfo;

		NeuInfo.ID_Neu = temp_ID_Neu;

		if(temp_ID_Neu < temp_layer_maxID){									
			NeuInfo.ID_layer = temp_layer;
			NeuInfo.ID_In_layer = temp_ID_In_layer;
			if(all_leyer_type[temp_layer] == 'c')
			{	
				NeuInfo.ID_conv = temp_ID_conv;
				//cout<<"("<< temp_ID_Neu<<"--"<<NeuInfo.ID_conv<<")--";

			}else if (all_leyer_type[temp_layer] == 'p' )
			{
				NeuInfo.ID_pool = temp_ID_pool;
				//cout<<"("<< temp_ID_Neu<<"--"<<NeuInfo.ID_pool<<")--";

			}

			if( temp_ID_In_Group >=  NoximGlobalParams::group_neu_num){					//** 2018.09.01 edit by Yueh-Chi,Yang **// change group
				Group_table.push_back(temp_Group_table);
        			temp_Group_table.clear();

				temp_ID_In_Group = 0;
				temp_leyer_ID_Group.push_back(temp_ID_Group);
				temp_ID_Group++;
			}
			NeuInfo.ID_In_Group = temp_ID_In_Group;
			NeuInfo.ID_Group = temp_ID_Group;
			/*----------------Debugging-----------------*/
			/*if( temp_layer == 1 || temp_layer == 3)
			{
				cout<< "ID Conv : "<<temp_ID_conv<<"--"; 
			}else if( temp_layer == 2 || temp_layer == 4)
			{
				cout<<"ID_pool: "<<temp_ID_pool<<"--";
			}*/
			/*------------------------------------------*/
		}
		else{													//change layer and group
			//cout<<"Temp id in layer: "<<temp_ID_In_layer<<endl;
			Group_table.push_back(temp_Group_table);
			temp_Group_table.clear();
					
			NeuInfo.ID_layer = ++temp_layer;
			temp_leyer_ID_Group.push_back(temp_ID_Group);
			all_leyer_ID_Group.push_back(temp_leyer_ID_Group);
			temp_leyer_ID_Group.clear();
			NeuInfo.ID_Group = ++temp_ID_Group;

			if(temp_ID_Neu >= all_Nue) break;

			if(temp_ID_Group >= NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y && NoximGlobalParams::mapping_method == STATIC)
				cout<<"error group_size or NoC_size" <<endl;						//output error!! 
			
			temp_ID_In_layer = 0;
			temp_ID_In_Group = 0;
			NeuInfo.ID_In_layer = temp_ID_In_layer;
			NeuInfo.ID_In_Group = temp_ID_In_Group;
			temp_layer_maxID += all_leyer_size[temp_layer][0];


			if(all_leyer_type[temp_layer] == 'c' )
			{	
				
				NeuInfo.ID_conv = ++temp_ID_conv;
				//cout<<"("<< temp_ID_Neu<<"--"<<NeuInfo.ID_conv<<")--";

			}else if (all_leyer_type[temp_layer] == 'p' )
			{
				NeuInfo.ID_pool = ++temp_ID_pool;
				//cout<<"("<< temp_ID_Neu<<"--"<<NeuInfo.ID_pool<<")--";

			}
			temp_bias.clear();
			if(all_leyer_type[temp_layer]=='f')
			{
				for(int k=0;k<all_leyer_size[temp_layer][0];k++)
				{
					fin_w >> temp_w;
					temp_bias.push_back(temp_w);
					
				}
				/*-------------Debugging------------*/
				//cout<<temp_bias.back()<<endl;
				//cout<<temp_bias.front()<<endl;
				//cout<<"Size of temp_bias: "<<temp_bias.size()<<"--"<<temp_layer<<endl;
				/*----------------------------------*/
			}
			
		}

		NeuInfo.Type_layer = all_leyer_type[NeuInfo.ID_layer];

		 if( NeuInfo.Type_layer == 'f')
		{
			for( int i=0 ; i < ( all_leyer_size[ temp_layer-1 ][0]) ; i++ )	
			{
				fin_w>>temp_w;
				NeuInfo.weight.push_back(temp_w);
			}
			NeuInfo.weight.push_back(temp_bias[NeuInfo.ID_In_layer]); //Include bias
			/*------Debugging------*/
			//cout<<NeuInfo.weight.size()<<"-"<<NeuInfo.weight.front()<<"-"<<NeuInfo.weight.back()<<endl;
			/*---------------------*/
		}
		//cout<<endl;
		//***********mapping****************
		//all_conv_weight.push_back(temp_conv_weight);
		temp_Group_table.push_back(NeuInfo);
		temp_ID_In_Group++;
		temp_ID_In_layer++;
		temp_ID_Neu++;
	}
	/*cout<<endl<<"Convolution weight: "<<endl;
	for( int i=0; i<all_leyer_type.size(); i++)
	{
	  if( all_leyer_type[i] == 'c')
	  {
	    for( int j= 0; j< (all_leyer_size[i][3] * all_leyer_size[i][4] *all_leyer_size[i][5] + all_leyer_size[i][3]); j++)
	    { 
	      fin_w >> temp_w;
	      temp_conv_weight.push_back(temp_w); //Convolution weight and then bias value
	      cout<<temp_w<< "----";
            }		
	  }
	  all_conv_weight.push_back(temp_conv_weight);
	} */	
	fin_w.close();
	
	//deque<float>().swap(temp_conv_weight);
	deque<NeuInformation>().swap(temp_Group_table);
	deque<int>().swap(temp_leyer_ID_Group);
	cout<<"model & group complete"<<endl;
    
	
	
	 /*-------------------Debugging---------------------*/
	/*
	 cout<< Group_table[1][0].Type_layer<<endl;
	 cout<< Group_table[50][0].Type_layer<<endl;;
	 
	 cout<<"Group Table"<<endl;
    cout<<"Neuron ID"<<endl;
    for( int i =0; i< Group_table.size();i++){
		for( int j=0; j< Group_table[i].size();j++ ){
             cout<< Group_table[i][j].ID_Neu<<"--";

		}	
	}
    cout<<endl;
	
	cout<<"Id in Group"<<endl;
	for( int i =0; i< Group_table.size();i++){
		for( int j=0; j< Group_table[i].size();j++ ){
             cout<< Group_table[i][j].ID_In_Group<<"--";

		}	
	}
    cout<<endl;
	cout<<"Id in layer"<<endl;
	for( int i =0; i< Group_table.size();i++){
		for( int j=0; j< Group_table[i].size();j++ ){
             cout<< Group_table[i][j].ID_In_layer<<"--";

		}	
	}
    cout<<endl;
	
	 //Reverse Eng
	 cout<<"All Layer ID Group"<<endl;
	
    
	for( int i =0; i< all_leyer_ID_Group.size();i++){
		for( int j=0; j< all_leyer_ID_Group[i].size();j++ ){
             cout<< all_leyer_ID_Group[i][j]<<"--";

		}
		
	}
	
	cout<<"All layer id group size: "<<all_leyer_ID_Group.size()<<endl;
	//cout<<"Last layer: "<<all_leyer_ID_Group[6].size()<<endl;
	//cout<<"First layer: "<<all_leyer_ID_Group[0].size()<<endl;
	cout<<"Group table size: "<<Group_table.size()<<endl;
	cout<<"all layer type size: "<<all_leyer_type.size()<<endl;
	cout<<"all layer size: "<<all_leyer_size.size()<<endl;
	
    cout<<endl;
	//Reverse Eng
	cout<<"All layer Size"<<endl;
	for( int i =0; i< all_leyer_size.size();i++){
		for( int j=0; j< all_leyer_size[i].size();j++ ){
             cout<< all_leyer_size[i][j]<<"--";

		}
		
	}
    cout<<endl;

	//Reverse Eng
	cout<<"All layer Type"<<endl;

	for( int i =0; i< all_leyer_type.size();i++)
		{
             cout<< all_leyer_type[i]<<"--";	
		}
	cout<<endl; */
	/*-----------------------------------*/

	active_layers.clear();

	/*------------------Faulty Nodes ---------------------*/
	if(NoximGlobalParams::faulty_mode != INACTIVE)
	{
		faulty_mapping_table.clear();
		faulty_mapping_table = mapping_table;
		FaultyMode();
		ShortestPath();
		
	}
	
//******************print floorplan****************
	Mapping_done = false;
	if(NoximGlobalParams::mapping_method == STATIC)
	{
		HardwarePlan();
		
		
	}else if (NoximGlobalParams::mapping_method == DYNAMIC)
	{
		int max_neu =all_leyer_size[1][0];   
		for(int i =2; i< all_leyer_size.size(); i++)
		{
			if( max_neu < all_leyer_size[i][0])
			{
				max_neu = all_leyer_size[i][0];
			}
		}
		//cout<<max_neu<<endl;
		//cout<< NoximGlobalParams::group_neu_num<<endl;
		//cout<<max_neu/NoximGlobalParams::group_neu_num<<endl;
		//cout<<(NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y)<<endl;
		assert( max_neu/NoximGlobalParams::group_neu_num <= (NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y));
		interm_data_in.clear();
		active_layers.clear();
		interm_data_out.clear();
		Dy_Group_table = Group_table;
		PEMappingCount.clear();

		Dymapping();
	}

	
	//***********input setting***************

	fstream fin_in(NoximGlobalParams::NNinput_filename, ios::in); 

	float temp_in;
	int i = -1;
	deque< float > temp_data_in;
	temp_data_in.clear();

	while(fin_in >> temp_in){
    		i++;
		temp_data_in.push_back(temp_in);
		if(i==input_size-1)
		{
			all_data_in.push_back(temp_data_in);
    			temp_data_in.clear();
			i=-1;
		}
	}

	deque<float>().swap(temp_data_in);
	fin_in.close();

	cout<<"all_data_in.size(): "<<all_data_in.size()<<endl;
	cout<<"load input complete"<<endl;
    
	/*--------Debugging-----------------*/
	//cout<<"All data in"<<endl;
	//for( int i =0; i< all_data_in.size();i++){
	//	for( int j=0; j< all_data_in[i].size();j++ ){
      //       cout<<"( "<<j<<": "<< all_data_in[i][j]<<")--";
	//	}	
	//}
	//cout<<endl;
	//cout<<"Size of input: "<<all_data_in[0].size()<<endl;
	/*----------------------------------*/
	
	//******************Save the coordinates for 2d Convolution and Pooling****************
	all_conv_coord.clear();
	all_pool_coord.clear();
	deque< int> temp_cell;
	deque < deque<int>> temp_matrix;
	for(int ab =0; ab< all_leyer_size.size(); ab++)
	{
		if(all_leyer_type[ab] == 'c')
		{
			int coord_x = all_leyer_size[ab][1];
			int coord_y = all_leyer_size[ab][2];
			int coordPrev_x = all_leyer_size[ab-1][1];
			int coordPrev_y = all_leyer_size[ab-1][2];
			int kernel_x = all_leyer_size[ab][4];
			int kernel_y = all_leyer_size[ab][5];
			int kernel_z = all_leyer_size[ab][6];
			int padding = all_leyer_size[ab][8];
			int stride_c = all_leyer_size[ab][7];

			if(padding == 0){
				if(stride_c == 1)
				{
					for(int aa =0; aa< coord_x; aa++)
					{
						for(int bb =0; bb < coord_y; bb++)
						{
							for(int cc =0; cc< kernel_z; cc++)
							{
								for(int dd =0; dd< kernel_x; dd++)
								{
									for(int ee =0; ee<kernel_y; ee++)
									{
										temp_cell.push_back((aa+dd)*coordPrev_y + (ee+ bb) + cc*coordPrev_x*coordPrev_y);
									}
								}
							}
							temp_matrix.push_back(temp_cell);
							temp_cell.clear();
						}
					}
					all_conv_coord.push_back(temp_matrix);
					temp_matrix.clear();
				}else
				{
					int horizontal =0;
					int vertical =0;
					for(int aa =0; aa< coord_x; aa++)
					{
						for(int bb =0; bb < coord_y; bb++)
						{
							if(aa >0){vertical =1;}
							if(bb >0){horizontal =1;}
							if(horizontal ==0 && vertical ==0) //0,0
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											temp_cell.push_back((aa+dd)*coordPrev_y + (ee+ bb) + cc*coordPrev_x*coordPrev_y);
										}
									}
								}
							
							}else if(horizontal ==0 && vertical ==1)
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											temp_cell.push_back((aa*stride_c+dd)*coordPrev_y + (ee+ bb) + cc*coordPrev_x*coordPrev_y);
										}
									}
								}
							}else if(horizontal ==1 && vertical == 0)
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											temp_cell.push_back((aa+dd)*coordPrev_y + (ee+ bb*stride_c) + cc*coordPrev_x*coordPrev_y);
										}
									}
								}
							}else if(horizontal ==1 && vertical == 1)
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											temp_cell.push_back((aa*stride_c+dd)*coordPrev_y + (ee+ bb*stride_c) + cc*coordPrev_x*coordPrev_y);
										}
									}
								}
							}
							horizontal =0;
							vertical =0;
							temp_matrix.push_back(temp_cell);
							temp_cell.clear();
						}
					}
					all_conv_coord.push_back(temp_matrix);
					temp_matrix.clear();

					/*------------------------Debugging---------------------*/
					/*int sf =11;
					for(int ff=0; ff<all_conv_coord[1][sf].size(); ff++)
					{
						cout<<all_conv_coord[1][sf][ff] <<"--";
					}
					cout<<endl<<"Size: "<<all_conv_coord[1][sf].size()<<endl;
					
					/*------------------------------------------------------*/
				}	
			}else if(padding == 1)
			{
				//Changes for Stride and padding in convolution
				if(stride_c == 1)
				{
					for(int aa =0; aa< coord_x; aa++)
					{
						for(int bb =0; bb < coord_y; bb++)
						{
							for(int cc =0; cc< kernel_z; cc++)
							{
								for(int dd =0; dd< kernel_x; dd++)
								{
									for(int ee =0; ee<kernel_y; ee++)
									{
										bool cond = (aa + dd == 0) || (aa+dd == (coordPrev_x+1)) || (ee+ bb == 0) || (ee+ bb == coordPrev_y+1);
										if(!cond)
										{
											temp_cell.push_back((aa+dd-1)*coordPrev_y + (ee+ bb-1) + cc*coordPrev_x*coordPrev_y);
										}
										
										
									}
								}
							}
							temp_matrix.push_back(temp_cell);
							temp_cell.clear();
						}
					}
					all_conv_coord.push_back(temp_matrix);
					temp_matrix.clear();

					/*-----------------Debugging---------------*/
					/*int sf =0;
					for(int ff=0; ff<all_conv_coord[1][sf].size(); ff++)
					{
						cout<<all_conv_coord[1][sf][ff] <<"--";
					}
					cout<<endl<<"Size: "<<all_conv_coord[1][sf].size()<<endl;
					cout<<coordPrev_y+1<<endl;
					
					/*-----------------------------------------*/
				}else
				{
					int horizontal =0;
					int vertical =0;
					for(int aa =0; aa< coord_x; aa++)
					{
						for(int bb =0; bb < coord_y; bb++)
						{
							if(aa >0){vertical =1;}
							if(bb >0){horizontal =1;}
							if(horizontal ==0 && vertical ==0) //0,0
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											bool cond = (aa + dd == 0) || (aa+dd == (coordPrev_x+1)) || (ee+ bb == 0) || (ee+ bb == coordPrev_y+1);
											if(!cond)
											{
												temp_cell.push_back((aa+dd-1)*coordPrev_y + (ee+ bb-1) + cc*coordPrev_x*coordPrev_y);
											}
										}
									}
								}
							
							}else if(horizontal ==0 && vertical ==1)
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											bool cond = (aa*stride_c + dd == 0) || (aa*stride_c+dd == (coordPrev_x+1)) || (ee+ bb == 0) || (ee+ bb == coordPrev_y+1);
											if(!cond)
											{
												temp_cell.push_back((aa*stride_c+dd-1)*coordPrev_y + (ee+ bb-1) + cc*coordPrev_x*coordPrev_y);
											}
											
										}
									}
								}
							}else if(horizontal ==1 && vertical == 0)
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											bool cond = (aa + dd == 0) || (aa+dd == (coordPrev_x+1)) || (ee+ bb*stride_c == 0) || (ee+ bb*stride_c == coordPrev_y+1);
											if(!cond)
											{
												temp_cell.push_back((aa+dd-1)*coordPrev_y + (ee+ bb*stride_c-1) + cc*coordPrev_x*coordPrev_y);
											}
											
										}
									}
								}
							}else if(horizontal ==1 && vertical == 1)
							{
								for(int cc =0; cc< kernel_z; cc++)
								{
									for(int dd =0; dd< kernel_x; dd++)
									{
										for(int ee =0; ee<kernel_y; ee++)
										{
											bool cond = (aa*stride_c + dd == 0) || (aa*stride_c+dd == (coordPrev_x+1)) || (ee+ bb*stride_c == 0) || (ee+ bb*stride_c == coordPrev_y+1);
											if(!cond)
											{
												temp_cell.push_back((aa*stride_c+dd-1)*coordPrev_y + (ee+ bb*stride_c-1) + cc*coordPrev_x*coordPrev_y);
											}
											
										}
									}
								}
							}
							horizontal =0;
							vertical =0;
							temp_matrix.push_back(temp_cell);
							temp_cell.clear();
						}
					}
					all_conv_coord.push_back(temp_matrix);
					temp_matrix.clear();
					/*-----------------Debugging---------------*/
					/*int sf =12;
					for(int ff=0; ff<all_conv_coord[1][sf].size(); ff++)
					{
						cout<<all_conv_coord[1][sf][ff] <<"--";
					}
					cout<<endl<<"Size: "<<all_conv_coord[1][sf].size()<<endl;
					cout<<coordPrev_y+1<<endl;
					
					/*-----------------------------------------*/
				}
				
				
			}
			
		}
		else if(all_leyer_type[ab] == 'p')
		{
			int coord_x =all_leyer_size[ab][1];
			int coord_y=all_leyer_size[ab][2];
			int coordPrev_x = all_leyer_size[ab-1][1];
			int coordPrev_y = all_leyer_size[ab-1][2];
			int kernel_x = all_leyer_size[ab][4];
			int kernel_y= all_leyer_size[ab][5];
			int stride = all_leyer_size[ab][6];
			int horizontal =0;
			int vertical =0;
			temp_matrix.clear();
			temp_cell.clear();

			for(int aa =0; aa< coord_x; aa++)
			{
				for(int bb =0; bb<coord_y; bb++)
				{
					if(aa >0){vertical =1;}
					if(bb >0){horizontal =1;}
					if(horizontal ==0 && vertical ==0) //0,0
					{
						for(int cc=0; cc<kernel_x; cc++)
						{
							for(int dd =0; dd< kernel_y; dd++)
							{
								temp_cell.push_back((aa+cc)*coordPrev_y+(bb+dd));
							}
						}

					}else if(horizontal ==0 && vertical ==1)
					{
						for(int cc=0; cc<kernel_x; cc++)
						{
							for(int dd =0; dd< kernel_y; dd++)
							{
								temp_cell.push_back((aa*stride+cc)*coordPrev_y+(bb+dd));
							}
						}										
					}else if(horizontal ==1 && vertical == 0)
					{
						for(int cc=0; cc<kernel_x; cc++)
						{
							for(int dd =0; dd< kernel_y; dd++)
							{
								temp_cell.push_back((aa+cc)*coordPrev_y+(bb*stride+dd));
							}
						}
					}else if(horizontal ==1 && vertical == 1)
					{
						for(int cc=0; cc<kernel_x; cc++)
						{
							for(int dd =0; dd< kernel_y; dd++)
							{
								temp_cell.push_back((aa*stride+cc)*coordPrev_y+(bb*stride+dd));
							}
						}	
					}
					horizontal =0;
					vertical =0;
					temp_matrix.push_back(temp_cell);
					temp_cell.clear();
				}
			}
			all_pool_coord.push_back(temp_matrix);
			temp_matrix.clear();
		}
	}

	cout<<"Convolution and Pooling layer's related activities are completed."<<endl;
	/*--------------------Debugging-----------------------*/
	//cout<<"Conv deque Size: "<<all_conv_coord.size()<<" Size zero: "<<all_conv_coord[0].size()<<"Size One: "<<all_conv_coord[1].size() <<endl;
	/*for(int gg =0; gg< all_conv_coord[1][0].size(); gg++)
	{
		cout<<all_conv_coord[1][0][gg]<<"--";
	}
	cout<<all_conv_coord[1][0].size()<<endl;*/
	//cout<<"Pool deque Size: "<<all_pool_coord.size()<<" Size zero: "<<all_pool_coord[0].size()<<"Size One: "<<all_pool_coord[1].size() <<endl;
	/*for(int gg=0; gg<all_pool_coord[1][0].size(); gg++)
	{
		cout<<all_pool_coord[1][0][gg]<<"--";
	}*/
	//cout<<"All pool Zero: "<< all_pool_coord[0][0][0]<<endl;
	//cout<<"size: "<< all_pool_coord[0][0].size()<<endl;
	/*----------------------------------------------------*/

    	return true;
}

void NNModel::Dymapping()
{
	bool map_Layer = false; 
	int mapped_layer;   // the previously mapped last layer
	Group_table.clear();

	if(active_layers.size() != 0)
	{
		mapped_layer = active_layers.back();
		active_layers.clear();
	}else
	{
		mapped_layer = -1; //first time mapping
	}

	if(mapped_layer == all_leyer_ID_Group.size())
	{
		//END - clear everything
		cout<<"------------------------------------------------------"<<endl;
		cout<<"No more mapping is required....."<<endl;
		cout<<"------------------------------------------------------"<<endl;
		interm_completed.clear();
		should_fill = -1;
		Mapping_done = false;
	}else //First mapping
	{
		int lay ;
		if(mapped_layer == -1)
		{
			lay =0;
		}else
		{
			lay = mapped_layer;
		}
		
		while(1)
		{
			map_Layer = Check_LayerMapping(lay);
			if(map_Layer)
			{
				for(int a=0; a< all_leyer_ID_Group[lay].size(); a++)
				{
					Group_table.push_back(Dy_Group_table[all_leyer_ID_Group[lay][a]]);
				}
				
				active_layers.push_back(lay+1);
				lay = active_layers.back();	
			}else
			{
				/*------------------Debugging-----------------*/
				//cout<< "Group Table size: "<< Group_table.size()<<endl;
				//cout<< "Group table: "<<Group_table[0][0].ID_Group -95<<endl;
				//cout<< "Group table: "<<Group_table[24][0].ID_Group-95<<endl;
				
				/*--------------------------------------------*/
				
				break;
			}
				
		}

		//int debug;
		//Correction of the group numbers for dynamic mapping
		if(PEMappingCount.size() > 0) //Ignoring the first mapping
		{
			int mapped_count = 0;
			for(int a=0; a< PEMappingCount.size(); a++)//the mapped PE count of previous mappings
			{
				mapped_count += PEMappingCount[a];
			}
			for(int b=0; b< Group_table.size(); b++)//adjusting the group ids in the group table
			{
				for(int c =0; c< Group_table[b].size(); c++)
				{
					int temp = Group_table[b][c].ID_Group;
					Group_table[b][c].ID_Group = temp - mapped_count;
				}
			}

			for(int d=0; d< active_layers.size(); d++)
			{
				for(int e=0; e< all_leyer_ID_Group[active_layers[d]-1].size();e++)
				{
					all_leyer_ID_Group[active_layers[d]-1][e] = all_leyer_ID_Group[active_layers[d]-1][e]- mapped_count; 
				}
			}
			//debug= mapped_count;
		}

		if(NoximGlobalParams::faulty_mode != INACTIVE)
		{
			FaultyMode_prep();
		}
		/*----------------Debugging-------------------*/
		//cout<< "Group table: "<<Group_table[0][0].ID_Group<<endl;
		//cout<< "Group table: "<<Group_table[24][0].ID_Group<<endl;
		//cout<<Group_table.size()<<endl;
		//if(active_layers.size() >0)
		//{
		//	cout<<endl<<debug<<endl;
		//	cout<<all_leyer_ID_Group[0][0]<<endl;
		//	cout<<all_leyer_ID_Group[1][0]<<endl;
		//	cout<<all_leyer_ID_Group[2][0]<<endl;
		//	cout<<all_leyer_ID_Group[3][0]<<endl;
		//	cout<<all_leyer_ID_Group[4][0]<<endl;
		//	cout<<all_leyer_ID_Group[5][0]<<endl;
		//	cout<<all_leyer_ID_Group[6][0]<<endl;
		//	cout<<all_leyer_ID_Group.size()<<endl;
		//}
		
		/*--------------------------------------------*/
		//Store the mapped group count for next mapping modification
		PEMappingCount.push_back(Group_table.size());

		HardwarePlan();

		//Deque ready for storing data	
		interm_data_in.clear();
		interm_data_in = interm_data_out;
		/*--------------Debugging----------------*/
		//if(interm_data_in.size() >0)
		//{
		//	cout<<"interm data in: "<< interm_data_in[303]<<endl;
		//}
		/*-------------------------------*/

		interm_data_out.clear();
		if( active_layers.back() != all_leyer_ID_Group.size())
		{
			deque <float> temp_deq;
			lay = active_layers.back();
			//temp_deq.assign(all_leyer_size[lay][1]*all_leyer_size[lay][2], 0.0);
			//for(int b=0; b< all_leyer_size[lay][3]; b++)
			//{
			interm_data_out.assign(all_leyer_size[lay][0], 0.0);
			//}
		}
		should_fill = all_leyer_size[lay][0];
		interm_completed.assign(should_fill, false);
		//cout<< "Should fill size: "<< should_fill<< endl;
		//cout<< "Mapped layer: "<< active_layers.back()<< endl;
		//cout<< active_layers.front()<<endl;
		//cout<< active_layers.back()<< endl;
		//cout<< all_leyer_ID_Group.size()<<endl;
		if(PEMappingCount.size() >1)
		{
			Mapping_done = true;
			cout<<"Mapping done"<<endl;
		}	
	}
	
	
}

void NNModel :: HardwarePlan()
{
	cout<<"Hardware floorplan:"<<endl;
	cout<<"  ";	
	for(int i=0;i<NoximGlobalParams::mesh_dim_x;i++)
	cout<<"-----";	
	cout<<"-"<<endl;
		
	for(int j=0;j<NoximGlobalParams::mesh_dim_y;j++)
	{
		cout<<"  |";
		for(int i=0;i<NoximGlobalParams::mesh_dim_x;i++)
		{

			int loacl_id = j*NoximGlobalParams::mesh_dim_x + i;
			int x;
				for(x=0;x<mapping_table.size();x++)
				{
					if(mapping_table[x]==loacl_id) break;
				}
				if(x<Group_table.size())
				{
					int temp_lay=Group_table[x][0].ID_layer;
					cout<<setw(3)<<temp_lay<<" |";
				}
				else
					cout<<setw(3)<<" "<<" |";

		}
		cout<<endl<<"  ";
		for(int i=0;i<NoximGlobalParams::mesh_dim_x;i++)
		cout<<"-----";	
		cout<<"-"<<endl;
	}
}


bool NNModel:: Check_LayerMapping(int already_mapped)
{
	bool ret_val;
	if(Group_table.size() != 0) 
	{
		if( already_mapped == all_leyer_ID_Group.size() )
		{
			ret_val = false;
		}else
		{
			int size_of_nextLayer = all_leyer_ID_Group[already_mapped].size();

			int temp;
			if(NoximGlobalParams::faulty_mode != INACTIVE)
			{
				int f_nodes = faultyNodes.size();
				temp = (NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y) - Group_table.size()-f_nodes;
				//cout<<"Dy faulty: "<<temp<<endl;
			}else
			{
				temp = (NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y) - Group_table.size();
			}
			
			
			
			if(temp >= size_of_nextLayer)
			{
				ret_val= true;
			}else
			{
				ret_val=  false;
			}
		}
		
		
		/*----------Debugging---------------*/
		//cout<<"Next layer size: "<<  size_of_nextLayer<<endl;
		//cout<<"Return val: "<< ret_val<<endl;
		//cout<<"Mapped layer: "<<already_mapped<<endl;
		//cout<<"Remaning PEs: "<< temp<<endl;
		//cout<<"------------------------------------------------"<<endl;

		/*----------------------------------*/
	}else
	{
		ret_val= true;
	}
	
  return ret_val;

}

void NNModel::FaultyMode()
{
	//Step 1: save the faulty nodes
	faultyNodes.clear();
	cout<<"Faulty Nodes file loading (filename: " << NoximGlobalParams::FaultyNodes_filename << ")..."<< endl;		//** 2018.09.02 edit by Yueh-Chi,Yang **//
	ifstream fin(NoximGlobalParams::FaultyNodes_filename, ios::in);
	char line[128];
	int temp_node;

	while(!fin.eof())
	{
		fin.getline(line, sizeof(line) - 1);		
		if(line[0] != '%')
		{
			if (sscanf
		    (line + 1, "%d", &temp_node) == 1) 
			{
				faultyNodes.push_back(temp_node);
				//cout<< "Faulty Nodes: "<<temp_node<<endl;
			}
		}	
	}

	//Step 2: check if mapping is possible with the given number of faulty nodes
	int FaultyNodes_count = faultyNodes.size();
	int effective_nodes = mapping_table.size() - FaultyNodes_count;
	int group_count;

	if(NoximGlobalParams::mapping_method == STATIC)
	{
		group_count = Group_table.size();
	}else if (NoximGlobalParams::mapping_method == DYNAMIC)
	{
		int neuron_count = all_leyer_size[1][0];   
		for(int i =2; i< all_leyer_size.size(); i++)
		{
			if( neuron_count < all_leyer_size[i][0])
			{
				neuron_count = all_leyer_size[i][0];
			}
		}
		float group_no =NoximGlobalParams::group_neu_num;
		float temp = neuron_count / group_no;
		group_count = ceil(temp) ;

	}
	/*----------------Debugging------------------*/
	/*cout<<endl;
	cout<<"Group count: "<< group_count<<endl;
	cout<<"Effective Nodes: "<<effective_nodes<<endl;
	cout<<"Faulty nodes: "<< FaultyNodes_count<<endl;
	cout<<endl;*/
	/*--------------------------------------------*/
	assert(effective_nodes >= group_count);

	if(NoximGlobalParams::mapping_method == STATIC)
	{
		FaultyMode_prep();	
	}
	

}

void NNModel:: FaultyMode_prep()
{
	//Step 3: Table based routing file creation
	mapping_table.assign(faulty_mapping_table.size(),0);
	bool node_faulty;
	bool pe_free;
		//Step 3.0 : Modify faulty_mapping_table considering the faulty nodes
		//cout<<faulty_mapping_table.size()<<endl;
	deque<bool> pe_status;  //false: Can be mapped; true: can't be mapped
	pe_status.assign(mapping_table.size(), false);

	for(int a=0; a< Group_table.size(); a++)
	{
		int c = a;
		do
		{
			node_faulty = false;
			pe_free = false;
			for(int b=0; b< faultyNodes.size(); b++) //find the first non_faulty_node
			{
				if(faulty_mapping_table[c] != faultyNodes[b])
				{
					node_faulty = false;
				}else
				{
					c++;
					node_faulty = true;
					break;
				}
				
			}
			if(node_faulty == false)//check if the non_faulty_node can be mapped
			{
				if(pe_status[c] == false)
				{
					pe_free = true;	
				}else
				{
					c++;
					pe_free = false;
				}
			}
		} while (node_faulty == true || pe_free == false);
		mapping_table[a] = faulty_mapping_table[c];
		pe_status[c] = true;
	}
	/*-----------------------Debugging--------------------*/
	/*cout<<"PE status: ";
	for(int f=0; f <pe_status.size(); f++)
	{
		cout<< mapping_table[f]<<"--";
	}
	/*----------------------------------------------------*/
		//Step 3.1 : Get to know the destination PEs for each PE
	/*cout<< active_layers.size()<<endl;
	if(active_layers.size() > 1 )
	{
		deque <deque<int>> faulty_pe_id;
		deque<int> temp_faulty;
		for(int s= active_layers.front()-1; s< active_layers.back()-1 ; s++) //Do not consider the first and the last layer
		{	
			temp_faulty.clear();
			for(int t=0; t< all_leyer_ID_Group[s].size();t++)
			{
				temp_faulty.push_back(mapping_table[all_leyer_ID_Group[s][t]]);
			}
			faulty_pe_id.push_back(temp_faulty);
		}
		/*-------------Debugging-------------------*/
		/*cout<<endl;
		int d=1;
		cout<<faulty_pe_id.size()<<endl;
		for(int f=0; f< faulty_pe_id[d].size();f++)
		{
			cout<<faulty_pe_id[d][f]<<"--";
		}
		cout<<endl;
		/*-----------------------------------------*/
		
		
		
			//Step 3.2 : Shortest path
			
			//Step 3.3 : Creation of table-based routing file in the format needed
	//}
	
}


void NNModel:: ShortestPath()
{
	
	// no. of vertices
	int v = mapping_table.size();

	// array of vectors is used to store the graph
	// in the form of an adjacency list
	vector<int> adj[v];

	// Creating graph given
	// add_edge function takes adjacency list, source
	// and destination vertex as argument and forms
	// an edge between them.
	vector<int> pos;

	char fileID_t[10] = "BFS.txt";
	remove(fileID_t);

	char fileID_r[25] = "Table_based_routing.txt";
	remove(fileID_r);

	for( int s=0; s< NoximGlobalParams::mesh_dim_x; s++)
	{
		for(int p=0;p <NoximGlobalParams::mesh_dim_y; p++)
		{
			pos.clear();
			bool faulty = false;
			for(int w=0; w<faultyNodes.size(); w++)
			{
				if(s*NoximGlobalParams::mesh_dim_y+ p == faultyNodes[w])
				{
					faulty = true;
					break;
				}
			}
			if(faulty == false)
			{
				bool fault_status= false;
				if(s+1 < NoximGlobalParams::mesh_dim_x )
				{
					for(int m=0; m< faultyNodes.size(); m++)
					{
						if(faultyNodes[m] == (s+1)*NoximGlobalParams::mesh_dim_x +p)
						{
							fault_status = true;
							break;
						}
					}
					if(fault_status == false)
					{
						pos.push_back((s+1)*NoximGlobalParams::mesh_dim_x +p);
					}
				}
				
				if(s-1 >=0 )
				{
					fault_status= false;
					for(int m=0; m< faultyNodes.size(); m++)
					{
						if(faultyNodes[m] == (s-1)*NoximGlobalParams::mesh_dim_x +p)
						{
							fault_status = true;
							break;
						}
					}
					if(fault_status == false )
					{
						pos.push_back((s-1)*NoximGlobalParams::mesh_dim_x +p);
					}
				}
				

				if(p+1 <NoximGlobalParams::mesh_dim_y)
				{
					fault_status= false;
					for(int m=0; m< faultyNodes.size(); m++)
					{
						if(faultyNodes[m] == s*NoximGlobalParams::mesh_dim_y+(p+1))
						{
							fault_status = true;
							break;
						}
					}
					if(fault_status == false )
					{
						pos.push_back(s*NoximGlobalParams::mesh_dim_y+(p+1));
					}
				}
				

				if(p-1 >=0)
				{
					fault_status= false;
					for(int m=0; m< faultyNodes.size(); m++)
					{
						if(faultyNodes[m] == s*NoximGlobalParams::mesh_dim_y+(p-1))
						{
							fault_status = true;
							break;
						}
					}
					if(fault_status == false)
					{
						pos.push_back(s*NoximGlobalParams::mesh_dim_y+(p-1));
					}
				}
				
			}else
			{
				pos.clear();
				pos.push_back(-1);
			}
			for(int l=0; l< pos.size(); l++)
			{
				add_edge(adj, s*NoximGlobalParams::mesh_dim_y +p, pos[l]);
			}
		}	
	}
	

	//automatic generation of source and destination
	/*for(int r=0; r<f_pe_id.size() -1; r++)
	{
		for(int p=0; p< f_pe_id[r].size(); p++)
		{
			for(int q=0; q<f_pe_id[r+1].size(); q++)
			{
				int source = f_pe_id[r][p], dest = f_pe_id[r+1][q];
				printShortestDistance(adj, source, dest, v);
			}
		}
	}*/
	for(int r=0; r<NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y; r++)
	{
		bool faulty_src=false;
		for(int p=0; p< NoximGlobalParams::mesh_dim_x*NoximGlobalParams::mesh_dim_y; p++)
		{
			bool faulty_dst = false;
			for(int po=0; po< faultyNodes.size(); po++)
			{
				if( faultyNodes[po] == r)
				{
					faulty_src = true;
					break;
				}
			}
			if(faulty_src)
			{
				break;
			}

			for(int pi=0; pi< faultyNodes.size(); pi++)
			{
				if(faultyNodes[pi]== p)
				{
					faulty_dst = true;
					break;
				}
			}
			if(faulty_dst || r==p)
			{
				continue;
			}	

			int source = r, dest = p;
			printShortestDistance(adj, source, dest, v);	
		}
	}
	//cout<<endl;
	/*--------------Debugging-------------------------*/
	/*for(int k=0; k< 100; k++)
	{
		cout<<endl;
		for (auto i = adj[k].begin(); i != adj[k].end(); ++i)
		{
			cout << *i << "--";
		}
	}
	
	/*-------------------------------------------*/

	

	//Creation of Table-based routing file
	//Read from BFS
	ifstream fin("BFS.txt", ios::in);
	char line[128];
	string str;
	int src, dst;
	vector<int> path;

	while(!fin.eof())
	{
		fin.getline(line, sizeof(line) - 1);		
		if (sscanf
		(line + 1, "%d--%d", &src, &dst) == 2) 
		{
			path.clear();
			str = line;
  			string sp = " ";
			int found = str.find(sp);
			char *ptr = line+found;
			//cout<<found<<endl;
			//cout<<ptr<<endl;
			str= ptr;
			int hop =count(str.begin(), str.end(),'>')+1;
			//cout<<"hop: "<<hop<<endl;
			for(int u=0; u<hop; u++)
			{
				int temp;
				sscanf(ptr, "%d",&temp);
				path.push_back(temp);
				ptr= strstr(ptr, ">");
				ptr++;
				//cout<<"Temp: "<<temp<<endl;
			}
			/*----------------Debugging-----------------*/
			/*for(int f=0;f<path.size() ;f++)
			{
				cout<<path[f]<<"--";
			}
			/*------------------------------------------*/
			//Store in the required format
			char fileID_t[25] = "Table_based_routing.txt";
			fstream file_t;
			file_t.open( fileID_t ,ios::out|ios::app);
			for(int s=0; s< path.size()-1; s++)
			{
				file_t<<" "<<path[s]<<" ";
				if(s==0)
				{
					file_t<<path[s]<<"->"<<path[s];
				}else
				{
					file_t<<path[s-1]<<"->"<<path[s];
				}
				file_t<<" "<<dst;
				file_t<<"             ";
				file_t<<path[s]<<"->"<<path[s+1]<<endl;
			}
			//cout<< "Faulty Nodes: "<<temp_node<<endl;
		}
			
	}
	/*NoximGlobalParams::routing_algorithm =ROUTING_TABLE_BASED;
	char fileID_t[25] = "Table_based_routing.txt";
	strcpy(NoximGlobalParams::routing_table_filename, fileID_t);*/
	
}

void NNModel:: add_edge(vector<int> adj[], int src, int dest)
{
    bool req= true;
    for (auto i = adj[src].begin(); i != adj[src].end(); ++i) 
    {
        if(*i == dest)
        {
            req = false;
            break;
        }
    }
    if(req == true)
    {
	    adj[src].push_back(dest);
    }
    
    /*req = true;
    
    for (auto i = adj[dest].begin(); i != adj[dest].end(); ++i) 
    {
        if(*i == src)
        {
            req = false;
            break;
        }
    }
    if( req == true)
    {
	    adj[dest].push_back(src);
    }*/
}

// a modified version of BFS that stores predecessor
// of each vertex in array p
// and its distance from source in array d
bool NNModel:: BFS(vector<int> adj[], int src, int dest, int v,
		int pred[], int dist[])
{
	// a queue to maintain queue of vertices whose
	// adjacency list is to be scanned as per normal
	// DFS algorithm
	deque<int> queue;

	// boolean array visited[] which stores the
	// information whether ith vertex is reached
	// at least once in the Breadth first search
	bool visited[v];

	// initially all vertices are unvisited
	// so v[i] for all i is false
	// and as no path is yet constructed
	// dist[i] for all i set to infinity
	for (int i = 0; i < v; i++) {
		visited[i] = false;
		dist[i] = INT_MAX;
		pred[i] = -1;
	}

	// now source is first to be visited and
	// distance from source to itself should be 0
	visited[src] = true;
	dist[src] = 0;
	queue.push_back(src);

	// standard BFS algorithm
	while (!queue.empty()) {
		int u = queue.front();
		queue.pop_front();
		for (int i = 0; i < adj[u].size(); i++) {
			if (visited[adj[u][i]] == false) {
				visited[adj[u][i]] = true;
				dist[adj[u][i]] = dist[u] + 1;
				pred[adj[u][i]] = u;
				queue.push_back(adj[u][i]);

				// We stop BFS when we find
				// destination.
				if (adj[u][i] == dest)
					return true;
			}
		}
	}

	return false;
}

// utility function to print the shortest distance
// between source vertex and destination vertex
void NNModel:: printShortestDistance(vector<int> adj[], int s,
						int dest, int v)
{
	// predecessor[i] array stores predecessor of
	// i and distance array stores distance of i
	// from s
	int pred[v], dist[v];

	if (BFS(adj, s, dest, v, pred, dist) == false) {
		cout << "Given source and destination"
			<< " are not connected";
		cout<< s<<" "<<dest<<endl;
		return;
	}

	// vector path stores the shortest path
	vector<int> path;
	int crawl = dest;
	path.push_back(crawl);
	while (pred[crawl] != -1) {
		path.push_back(pred[crawl]);
		crawl = pred[crawl];
	}

	// distance from source is in distance array
	/*cout << "Shortest path length is : "
		<< dist[dest];
    
    // printing path from source to destination
	cout << "\nPath is::\n";
	for (int i = path.size() - 1; i >= 0; i--)
		cout << path[i] << " ";*/

	char fileID_t[10] = "BFS.txt";
	fstream file_t;
	
	file_t.open( fileID_t ,ios::out|ios::app);
	file_t<<" "<< s<< "--" << dest <<"         ";
	for (int i = path.size() - 1; i >= 0; i--)
	{
		file_t << path[i];
		if(i-1 != -1 )
		{
			file_t <<"->";
		}
	}
	file_t<< endl;
	 
}