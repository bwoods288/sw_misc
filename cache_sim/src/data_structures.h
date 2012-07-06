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

/* used for each data item */
typedef struct { 
	unsigned long long time;	/* is used for the time stamp */
	int		flags;		/* flags, used for R/W/ etc */
	unsigned	addr;		/* 32 bit addr */
} item_data_t;
 
/* used for the settings of the cache and the program */
typedef struct {
	int		data_size;	/* the targeted data size */
	int		associativity;	/* associativity of each dir map sec */
	int		block_size;	/* block size */
	int		write_policy;	/* write policy, see README for more 
					 *  info. 0 for write back, 1 for
					 *  write through */
	int		vic_block_size;	/* victim block size */
	int		map_secs;	/* how many dir map sections there
					 *  are */
	int		ef_data_size;	/* the effective data size of the
					 *  cache */
	int		verbose;	/* verbose flag */
	int		step;		/* step flag */
	char 		*file_path;	/* file path name */
	item_data_t	*data_array;	/* the array used to keep track of the
					 *  data */
	item_data_t	*vic_array;	/* the array used to keep track of the
					 *  data */
} settings_t;


typedef struct { 
	unsigned long long time;
	unsigned	addr;
	int		data_len; 
	int 		flags; 
} mem_rw_t;


typedef struct { 
	unsigned long long l1_rd_h;
	unsigned long long l1_rd_m;
	unsigned long long vic_rd_h;
	unsigned long long vic_rd_m;
	unsigned long long l1_wr_h;
	unsigned long long l1_wr_m;
	unsigned long long vic_wr_h;
	unsigned long long vic_wr_m;
	unsigned long long mem_wr;
	unsigned long long mem_rd;
} save_data_t; 

