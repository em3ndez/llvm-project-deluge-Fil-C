/*
 * Copyright (C) 2017-2023 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "IsoSubspace.h"

#include "IsoAlignedMemoryAllocator.h"
#include "IsoCellSetInlines.h"
#include "JSCellInlines.h"
#include "MarkedSpaceInlines.h"
#include <wtf/TZoneMallocInlines.h>

namespace JSC {

WTF_MAKE_TZONE_ALLOCATED_IMPL(IsoSubspace);

IsoSubspace::IsoSubspace(CString name, JSC::Heap& heap, const HeapCellType& heapCellType, size_t size, uint8_t numberOfLowerTierCells, std::unique_ptr<IsoMemoryAllocatorBase>&& allocator)
    : Subspace(name, heap)
{
    initialize(heapCellType, nullptr);
    UNUSED_PARAM(size);
    UNUSED_PARAM(numberOfLowerTierCells);
    UNUSED_PARAM(allocator);
}

IsoSubspace::~IsoSubspace()
{
}

void IsoSubspace::didResizeBits(unsigned blockIndex)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(blockIndex);
}

void IsoSubspace::didRemoveBlock(unsigned blockIndex)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(blockIndex);
}

void IsoSubspace::didBeginSweepingToFreeList(MarkedBlock::Handle* block)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(block);
}

void* IsoSubspace::tryAllocateFromLowerTier()
{
    UNREACHABLE_FOR_PLATFORM();
    return nullptr;
}

void IsoSubspace::sweepLowerTierCell(PreciseAllocation* preciseAllocation)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(preciseAllocation);
}

void IsoSubspace::destroyLowerTierFreeList()
{
    UNREACHABLE_FOR_PLATFORM();
}

namespace GCClient {

IsoSubspace::IsoSubspace(JSC::IsoSubspace& server)
    : m_attributes(server.attributes())
{
}

} // namespace GCClient

} // namespace JSC

