#ifndef __TEMPERATURE_GRID_H_
#define __TEMPERATURE_GRID_H_

/* grid model differs from the block model in its use of
 * a mesh of cells whose resolution is configurable. unlike 
 * the block model, it can also model a stacked 3-D chip,
 * with each layer having a different floorplan. information
 * about the floorplan and the properties of each layer is
 * input in the form of a layer configuration file. 
 */
#include "temperature.h"

/* layer configuration file constants */
#define LCF_NPARAMS			7	/* no. of parameters per layer	*/
#define LCF_SNO				0	/* serial number	*/
#define LCF_LATERAL			1	/* has lateral heat flow?	*/
#define LCF_POWER			2	/* dissipates power?	*/
#define LCF_SP				3	/* specific heat capacity	*/
#define LCF_RHO				4	/* resistivity	*/
#define LCF_THICK			5	/* thickness	*/
#define LCF_FLP				6	/* floorplan file	*/

/* vector types - power / temperature	*/
#define V_POWER				0
#define V_TEMP				1

/* default no. of chip layers (excluding spreader 
 * and sink). used when LCF file is not specified
 */
#define DEFAULT_CHIP_LAYERS	2
#define LAYER_SI			0
#define LAYER_INT			1

/* layers of secondary path with same area as die */
#define SEC_CHIP_LAYERS	2
#define LAYER_METAL 0
#define LAYER_C4	1

/* default no. of package layers	*/
#define DEFAULT_PACK_LAYERS	2
#define LAYER_SP			0
#define LAYER_SINK			1

/* additional package layers from secondary path */
#define SEC_PACK_LAYERS	3
#define LAYER_SUB	0
#define LAYER_SOLDER	1
#define LAYER_PCB	2

/* block list: block to grid mapping data structure.
 * list of blocks mapped to a grid cell	
 */
typedef struct blist_t_st
{
	/* index of the mapped block	*/
	int idx;
	/* ratio of this block's area within the grid cell 
	 * to the total area of the grid cell
	 */
	double occupancy;
	/* next block mapped to the same cell	*/
	struct blist_t_st *next;
}blist_t;

/* grid list: grid to block mapping data structure.
 * start and end indices of grid cells in a block
 * (both in the x and y directions)
 */
typedef struct glist_t_st
{
	/* start index in the y direction	*/
	int i1;
	/* end index in the y direction + 1	*/
	int i2;
	/* start index in the x direction	*/
	int j1;
	/* end index in the x direction + 1	*/
	int j2;
} glist_t;

/* one layer of the grid model. a 3-D chip is a stacked
 * set of layers
 */
typedef struct layer_t_st 
{
	/* floorplan */
	flp_t *flp;

	/* configuration parameters	*/
	int no;				/* serial number	*/
	int has_lateral;	/* model lateral spreading of heat?	*/
	int has_power;		/* dissipates power?	*/
	double k;			/* 1/resistivity	*/
	double k1;	/* thermal conductivity of the other material in some layers, such as C4/underfill */
	double thickness;
	double sp;			/* specific heat capacity	*/
	double sp1; /* specific heat of the other material in some layers, such as C4/underfill */

	/* extracted information	*/
	double rx, ry, rz;	/* x, y and z resistors	*/
	double rx1, ry1, rz1; /* resistors of the other material in some layers, e.g. c4/underfill*/
	double c, c1;			/* capacitance	*/

	/* block-grid map - 2-d array of block lists	*/
	blist_t ***b2gmap;
	/* grid-block map - a 1-d array of grid lists	*/
	glist_t *g2bmap;
}layer_t;

/* grid model's internal vector datatype	*/
typedef struct grid_model_vector_t_st
{
	/* 3-d grid of nodes	*/
	double ***cuboid;
	/* extra spreader and sink  nodes	*/
	double *extra;
}grid_model_vector_t;

/* grid thermal model	*/
typedef struct grid_model_t_st
{
	/* configuration	*/
	thermal_config_t config;

	/* layer information	*/
	layer_t *layers;
	int n_layers;

	/* grid resolution	*/
	int rows;
	int cols;
	/* dimensions	*/
	double width;
	double height;

	/* package parameters	*/
	package_RC_t pack;

	/* sum total of the functional blocks of all floorplans	*/
	int total_n_blocks;
	/* grid-to-block mapping mode	*/
	int map_mode;

	/* flags	*/
	int r_ready;	/* are the R's initialized?	*/
	int c_ready;	/* are the C's initialized?	*/
	int has_lcf;	/* LCF file specified?		*/

	/* internal state - most recently computed 
	 * steady state temperatures
	 */
	grid_model_vector_t *last_steady;
	/* internal state - most recently computed 
	 * transient temperatures
	 */
	/* grid cell temperatures	*/
	grid_model_vector_t *last_trans;
	/* block temperatures	*/
	double *last_temp;

	/* to allow for resizing	*/
	int base_n_units;
}grid_model_t;

/* constructor/destructor	*/
grid_model_t *alloc_grid_model(thermal_config_t *config, flp_t *flp_default);
void delete_grid_model(grid_model_t *model);

/* initialization	*/
void populate_R_model_grid(grid_model_t *model, flp_t *flp);
void populate_C_model_grid(grid_model_t *model, flp_t *flp);

/* hotspot main interfaces - temperature.c	*/
void steady_state_temp_grid(grid_model_t *model, double *power, double *temp);
void compute_temp_grid(grid_model_t *model, double *power, double *temp, double time_elapsed);

/* differs from 'dvector()' in that memory for internal nodes is also allocated	*/
double *hotspot_vector_grid(grid_model_t *model);
/* copy 'src' to 'dst' except for a window of 'size'
 * elements starting at 'at'. useful in floorplan
 * compaction
 */
void trim_hotspot_vector_grid(grid_model_t *model, double *dst, double *src, 
						 	  int at, int size);
/* update the model's node count	*/						 
void resize_thermal_model_grid(grid_model_t *model, int n_units);
void set_temp_grid (grid_model_t *model, double *temp, double val);
void dump_top_layer_temp_grid(grid_model_t *model, char *file, grid_model_vector_t *temp);
void dump_steady_temp_grid (grid_model_t *model, char *file);
void dump_temp_grid (grid_model_t *model, double *temp, char *file);
void copy_temp_grid (grid_model_t *model, double *dst, double *src);
void read_temp_grid (grid_model_t *model, double *temp, char *file, int clip);
void dump_power_grid(grid_model_t *model, double *power, char *file);
void read_power_grid (grid_model_t *model, double *power, char *file);
double find_max_temp_grid(grid_model_t *model, double *temp);
double find_avg_temp_grid(grid_model_t *model, double *temp);
double calc_sink_temp_grid(grid_model_t *model, double *temp, thermal_config_t *config);

/* grid_model_vector routines	*/
/* constructor	*/
grid_model_vector_t *new_grid_model_vector(grid_model_t *model);
/* destructor	*/
void free_grid_model_vector(grid_model_vector_t *v);
/* translate power/temperature between block and grid vectors	*/
void xlate_vector_b2g(grid_model_t *model, double *b, grid_model_vector_t *g, int type);
/* translate temperature between grid and block vectors	*/
void xlate_temp_g2b(grid_model_t *model, double *b, grid_model_vector_t *g);
/* debug print	*/
void debug_print_grid(grid_model_t *model);

#endif
