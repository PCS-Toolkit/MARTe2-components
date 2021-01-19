/**
 * @file ISynchronisableInput.h
 * @brief Header file for class ISynchronisableInput
 * @date 15/01/2021
 * @author Giuseppe Avon
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

 * @details This header file contains the declaration of the class (interface) ISynchronisableInput
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef DATASOURCES_PROFINET_ISYNCHRONISABLEINPUT_H_
#define DATASOURCES_PROFINET_ISYNCHRONISABLEINPUT_H_

namespace MARTe {
    class ISynchronisableInput {
        /**
         * @brief     Defines a specialization of the MARTe default Synchronise method
         *            in which differentiation between input and output synchronisation
         *            shall occur.
         */
        public:
            /**
             * @brief Entry point to inform that an input synchronisation is occurring.
             */
            virtual bool SynchroniseInput() = 0;
    };
}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DATASOURCES_PROFINET_ISYNCHRONISABLEINPUT_H_ */