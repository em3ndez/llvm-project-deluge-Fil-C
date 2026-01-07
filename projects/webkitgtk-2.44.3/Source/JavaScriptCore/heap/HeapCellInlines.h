/*
 * Copyright (C) 2016-2019 Apple Inc. All rights reserved.
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

#pragma once

#include "CellContainer.h"
#include "HeapCell.h"
#include "PreciseAllocation.h"
#include "VM.h"

namespace JSC {

ALWAYS_INLINE bool HeapCell::isPreciseAllocation() const
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

ALWAYS_INLINE CellContainer HeapCell::cellContainer() const
{
    UNREACHABLE_FOR_PLATFORM();
    return CellContainer();
}

ALWAYS_INLINE MarkedBlock& HeapCell::markedBlock() const
{
    UNREACHABLE_FOR_PLATFORM();
    MarkedBlock* result = nullptr;
    return *result;
}

ALWAYS_INLINE PreciseAllocation& HeapCell::preciseAllocation() const
{
    UNREACHABLE_FOR_PLATFORM();
    PreciseAllocation* result = nullptr;
    return *result;
}

ALWAYS_INLINE JSC::Heap* HeapCell::heap() const
{
    UNREACHABLE_FOR_PLATFORM();
    return nullptr;
}

ALWAYS_INLINE VM& HeapCell::vm() const
{
    UNREACHABLE_FOR_PLATFORM();
    VM* vm = nullptr;
    return *vm;
}
    
ALWAYS_INLINE size_t HeapCell::cellSize() const
{
    UNREACHABLE_FOR_PLATFORM();
    return 0;
}

ALWAYS_INLINE CellAttributes HeapCell::cellAttributes() const
{
    UNREACHABLE_FOR_PLATFORM();
    return CellAttributes();
}

ALWAYS_INLINE DestructionMode HeapCell::destructionMode() const
{
    return cellAttributes().destruction;
}

ALWAYS_INLINE HeapCell::Kind HeapCell::cellKind() const
{
    return cellAttributes().cellKind;
}

ALWAYS_INLINE Subspace* HeapCell::subspace() const
{
    if (isPreciseAllocation())
        return preciseAllocation().subspace();
    return markedBlock().subspace();
}

} // namespace JSC

