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
#include "IsoCellSet.h"

#include "MarkedBlockInlines.h"

namespace JSC {

IsoCellSet::IsoCellSet(IsoSubspace& subspace)
    : m_subspace(subspace)
{
}

IsoCellSet::~IsoCellSet()
{
}

Ref<SharedTask<MarkedBlock::Handle*()>> IsoCellSet::parallelNotEmptyMarkedBlockSource()
{
    UNREACHABLE_FOR_PLATFORM();
    return createSharedTask<MarkedBlock::Handle*()>(
        [] () -> MarkedBlock::Handle* { return nullptr; });
}

NEVER_INLINE WTF::BitSet<MarkedBlock::atomsPerBlock>* IsoCellSet::addSlow(unsigned blockIndex)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(blockIndex);
    return nullptr;
}

void IsoCellSet::didResizeBits(unsigned newSize)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(newSize);
}

void IsoCellSet::didRemoveBlock(unsigned blockIndex)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(blockIndex);
}

void IsoCellSet::sweepToFreeList(MarkedBlock::Handle* block)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(block);
}

} // namespace JSC

