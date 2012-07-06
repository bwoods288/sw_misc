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

#include <stdio.h>
#include "data_structures.h"
#include "get_settings.h"
#include "main.h"
#include "cache_functions.h"
         
int main (int argc, char *argv[]) { 
	
	int		rv; 		/* return value for functionss */
	FILE		*file_in;	/* file for mem RW/addr input */
	settings_t	settings;	/* settings structure */
	char		line_buffer[64];/* buffer for reading in lines */
	mem_rw_t	parse_data;		
	save_data_t	save_data;

	init_save(&save_data);	/* inits the stats  */

	init_settings(&settings);	/* inits the settings struct */

	/* parses the input arguements for the cache set up and run info */
	if ((rv = get_settings(&settings, argc, argv)) != RTN_SUCCESS) { 
		if (rv == RTN_PARSE_ERR){
			fprintf(stderr, "Error parsing input arguements"
			    "exiting\n");
			return rv;
		} 
		else if (rv ==  RTN_NO_SET) { 
			fprintf(stderr, "Not all of the settings were"
			     "present or set correctly, exiting\n");
			return rv;
		}	
		else { 
			fprintf(stderr, "Unknown error, exiting\n");
			return rv;
		}
	}


	/* sets up the cache array in memory */
	if (cache_setup(&settings) == RTN_FAIL) { 
		fprintf(stderr, "Failed to malloc memory, exiting\n");
	}	

	/* opens the input file */
	if ((file_in = fopen(settings.file_path, "r")) == NULL) {
		fprintf(stderr, "Can not open input file, exiting\n");
		return RTN_FILE_ERR;
	}

	
	/* reads one line from the  input */
	if(fgets(line_buffer, 64, file_in) == NULL) { 
		fprintf(stderr, "The input file is empty, exiting\n");
		return 1;
	}
	parse_data.time = 0ull; 

	do { 
		if (sscanf(line_buffer, "%d %d %u", &parse_data.flags, 
		    &parse_data.data_len, &parse_data.addr) != 3) {
			fprintf(stderr, "%llu: could not be parsed"
			    ", skipping\n", parse_data.time + 1ull);
			continue;		/* skip if the data isn't
						 * read correctly */
		}
		if (parse_data.flags == 0) { 		/* read */
			read_cache(&settings, &parse_data, &save_data);
			parse_data.time++;
		} 
		else if (parse_data.flags == 1) { 	/* write */
			;
		}
		else {					/* error */
			fprintf(stderr, "%llu: neither read nor write"
			    ", skipping\n", parse_data.time + 1ull);
		}
		fgets(line_buffer, 64, file_in);
		
	}	
	while  (!feof(file_in));

	print_save(&save_data);
	
	return 0;
}
