/*--------------------------------------------------------------*/
/* 								*/
/*		compute_potential_N_uptake_Waring					*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_potential_N_uptake_Waring -				*/
/*		computes potential N uptake from soil		*/
/*		for this strata without mineralization		*/
/*		limitation					*/
/*								*/
/*	SYNOPSIS						*/
/*	int compute_potential_N_uptake_Waring(					*/
/*                          struct epconst_struct,              */
/*			    struct epvar_struct *epv,		*/
/*                          struct cstate_struct *,             */
/*                          struct nstate_struct *,             */
/*                          struct cdayflux_struct *)           */
/*								*/
/*								*/
/*	returns int:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*              modified from Peter Thornton (1998)             */
/*                      dynamic - 1d-bgc ver4.0                 */
/*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"
double compute_potential_N_uptake_Waring(
								  struct	epconst_struct epc,
								  struct	epvar_struct *epv,
								  struct cstate_struct *cs,
								  struct nstate_struct *ns,
								  struct cdayflux_struct *cdf,
                                  struct canopy_strata_object     *stratum)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double day_gpp;     /* daily gross production */
	double day_mresp;   /* daily total maintenance respiration */
	double froot, fleaf, fwood;          /* RATIO   new fine root C : new leaf C     */
	double f2;          /* RATIO   fraction to leaf and fraction to root*/
	double f4;          /* RATIO   new live wood C : new wood C     */
	double f3;          /* RATIO   new leaf C : new wood C     */
	double g1;          /* RATIO   C respired for growth : C grown  */
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	double cnmax;       /* RATIO   max of root and leaf C:N      */
	double c_allometry, n_allometry, mean_cn, transfer;
	double plant_ndemand;
	double k2, c; /* working variables */
	/*---------------------------------------------------------------
	Assess the carbon availability on the basis of this day's
	gross production and maintenance respiration costs
	----------------------------------------------------------------*/
	cs->availc = cdf->psn_to_cpool-cdf->total_mr;
	/* no allocation when the daily C balance is negative */
	if (cs->availc < 0.0) cs->availc = 0.0;
	/* test for cpool deficit */
	if (cs->cpool < 0.0){
	/*--------------------------------------------------------------
	running a deficit in cpool, so the first priority
	is to let today's available C accumulate in cpool.  The actual
	accumulation in the cpool is resolved in day_carbon_state().
		--------------------------------------------------------------*/
		/*------------------------------------------------
		cpool deficit is less than the available
		carbon for the day, so aleviate cpool deficit
		and use the rest of the available carbon for
		new growth and storage.
			-----------------------------------------------*/
			transfer = min(cs->availc, -cs->cpool);
			cs->availc -= transfer;
			cs->cpool += transfer;
	} /* end if negative cpool */
    if(cs->availc <0) printf("compute_potential_N_uptake_Waring: (%e,%e,%e)\n",
                             cdf->psn_to_cpool,
                             cdf->total_mr,
                             cs->availc);
    
	/* assign local values for the allocation control parameters */
	f2 = epc.alloc_crootc_stemc;
	f3 = epc.alloc_stemc_leafc;
	f4 = epc.alloc_livewoodc_woodc;
	g1 = epc.gr_perc;
	cnl = epc.leaf_cn;
	cnfr = epc.froot_cn;
	cnlw = epc.livewood_cn;
	cndw = epc.deadwood_cn;
	/*--------------------------------------------------------------- */
	/*	given the available C, use Waring allometric relationships to */
	/*	estimate N requirements -					*/ 
	/*	constants a and b are taken from Landsberg and Waring, 1997 */
	/*--------------------------------------------------------------*/
    // problem, need to count for "excess_lai" Jan 11th. 2019
    double MAX_LAI = stratum[0].local_max_lai; //epc.max_lai;
    
	if (((cdf->potential_psn_to_cpool) > ZERO) && (cdf->psn_to_cpool > ZERO)) {
        
        c = max(cdf->potential_psn_to_cpool, cdf->psn_to_cpool);
        // develop the root > leaf > wood
        if ((cs->availc > ZERO) && (c > 0)){
            froot = epc.waring_pa / (1.0 + epc.waring_pb * cdf->psn_to_cpool / c);
            // epc.waring_pa = 0.8 default
            // epc.waring_pb = 2.5 default
            if (epc.veg_type == TREE){
                fleaf = (1.0 - froot) / (1 + (1+f2)*f3);
                fwood = 1.0 - froot - fleaf;
            } else{
                fleaf = 1.0 - froot;
                fwood = 0.0;
            }
        } else {
            froot = 0.0;
            fleaf = 0.0;
            fwood = 0.0;
        }



        if ((fleaf + froot) > ZERO) {
            if (epc.veg_type == TREE){
                mean_cn = 1.0 / (fleaf / cnl + froot / cnfr + f4 * fwood / cnlw + fwood * (1.0-f4) / cndw);
            }else{
                mean_cn = 1.0 / (fleaf / cnl + froot / cnfr);
            }
        } else mean_cn = 1.0;

        if (mean_cn > ZERO)
            plant_ndemand = cs->availc / (1.0+epc.gr_perc) / mean_cn;// - max(ns->retransn,0.0);
        else
            plant_ndemand = 0.0;

	} else {
		plant_ndemand = 0.0;
		fleaf = 0.0;
		froot = 0.0;
		fwood = 0.0;
	}

	cdf->fleaf = fleaf;
	cdf->fwood = fwood;
	cdf->froot = froot;

//    printf("plant uptake (waring): %f,%f,%f,%f,%f\n",cdf->psn_to_cpool,cdf->total_mr,cs->cpool,cs->availc,plant_ndemand);
	return(plant_ndemand);
} /* 	end compute_potential_N_uptake_Waring */
