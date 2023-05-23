#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#include<cynw_utilities.h>
#endif

#include "filter_def.h"

class SobelFilter: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<18> >::in i_rgb;
	cynw_p2p< sc_dt::sc_uint<34> >::out o_result;
#else
	
#endif

	SC_HAS_PROCESS( SobelFilter );
	SobelFilter( sc_module_name n );
	~SobelFilter();
private:
	void do_filter();

	sc_dt::sc_uint<8> med_buffer[(MASK_X*MASK_Y)]={0};
  	sc_dt::sc_uint<8> mean_buffer[MASK_X][MASK_Y]={{0,0,0},{0,0,0},{0,0,0}};
	sc_dt::sc_uint<8> temp[(MASK_X*MASK_Y)]={0};
};
#endif
