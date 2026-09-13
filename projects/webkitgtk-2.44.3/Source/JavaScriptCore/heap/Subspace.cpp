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
#include "Subspace.h"

#include "AlignedMemoryAllocator.h"
#include "HeapCellType.h"
#include "MarkedSpaceInlines.h"
#include "ParallelSourceAdapter.h"
#include "SubspaceInlines.h"
#include <wtf/TZoneMallocInlines.h>

namespace JSC {

WTF_MAKE_TZONE_ALLOCATED_IMPL(Subspace);

Subspace::Subspace(CString name, JSC::Heap& heap)
{
    UNUSED_PARAM(name);
    UNUSED_PARAM(heap);
}

void Subspace::initialize(const HeapCellType& heapCellType, AlignedMemoryAllocator* alignedMemoryAllocator)
{
    m_heapCellType = &heapCellType;
    UNUSED_PARAM(alignedMemoryAllocator);
}

Subspace::~Subspace()
{
}

void Subspace::finishSweep(MarkedBlock::Handle& block, FreeList* freeList)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(block);
    UNUSED_PARAM(freeList);
}

void Subspace::destroy(VM& vm, JSCell* cell)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(vm);
    UNUSED_PARAM(cell);
}

void Subspace::prepareForAllocation()
{
    UNREACHABLE_FOR_PLATFORM();
}

MarkedBlock::Handle* Subspace::findEmptyBlockToSteal()
{
    UNREACHABLE_FOR_PLATFORM();
    return nullptr;
}

Ref<SharedTask<BlockDirectory*()>> Subspace::parallelDirectorySource()
{
    UNREACHABLE_FOR_PLATFORM();
    return createSharedTask<BlockDirectory*()>([] () -> BlockDirectory* { return nullptr; });
}

Ref<SharedTask<MarkedBlock::Handle*()>> Subspace::parallelNotEmptyMarkedBlockSource()
{
    UNREACHABLE_FOR_PLATFORM();
    return createSharedTask<MarkedBlock::Handle*()>([] () -> MarkedBlock::Handle* { return nullptr; });
}

void Subspace::sweepBlocks()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Subspace::didResizeBits(unsigned)
{
    UNREACHABLE_FOR_PLATFORM();
}

void Subspace::didRemoveBlock(unsigned)
{
    UNREACHABLE_FOR_PLATFORM();
}

void Subspace::didBeginSweepingToFreeList(MarkedBlock::Handle*)
{
    UNREACHABLE_FOR_PLATFORM();
}

} // namespace JSC

