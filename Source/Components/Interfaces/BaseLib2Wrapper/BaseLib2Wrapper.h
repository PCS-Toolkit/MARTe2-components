/**
 * @file BaseLib2Wrapper.h
 * @brief Header file for class BaseLib2Wrapper
 * @date 2/12/2016
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

 * @details This header file contains the declaration of the class BaseLib2Wrapper
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef BASELIB2WRAPPER_H_
#define BASELIB2WRAPPER_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdapterMessageListener.h"

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "QueuedMessageI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
namespace MARTe {

/**
 * @brief TODO
 */
class BaseLib2Wrapper : public Object, public QueuedMessageI, public BaseLib2::AdapterMessageListener {
public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief Default constructor. TODO
     */
    BaseLib2Wrapper();

    /**
     * @brief Destructor. TODO
     */
    virtual ~BaseLib2Wrapper();

    /**
     * @brief TODO Retrieves the GAM main parameters and adds the GAM to the BaseLib2::GAMAdapter.
     * @param[in] data configuration in the form described in the class description.
     * The parameters GAMName, BaseLib2Config and GAMFunctionNumber are compulsory.
     * @return true iff all the parameters set and if the GAM can be successfully added to the BaseLib2::GAMAdapter,
     * which in turn means that the BaseLib2Config could be successfully parsed.
     */
    virtual bool Initialise(StructuredDataI & data);

    /**
     * TODO
     */
    virtual bool HandleBaseLib2Message(const char8 *destination, const char8 *content, uint32 code);
};

}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* BASELIB2WRAPPER_H_ */

