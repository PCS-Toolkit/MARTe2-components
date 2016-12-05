/**
 * @file BaseLib2WrapperTest.h
 * @brief Header file for class BaseLib2WrapperTest
 * @date 05/12/2016
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

 * @details This header file contains the declaration of the class BaseLib2WrapperTest
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef BASELIB2WRAPPERTEST_H_
#define BASELIB2WRAPPERTEST_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief Tests the BaseLib2Wrapper public methods.
 */
class BaseLib2WrapperTest {
public:
    /**
     * @brief Tests the constructor.
     */
    bool TestConstructor();

    /**
     * @brief Tests the Initialise method.
     */
    bool TestInitialise();

    /**
     * @brief Tests the Initialise method without specifying the BaseLib2Config parameter.
     */
    bool TestInitialise_False_NoBaseLib2Config();

    /**
     * @brief Tests the Initialise method specifying a wrong BaseLib2Config parameter.
     */
    bool TestInitialise_False_BadBaseLib2Config();

    /**
     * @brief Tests the HandleBaseLib2Message method.
     */
    bool TestHandleBaseLib2Message();
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* BASELIB2WRAPPERTEST_H_ */

