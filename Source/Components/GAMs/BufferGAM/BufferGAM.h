/**
 * @file BufferGAM.h
 * @brief Header file for class BufferGAM
 * @date 6 Aug 2016
 * @author andre
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

 * @details This header file contains the declaration of the class BufferGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef BUFFERGAM_H_
#define BUFFERGAM_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "GAM.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief GAM which copies its inputs to its outputs.
 * @details This GAM copies its inputs to its outputs. The number of inputs shall
 *  be exactly the number of the outputs and, for each signal, the types shall also be the same.
 *
 * The configuration syntax is (names and signal quantity are only given as an example):
 * +Buffer = {
 *     Class = BufferGAM
 *     InputSignals = {
 *         Signal1 = {
 *             DataSource = "Drv1"
 *             Type = uint32
 *         }
 *         Signal2 = {
 *             DataSource = "Drv2"
 *             Type = int32
 *         }
 *     }
 *     OutputSignals = {
 *         Signal1O = {
 *             DataSource = "LCD"
 *             Type = uint32
 *         }
 *         Signal2O = {
 *             DataSource = "LCD"
 *             Type = int32
 *         }
 *     }
 * }
 */
namespace MARTe {
class BufferGAM: public GAM {
public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief Constructor. NOOP.
     */
    BufferGAM();

    /**
     * @brief Verifies correctness of the GAM configuration.
     * @details Checks that the number of input signals is equal to the number of output signals and that,
     * for each signal, the same type is used.
     * @return is the pre-conditions are met.
     * @pre
     *   SetConfiguredDatabase() &&
     *   GetNumberOfInputSignals() == GetNumberOfOutputSignals() &&
     *   for each signal i: GetSignalType(InputSignals, i) == GetSignalType(OutputSignals, i) &&
     *   for each signal i: GetSignalByteSize(InputSignals, i) == GetSignalByteSize(OutputSignals, i) &&
     */
    virtual bool Setup();

    /**
     * @brief Copies the input signals memory to the output signal memory.
     * @return true if all the signals memory can be successfully copied.
     */
    virtual bool Execute();

private:
    /**
     * Total number of bytes to copy.
     */
    uint32 totalSignalsByteSize;
};
}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* BUFFERGAM_H_ */

