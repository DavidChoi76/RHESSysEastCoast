/*--------------------------------------------------------------*/
/* 																*/
/*					construct_hillslope							*/
/*																*/
/*	construct_hillslope.c - creates a hillslope object			*/
/*																*/
/*	NAME														*/
/*	construct_hillslope.c - creates a hillslope object			*/
/*																*/
/*	SYNOPSIS													*/
/* 	void construct_hillslope(									*/
/*			struct	command_line_object	*command_line,			*/
/*			FILE	*world_file,								*/
/*			int		num_world_base_stations,					*/
/*			struct	base_station_object	**world_base_stations,	*/
/*			struct	hillslope_object	**hillslope_list,		*/
/*			struct	default_object		*defaults)				*/
/*																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Reads the world file to get data for 1 hillslope.			*/
/*																*/
/*	Points to base_stations assigned to the hillslope.			*/
/*																*/
/*	Invokes construction of zones in the hillslope.				*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	March 4, 1997 C. Tague										*/
/*			- removed m parameter to hillslope defaults to		*/
/*			 be consistent with patch level soil parameteriza	*/
/*	March 9, 1997 C. Tague										*/
/*			- added baseflow as an input state					*/
/*	Sept 21, 1998 C. Tague						*/
/*		removed input of lambda - calculated from patch lambdas */
/*		in topmodel					 	*/

/*																*/
/*																*/
/*	Original code, January 16, 1996.							*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "rhessys.h"
#include "params.h"

struct hillslope_object *construct_hillslope(
											 struct	command_line_object	*command_line,
											 FILE	*world_file,
											 int	*num_world_base_stations,
											 struct	base_station_object	**world_base_stations,
											 struct	default_object	*defaults,
											 struct base_station_ncheader_object *base_station_ncheader,
											 struct world_object *world)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	struct base_station_object *assign_base_station(
		int	,
		int	,
		struct base_station_object **);
	
	struct zone_object *construct_zone(
		struct command_line_object *,
		FILE	*,
		int		*num_world_base_stations,
		struct	base_station_object	**world_base_stations,
		struct	default_object *,
		struct base_station_ncheader_object *,
	    struct world_object *);
	
	void	*alloc(	size_t,
		char	*,
		char	*);
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	int		i,j;
	int		base_stationID;
	int		default_object_ID;
	char		record[MAXSTR];
	struct	hillslope_object *hillslope;
	param        *paramPtr=NULL;
    int        paramCnt=0;
	/*--------------------------------------------------------------*/
	/*	Allocate a hillslope object.								*/
	/*--------------------------------------------------------------*/
	hillslope = (struct hillslope_object *) alloc( 1 *
		sizeof( struct hillslope_object ),"hillslope",
		"construct_hillsope" );
	
	/*--------------------------------------------------------------*/
	/*	Create the grow extension to the hillslope object if 		*/
	/*	needed.														*/
	/*--------------------------------------------------------------*/
	if ( command_line[0].grow_flag  == 1){
		/*--------------------------------------------------------------*/
		/*		Allocate the grow object.								*/
		/*--------------------------------------------------------------*/
		hillslope[0].grow = (struct grow_hillslope_object *)
			alloc(1 * sizeof( struct grow_hillslope_object ),
			"grow","construct_hillslope");
	} /*end if*/
	/*--------------------------------------------------------------*/
	/*	Read in the hillslope record from the world file.			*/
	/*--------------------------------------------------------------*/
//	fscanf(world_file,"%d",&(hillslope[0].ID));
//	read_record(world_file, record);
//	fscanf(world_file,"%lf",&(hillslope[0].x));
//	read_record(world_file, record);
//	fscanf(world_file,"%lf",&(hillslope[0].y));
//	read_record(world_file, record);
//	fscanf(world_file,"%lf",&(hillslope[0].z));
//	read_record(world_file, record);
//	fscanf(world_file,"%d",&(default_object_ID));
//	read_record(world_file, record);
//	fscanf(world_file,"%lf",&(hillslope[0].gw.storage));
//	read_record(world_file, record);
//	fscanf(world_file,"%lf",&(hillslope[0].gw.NO3));
//	read_record(world_file, record);
    
    paramPtr=readtag_worldfile(&paramCnt,world_file,"Hillslope");
    hillslope[0].ID = getIntWorldfile(&paramCnt,&paramPtr,"hillslope_ID","%d",-9999,0);
    hillslope[0].x = getDoubleWorldfile(&paramCnt,&paramPtr,"x","%lf",0.0,1);
    hillslope[0].y = getDoubleWorldfile(&paramCnt,&paramPtr,"y","%lf",0.0,1);
    hillslope[0].z = getDoubleWorldfile(&paramCnt,&paramPtr,"z","%lf",-9999,0);
    default_object_ID = getIntWorldfile(&paramCnt,&paramPtr,"hill_parm_ID","%d",1,1);
    hillslope[0].gw.storage = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.storage","%lf",0.0,1);
    hillslope[0].gw.NO3 = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.NO3","%lf",0.0,1);
    hillslope[0].gw.NH4 = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.NH4","%lf",0.0,1);
    hillslope[0].gw.DOC = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.DOC","%lf",0.0,1);
    hillslope[0].gw.DON = getDoubleWorldfile(&paramCnt,&paramPtr,"gw.DON","%lf",0.0,1);
    hillslope[0].num_base_stations = getIntWorldfile(&paramCnt,&paramPtr,"hillslope_n_basestations","%d",0,1);

    hillslope[0].streamflow_NO3 = 0.0;
    hillslope[0].streamflow_NH4 = 0.0;
    hillslope[0].streamflow_DON = 0.0;
    hillslope[0].streamflow_DOC = 0.0;
    ///---------------------------------------------------  need a flag
//    if( command_line[0].readinWFdoc_flag  == 1 ){
//        fscanf(world_file,"%lf",&(hillslope[0].gw.DOC));
//        read_record(world_file, record);
//        fscanf(world_file,"%lf",&(hillslope[0].gw.DON));
//        read_record(world_file, record);
//    }else{
//        hillslope[0].gw.DOC = 0.0018;  //kgC/m2 learned from long run trend
//        hillslope[0].gw.DON = hillslope[0].gw.DOC * 0.04651163; //CN 21.5
//    }// end of if
    ///---------------------------------------------------
    
//	fscanf(world_file,"%d",&(hillslope[0].num_base_stations));
//	read_record(world_file, record);
//
//	hillslope[0].streamflow_NO3 = 0.0;
//	hillslope[0].streamflow_NH4 = 0.0;
	/*--------------------------------------------------------------*/
	/*  Assign  defaults for this hillslope                             */
	/*--------------------------------------------------------------*/
	hillslope[0].defaults = (struct hillslope_default **)
		alloc( sizeof(struct hillslope_default *),"defaults",
		"construct_hillslopes" );
	
	i = 0;
	while (defaults[0].hillslope[i].ID != default_object_ID) {
		i++;
		/*--------------------------------------------------------------*/
		/*  Report an error if no match was found.  Otherwise assign    */
		/*  the default to point to this hillslope.                         */
		/*--------------------------------------------------------------*/
		if ( i>= defaults[0].num_hillslope_default_files ){
			fprintf(stderr,
				"\nFATAL ERROR: in construct_hillslope,hillslope default ID %d not found.\n",
				default_object_ID);
			exit(EXIT_FAILURE);
		}
	} /* end-while */
	hillslope[0].defaults[0] = &defaults[0].hillslope[i];
    if(hillslope[0].gw.storage<=0){
        hillslope[0].gw.storage = 0.95 * hillslope[0].defaults[0][0].gw_storage_capacity;
    }
        
	/*--------------------------------------------------------------*/
	/*	Allocate a list of base stations for this hillslope.		*/
	/*--------------------------------------------------------------*/
    hillslope[0].base_stations = NULL;
//	hillslope[0].base_stations = (struct base_station_object **) alloc(hillslope[0].num_base_stations * sizeof(struct base_station_object *),"base_stations", "construct_hillslopes" );
//	/*--------------------------------------------------------------*/
//	/*	Read each base_station ID and then point to that base_statio*/
//	/*--------------------------------------------------------------*/
//	for (i=0 ; i<hillslope[0].num_base_stations; i++){
//		fscanf(world_file,"%d",&(base_stationID));
//		read_record(world_file, record);
//		/*--------------------------------------------------------------*/
//		/*		Point to the appropriate base station in the base       */
//		/*		station list for this world.							*/
//		/*																*/
//		/*--------------------------------------------------------------*/
//		hillslope[0].base_stations[i] = assign_base_station(
//			base_stationID,
//			*num_world_base_stations,
//			world_base_stations);
//	} /*end for*/
	
	/*--------------------------------------------------------------*/
	/*	Read in number of zones in this hillslope.					*/
	/*--------------------------------------------------------------*/
//	fscanf(world_file,"%d",&(hillslope[0].num_zones));
//	read_record(world_file, record);
	hillslope[0].num_zones = getIntWorldfile(&paramCnt,&paramPtr,"NUM_of_","%d",0,0);
	/*--------------------------------------------------------------*/
	/*	Allocate list of pointers to zone objects .					*/
	/*--------------------------------------------------------------*/
	hillslope[0].zones = ( struct zone_object ** )
		alloc( hillslope[0].num_zones * sizeof( struct zone_object *),
		"zones","construct_hillslopes");
	
	
	
	/*--------------------------------------------------------------*/
	/*	Construct the zones in this hillslope.						*/
	/*	and calculate hillslope area								*/
	/*--------------------------------------------------------------*/

	hillslope[0].area = 0.0;
	hillslope[0].riparian_area = 0.0;
	hillslope[0].slope = 0.0;
	hillslope[0].aggdefs.soil_water_cap = 0.0;
	hillslope[0].aggdefs.m = 0.0;
	hillslope[0].aggdefs.psi_air_entry = 0.0;
	hillslope[0].aggdefs.porosity_0 = 0.0;
	hillslope[0].aggdefs.porosity_decay = 0.0;
	hillslope[0].aggdefs.N_decay_rate = 0.0;
	hillslope[0].aggdefs.DOM_decay_rate = 0.0;
	hillslope[0].aggdefs.active_zone_z = 0.0;
	hillslope[0].aggdefs.Ksat_0 = 0.0;
	hillslope[0].aggdefs.soil_depth = 0.0;
	hillslope[0].aggdefs.NO3_adsorption_rate = 0.0;
	hillslope[0].aggdefs.NH4_adsorption_rate = 0.0;
	hillslope[0].aggdefs.DON_adsorption_rate = 0.0;
	hillslope[0].aggdefs.DOC_adsorption_rate = 0.0;
	
	for ( i=0 ; i<hillslope[0].num_zones ; i++ ){
		hillslope[0].zones[i] = construct_zone( command_line,
			world_file,
			num_world_base_stations,
			world_base_stations, defaults,
			base_station_ncheader, world);
		for	 (j =0; j < hillslope[0].zones[i][0].num_patches ; j++) {
			hillslope[0].area += hillslope[0].zones[i][0].patches[j][0].area;
            
			if (hillslope[0].zones[i][0].patches[j][0].drainage_type>0 && hillslope[0].zones[i][0].patches[j][0].drainage_type % actionRIPARIAN==0)// <=
				hillslope[0].riparian_area += hillslope[0].zones[i][0].patches[j][0].area;
            
			hillslope[0].slope += hillslope[0].zones[i][0].patches[j][0].slope *
					hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.soil_water_cap +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].soil_water_cap *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.Ksat_0 +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].Ksat_0 *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.m +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].m *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.psi_air_entry +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].psi_air_entry*
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.porosity_0 +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].porosity_0 *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.porosity_decay +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].porosity_decay *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.DOM_decay_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].DOMdecayRate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.N_decay_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NO3decayRate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.active_zone_z +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].active_zone_z *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.soil_depth +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].soil_depth *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.NO3_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NO3_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.NH4_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].NH4_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.DON_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].DON_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;
			hillslope[0].aggdefs.DOC_adsorption_rate +=
				hillslope[0].zones[i][0].patches[j][0].soil_defaults[0][0].DOC_adsorption_rate *
				hillslope[0].zones[i][0].patches[j][0].area;

		}//j
	}//i
	hillslope[0].slope /= hillslope[0].area;
	hillslope[0].aggdefs.psi_air_entry /= hillslope[0].area;
	hillslope[0].aggdefs.m /= hillslope[0].area;
	hillslope[0].aggdefs.Ksat_0 /= hillslope[0].area;
	hillslope[0].aggdefs.porosity_0 /= hillslope[0].area;
	hillslope[0].aggdefs.porosity_decay /= hillslope[0].area;
	hillslope[0].aggdefs.N_decay_rate /= hillslope[0].area;
	hillslope[0].aggdefs.DOM_decay_rate /= hillslope[0].area;
	hillslope[0].aggdefs.active_zone_z /= hillslope[0].area;
	hillslope[0].aggdefs.soil_depth /= hillslope[0].area;
	hillslope[0].aggdefs.soil_water_cap /= hillslope[0].area;
	hillslope[0].aggdefs.NO3_adsorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.NH4_adsorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.DON_adsorption_rate /= hillslope[0].area;
	hillslope[0].aggdefs.DOC_adsorption_rate /= hillslope[0].area;


	

	return(hillslope);
} /*end construct_hillslope.c*/
