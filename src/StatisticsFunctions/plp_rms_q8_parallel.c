/* =====================================================================
 * Project:      PULP DSP Library
 * Title:        plp_rms_q8_parallel.c
 * Description:  Parallelized RMS calculation of an 8-bit fixed point vector glue code
 *
 * $Date:        31. July 2020
 * $Revision:    V0
 *
 * Target Processor: PULP cores
 * ===================================================================== */
/*
 * Copyright (C) 2019 ETH Zurich and University of Bologna.
 *
 * Author: Yvan Bosshard, ETH Zurich
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "plp_math.h"
#include "rt/rt_api.h"

RT_CL_DATA int8_t *resultsBuffer;

/**
   @ingroup groupStats
*/

/**
   @defgroup power Power
   Calculates the RMS value of the input vector.
   There are separate functions for floating point, integer, and fixed point 32- 16- 8-bit data
   types. For lower precision integers (16- and 8-bit), functions exploiting SIMD instructions are
   provided.

   The naming scheme of the functions follows the following pattern (for example plp_dot_prod_i32s):
   <pre>
   \<pulp\> _ \<function name\> _ \<data type\> \<precision\> \<method\> _ \<isa extension\>, with

   data type = {f, i, q} respectively for floats, integers, fixed points

   precision = {32, 16, 8} bits

   method = {s, p} respectively meaning single core or parallel multicore implementation.

   isa extension = rv32im, xpulpv2, etc. of which rv32im is the most general one.

   </pre>

*/

/**
   @addtogroup power
   @{
*/

/**
   @brief         Glue code for RMS value of a 8-bit fixed point vector.
   @param[in]     pSrc       points to the input vector
   @param[in]     blockSize  number of samples in input vector
   @param[in]     fracBits   number of fractional bits
   @param[in]     nPE        number of cores to compute on
   @param[out]    pRes    RMS value returned here
   @return        none
 */

void plp_rms_q8_parallel(const int8_t *__restrict__ pSrc,
                const uint32_t blockSize,
                const uint32_t fracBits,
                const uint8_t nPE,
                int8_t *__restrict__ pRes) {

    if (rt_cluster_id() == ARCHI_FC_CID) {
        printf("parallel processing supported only for cluster side\n");
        return;
    } else {

        /* Calculate block size per core and allocate buffers for result */
        int8_t *resBuf;

        if (nPE > 1) {
            resultsBuffer = (int8_t *) rt_alloc(RT_ALLOC_CL_DATA, sizeof(int8_t) * nPE);
            resBuf = resultsBuffer;
        } else {
            resultsBuffer = pRes;
        }

        /* Set parameters and call workers */
        plp_rms_instance_q8 S = {
            .pSrc = pSrc,
            .blockSize = blockSize,
            .fracBits = fracBits,
            .pRes = resultsBuffer,
            .nPE = nPE
        };

        rt_team_fork(nPE, plp_rms_q8p_xpulpv2, (void *)&S);

        /* collect results and finish calculation */
        if (nPE > 1) {

            int32_t accu = 0;
            for(uint8_t i = 0; i < nPE; ++i) {
                accu += *(resultsBuffer + i);
            }

            *pRes = accu / nPE;
        } else {
            *pRes = *resultsBuffer;
        }
    }
}

/**
  @} end of power group
 */
