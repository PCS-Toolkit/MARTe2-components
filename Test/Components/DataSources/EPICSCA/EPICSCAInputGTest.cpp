/**
 * @file EPICSCAInputGTest.cpp
 * @brief Source file for class EPICSCAInputGTest
 * @date 21/04/2017
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

 * @details This source file contains the definition of all the methods for
 * the class EPICSCAInputGTest (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include <limits.h>
#include "gtest/gtest.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
#include "EPICSCAInputTest.h"

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
TEST(EPICSCAInputGTest,TestConstructor) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestConstructor());
}

TEST(EPICSCAInputGTest,TestAllocateMemory) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestAllocateMemory());
}

TEST(EPICSCAInputGTest,TestGetNumberOfMemoryBuffers) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestGetNumberOfMemoryBuffers());
}

TEST(EPICSCAInputGTest,TestGetSignalMemoryBuffer) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestGetSignalMemoryBuffer());
}

TEST(EPICSCAInputGTest,TestGetBrokerName) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestGetBrokerName());
}

TEST(EPICSCAInputGTest,TestGetBrokerName_OutputSignals) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestGetBrokerName_OutputSignals());
}

TEST(EPICSCAInputGTest,TestGetInputBrokers) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestGetInputBrokers());
}

TEST(EPICSCAInputGTest,TestGetOutputBrokers) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestGetOutputBrokers());
}

TEST(EPICSCAInputGTest,TestInitialise) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestInitialise());
}

TEST(EPICSCAInputGTest,TestInitialise_Defaults) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestInitialise_Defaults());
}

TEST(EPICSCAInputGTest,TestInitialise_False_Signals) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestInitialise_False_Signals());
}

TEST(EPICSCAInputGTest,TestSetConfiguredDatabase) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase());
}

TEST(EPICSCAInputGTest,TestSetConfiguredDatabase_False_NoSignals) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_NoSignals());
}

TEST(EPICSCAInputGTest,TestExecute) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestExecute());
}

TEST(EPICSCAInputGTest,TestExecute_Arrays) {
    EPICSCAInputTest test;
    ASSERT_TRUE(test.TestExecute_Arrays());
}
	
