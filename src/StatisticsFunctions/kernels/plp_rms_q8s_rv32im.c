/* =====================================================================
 * Project:      PULP DSP Library
 * Title:        plp_rms_q8s_rv32im.c
 * Description:  Calculates the RMS value on RV32IM cores
 *
 * $Date:        30.06.2020
 *
 * Target Processor: PULP cores
 * ===================================================================== */
/*
 * Copyright (C) 2020 ETH Zurich and University of Bologna.
 *
 * Author: Moritz Scherer, ETH Zurich
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
   @defgroup RMSkernels RMS Kernels
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
   @addtogroup RMSkernels
   @{
*/

/**
   @brief         RMS value of a 8-bit fixed point vector for RV32IM extension.
   @param[in]     pSrc       points to the input vector
   @param[in]     blockSize  number of samples in input vector
   @param[out]    pRes    RMS value returned here
   @return        none
*/

void plp_rms_q8s_rv32im(const int8_t *__restrict__ pSrc,
                        uint32_t blockSize,
                        uint32_t fracBits,
                        int8_t *__restrict__ pRes) {
    int32_t accu = 0;
    int32_t temp1, temp2;

#if defined(PLP_MATH_LOOPUNROLL)

    for (int i = 0; i < (blockSize >> 1); i++) {
        temp1 = (*pSrc++);
        temp2 = (*pSrc++);
        accu += ((temp1 * temp1) >> fracBits);
        accu += ((temp2 * temp2) >> fracBits);
    }

    if (blockSize % 2 == 1) {
        temp1 = (*pSrc++);
        accu += ((temp1 * temp1) >> fracBits);
    }

#else // PLP_MATH_LOOPUNROLL

    for (int i = 0; i < blockSize; i++) {
        temp1 = (*pSrc++);
        accu += ((temp1 * temp1) >> fracBits);
    }

#endif // PLP_MATH_LOOPUNROLL

    *pRes = accu / blockSize;
}
