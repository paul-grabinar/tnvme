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

#include "prpData.h"
#include "../Utils/buffers.h"

using namespace std;

const send_64b_bitmask PrpData::ALLOWED_BITS =
    (send_64b_bitmask)(MASK_PRP1_PAGE | MASK_PRP1_LIST |
                       MASK_PRP2_PAGE | MASK_PRP2_LIST);


PrpData::PrpData()
{
    mBufRW = MemBuffer::NullMemBufferPtr;
    mBufRO = NULL;
    mBufSize = 0;
    mPrpFields = (send_64b_bitmask)0;
}


PrpData::~PrpData()
{
}


void
PrpData::SetPrpBuffer(send_64b_bitmask prpFields, SharedMemBufferPtr memBuffer)
{
    if (prpFields & ~ALLOWED_BITS) {
        LOG_DBG("Param prpFields only supports bits specific to PRP fields");
        throw exception();
    } else if (mBufRO != NULL) {
        LOG_DBG("Buffer already setup as RO, cannot also setup RW buffer");
        throw exception();
    }

    // Do allow associating a new buffer with an existing cmd, free the old 1st.
    // These are just user space buffers and it will allow reusing an existing
    // cmd over and over by simply changing its associated data buffer.
    if (mBufRW != MemBuffer::NullMemBufferPtr)
        mBufRW.reset();

    mBufRW = memBuffer;
    mBufSize = memBuffer->GetBufSize();
    mPrpFields = prpFields;
}


void
PrpData::SetPrpBuffer(send_64b_bitmask prpFields, uint8_t const *memBuffer,
    uint64_t bufSize)
{
    if (prpFields & ~ALLOWED_BITS) {
        LOG_DBG("Param prpFields only supports bits specific to PRP fields");
        throw exception();
    } else if (mBufRW != MemBuffer::NullMemBufferPtr) {
        LOG_DBG("Buffer already setup as RW, cannot also setup RO buffer");
        throw exception();
    } else if (((bufSize == 0) && (memBuffer != NULL)) ||
               ((bufSize != 0) && (memBuffer == NULL))) {
        LOG_DBG("Ambiguous; memBuffer = %p, size = %llu", memBuffer,
            (long long unsigned int)bufSize);
        throw exception();
    }

    // This version of SetBuffer() is intended solely for the Creation of IOQ's.
    // Do not allow associating a new buffer with an existing Create IOQ cmd
    // because this is almost certainly a programming mistake which will likely
    // lead to a kernel core dump or tnvme seg fault. Rethink your logic.
    // You most likely want to gCtrlrConfig->SetState(DISABLE_XXXX) and then
    // completely destroy the Create IOQ cmd and the delete the IOQ object
    // representing an IOQ. And finally re-create it all over again is safest.
    if (mBufRO != NULL) {
        LOG_DBG("Buffer already setup as RO, not allowing reconfiguring");
        throw exception();
    }

    mBufRO = memBuffer;
    mBufSize = bufSize;
    mPrpFields = prpFields;
}


uint8_t const *
PrpData::GetROPrpBuffer() const
{
    if (mBufRW != MemBuffer::NullMemBufferPtr)
        return mBufRW->GetBuffer();
    else if (mBufRO != NULL)
        return mBufRO;
    return NULL;
}


void
PrpData::Dump(LogFilename filename, string fileHdr) const
{
    const uint8_t *buf = GetROPrpBuffer();
    Buffers::Dump(filename, buf, 0, ULONG_MAX, GetPrpBufferSize(), fileHdr);
}
