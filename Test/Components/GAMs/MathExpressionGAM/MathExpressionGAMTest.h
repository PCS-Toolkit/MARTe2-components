/**
 * @file MathExpressionGAMTest.h
 * @brief Header file for class IOGAMGTest
 * @date 03/06/2020
 * @author RFX
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

 * @details This header file contains the declaration of the class IOGAMTest
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef MATHEXPRESSIONGAMTEST_H_
#define MATHEXPRESSIONGAMTEST_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

#include "MathExpressionGAM.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

using namespace MARTe;

/**
 * @brief Tests the MathExpressionGAM public methods.
 */
class MathExpressionGAMTest {
    
public:
    /**
     * @brief Tests the constructor.
     */
    bool TestConstructor();
    
    /**
     * @brief Tests the constructor.
     */
    bool TestInitialise(const char8* expression);
    
    ///**
     //* @brief Tests the Initialise method.
     //*/
    //bool TestInitialise();
    
    ///**
     //* @brief Tests the Setup method.
     //*/
    //bool TestSetup();

    ///**
     //* @brief Tests the Setup method with a different number of inputs and output signals.
     //*/
    //bool TestSetup_LessInputs();

    ///**
     //* @brief Tests the Setup method with different signal types.
     //*/
    //bool TestSetup_DifferentType();

    ///**
     //* @brief Tests the Setup method with different signal sizes.
     //*/
    //bool TestSetup_False_DifferentSize();

    ///**
     //* @brief Tests the Execute method.
     //*/
    //bool TestExecute();

    ///**
     //* @brief Tests the Execute method with samples > 0.
     //*/
    //bool TestExecute_Samples();
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* MATHEXPRESSIONGAMTEST_H_ */

