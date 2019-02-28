/**
 * @file EPICSPVARecordTest.h
 * @brief Header file for class EPICSPVARecordTest
 * @date 13/10/2018
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

 * @details This header file contains the declaration of the class EPICSPVARecordTest
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef EPICSPVA_EPICSPVARECORDTEST_H_
#define EPICSPVA_EPICSPVARECORDTEST_H_

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
 * @brief Tests the EPICSPVARecord public methods.
 */
class EPICSPVARecordTest {
public:
    /**
     * @brief Tests the constructor.
     */
    bool TestConstructor();

    /**
     * @brief Tests the CreatePVRecord method.
     */
    bool TestCreatePVRecord();

    /**
     * @brief Tests the Initialise method.
     */
    bool TestInitialise();

    /**
     * @brief Tests that the Initialise method fails if no Structure field is defined.
     */
    bool TestInitialise_False_NoStructure();

    /**
     * @brief Tests that the Initialise method fails if an invalid type is specified.
     */
    bool TestInitialise_False_InvalidType();

    /**
     * @brief Tests that the Initialise method fails if a string type is specified.
     */
    bool TestInitialise_False_InvalidType_CharString();

    /**
     * @brief Tests that the Initialise method fails if no type is specified.
     */
    bool TestInitialise_False_NoType();

    /**
     * @brief Tests that the Initialise method fails if the type has no introspection.
     */
    bool TestInitialise_False_NoIntrospection();

    /**
     * @brief Tests that the Initialise method reports a warning if an array of strings is declared.
     */
    bool TestInitialise_Warning_Char8Array();

    /**
     * @brief Test the GetRecordName method.
     */
    bool TestGetRecordName();
};


/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* EPICSPVA_EPICSPVARECORDTEST_H_ */
