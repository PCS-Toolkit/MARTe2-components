/**
 * @file EPICSPVAOutput.h
 * @brief Header file for class EPICSPVAOutput
 * @date 20/04/2017
 * @author Andre Neto
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing, 
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This header file contains the declaration of the class EPICSPVAOutput
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef EPICSPVAOUTPUT_H_
#define EPICSPVAOUTPUT_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "DataSourceI.h"
#include "EmbeddedServiceMethodBinderI.h"
#include "EPICSPVAChannelWrapper.h"
#include "EventSem.h"
#include "SingleThreadService.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
namespace MARTe {
/**
 * @brief A DataSource which allows to output data into any number of records using the EPICS PVA client protocol.
 * Data is asynchronously written in the PVA in the context of a different thread (w.r.t. to the real-time thread).
 *
 * Each signal root name defines the name of the record (signal).
 * The configuration syntax is (names are only given as an example):
 *
 * <pre>
 * +EPICSPVAOutput_1 = {
 *     Class = EPICSPVA::EPICSPVAOutput
 *     StackSize = 1048576 //Optional the EmbeddedThread stack size. Default value is THREADS_DEFAULT_STACKSIZE * 4u
 *     CPUs = 0xff //Optional the affinity of the EmbeddedThread which actually performs the PVA puts.
 *     IgnoreBufferOverrun = 1 //Optional. If true no error will be triggered when the thread that writes into EPICS does not consume the data fast enough.
 *     NumberOfBuffers = 10 //Compulsory. Number of buffers in a circular buffer that asynchronously writes the values. Each buffer is capable of holding a copy of all the DataSourceI signals.
 *     Signals = {
 *         RecordOut1 = {//Record name if the Alias field is not set
 *             Alias = "alternative::channel::name"
 *             UnsignedIntegers = {
 *                 UInt8 = {
 *                     Type = uint8
 *                     NumberOfElements = 8
 *                 }
 *                 UInt16 = {
 *                     Type = uint16
 *                     NumberOfElements = 1
 *                 }
 *                 UInt32 = {
 *                     Type = uint32
 *                     NumberOfElements = 1
 *                 }
 *                 UInt64 = {
 *                     Type = uint64
 *                     NumberOfElements = 1
 *                 }
 *             }
 *         }
 *         RecordOut2 = {
 *             SignedIntegers = {
 *                 Int8 = {
 *                     Type = int8
 *                     NumberOfElements = 2
 *                 }
 *                 Int16 = {
 *                     Type = int16
 *                     NumberOfElements = 4
 *                 }
 *                 Int32 = {
 *                     Type = int32
 *                     NumberOfElements = 1
 *                 }
 *                 Int64 = {
 *                     Type = int64
 *                     NumberOfElements = 1
 *                 }
 *             }
 *         }
 *         ...
 *     }
 * }
 *
 * </pre>
 */
class EPICSPVAOutput: public DataSourceI {
public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief Default constructor. NOOP.
     */
EPICSPVAOutput    ();

    /**
     * @brief Destructor.
     * @details TODO.
     */
    virtual ~EPICSPVAOutput();

    /**
     * @brief See DataSourceI::AllocateMemory. NOOP.
     * @return true.
     */
    virtual bool AllocateMemory();

    /**
     * @brief See DataSourceI::GetNumberOfMemoryBuffers.
     * @return 1.
     */
    virtual uint32 GetNumberOfMemoryBuffers();

    /**
     * @brief See DataSourceI::GetSignalMemoryBuffer.
     * @pre
     *   SetConfiguredDatabase
     */
    virtual bool GetSignalMemoryBuffer(const uint32 signalIdx,
            const uint32 bufferIdx,
            void *&signalAddress);

    /**
     * @brief See DataSourceI::GetNumberOfMemoryBuffers.
     * @details Only OutputSignals are supported.
     * @return MemoryMapAsyncOutputBroker.
     */
    virtual const char8 *GetBrokerName(StructuredDataI &data,
            const SignalDirection direction);

    /**
     * @brief See DataSourceI::PrepareNextState. NOOP.
     * @return true.
     */
    virtual bool PrepareNextState(const char8 * const currentStateName,
            const char8 * const nextStateName);

    /**
     * @brief Loads and verifies the configuration parameters detailed in the class description.
     * @return true if all the mandatory parameters are correctly specified and if the specified optional parameters have valid values.
     */
    virtual bool Initialise(StructuredDataI & data);

    /**
     * @brief Final verification of all the parameters. Setup of the memory required to hold all the signals.
     * @details This method verifies that all the parameters requested by the GAMs interacting with this DataSource
     *  are valid and consistent with the parameters set during the initialisation phase.
     * In particular the following conditions shall be met:
     * - All the signals have NumberOfSamples = 1
     * - Only one Function writes into this DataSourceI
     * @return true if all the parameters are valid and the conditions above are met.
     */
    virtual bool SetConfiguredDatabase(StructuredDataI & data);

    /**
     * @brief Gets the affinity of the thread which is going to be used to asynchronously write data with pva::put.
     * @return the affinity of the thread which is going to be used to asynchronously write data with pva::put.
     */
    uint32 GetCPUMask() const;

    /**
     * @brief Gets the stack size of the thread which is going to be used to asynchronously write data with pva::put.
     * @return the stack size of the thread which is going to be used to asynchronously write data with pva::put.
     */
    uint32 GetStackSize() const;

    /**
     * @brief Gets the number of buffers in the circular buffer that asynchronously writes the pva values.
     * @return the number of buffers in the circular buffer that asynchronously writes the pva values.
     */
    uint32 GetNumberOfBuffers() const;

    /**
     * @brief Provides the context to execute all the EPICS calls.
     * @return true if all the all the variables can be successfully set and exec.
     */
    virtual bool Synchronise();

    /**
     * @brief See DataSourceI::GetOutputBrokers.
     * @details adds a memory MemoryMapOutputBroker instance to the outputBrokers
     * @return true.
     */
    virtual bool GetOutputBrokers(ReferenceContainer &outputBrokers,
            const char8* const functionName,
            void * const gamMemPtr);

    /**
     * @brief Gets if buffer overruns is being ignored (i.e. the consumer thread which writes into EPICS is not consuming the data fast enough).
     * @return if true no error is to be triggered when there is a buffer overrun.
     */
    bool IsIgnoringBufferOverrun() const;

private:

    /**
     * One channel per signal at the root level.
     */
    EPICSPVAChannelWrapper *channelList;

    /**
     * Number of channels (records).
     */
    uint32 numberOfChannels;

    /**
     * The CPU mask for the executor
     */
    uint32 cpuMask;

    /**
     * The stack size
     */
    uint32 stackSize;

    /**
     * The number of buffers for the circular buffer that flushes data into EPICS
     */
    uint32 numberOfBuffers;

    /**
     * If true no error will be triggered when the data cannot be consumed by the thread doing the pva::set/exec.
     */
    uint32 ignoreBufferOverrun;
};
}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* EPICSPVAOUTPUT_H_ */

