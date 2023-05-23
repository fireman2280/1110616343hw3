#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "SobelFilter.h"

SobelFilter::SobelFilter( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_FLATTEN_ARRAY(med_buffer);
	HLS_FLATTEN_ARRAY(mean_buffer);
	HLS_FLATTEN_ARRAY(temp);
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
  	o_result.clk_rst(i_clk, i_rst);
#endif
}

SobelFilter::~SobelFilter() {}

// sobel mask
const int mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

void SobelFilter::do_filter() {
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
#endif
		wait();
	

	while (true) {

		#if defined (II)
      	HLS_PIPELINE_LOOP(SOFT_STALL, II, "Loop" );
		#endif
		sc_dt::sc_uint<34> result;
		sc_dt::sc_uint<50> rgb;
		for (unsigned int v = 0; v<MASK_Y; ++v) {
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				}
#else
				rgb = i_rgb.read();
#endif
		med_buffer[v] = rgb.range(7,0);

		}
		if(rgb.range(16,16)==1){				
			for(int i=0;i<9;i++){
 				temp[i]=med_buffer[i];
			}

			for(int i = 8; i > 0; i--){
    			for(int j = 0; j <= i-1; j++){
					HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
        			if( temp[j] > temp[j+1]){
           			int tmp = temp[j];
            		temp[j] = temp[j+1];
            		temp[j+1] = tmp;
        			}
  				}
			}

			
      		result.range(15,0)=temp[4];
			result.range(32,32)=1;
		}else{
			result.range(32,32)=0;
			}
				
    	for ( int v = 8; v >=3; --v) {            /*shift buffer*/
      		med_buffer[v]=med_buffer[v-3];
			HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
   		 	}	
		int val_mean=0;
		if(rgb.range(17,17)==1){	
			 	for (unsigned int v = 0; v < MASK_Y; ++v) {
      				for (unsigned int u = 0; u < MASK_X; ++u) {
        				if(u==0){ 
          					mean_buffer[0][v] =rgb.range(15, 8);
        				}	
        			val_mean +=mean_buffer[u][v]*mask[u][v];
      				}	
    			}		
      	result.range(31,16)=(short int)(val_mean);
		result.range(33,33)=1;
		}else{
			result.range(33,33)=0;
			}

		for (unsigned int v = 0; v < MASK_Y; ++v) {
        	for (unsigned int u = (MASK_X-1); u > 0; --u) {      //shift col1->col2 col0->col1
			HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
            mean_buffer[u][v] = mean_buffer[u-1][v] ;
        	}
    	}	
	
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result.put(result);
			wait();
		}
#else
		o_result.write(result);
#endif
	}
}



