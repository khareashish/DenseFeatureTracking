#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "sds_lib.h"
#include <ap_int.h>
#include <ap_fixed.h>
#include <stdint.h>
#include <assert.h>
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_int.h"





#define WIDTH  1920
#define HEIGHT 1080



typedef struct
{
	ap_int<16> dIdx ;
	ap_int<16> dIdy ;
}Grad_t;


typedef ap_fixed<32,17, AP_RND_ZERO> Fixed_t ;


//
//typedef ap_uint<256> HistPk_t ;



typedef ap_uint<8> Pixel_t;



