/*
 * Copyright (C) 2014-2022 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "GCDeferralContext.h"
#include "Heap.h"
#include "HeapCellInlines.h"
#include "IndexingHeader.h"
#include "JSCast.h"
#include "Structure.h"
#include <type_traits>
#include <wtf/Assertions.h>
#include <wtf/MainThread.h>

namespace JSC {

ALWAYS_INLINE VM& Heap::vm() const
{
    return *bitwise_cast<VM*>(bitwise_cast<uintptr_t>(this) - OBJECT_OFFSETOF(VM, heap));
}

ALWAYS_INLINE JSC::Heap* Heap::heap(const HeapCell* cell)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(cell);
    return nullptr;
}

inline JSC::Heap* Heap::heap(const JSValue v)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(v);
    return nullptr;
}

inline bool Heap::hasHeapAccess() const
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

inline bool Heap::worldIsStopped() const
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

ALWAYS_INLINE bool Heap::isMarked(const void* rawCell)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(rawCell);
    return false;
}

ALWAYS_INLINE bool Heap::testAndSetMarked(HeapVersion markingVersion, const void* rawCell)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(markingVersion);
    UNUSED_PARAM(rawCell);
    return false;
}

ALWAYS_INLINE size_t Heap::cellSize(const void* rawCell)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(rawCell);
    return 0;
}

inline void Heap::writeBarrier(const JSCell* from, JSValue to)
{
    UNUSED_PARAM(from);
    UNUSED_PARAM(to);
}

inline void Heap::writeBarrier(const JSCell* from, JSCell* to)
{
    UNUSED_PARAM(from);
    UNUSED_PARAM(to);
}

inline void Heap::writeBarrier(const JSCell* from)
{
    UNUSED_PARAM(from);
}

inline void Heap::mutatorFence()
{
}

template<typename Functor> inline void Heap::forEachCodeBlock(const Functor& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(func);
}

template<typename Functor> inline void Heap::forEachCodeBlockIgnoringJITPlans(const AbstractLocker& codeBlockSetLocker, const Functor& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(codeBlockSetLocker);
    UNUSED_PARAM(func);
}

template<typename Functor> inline void Heap::forEachProtectedCell(const Functor& functor)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(functor);
}

#if USE(FOUNDATION)
template <typename T>
inline void Heap::releaseSoon(RetainPtr<T>&& object)
{
    m_delayedReleaseObjects.append(WTFMove(object));
}
#endif

#ifdef JSC_GLIB_API_ENABLED
inline void Heap::releaseSoon(std::unique_ptr<JSCGLibWrapperObject>&& object)
{
    m_delayedReleaseObjects.append(WTFMove(object));
}
#endif

inline void Heap::incrementDeferralDepth()
{
    ASSERT(!Thread::mayBeGCThread());
    m_deferralDepth++;
}

inline void Heap::decrementDeferralDepth()
{
    ASSERT(!Thread::mayBeGCThread());
    m_deferralDepth--;
}

inline void Heap::decrementDeferralDepthAndGCIfNeeded()
{
    ASSERT(!Thread::mayBeGCThread());
    m_deferralDepth--;
}

inline HashSet<MarkedVectorBase*>& Heap::markListSet()
{
    UNREACHABLE_FOR_PLATFORM();
    HashSet<MarkedVectorBase*>* ptr = nullptr;
    return *ptr;
}

inline void Heap::reportExtraMemoryAllocated(const JSCell* cell, size_t size)
{
    UNUSED_PARAM(cell);
    UNUSED_PARAM(size);
}

inline void Heap::reportExtraMemoryAllocated(GCDeferralContext* deferralContext, const JSCell* cell, size_t size)
{
    UNUSED_PARAM(deferralContext);
    UNUSED_PARAM(cell);
    UNUSED_PARAM(size);
}

inline void Heap::deprecatedReportExtraMemory(size_t size)
{
    UNUSED_PARAM(size);
}

inline void Heap::acquireAccess()
{
}

inline bool Heap::hasAccess() const
{
    return true;
}

inline void Heap::releaseAccess()
{
}

inline bool Heap::mayNeedToStop()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

inline void Heap::stopIfNecessary()
{
    UNREACHABLE_FOR_PLATFORM();
}

template<typename Func>
void Heap::forEachSlotVisitor(const Func& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(func);
}

namespace GCClient {

ALWAYS_INLINE VM& Heap::vm() const
{
    return *bitwise_cast<VM*>(bitwise_cast<uintptr_t>(this) - OBJECT_OFFSETOF(VM, clientHeap));
}

} // namespace GCClient

} // namespace JSC
