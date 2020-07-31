/* =====================================================================
 * Project:      PULP DSP Library
 * Title:        plp_rms_q8p_xpulpv2.c
 * Description:  8-bit fixed point parallel calculation of the RMS value on XPULPV2 cores
 *
 * $Date:        31.07.2020
 * $Revision:    V0
 *
 * Target Processor: PULP cores
 * ===================================================================== */
/*
 * Copyright (C) 2020 ETH Zurich and University of Bologna.
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

/**
   @ingroup power
*/

/**
   @addtogroup RMSkernels
   @{
*/

/**
   @brief         Parallel RMS value of a 8-bit fixed point vector.
   @param[in]  task_args      pointer to plp_rms_instance_q8 struct initialized by
   plp_rms_q8_parallel
   @return        none
 */

void plp_rms_q8p_xpulpv2(void *task_args) {
    plp_rms_instance_q8 *S = (plp_rms_instance_q8 *) task_args;

    uint32_t blockSizeP = ((S->blockSize + S->nPE - 1) / S->nPE);
    uint32_t blockSizeC = blockSizeP;

    /* if it is the last core, use remainder of blocksize */
    if (rt_core_id() == (S->nPE - 1)) {
        if( S->blockSize % S->nPE ) {
            blockSizeC = S->blockSize % blockSizeP;
        }
    }

    int8_t *pSrc = S->pSrc + (rt_core_id() * blockSizeP);
    int8_t *pRes = S->pRes + rt_core_id();

    plp_rms_q8s_xpulpv2(pSrc, blockSizeC, S->fracBits, pRes);

    /* Set barrier for completion in all cores */
    rt_team_barrier();
}

/**
  @} end of RMSkernels group
 */
