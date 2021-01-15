/**
 * @file MemoryMapSynchNMutexOutputBroker.h
 * @brief Specialized MemoryMapOutputBroker with output-related synch features.
 * @date 
 * @author
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

 * @details This header file contains the declaration of the class MemoryMapSynchNMutexOutputBroker
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef DATASOURCES_PROFINET_MEMMAPSYNCHNMUTEXOUTPUTBROKER_H_
#define DATASOURCES_PROFINET_MEMMAPSYNCHNMUTEXOUTPUTBROKER_H_

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "MemoryMapOutputBroker.h"
#include "ISynchronisableOutput.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

    /**
    * @brief    Specialization of the MemoryMapOutputBroker, which leverages ISynchronisableOutput
    *           interface, implemented by the DataSource, to synchronise outputs.
    *           Before starting with the copy the Synchronise is called on outputs (SynchroniseOutput)
    *           while the DataSource is expected to mutually exclude memory access, until the broker signals
    *           using the TerminateOutputCopy its ending.
    */
    class MemoryMapSynchNMutexOutputBroker : public MemoryMapOutputBroker {

        public:
            CLASS_REGISTER_DECLARATION()

            /**
             * @brief Constructs the MemoryMapSynchNMutexOutputBroker instance.
             *        Refer to the parent classes for detailed info.
            */
            MemoryMapSynchNMutexOutputBroker();

            /**
             * @brief Destructs the MemoryMapSynchNMutexOutputBroker instance.
             *        Refer to the parent classes for detailed info.
            */
            virtual ~MemoryMapSynchNMutexOutputBroker();

            /**
             * @brief Runs the execute method on the DataSource, causing the input/output image copy.
             * This specific implementation locks outputs on the DataSource, using a SynchroniseOutput before copying
             * and releases it with the shipped TerminateOutputCopy, used without input parameters.
             */
            virtual bool Execute();

    };

}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DATASOURCES_PROFINET_MEMMAPSYNCHNMUTEXOUTPUTBROKER_H_ */