/*
 * Copyright (c) 2012, Brian Woods
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * The name of the author(s) may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>		/* */
#include <string.h>		/* */
#include "data_structures.h"	/* defines and structs */
#include "get_settings.h"	/* for the defines */

/* inits the settings struct */
void init_settings(settings_t *settings) { 
	settings->data_size 	= 0;
	settings->associativity	= 0;
	settings->block_size	= 0;
	settings->write_policy	= 0;
	settings->vic_block_size= 0;
	settings->map_secs	= 0;
	settings->ef_data_size	= 0;
	settings->verbose	= 0;
	settings->step		= 0;
	settings->file_path	= NULL;

	return; 
} /* end init_settings */	


/* inits the save data */
void init_save(save_data_t *sd) { 
	sd->l1_rd_h	= 0;
	sd->l1_rd_m	= 0;
	sd->vic_rd_h	= 0;
	sd->vic_rd_m	= 0;
	sd->l1_wr_h	= 0;
	sd->l1_wr_m	= 0;
	sd->vic_wr_h	= 0;
	sd->vic_wr_m	= 0;
	sd->mem_wr	= 0;
	sd->mem_rd	= 0;
	
	return;
} 

/* parses through the arguments and extracts the settings */
int get_settings(settings_t *settings, int argc, char *argv[]) { 
	int set	= 0;		/* count of settings written */	
	int i	= 1;		/* which input argument we're on */
	while (i < argc) { 
		/* parses for cache data size */
		if (strncmp(argv[i], CACHE_SIZE_OP, 3) == 0){
			if (sscanf(argv[i+1], "%d", &settings->data_size)
			    != 1) { 
				fprintf(stderr, "Couldn't parse cache"
				    " data size\n");
				return RTN_PARSE_ERR; 
			}
			i += 2;
			set++;
		} 
		/* parses for block size */
		else if (strncmp(argv[i], BLOCK_SIZE_OP, 3) == 0){
			if (sscanf(argv[i+1], "%d", &settings->block_size)
			    != 1) { 
				fprintf(stderr, "Couldn't parse block"
				    " size\n");
				return RTN_PARSE_ERR; 
			}
			i += 2;
			set++;
		} 
		/* parses for victim size */
		else if (strncmp(argv[i], VICTIM_SIZE_OP, 3) == 0){
			if (sscanf(argv[i+1], "%d", 
			    &settings->vic_block_size) != 1) { 
				fprintf(stderr, "Couldn't parse victim"
				    " block size\n");
				return RTN_PARSE_ERR; 
			}
			i += 2;
			set++;
		} 
		/* parses for associativity */
		else if (strncmp(argv[i], ASSOC_OP, 3) == 0){
			if (sscanf(argv[i+1], "%d", 
			    &settings->associativity) != 1) { 
				fprintf(stderr, "Couldn't parse"
				    " associativity\n");
				return RTN_PARSE_ERR; 
			}
			i += 2;
			set++;
		}	
		/* parses for write polity */
		else if (strncmp(argv[i], WRITE_POL_OP, 3) == 0){
			if (sscanf(argv[i+1], "%d", 
			    &settings->write_policy) != 1) { 
				fprintf(stderr, "Couldn't parse write"
				    " policy\n");
				return RTN_PARSE_ERR; 
			}
			i += 2;
			set++;
		}
		/* parses for input file */
		else if (strncmp(argv[i], FILE_OP, 3) == 0){
			settings->file_path = argv[i+1];
			i += 2;
			set++; 
		}
		/* parses for verbosness */
		else if (strncmp(argv[i], VERBOSE_OP, 2) == 0){
			settings->verbose = 1;
			i++;
		}	
		/* parses for stepping */
		else if (strncmp(argv[i], STEP_OP, 3) == 0){
			settings->step = 1;
			i++;
		}	
		else {
			i++;
		}

	} /* RTN while(i< argc) */

	/* RTNs non-zero if all the needed settings aren't read */	
	if (set < 6) { 	
		return RTN_NO_SET;	
	}
	else {
		return RTN_SUCCESS; 
	}

} /* end get_settings */

void print_settings(settings_t *settings) { 
	fprintf(stdout, "cache size = %d\n",settings->data_size);
	fprintf(stdout, "associativity= %d\n",settings->associativity);
	fprintf(stdout, "block size = %d\n",settings->block_size);
	fprintf(stdout, "write policy = %d\n",settings->write_policy);
	fprintf(stdout, "vic block size = %d\n",settings->vic_block_size);
	fprintf(stdout, "dir mem secs = %d\n",settings->map_secs);
	fprintf(stdout, "effective mem size = %d\n",
	    settings->ef_data_size);
	fprintf(stdout, "verbose = %d\n",settings->verbose);
	fprintf(stdout, "step = %d\n",settings->step);
	fprintf(stdout, "file in = %s\n",settings->file_path);

	return;
}
void print_save(save_data_t *sd) { 
	fprintf(stdout, "l1 read hits = %llu\n",sd->l1_rd_h);
	fprintf(stdout, "l1 read misses = %llu\n",sd->l1_rd_m);
	fprintf(stdout, "vic read hits = %llu\n",sd->vic_rd_h);
	fprintf(stdout, "vic reads misses = %llu\n",sd->vic_rd_m);
	fprintf(stdout, "l1 write hits = %llu\n",sd->l1_wr_h);
	fprintf(stdout, "l1 write missess = %llu\n",sd->l1_wr_m);
	fprintf(stdout, "vic write hits = %llu\n",sd->vic_wr_h);
	fprintf(stdout, "vic write misses = %llu\n",sd->vic_wr_m);
	fprintf(stdout, "l1 block writes = %llu\n",sd->mem_wr);
	fprintf(stdout, "l1 block reads = %llu\n",sd->mem_rd);
	
	return;
}
