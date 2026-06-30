/*
 * Copyright (C) 2017-2021 Apple Inc. All rights reserved.
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
#include "AllocatorInlines.h"
#include "JSCellInlines.h"
#include "LocalAllocatorInlines.h"
#include "MarkedSpaceInlines.h"
#include "SubspaceInlines.h"

namespace JSC {

CompleteSubspace::CompleteSubspace(CString name, JSC::Heap& heap, const HeapCellType& heapCellType, AlignedMemoryAllocator* alignedMemoryAllocator)
    : Subspace(name, heap)
{
    initialize(heapCellType, alignedMemoryAllocator);
}

CompleteSubspace::~CompleteSubspace()
{
}

Allocator CompleteSubspace::allocatorForNonInline(size_t size, AllocatorForMode mode)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(size);
    UNUSED_PARAM(mode);
    return Allocator();
}

Allocator CompleteSubspace::allocatorForSlow(size_t size)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(size);
    return Allocator();
}

void* CompleteSubspace::allocateSlow(VM& vm, size_t size, GCDeferralContext* deferralContext, AllocationFailureMode failureMode)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(vm);
    UNUSED_PARAM(size);
    UNUSED_PARAM(deferralContext);
    UNUSED_PARAM(failureMode);
    return nullptr;
}

void* CompleteSubspace::tryAllocateSlow(VM& vm, size_t size, GCDeferralContext* deferralContext)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(vm);
    UNUSED_PARAM(size);
    UNUSED_PARAM(deferralContext);
    return nullptr;
}

void* CompleteSubspace::reallocatePreciseAllocationNonVirtual(VM& vm, HeapCell* oldCell, size_t size, GCDeferralContext* deferralContext, AllocationFailureMode failureMode)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(vm);
    UNUSED_PARAM(oldCell);
    UNUSED_PARAM(size);
    UNUSED_PARAM(deferralContext);
    UNUSED_PARAM(failureMode);
    return nullptr;
}

} // namespace JSC

