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
#include <stdlib.h>		/* malloc() */
#include <string.h>		/* memset */
#include "data_structures.h"
#include "cache_functions.h"

void read_block (settings_t *settings, item_data_t *read_data,
    save_data_t *save_data);

int find_lru(item_data_t *list, int depth); 

/* finds the LRU */
int find_lru(item_data_t *list, int depth) { 
	int 	lru;		/* index of the LRU */
	//unsigned long long time;	/* time of the LRU */
	int	i;		/* index */

	lru = 0;
	//time = list[0].time;

	/* searches for a empty slot first  */
	for (i=0; i<depth; i++) {
		if ((list[i].flags & VALID) != VALID) {
			return i;
		}
	}
	
	/* actually finds the LRU, at this point all of them are valid  */
	for (i=0; i<depth; i++) { 
		if (list[i].time < list[lru].time) {
			lru = i; 
		} 
	}

	return lru; 
}

 

int cache_setup (settings_t *settings) { 
	
	/* calcs the number of direct memory mappings */
	settings->map_secs = settings->data_size/
	    (settings->associativity * settings->block_size); 
	

	/* calcs the effective data size */
	settings->ef_data_size = settings->map_secs * 
	    settings->associativity * settings->block_size; 

	/*
	 * mallocs the sections items needed
	 *
	 * for each associative section in the direct memory mapped part
	 * there is one item data
	 */	
	settings->data_array = malloc( sizeof(item_data_t) *
	    settings->map_secs * settings->associativity );
	
	if (settings->data_array == NULL) { 
		return RTN_FAIL;
	}
	
	/* inits the data to zero, needed for the flags */
	memset(settings->data_array, 0, sizeof(item_data_t) *
	    settings->map_secs * settings->associativity );


	/* here we create the array for the victim cache */
	if (settings->vic_block_size > 0) {	/* if there's vic cache */ 
		settings->vic_array = malloc( sizeof(item_data_t) *
		    settings->vic_block_size);
		if (settings->data_array == NULL) { /* malloc fail */
			free(settings->data_array); 
			return RTN_FAIL;
		}
		memset(settings->vic_array, 0, sizeof(item_data_t) *
		    settings->vic_block_size);
	}
	else { 					/* no vic cache */
		settings->vic_array = NULL; 
	}

	return RTN_SUCESS;
}

/*
 * Here we take in a memory address and transform it into block addrs
 * once we have the block addrs we read from each on of the blocks.  
 */

void read_cache	(settings_t *settings, mem_rw_t *read_data,
    save_data_t *save_data) { 
	int 		i;
	item_data_t 	block_read;
	unsigned long long init_addr;	
	unsigned long long temp_addr;	

	/* calcs the block addr */	
	init_addr = (read_data->addr / settings->block_size) *
	    settings->block_size;

	block_read.time = read_data->time;
	block_read.addr = init_addr;
	block_read.flags = READ; 
	read_block(settings,  &block_read, save_data);

	/* 
	 * checks to see if the rest of the data being read is in another
	 * block and if it is it also reads that data
	 */
	for (i=1; i<read_data->data_len; i++){
		temp_addr = ((read_data->addr + i) / settings->block_size)*
		    settings->block_size; 	/* calcs the new block addr */
		
		if(temp_addr != init_addr) { 
			init_addr = temp_addr;
			block_read.addr = init_addr;
			read_block(settings,  &block_read, save_data);
		}
	}
}


 /*
  * Here we take the block addr and read to that block.   
  */
void read_block (settings_t *settings, item_data_t *read_data,
    save_data_t *save_data) {
	int	dir_map_off;		/* the offset in the data_array to
					 * get to the associated part */
	int 	as_off;			/* offset to associated block */
	int	lru_cache;		/* lru for the data cach */
	int	lru_vic;		/* lru for the vic cache */


	dir_map_off = ( read_data->addr % 
	    (settings->map_secs * settings->block_size ) ) /
	    settings->block_size; 
	

	#if 0 
		fprintf(stdout, "dir_map_off = %d\n",dir_map_off);
	#endif /* DEBUG */

	/* searches the assocaitivity part of the cache */
	for(as_off=0; as_off<settings->associativity; as_off++) {
		if (((settings->data_array[dir_map_off+as_off].flags & VALID)
		    == VALID) 
		    && (settings->data_array[dir_map_off+as_off].addr == 
		    read_data->addr)) { 		/* match */
		    	save_data->l1_rd_h++;	/* cache hit  */
			goto read; 		/* read */
		}
	}	

	/* it's a l1 miss */	
  	save_data->l1_rd_m++;

	if (settings->vic_block_size > 0) { 
		/* searches the victim cache */
		for(as_off=0; as_off>settings->vic_block_size; as_off++) {
			if (settings->vic_array[as_off].flags==VALID
			    && settings->vic_array[as_off].addr == 
			    read_data->addr) { 		/* match */
			    	save_data->vic_rd_h++;	/* vic hit */
				/* calcs the lru of the cache */
				lru_cache = find_lru(
				    &settings->data_array[dir_map_off],
				    settings->associativity);
				
				/* checks for write back */
				if (settings->write_policy==WR_BACK && 
				    settings->data_array[dir_map_off+
				    lru_cache].flags == DIRTY) { 
					save_data->mem_wr++; 
				}	
				/* copies the cache tag to the vic cache
				 * if it's found in the vic cache then
				 * cache is full so we don't have to worry
				 * about the cache data being valid */
				settings->vic_array[as_off].addr =
				    settings->data_array[dir_map_off +
				        lru_cache].addr;
				settings->vic_array[as_off].time =
				    settings->data_array[dir_map_off +
				        lru_cache].time;
				settings->vic_array[as_off].flags =
				    settings->vic_array[as_off].flags | 
				    VALID; 
				    	
				/* reads the tag back in  cache tag */
				settings->data_array[dir_map_off+lru_cache]
				  .addr = read_data->addr;
				  
	
				/* read */
				goto read; 
			}
	
		}

		/* if the data isn't in the vic cache, then we have to 
		 * write the lru cache block to the lru vic block */

		save_data->vic_rd_m++;	/* vic miss */

		/* the cache and vic lru */
		lru_cache = find_lru(&settings->data_array[dir_map_off],
		    settings->associativity);
		
		lru_vic = find_lru(settings->vic_array,
		    settings->vic_block_size)
		;
		/* updates the memory write */
		if ((settings->write_policy == WR_BACK) && 
		    (settings->data_array[dir_map_off+lru_cache].flags 
		    == (DIRTY | VALID))) { 
			save_data->mem_wr++; 
		}	
		
		if ((settings->data_array[dir_map_off+ lru_cache].flags &
		    VALID) == VALID) { /* there's valid data in cache */
			/* pushes the cache lru to the vic lru */
			settings->vic_array[lru_vic].addr =
			    settings->data_array[dir_map_off + 
			    lru_cache].addr;
			settings->vic_array[lru_vic].flags = VALID; 
		} 

		#if 0 
			fprintf(stdout, "dir_map_off = %d\n",dir_map_off);
			fprintf(stdout, "off_set = %d\n",lru_cache);
		#endif /* DEBUG */

		/* reads in the new data */
		settings->data_array[dir_map_off+lru_cache].addr = 
		    read_data->addr;
		settings->data_array[dir_map_off+lru_cache].time = 
		    read_data->time;
		settings->data_array[dir_map_off+lru_cache].flags = 
		    VALID;

		save_data->mem_rd++;	/* incs blocks read */ 
		    
	} /* end if vic cache */	
	else {		/* there isn't any victim cache */

		/* updates the memory write */
		if (settings->write_policy==WR_BACK)
		    save_data->mem_wr++; 

		/* the cache lru*/
		lru_cache = find_lru(&settings->data_array[dir_map_off],
		    settings->associativity);


		/* reads in the new data */
		settings->data_array[dir_map_off+lru_cache].addr = 
		    read_data->addr;
		settings->data_array[dir_map_off+lru_cache].time = 
		    read_data->time;
		settings->data_array[dir_map_off+lru_cache].flags = VALID;

		save_data->mem_rd++;	/* incs blocks read */ 

	} /* end else  vic cache */	

    read: 
	
	return;

}  /* end func read_block() */
