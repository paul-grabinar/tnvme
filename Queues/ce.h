/*
 * Copyright (c) 2011, Intel Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _CE_H_
#define _CE_H_

#include <stdint.h>


struct CEbyDW {
    uint32_t dw0;
    uint32_t dw1;
    uint32_t dw2;
    uint32_t dw3;
} __attribute__((__packed__));

struct CEbyName {
    uint32_t cmdSpec;
    uint32_t reserved;
    uint16_t sqHdPtr;
    uint16_t sqId;
    uint16_t cmdId;
    uint16_t pBit   : 1;
    uint16_t status : 15;
} __attribute__((__packed__));

/**
 * Completion Element (CE) definition.
 * @note: For convenient methods to log/dump/peek; refer to class CQ
 */
union CE {
    struct CEbyDW   d;
    struct CEbyName n;
};


#endif
