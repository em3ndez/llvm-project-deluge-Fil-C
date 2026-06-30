/*
 *  Copyright (C) 2003-2023 Apple Inc. All rights reserved.
 *  Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"
#include "Heap.h"

#include "BuiltinExecutables.h"
#include "CodeBlock.h"
#include "CodeBlockSetInlines.h"
#include "CollectingScope.h"
#include "ConservativeRoots.h"
#include "EdenGCActivityCallback.h"
#include "Exception.h"
#include "FastMallocAlignedMemoryAllocator.h"
#include "FullGCActivityCallback.h"
#include "FunctionExecutableInlines.h"
#include "GCActivityCallback.h"
#include "GCIncomingRefCountedInlines.h"
#include "GCIncomingRefCountedSetInlines.h"
#include "GCSegmentedArrayInlines.h"
#include "GCTypeMap.h"
#include "GigacageAlignedMemoryAllocator.h"
#include "HasOwnPropertyCache.h"
#include "HeapHelperPool.h"
#include "HeapIterationScope.h"
#include "HeapProfiler.h"
#include "HeapSnapshot.h"
#include "HeapSubspaceTypes.h"
#include "HeapVerifier.h"
#include "IncrementalSweeper.h"
#include "Interpreter.h"
#include "IsoCellSetInlines.h"
#include "IsoInlinedHeapCellTypeInlines.h"
#include "IsoSubspacePerVM.h"
#include "JITStubRoutineSet.h"
#include "JITWorklistInlines.h"
#include "JSFinalizationRegistry.h"
#include "JSRemoteFunction.h"
#include "JSVirtualMachineInternal.h"
#include "JSWeakMap.h"
#include "JSWeakObjectRef.h"
#include "JSWeakSet.h"
#include "MachineStackMarker.h"
#include "MarkStackMergingConstraint.h"
#include "MarkedJSValueRefArray.h"
#include "MarkedSpaceInlines.h"
#include "MarkingConstraintSet.h"
#include "MegamorphicCache.h"
#include "NumberObject.h"
#include "PreventCollectionScope.h"
#include "SamplingProfiler.h"
#include "ShadowChicken.h"
#include "SpaceTimeMutatorScheduler.h"
#include "StochasticSpaceTimeMutatorScheduler.h"
#include "StopIfNecessaryTimer.h"
#include "StructureAlignedMemoryAllocator.h"
#include "SubspaceInlines.h"
#include "SuperSampler.h"
#include "SweepingScope.h"
#include "SymbolTableInlines.h"
#include "SynchronousStopTheWorldMutatorScheduler.h"
#include "TypeProfiler.h"
#include "TypeProfilerLog.h"
#include "VM.h"
#include "VerifierSlotVisitorInlines.h"
#include "WeakMapImplInlines.h"
#include "WeakSetInlines.h"
#include <algorithm>
#include <wtf/CryptographicallyRandomNumber.h>
#include <wtf/ListDump.h>
#include <wtf/RAMSize.h>
#include <wtf/Scope.h>
#include <wtf/SimpleStats.h>
#include <wtf/Threading.h>

#if USE(BMALLOC_MEMORY_FOOTPRINT_API)
#include <bmalloc/bmalloc.h>
#endif

#if USE(FOUNDATION)
#include <wtf/spi/cocoa/objcSPI.h>
#endif

#ifdef JSC_GLIB_API_ENABLED
#include "JSCGLibWrapperObject.h"
#endif

namespace JSC {

#define INIT_SERVER_ISO_SUBSPACE(name, heapCellType, type) \
    , name ISO_SUBSPACE_INIT(*this, heapCellType, type)

#define INIT_SERVER_STRUCTURE_ISO_SUBSPACE(name, heapCellType, type) \
    , name("IsoSubspace" #name, *this, heapCellType, WTF::roundUpToMultipleOf<type::atomSize>(sizeof(type)), type::numberOfLowerTierCells, makeUnique<StructureAlignedMemoryAllocator>("Structure"))

Heap::Heap(VM& vm, HeapType heapType)
    : m_sweeper(adoptRef(*new IncrementalSweeper(this)))

    // HeapCellTypes
    , auxiliaryHeapCellType(CellAttributes(DoesNotNeedDestruction, HeapCell::Auxiliary))
    , immutableButterflyHeapCellType(CellAttributes(DoesNotNeedDestruction, HeapCell::JSCellWithIndexingHeader))
    , cellHeapCellType(CellAttributes(DoesNotNeedDestruction, HeapCell::JSCell))
    , destructibleCellHeapCellType(CellAttributes(NeedsDestruction, HeapCell::JSCell))
    , apiGlobalObjectHeapCellType(IsoHeapCellType::Args<JSAPIGlobalObject>())
    , callbackConstructorHeapCellType(IsoHeapCellType::Args<JSCallbackConstructor>())
    , callbackGlobalObjectHeapCellType(IsoHeapCellType::Args<JSCallbackObject<JSGlobalObject>>())
    , callbackObjectHeapCellType(IsoHeapCellType::Args<JSCallbackObject<JSNonFinalObject>>())
    , customGetterFunctionHeapCellType(IsoHeapCellType::Args<JSCustomGetterFunction>())
    , customSetterFunctionHeapCellType(IsoHeapCellType::Args<JSCustomSetterFunction>())
    , dateInstanceHeapCellType(IsoHeapCellType::Args<DateInstance>())
    , errorInstanceHeapCellType(IsoHeapCellType::Args<ErrorInstance>())
    , finalizationRegistryCellType(IsoHeapCellType::Args<JSFinalizationRegistry>())
    , globalLexicalEnvironmentHeapCellType(IsoHeapCellType::Args<JSGlobalLexicalEnvironment>())
    , globalObjectHeapCellType(IsoHeapCellType::Args<JSGlobalObject>())
    , injectedScriptHostSpaceHeapCellType(IsoHeapCellType::Args<Inspector::JSInjectedScriptHost>())
    , javaScriptCallFrameHeapCellType(IsoHeapCellType::Args<Inspector::JSJavaScriptCallFrame>())
    , jsModuleRecordHeapCellType(IsoHeapCellType::Args<JSModuleRecord>())
    , syntheticModuleRecordHeapCellType(IsoHeapCellType::Args<SyntheticModuleRecord>())
    , moduleNamespaceObjectHeapCellType(IsoHeapCellType::Args<JSModuleNamespaceObject>())
    , nativeStdFunctionHeapCellType(IsoHeapCellType::Args<JSNativeStdFunction>())
    , weakMapHeapCellType(IsoHeapCellType::Args<JSWeakMap>())
    , weakSetHeapCellType(IsoHeapCellType::Args<JSWeakSet>())
#if JSC_OBJC_API_ENABLED
    , apiWrapperObjectHeapCellType(IsoHeapCellType::Args<JSCallbackObject<JSAPIWrapperObject>>())
    , objCCallbackFunctionHeapCellType(IsoHeapCellType::Args<ObjCCallbackFunction>())
#endif
#ifdef JSC_GLIB_API_ENABLED
    , apiWrapperObjectHeapCellType(IsoHeapCellType::Args<JSCallbackObject<JSAPIWrapperObject>>())
    , callbackAPIWrapperGlobalObjectHeapCellType(IsoHeapCellType::Args<JSCallbackObject<JSAPIWrapperGlobalObject>>())
    , jscCallbackFunctionHeapCellType(IsoHeapCellType::Args<JSCCallbackFunction>())
#endif
    , intlCollatorHeapCellType(IsoHeapCellType::Args<IntlCollator>())
    , intlDateTimeFormatHeapCellType(IsoHeapCellType::Args<IntlDateTimeFormat>())
    , intlDisplayNamesHeapCellType(IsoHeapCellType::Args<IntlDisplayNames>())
    , intlDurationFormatHeapCellType(IsoHeapCellType::Args<IntlDurationFormat>())
    , intlListFormatHeapCellType(IsoHeapCellType::Args<IntlListFormat>())
    , intlLocaleHeapCellType(IsoHeapCellType::Args<IntlLocale>())
    , intlNumberFormatHeapCellType(IsoHeapCellType::Args<IntlNumberFormat>())
    , intlPluralRulesHeapCellType(IsoHeapCellType::Args<IntlPluralRules>())
    , intlRelativeTimeFormatHeapCellType(IsoHeapCellType::Args<IntlRelativeTimeFormat>())
    , intlSegmentIteratorHeapCellType(IsoHeapCellType::Args<IntlSegmentIterator>())
    , intlSegmenterHeapCellType(IsoHeapCellType::Args<IntlSegmenter>())
    , intlSegmentsHeapCellType(IsoHeapCellType::Args<IntlSegments>())
#if ENABLE(WEBASSEMBLY)
    , webAssemblyArrayHeapCellType(IsoHeapCellType::Args<JSWebAssemblyArray>())
    , webAssemblyExceptionHeapCellType(IsoHeapCellType::Args<JSWebAssemblyException>())
    , webAssemblyFunctionHeapCellType(IsoHeapCellType::Args<WebAssemblyFunction>())
    , webAssemblyGlobalHeapCellType(IsoHeapCellType::Args<JSWebAssemblyGlobal>())
    , webAssemblyInstanceHeapCellType(IsoHeapCellType::Args<JSWebAssemblyInstance>())
    , webAssemblyMemoryHeapCellType(IsoHeapCellType::Args<JSWebAssemblyMemory>())
    , webAssemblyStructHeapCellType(IsoHeapCellType::Args<JSWebAssemblyStruct>())
    , webAssemblyModuleHeapCellType(IsoHeapCellType::Args<JSWebAssemblyModule>())
    , webAssemblyModuleRecordHeapCellType(IsoHeapCellType::Args<WebAssemblyModuleRecord>())
    , webAssemblyTableHeapCellType(IsoHeapCellType::Args<JSWebAssemblyTable>())
    , webAssemblyTagHeapCellType(IsoHeapCellType::Args<JSWebAssemblyTag>())
#endif

    // AlignedMemoryAllocators
    , fastMallocAllocator(makeUnique<FastMallocAlignedMemoryAllocator>())
    , primitiveGigacageAllocator(makeUnique<GigacageAlignedMemoryAllocator>(Gigacage::Primitive))
    , jsValueGigacageAllocator(makeUnique<GigacageAlignedMemoryAllocator>(Gigacage::JSValue))

    // Subspaces
    , primitiveGigacageAuxiliarySpace("Primitive Gigacage Auxiliary", *this, auxiliaryHeapCellType, primitiveGigacageAllocator.get()) // Hash:0x3e7cd762
    , jsValueGigacageAuxiliarySpace("JSValue Gigacage Auxiliary", *this, auxiliaryHeapCellType, jsValueGigacageAllocator.get()) // Hash:0x241e946
    , immutableButterflyJSValueGigacageAuxiliarySpace("ImmutableButterfly Gigacage JSCellWithIndexingHeader", *this, immutableButterflyHeapCellType, jsValueGigacageAllocator.get()) // Hash:0x7a945300
    , cellSpace("JSCell", *this, cellHeapCellType, fastMallocAllocator.get()) // Hash:0xadfb5a79
    , variableSizedCellSpace("Variable Sized JSCell", *this, cellHeapCellType, fastMallocAllocator.get()) // Hash:0xbcd769cc
    , destructibleObjectSpace("JSDestructibleObject", *this, destructibleObjectHeapCellType, fastMallocAllocator.get()) // Hash:0x4f5ed7a9
    FOR_EACH_JSC_COMMON_ISO_SUBSPACE(INIT_SERVER_ISO_SUBSPACE)
    FOR_EACH_JSC_STRUCTURE_ISO_SUBSPACE(INIT_SERVER_STRUCTURE_ISO_SUBSPACE)
    , codeBlockSpaceAndSet ISO_SUBSPACE_INIT(*this, destructibleCellHeapCellType, CodeBlock) // Hash:0x77e66ec9
    , functionExecutableSpaceAndSet ISO_SUBSPACE_INIT(*this, destructibleCellHeapCellType, FunctionExecutable) // Hash:0x5d158f3
    , programExecutableSpaceAndSet ISO_SUBSPACE_INIT(*this, destructibleCellHeapCellType, ProgramExecutable) // Hash:0x527c77e7
    , unlinkedFunctionExecutableSpaceAndSet ISO_SUBSPACE_INIT(*this, destructibleCellHeapCellType, UnlinkedFunctionExecutable) // Hash:0xf6b828d9

{
    UNUSED_PARAM(vm);
    UNUSED_PARAM(heapType);
}

#undef INIT_SERVER_ISO_SUBSPACE
#undef INIT_SERVER_STRUCTURE_ISO_SUBSPACE

Heap::~Heap()
{
}

bool Heap::isPagedOut()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

void Heap::dumpHeapStatisticsAtVMDestruction()
{
}

// The VM is being destroyed and the collector will never run again.
// Run all pending finalizers now because we won't get another chance.
void Heap::lastChanceToFinalize()
{
    MonotonicTime before;
    if (UNLIKELY(Options::logGC())) {
        before = MonotonicTime::now();
        dataLog("[GC<", RawPointer(this), ">: shutdown ");
    }
    
    RELEASE_ASSERT(!vm().entryScope);
    
    dataLogIf(Options::logGC(), "1");
    
    dataLogIf(Options::logGC(), "2");

    dataLogIf(Options::logGC(), "3");

    dataLogIf(Options::logGC(), "4");
    
    dataLogIf(Options::logGC(), "5 ");

    if (UNLIKELY(Options::dumpHeapStatisticsAtVMDestruction()))
        dumpHeapStatisticsAtVMDestruction();
    
    releaseDelayedReleasedObjects();

    dataLogIf(Options::logGC(), (MonotonicTime::now() - before).milliseconds(), "ms]\n");
}

void Heap::releaseDelayedReleasedObjects()
{
#if USE(FOUNDATION) || defined(JSC_GLIB_API_ENABLED)
    // We need to guard against the case that releasing an object can create more objects due to the
    // release calling into JS. When those JS call(s) exit and all locks are being dropped we end up
    // back here and could try to recursively release objects. We guard that with a recursive entry
    // count. Only the initial call will release objects, recursive calls simple return and let the
    // the initial call to the function take care of any objects created during release time.
    // This also means that we need to loop until there are no objects in m_delayedReleaseObjects
    // and use a temp Vector for the actual releasing.
    if (!m_delayedReleaseRecursionCount++) {
        while (!m_delayedReleaseObjects.isEmpty()) {
            ASSERT(vm().currentThreadIsHoldingAPILock());

            auto objectsToRelease = WTFMove(m_delayedReleaseObjects);

            {
                // We need to drop locks before calling out to arbitrary code.
                JSLock::DropAllLocks dropAllLocks(vm());

#if USE(FOUNDATION)
                void* context = objc_autoreleasePoolPush();
#endif
                objectsToRelease.clear();
#if USE(FOUNDATION)
                objc_autoreleasePoolPop(context);
#endif
            }
        }
    }
    m_delayedReleaseRecursionCount--;
#endif
}

void Heap::reportExtraMemoryAllocatedPossiblyFromAlreadyMarkedCell(const JSCell* cell, size_t size)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(cell);
    UNUSED_PARAM(size);
}

void Heap::reportExtraMemoryAllocatedSlowCase(GCDeferralContext* deferralContext, const JSCell* cell, size_t size)
{
    UNUSED_PARAM(deferralContext);
    UNUSED_PARAM(cell);
    UNUSED_PARAM(size);
}

void Heap::deprecatedReportExtraMemorySlowCase(size_t size)
{
    UNUSED_PARAM(size);
}

bool Heap::overCriticalMemoryThreshold(MemoryThresholdCallType memoryThresholdCallType)
{
    UNUSED_PARAM(memoryThresholdCallType);
    return false;
}

void Heap::reportAbandonedObjectGraph()
{
}

void Heap::protect(JSValue k)
{
    UNUSED_PARAM(k);
}

bool Heap::unprotect(JSValue k)
{
    UNUSED_PARAM(k);
    return false;
}

void Heap::addReference(JSCell* cell, ArrayBuffer* buffer)
{
    buffer->addIncomingReference(cell);
}

template<typename CellType, typename CellSet>
void Heap::finalizeMarkedUnconditionalFinalizers(CellSet& cellSet, CollectionScope collectionScope)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(cellSet);
    UNUSED_PARAM(collectionScope);
}

void Heap::finalizeUnconditionalFinalizers()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::willStartIterating()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::didFinishIterating()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::completeAllJITPlans()
{
    UNREACHABLE_FOR_PLATFORM();
}

template<typename Visitor>
void Heap::iterateExecutingAndCompilingCodeBlocks(Visitor& visitor, const Function<void(CodeBlock*)>& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(func);
}

template<typename Func, typename Visitor>
void Heap::iterateExecutingAndCompilingCodeBlocksWithoutHoldingLocks(Visitor& visitor, const Func& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(func);
}

void Heap::assertMarkStacksEmpty()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::gatherStackRoots(ConservativeRoots& roots)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(roots);
}

void Heap::gatherJSStackRoots(ConservativeRoots& roots)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(roots);
}

void Heap::gatherScratchBufferRoots(ConservativeRoots& roots)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(roots);
}

void Heap::beginMarking()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::removeDeadCompilerWorklistEntries()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::gatherExtraHeapData(HeapProfiler& heapProfiler)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(heapProfiler);
}

void Heap::removeDeadHeapSnapshotNodes(HeapProfiler& heapProfiler)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(heapProfiler);
}

void Heap::updateObjectCounts()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::endMarking()
{
    UNREACHABLE_FOR_PLATFORM();
}

size_t Heap::objectCount()
{
    return 0;
}

size_t Heap::extraMemorySize()
{
    return 0;
}

size_t Heap::size()
{
    return 0;
}

size_t Heap::capacity()
{
    return 0;
}

size_t Heap::protectedGlobalObjectCount()
{
    return 0;
}

size_t Heap::globalObjectCount()
{
    return 0;
}

size_t Heap::protectedObjectCount()
{
    return 0;
}

std::unique_ptr<TypeCountSet> Heap::protectedObjectTypeCounts()
{
    std::unique_ptr<TypeCountSet> result = makeUnique<TypeCountSet>();
    return result;
}

std::unique_ptr<TypeCountSet> Heap::objectTypeCounts()
{
    std::unique_ptr<TypeCountSet> result = makeUnique<TypeCountSet>();
    return result;
}

void Heap::deleteAllCodeBlocks(DeleteAllCodeEffort effort)
{
    UNUSED_PARAM(effort);
    // FIXME: Maybe it would be good to have some version of this work in Fil-C, since it's about
    // deleting bytecode that we don't need anymore.
}

void Heap::deleteAllUnlinkedCodeBlocks(DeleteAllCodeEffort effort)
{
    UNUSED_PARAM(effort);
    // FIXME: Maybe it would be good to have some version of this work in Fil-C, since it's about
    // deleting bytecode that we don't need anymore.
}

void Heap::deleteUnmarkedCompiledCode()
{
}

void Heap::addToRememberedSet(const JSCell* constCell)
{
    UNUSED_PARAM(constCell);
}

void Heap::sweepSynchronously()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::collect(Synchronousness synchronousness, GCRequest request)
{
    UNUSED_PARAM(request);
    
    if (UNLIKELY(!Options::useGC()))
        return;

    switch (synchronousness) {
    case Async: {
        // FIXME: Call FUGC API
        return;
    }
    case Sync:
        // FIXME: Call FUGC API
        return;
    }
    RELEASE_ASSERT_NOT_REACHED();
}

void Heap::collectNow(Synchronousness synchronousness, GCRequest request)
{
    UNUSED_PARAM(request);
    
    if (UNLIKELY(!Options::useGC()))
        return;

    switch (synchronousness) {
    case Async: {
        // FIXME: Call FUGC API
        return;
    }
        
    case Sync: {
        // FIXME: Call FUGC API
        return;
    } }
    RELEASE_ASSERT_NOT_REACHED();
}

void Heap::collectAsync(GCRequest request)
{
    UNUSED_PARAM(request);
    
    if (UNLIKELY(!Options::useGC()))
        return;

    // FIXME: Call FUGC API
}

void Heap::collectSync(GCRequest request)
{
    UNUSED_PARAM(request);
    
    if (UNLIKELY(!Options::useGC()))
        return;

    // FIXME: Call FUGC API
}

bool Heap::shouldCollectInCollectorThread(const AbstractLocker&)
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

void Heap::collectInCollectorThread()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::checkConn(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
}

auto Heap::runCurrentPhase(GCConductor conn, CurrentThreadState* currentThreadState) -> RunCurrentPhaseResult
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    UNUSED_PARAM(currentThreadState);
    return RunCurrentPhaseResult::Finished;
}

NEVER_INLINE bool Heap::runNotRunningPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

NEVER_INLINE bool Heap::runBeginPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

NEVER_INLINE bool Heap::runFixpointPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

NEVER_INLINE bool Heap::runConcurrentPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

NEVER_INLINE bool Heap::runReloopPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

NEVER_INLINE bool Heap::runEndPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

bool Heap::changePhase(GCConductor conn, CollectorPhase nextPhase)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    UNUSED_PARAM(nextPhase);
    return false;
}

NEVER_INLINE bool Heap::finishChangingPhase(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
    return false;
}

void Heap::stopThePeriphery(GCConductor conn)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(conn);
}

NEVER_INLINE void Heap::resumeThePeriphery()
{
    UNREACHABLE_FOR_PLATFORM();
}

bool Heap::stopTheMutator()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

NEVER_INLINE void Heap::resumeTheMutator()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::stopIfNecessarySlow()
{
    UNREACHABLE_FOR_PLATFORM();
}

bool Heap::stopIfNecessarySlow(unsigned oldState)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(oldState);
    return false;
}

NEVER_INLINE void Heap::collectInMutatorThread()
{
    UNREACHABLE_FOR_PLATFORM();
}

template<typename Func>
void Heap::waitForCollector(const Func& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(func);
}

void Heap::acquireAccessSlow()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::releaseAccessSlow()
{
    UNREACHABLE_FOR_PLATFORM();
}

bool Heap::relinquishConn(unsigned oldState)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(oldState);
    return false;
}

void Heap::finishRelinquishingConn()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::relinquishConn()
{
    UNREACHABLE_FOR_PLATFORM();
}

NEVER_INLINE bool Heap::handleNeedFinalize(unsigned oldState)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(oldState);
    return false;
}

void Heap::handleNeedFinalize()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::setNeedFinalize()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::waitWhileNeedFinalize()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::setMutatorWaiting()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::clearMutatorWaiting()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::notifyThreadStopping(const AbstractLocker&)
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::finalize()
{
    UNREACHABLE_FOR_PLATFORM();
}

Heap::Ticket Heap::requestCollection(GCRequest request)
{
    UNUSED_PARAM(request);
    UNREACHABLE_FOR_PLATFORM();
    return 0;
}

void Heap::waitForCollection(Ticket ticket)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(ticket);
}

void Heap::sweepInFinalize()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::suspendCompilerThreads()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::willStartCollection()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::prepareForMarking()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::reapWeakHandles()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::pruneStaleEntriesFromWeakGCHashTables()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::sweepArrayBuffers()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::snapshotUnswept()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::deleteSourceProviderCaches()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::notifyIncrementalSweeper()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::updateAllocationLimits()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::didFinishCollection()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::resumeCompilerThreads()
{
    UNREACHABLE_FOR_PLATFORM();
}

GCActivityCallback* Heap::fullActivityCallback()
{
    UNREACHABLE_FOR_PLATFORM();
    return nullptr;
}

GCActivityCallback* Heap::edenActivityCallback()
{
    UNREACHABLE_FOR_PLATFORM();
    return nullptr;
}

IncrementalSweeper& Heap::sweeper()
{
    return m_sweeper.get();
}

void Heap::setGarbageCollectionTimerEnabled(bool enable)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(enable);
}

void Heap::didAllocate(size_t bytes)
{
    UNUSED_PARAM(bytes);
}

void Heap::addFinalizer(JSCell* cell, CFinalizer finalizer)
{
    // FIXME: Use FUGC API here.
    UNUSED_PARAM(cell);
    UNUSED_PARAM(finalizer);
}

void Heap::addFinalizer(JSCell* cell, LambdaFinalizer function)
{
    // FIXME: Use FUGC API here.
    UNUSED_PARAM(cell);
    UNUSED_PARAM(function);
}

void Heap::collectNowFullIfNotDoneRecently(Synchronousness synchronousness)
{
    collectNow(synchronousness, CollectionScope::Full);
}

void Heap::setFullActivityCallback(RefPtr<GCActivityCallback>&& callback)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(callback);
}

void Heap::setEdenActivityCallback(RefPtr<GCActivityCallback>&& callback)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(callback);
}

bool Heap::useGenerationalGC()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

bool Heap::shouldSweepSynchronously()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

bool Heap::shouldDoFullCollection()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

void Heap::addLogicallyEmptyWeakBlock(WeakBlock* block)
{
    UNUSED_PARAM(block);
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::sweepAllLogicallyEmptyWeakBlocks()
{
    UNREACHABLE_FOR_PLATFORM();
}

bool Heap::sweepNextLogicallyEmptyWeakBlock()
{
    UNREACHABLE_FOR_PLATFORM();
    return false;
}

size_t Heap::visitCount()
{
    UNREACHABLE_FOR_PLATFORM();
    return 0;
}

size_t Heap::bytesVisited()
{
    UNREACHABLE_FOR_PLATFORM();
    return 0;
}

void Heap::forEachCodeBlockImpl(const ScopedLambda<void(CodeBlock*)>& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(func);
}

void Heap::forEachCodeBlockIgnoringJITPlansImpl(const AbstractLocker& locker, const ScopedLambda<void(CodeBlock*)>& func)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(locker);
    UNUSED_PARAM(func);
}

void Heap::writeBarrierSlowPath(const JSCell* from)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(from);
}

bool Heap::currentThreadIsDoingGCWork()
{
    return false;
}

void Heap::reportExtraMemoryVisited(size_t size)
{
    UNUSED_PARAM(size);
}

#if ENABLE(RESOURCE_USAGE)
void Heap::reportExternalMemoryVisited(size_t size)
{
    UNUSED_PARAM(size);
}
#endif

void Heap::collectIfNecessaryOrDefer(GCDeferralContext* deferralContext)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(deferralContext);
}

void Heap::decrementDeferralDepthAndGCIfNeededSlow()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::registerWeakGCHashTable(WeakGCHashTable* weakGCHashTable)
{
    UNUSED_PARAM(weakGCHashTable);
}

void Heap::unregisterWeakGCHashTable(WeakGCHashTable* weakGCHashTable)
{
    UNUSED_PARAM(weakGCHashTable);
}

void Heap::didAllocateBlock(size_t capacity)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(capacity);
}

void Heap::didFreeBlock(size_t capacity)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(capacity);
}

void Heap::addCoreConstraints()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::addMarkingConstraint(std::unique_ptr<MarkingConstraint> constraint)
{
    UNUSED_PARAM(constraint);
}

void Heap::notifyIsSafeToCollect()
{
}

// Use WTF_IGNORES_THREAD_SAFETY_ANALYSIS because this function conditionally locks m_collectContinuouslyLock,
// which is not supported by analysis.
void Heap::preventCollection() WTF_IGNORES_THREAD_SAFETY_ANALYSIS
{
}

// Use WTF_IGNORES_THREAD_SAFETY_ANALYSIS because this function conditionally unlocks m_collectContinuouslyLock,
// which is not supported by analysis.
void Heap::allowCollection() WTF_IGNORES_THREAD_SAFETY_ANALYSIS
{
}

void Heap::setMutatorShouldBeFenced(bool value)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(value);
}

void Heap::performIncrement(size_t bytes)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(bytes);
}

void Heap::addHeapFinalizerCallback(const HeapFinalizerCallback& callback)
{
    UNUSED_PARAM(callback);
}

void Heap::removeHeapFinalizerCallback(const HeapFinalizerCallback& callback)
{
    UNUSED_PARAM(callback);
}

void Heap::setBonusVisitorTask(RefPtr<SharedTask<void(SlotVisitor&)>> task)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(task);
}


void Heap::addMarkedJSValueRefArray(MarkedJSValueRefArray* array)
{
    UNUSED_PARAM(array);
}

void Heap::runTaskInParallel(RefPtr<SharedTask<void(SlotVisitor&)>> task)
{
    UNREACHABLE_FOR_PLATFORM();
    UNUSED_PARAM(task);
}

void Heap::verifyGC()
{
    UNREACHABLE_FOR_PLATFORM();
}

void Heap::scheduleOpportunisticFullCollection()
{
    UNREACHABLE_FOR_PLATFORM();
}

#define DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW(name, heapCellType, type) \
    IsoSubspace* Heap::name##Slow() \
    { \
        ASSERT(!m_##name); \
        auto space = makeUnique<IsoSubspace> ISO_SUBSPACE_INIT(*this, heapCellType, type); \
        WTF::storeStoreFence(); \
        m_##name = WTFMove(space); \
        return m_##name.get(); \
    }

FOR_EACH_JSC_DYNAMIC_ISO_SUBSPACE(DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW)

#undef DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW

#define DEFINE_DYNAMIC_SPACE_AND_SET_MEMBER_SLOW(name, heapCellType, type, spaceType) \
    IsoSubspace* Heap::name##Slow() \
    { \
        ASSERT(!m_##name); \
        auto space = makeUnique<spaceType> ISO_SUBSPACE_INIT(*this, heapCellType, type); \
        WTF::storeStoreFence(); \
        m_##name = WTFMove(space); \
        return &m_##name->space; \
    }

DEFINE_DYNAMIC_SPACE_AND_SET_MEMBER_SLOW(evalExecutableSpace, destructibleCellHeapCellType, EvalExecutable, Heap::ScriptExecutableSpaceAndSets) // Hash:0x958e3e9d
DEFINE_DYNAMIC_SPACE_AND_SET_MEMBER_SLOW(moduleProgramExecutableSpace, destructibleCellHeapCellType, ModuleProgramExecutable, Heap::ScriptExecutableSpaceAndSets) // Hash:0x6506fa3c

#undef DEFINE_DYNAMIC_SPACE_AND_SET_MEMBER_SLOW


namespace GCClient {

#define INIT_CLIENT_ISO_SUBSPACE_FROM_SPACE_AND_SET(subspace) subspace(heap.subspace##AndSet.space)

#define INIT_CLIENT_ISO_SUBSPACE(name, heapCellType, type) \
    , name(heap.name)

Heap::Heap(JSC::Heap& heap)
    : m_server(heap)
    FOR_EACH_JSC_ISO_SUBSPACE(INIT_CLIENT_ISO_SUBSPACE)
    , INIT_CLIENT_ISO_SUBSPACE_FROM_SPACE_AND_SET(codeBlockSpace)
    , INIT_CLIENT_ISO_SUBSPACE_FROM_SPACE_AND_SET(functionExecutableSpace)
    , INIT_CLIENT_ISO_SUBSPACE_FROM_SPACE_AND_SET(programExecutableSpace)
    , INIT_CLIENT_ISO_SUBSPACE_FROM_SPACE_AND_SET(unlinkedFunctionExecutableSpace)
{
}

Heap::~Heap()
{
}

#undef INIT_CLIENT_ISO_SUBSPACE
#undef CLIENT_ISO_SUBSPACE_INIT_FROM_SPACE_AND_SET


#define DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW_IMPL(name, heapCellType, type) \
    IsoSubspace* Heap::name##Slow() \
    { \
        ASSERT(!m_##name); \
        Locker locker { server().m_lock }; \
        JSC::IsoSubspace& serverSpace = *server().name<SubspaceAccess::OnMainThread>(); \
        auto space = makeUnique<IsoSubspace>(serverSpace); \
        WTF::storeStoreFence(); \
        m_##name = WTFMove(space); \
        return m_##name.get(); \
    }

#define DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW(name) \
    DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW_IMPL(name, unused, unused2) \

FOR_EACH_JSC_DYNAMIC_ISO_SUBSPACE(DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW_IMPL)

DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW(evalExecutableSpace)
DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW(moduleProgramExecutableSpace)

#undef DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW_IMPL
#undef DEFINE_DYNAMIC_ISO_SUBSPACE_MEMBER_SLOW

} // namespace GCClient

} // namespace JSC
