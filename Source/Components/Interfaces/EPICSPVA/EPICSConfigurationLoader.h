/**
 * @file EPICSConfigurationLoader.h
 * @brief Header file for class EPICSConfigurationLoader
 * @date 18/06/2018
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

 * @details This header file contains the declaration of the class EPICSConfigurationLoader
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef EPICSCONFIGURATIONLOADER_H_
#define EPICSCONFIGURATIONLOADER_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/
#include "pv/rpcService.h"

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "Object.h"
#include "ReferenceT.h"
#include "StreamString.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief TODO
 */
namespace MARTe {
class epicsShareClass EPICSConfigurationLoader: public virtual epics::pvAccess::RPCService, public Object {
public:
    POINTER_DEFINITIONS(EPICSConfigurationLoader);
    CLASS_REGISTER_DECLARATION()
    /**
     * @brief TODO
     */
    EPICSConfigurationLoader();

    /**
     * @brief TODO
     */
    virtual ~EPICSConfigurationLoader();

    /**
     * @brief TODO
     */
    epics::pvData::PVStructurePtr request(epics::pvData::PVStructure::shared_pointer const & args) throw (epics::pvAccess::RPCRequestException);

};
}
/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* EPICSCONFIGURATIONLOADER_H_ */
