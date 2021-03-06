/* Copyright (c) 2016 ZhaiShaoMin Limited
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author:zhaishaomin
*/

#include"mem/cache/ldsp/direction_predictor.hh"

Direction_Predictor::Direction_Predictor(const Direction_PredictorParams *params)
     :Dire_PredSize(params->dire_predsize),
      cache_block_size(params->cacheblksize)
{
     if (!isPowerOf2(Dire_PredSize)) {
        fatal("Invalid local predictor size!\n");
     }
     
     //Set up the array of dp_entrys for the direction predictor
     dp_array.resize(Dire_PredSize);
     
     for (int i = 0; i < Dire_PredSize; ++i){
        dp_array[i].pref1=false;
        dp_array[i].pref2=false;
        dp_array[i].prefed=false;
        dp_array[i].pollution=false;
     }

     // Set up the Direction_Predictor index mask
     // this is equivalent to mask(log2(Dire_PredSize)
     Dire_PredMask=Dire_PredSize-1;
     
}

unsigned  Direction_Predictor::calcu2DP_Eidx(unsigned addr)
{
     return (addr>>(1+cache_block_size))&Dire_PredMask;
}

unsigned  Direction_Predictor::calcu1DP_Eidx(unsigned addr)
{
     return (addr>>cache_block_size)&Dire_PredMask;
}

dp_entry  Direction_Predictor::getPrediction(unsigned addr )
{
     unsigned index=calcu1DP_Eidx(addr);
     return dp_array[index];
}

void  Direction_Predictor::updateDP_array(unsigned addr, dp_entry _dp_entry)
{
     unsigned index=calcu1DP_Eidx(addr);
     assign_DP_entry(dp_array[index], _dp_entry);
}

void Direction_Predictor::assign_DP_entry(dp_entry &new_entry, dp_entry old_entry)
{ 
     dp_array.pref1=old_entry.pref1;
     dp_array.pref2=old_entry.pref2;
     dp_array.prefed=old_entry.prefed;
     dp_array.pollution=old_entry.pollution;
}

Direction_Predictor*
Direction_PredictorParams::create()
{
    return new Direction_Predictor(this);
}

// update the DP_array according to pred mode two entry or one entry
void  Direction_Predictor::update_array(unsigned addr, dp_entry dp_entry[], bool two_mode)
{
    if(two_mode){
    unsigned  index1=calcu2DP_Eidx( addr);
    unsigned  index2=calcu2DP_Eidx( addr)+1;
    assign_DP_entry(dp_array[index1],dp_entry[0]);
    assign_DP_entry(dp_array[index2],dp_entry[1]);
    }
    else{
    updateDP_array(addr,dp_entry[0]);
    }
}
// return the dp_entry result of prediction acording to the pred mode
void  Direction_Predictor::getPrediction(unsigned addr, dp_entry &dp_entry[], bool two_mode)
{
    if(two_mode){
     unsigned index1=calcu2DP_Eidx( addr);
     dp_entry[0]=dp_array[index1];
     unsigned  index2=calcu2DP_Eidx( addr)+1;
     dp_entry[1]=dp_array[index2];
    }else{
     dp_entry[0]=getPrediction(addr);
    }
} 
