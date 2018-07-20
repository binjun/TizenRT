/* *****************************************************************
 *
 * Copyright 2017 Microsoft
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file
 * This file implements APIs related to atomic operations compatible with GCC compilers.
 */

#include "ocatomic.h"

int32_t oc_atomic_increment(volatile int32_t *addend)
{
    return __sync_add_and_fetch(addend, 1);
}

int32_t oc_atomic_decrement(volatile int32_t *addend)
{
    return __sync_sub_and_fetch(addend, 1);
}
