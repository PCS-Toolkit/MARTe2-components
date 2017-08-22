/**
 * @file FileWriterTest.cpp
 * @brief Source file for class FileWriterTest
 * @date 21/08/2017
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
 * the class FileWriterTest (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "Directory.h"
#include "FileWriter.h"
#include "FileWriterTest.h"
#include "GAM.h"
#include "ObjectRegistryDatabase.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/**
 * Helper class that reacts to messages received from the FileWriter class
 */
class FileWriterTestHelper: public MARTe::Object, public MARTe::MessageI {
public:
    CLASS_REGISTER_DECLARATION()FileWriterTestHelper() : Object(), MessageI() {
        using namespace MARTe;
        pulseNumber = 0u;
        openFailFunctionCalled = false;
        openOKFunctionCalled = false;
        flushSegmentsFunctionCalled = false;
        closeFunctionCalled = false;
        ReferenceT<RegisteredMethodsMessageFilter> filter = ReferenceT<RegisteredMethodsMessageFilter>(GlobalObjectsDatabase::Instance()->GetStandardHeap());
        filter->SetDestination(this);
        ErrorManagement::ErrorType ret = MessageI::InstallMessageFilter(filter);
        if (!ret.ErrorsCleared()) {
            REPORT_ERROR(ErrorManagement::FatalError, "Failed to install message filters");
        }
    }

    MARTe::ErrorManagement::ErrorType HandleOpenFail() {
        openFailFunctionCalled = true;
        return MARTe::ErrorManagement::NoError;
    }

    MARTe::ErrorManagement::ErrorType HandleOpenOK(const MARTe::int32 newValue) {
        pulseNumber = newValue;
        openOKFunctionCalled = true;
        return MARTe::ErrorManagement::NoError;
    }

    MARTe::ErrorManagement::ErrorType HandleFlushSegments() {
        flushSegmentsFunctionCalled = true;
        return MARTe::ErrorManagement::NoError;
    }

    MARTe::ErrorManagement::ErrorType HandleClose() {
        closeFunctionCalled = true;
        return MARTe::ErrorManagement::NoError;
    }

    MARTe::int32 pulseNumber;
    bool openOKFunctionCalled;
    bool openFailFunctionCalled;
    bool closeFunctionCalled;
    bool flushSegmentsFunctionCalled;
};
CLASS_REGISTER(FileWriterTestHelper, "1.0")
CLASS_METHOD_REGISTER(FileWriterTestHelper, HandleOpenOK)
CLASS_METHOD_REGISTER(FileWriterTestHelper, HandleOpenFail)
CLASS_METHOD_REGISTER(FileWriterTestHelper, HandleFlushSegments)
CLASS_METHOD_REGISTER(FileWriterTestHelper, HandleClose)

/**
 * @brief GAM which generates a given signal trigger, time and signal pattern which is then sinked to the FileWriter
 */
class FileWriterGAMTriggerTestHelper: public MARTe::GAM {
public:
    CLASS_REGISTER_DECLARATION()FileWriterGAMTriggerTestHelper() {
        counter = 0;
        period = 2;
        numberOfExecutes = 0;
        signalToGenerate = NULL;
        triggerToGenerate = NULL;
        elements = NULL;
        invertSigned = 0;
    }

    virtual ~FileWriterGAMTriggerTestHelper() {
        using namespace MARTe;
        if (signalToGenerate != NULL) {
            delete [] signalToGenerate;
        }
        if (triggerToGenerate != NULL) {
            delete [] triggerToGenerate;
        }
        if (elements != NULL) {
            delete [] elements;
        }
    }

    virtual bool Initialise(MARTe::StructuredDataI & data) {
        using namespace MARTe;
        bool ok = GAM::Initialise(data);
        counter = 0;
        numberOfExecutes = 0;
        period = 2;

        AnyType signalAT = data.GetType("Signal");
        numberOfExecutes = signalAT.GetNumberOfElements(0);
        signalToGenerate = new uint32[numberOfExecutes];
        triggerToGenerate = new uint8[numberOfExecutes];
        Vector<uint32> signalV(signalToGenerate, numberOfExecutes);
        data.Read("Signal", signalV);
        Vector<uint8> triggerV(triggerToGenerate, numberOfExecutes);
        data.Read("Trigger", triggerV);
        data.Read("Period", period);
        data.Read("InvertSigned", invertSigned);
        return ok;
    }

    virtual bool Setup() {
        using namespace MARTe;
        elements = new uint32[GetNumberOfOutputSignals()];
        uint32 n;
        for (n=0; n<GetNumberOfOutputSignals(); n++) {
            uint32 el;
            GetSignalNumberOfElements(OutputSignals, n, el);
            elements[n] = el;
        }
        return true;
    }

    virtual bool Execute() {
        using namespace MARTe;
        if (counter < numberOfExecutes) {
            uint32 n;
            uint8 *trigger = reinterpret_cast<uint8 *>(GetOutputSignalMemory(0));
            *trigger = triggerToGenerate[counter];
            uint32 *timeSignal = reinterpret_cast<uint32 *>(GetOutputSignalMemory(1));
            *timeSignal = (counter * (period * 1e6) * elements[2]);
            int32 signInv = -1;
            if (invertSigned) {
                if ((counter %2) == 0) {
                    signInv = 1;
                }
            }
            for (n=2; n<GetNumberOfOutputSignals(); n++) {
                TypeDescriptor signalType = GetSignalType(OutputSignals, n);
                if (signalType == UnsignedInteger8Bit) {
                    uint8 *signalOut = reinterpret_cast<uint8 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = static_cast<uint8>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == UnsignedInteger16Bit) {
                    uint16 *signalOut = reinterpret_cast<uint16 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = static_cast<uint16>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == UnsignedInteger32Bit) {
                    uint32 *signalOut = reinterpret_cast<uint32 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = static_cast<uint32>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == UnsignedInteger64Bit) {
                    uint64 *signalOut = reinterpret_cast<uint64 *>(GetOutputSignalMemory(n));
                    uint64 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = static_cast<uint64>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == SignedInteger8Bit) {
                    int8 *signalOut = reinterpret_cast<int8 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = signInv * static_cast<int8>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == SignedInteger16Bit) {
                    int16 *signalOut = reinterpret_cast<int16 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = signInv * static_cast<int16>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == SignedInteger32Bit) {
                    int32 *signalOut = reinterpret_cast<int32 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = signInv * static_cast<int32>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == SignedInteger64Bit) {
                    int64 *signalOut = reinterpret_cast<int64 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = signInv * static_cast<int64>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == Float32Bit) {
                    float32 *signalOut = reinterpret_cast<float32 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = signInv * static_cast<float32>(signalToGenerate[counter]);
                    }
                }
                else if (signalType == Float64Bit) {
                    float64 *signalOut = reinterpret_cast<float64 *>(GetOutputSignalMemory(n));
                    uint32 e;
                    for (e=0; e<elements[n]; e++) {
                        signalOut[e] = signInv * static_cast<float64>(signalToGenerate[counter]);
                    }
                }
            }

        }
        counter++;
        return true;
    }

    MARTe::uint8 *triggerToGenerate;
    MARTe::uint32 *signalToGenerate;
    MARTe::uint32 *elements;
    MARTe::uint32 counter;
    MARTe::uint32 invertSigned;
    MARTe::uint32 numberOfExecutes;
    MARTe::float32 period;
};
CLASS_REGISTER(FileWriterGAMTriggerTestHelper, "1.0")

/**
 * @brief Manual scheduler to test the correct interface between the FileWriter and the GAMs
 */
class FileWriterSchedulerTestHelper: public MARTe::GAMSchedulerI {
public:

    CLASS_REGISTER_DECLARATION()

FileWriterSchedulerTestHelper    () : MARTe::GAMSchedulerI() {
        scheduledStates = NULL;
    }

    virtual MARTe::ErrorManagement::ErrorType StartNextStateExecution() {
        return MARTe::ErrorManagement::NoError;
    }

    virtual MARTe::ErrorManagement::ErrorType StopCurrentStateExecution() {
        return MARTe::ErrorManagement::NoError;
    }

    void ExecuteThreadCycle(MARTe::uint32 threadId) {
        using namespace MARTe;
        ExecuteSingleCycle(scheduledStates[RealTimeApplication::GetIndex()]->threads[threadId].executables,
                scheduledStates[RealTimeApplication::GetIndex()]->threads[threadId].numberOfExecutables);
    }

    virtual bool ConfigureScheduler() {

        bool ret = GAMSchedulerI::ConfigureScheduler();
        if (ret) {
            scheduledStates = GetSchedulableStates();
        }
        return ret;
    }

    virtual void CustomPrepareNextState() {
    }

private:

    MARTe::ScheduledState * const * scheduledStates;
};

CLASS_REGISTER(FileWriterSchedulerTestHelper, "1.0")

#if 0
static bool TestIntegratedInApplication(const MARTe::char8 * const config, bool destroy) {
    using namespace MARTe;

    ConfigurationDatabase cdb;
    StreamString configStream = config;
    configStream.Seek(0);
    StreamString err;
    StandardParser parser(configStream, cdb, &err);

    bool ok = parser.Parse();
    if (!ok) {
        REPORT_ERROR_STATIC(ErrorManagement::FatalError, "%s", err.Buffer());
    }

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();

    if (ok) {
        god->Purge();
        ok = god->Initialise(cdb);
    }
    ReferenceT<RealTimeApplication> application;
    if (ok) {
        application = god->Find("Test");
        ok = application.IsValid();
    }
    if (ok) {
        ok = application->ConfigureApplication();
    }

    if (destroy) {
        god->Purge();
    }
    return ok;
}
#endif

static bool TestIntegratedExecution(const MARTe::char8 * const config, MARTe::uint32 *signalToGenerate, MARTe::uint32 toGenerateNumberOfElements,
                                    MARTe::uint8 *triggerToGenerate, MARTe::uint32 numberOfElements, MARTe::uint32 numberOfBuffers,
                                    MARTe::uint32 numberOfPreTriggers, MARTe::uint32 numberOfPostTriggers, MARTe::float32 period,
                                    const MARTe::char8 * const filename, const MARTe::char8 * const expectedFileContent, bool csv,
                                    const MARTe::uint32 sleepMSec = 100) {
    using namespace MARTe;
    ConfigurationDatabase cdb;
    StreamString configStream = config;
    configStream.Seek(0);
    StandardParser parser(configStream, cdb);

    bool ok = parser.Parse();

    cdb.MoveAbsolute("$Test.+Functions.+GAM1");
    cdb.Delete("Signal");
    Vector<uint32> signalV(signalToGenerate, toGenerateNumberOfElements);
    cdb.Write("Signal", signalV);
    cdb.Delete("Period");
    cdb.Write("Period", period);
    cdb.Delete("Trigger");

    bool triggerToGenerateWasNULL = false;
    if (triggerToGenerate == NULL) {
        triggerToGenerate = new uint8[toGenerateNumberOfElements];
        uint32 t;
        for (t = 0; t < toGenerateNumberOfElements; t++) {
            triggerToGenerate[t] = 0;
        }
        triggerToGenerateWasNULL = true;
    }

    Vector<uint8> triggerV(triggerToGenerate, toGenerateNumberOfElements);
    cdb.Write("Trigger", triggerV);

    cdb.MoveAbsolute("$Test.+Data.+Drv1");
    cdb.Delete("NumberOfBuffers");
    cdb.Write("NumberOfBuffers", numberOfBuffers);
    cdb.Delete("NumberOfPreTriggers");
    cdb.Write("NumberOfPreTriggers", numberOfPreTriggers);
    cdb.Delete("NumberOfPostTriggers");
    cdb.Write("NumberOfPostTriggers", numberOfPostTriggers);
    cdb.Delete("Filename");
    cdb.Write("Filename", filename);
    cdb.Delete("StoreOnTrigger");
    uint32 storeOnTrigger = (triggerToGenerateWasNULL ? 0 : 1);
    cdb.Write("StoreOnTrigger", storeOnTrigger);

    cdb.Delete("FileFormat");
    if (csv) {
        cdb.Write("FileFormat", "csv");
        cdb.Delete("CSVSeparator");
        cdb.Write("CSVSeparator", ";");
    }
    else {
        cdb.Write("FileFormat", "binary");
    }
    cdb.MoveRelative("Signals");

    uint32 i;
    uint32 numberOfChildren = cdb.GetNumberOfChildren();
    //Skip trigger and time signal
    for (i = 2u; i < numberOfChildren; i++) {
        cdb.MoveToChild(i);
        cdb.Delete("NumberOfElements");
        cdb.Write("NumberOfElements", numberOfElements);
        cdb.MoveToAncestor(1u);
    }

    cdb.MoveToRoot();

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();

    if (ok) {
        god->Purge();
        ok = god->Initialise(cdb);
    }
    ReferenceT<RealTimeApplication> application;
    ReferenceT<FileWriterSchedulerTestHelper> scheduler;
    ReferenceT<FileWriterGAMTriggerTestHelper> gam;
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    if (ok) {
        application = god->Find("Test");
        ok = application.IsValid();
    }
    if (ok) {
        ok = application->ConfigureApplication();
    }
    if (ok) {
        scheduler = application->Find("Scheduler");
        ok = scheduler.IsValid();
    }
    if (ok) {
        gam = application->Find("Functions.GAM1");
        ok = gam.IsValid();
    }
    ReferenceT<FileWriter> fileWriter;
    if (ok) {
        fileWriter = godb->Find("Test.Data.Drv1");
        ok = fileWriter.IsValid();
    }
    if (ok) {
        ok = application->PrepareNextState("State1");
    }
    if (ok) {
        ok = application->StartNextStateExecution();
    }

    if (ok) {
        for (i = 0; (i < gam->numberOfExecutes); i++) {
            scheduler->ExecuteThreadCycle(0);
            Sleep::MSec(sleepMSec);
        }
    }
    if (ok) {
        ok = application->StopCurrentStateExecution();
    }

    if (ok) {
        //If the number of elements is not a multiple of the segment size then we have to flush so that an extra segment
        //with the missing data is added.
        ReferenceT<Message> messageFlush = ObjectRegistryDatabase::Instance()->Find("TestMessages.MessageFlush");
        if (ok) {
            ok = messageFlush.IsValid();
        }
        if (ok) {
            MessageI::SendMessage(messageFlush, NULL);
        }
    }
    godb->Purge();
    File generatedFile;
    if (ok) {
        ok = generatedFile.Open(filename, BasicFile::ACCESS_MODE_R);
    }
    if (ok) {
        const uint32 BUFFER_SIZE = 64u;
        char8 buffer[BUFFER_SIZE];
        uint32 readSize = BUFFER_SIZE;

        generatedFile.Seek(0u);
        uint32 z = 0u;
        bool readOnce = false;
        while (generatedFile.Read(&buffer[0], readSize)) {
            if (readSize == 0u) {
                break;
            }
            readOnce = true;
            uint32 r;
            for (r = 0u; (r < readSize) && (ok); r++) {
                ok = (buffer[r] == expectedFileContent[z]);
                printf("[%x] vs [%x]\n", (uint8) buffer[r], (uint8) expectedFileContent[z]);
                z++;
            }
            readSize = BUFFER_SIZE;
        }
        if (ok) {
            ok = readOnce;
        }
    }
    generatedFile.Close();
    Directory toDelete(filename);
    //toDelete.Delete();

    if (triggerToGenerateWasNULL) {
        delete[] triggerToGenerate;
    }

    return ok;
}

//Standard configuration with no trigger source
static const MARTe::char8 * const config1 = ""
        "$Test = {"
        "    Class = RealTimeApplication"
        "    +Functions = {"
        "        Class = ReferenceContainer"
        "        +GAM1 = {"
        "            Class = FileWriterGAMTriggerTestHelper"
        "            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
        "            InvertSigned = 1"
        "            OutputSignals = {"
        "                Trigger = {"
        "                    Type = uint8"
        "                    DataSource = Drv1"
        "                }"
        "                Time = {"
        "                    Type = uint32"
        "                    DataSource = Drv1"
        "                }"
        "                SignalUInt8 = {"
        "                    Type = uint8"
        "                    DataSource = Drv1"
        "                }"
        "                SignalUInt16 = {"
        "                    Type = uint16"
        "                    DataSource = Drv1"
        "                }"
        "                SignalUInt32 = {"
        "                    Type = uint32"
        "                    DataSource = Drv1"
        "                }"
        "                SignalUInt64 = {"
        "                    Type = uint64"
        "                    DataSource = Drv1"
        "                }"
        "                SignalInt8 = {"
        "                    Type = int8"
        "                    DataSource = Drv1"
        "                }"
        "                SignalInt16 = {"
        "                    Type = int16"
        "                    DataSource = Drv1"
        "                }"
        "                SignalInt32 = {"
        "                    Type = int32"
        "                    DataSource = Drv1"
        "                }"
        "                SignalInt64 = {"
        "                    Type = int64"
        "                    DataSource = Drv1"
        "                }"
        "                SignalFloat32 = {"
        "                    Type = float32"
        "                    DataSource = Drv1"
        "                }"
        "                SignalFloat64 = {"
        "                    Type = float64"
        "                    DataSource = Drv1"
        "                }"
        "            }"
        "        }"
        "    }"
        "    +Data = {"
        "        Class = ReferenceContainer"
        "        DefaultDataSource = DDB1"
        "        +Timings = {"
        "            Class = TimingDataSource"
        "        }"
        "        +Drv1 = {"
        "            Class = FileWriter"
        "            NumberOfBuffers = 10"
        "            CPUMask = 15"
        "            StackSize = 10000000"
        "            Filename = \"filewriter_test.bin\""
        "            FileFormat = csv"
        "            CSVSeparator = \";\""
        "            Overwrite = yes"
        "            StoreOnTrigger = 0"
        "            Signals = {"
        "                Trigger = {"
        "                    Type = uint8"
        "                }"
        "                Time = {"
        "                    Type = uint32"
        "                }"
        "                SignalUInt8 = {"
        "                    Type = uint8"
        "                }"
        "                SignalUInt16 = {"
        "                    Type = uint16"
        "                }"
        "                SignalUInt32 = {"
        "                    Type = uint32"
        "                }"
        "                SignalUInt64 = {"
        "                    Type = uint64"
        "                }"
        "                SignalInt8 = {"
        "                    Type = int8"
        "                }"
        "                SignalInt16 = {"
        "                    Type = int16"
        "                }"
        "                SignalInt32 = {"
        "                    Type = int32"
        "                }"
        "                SignalInt64 = {"
        "                    Type = int64"
        "                }"
        "                SignalFloat32 = {"
        "                    Type = float32"
        "                }"
        "                SignalFloat64 = {"
        "                    Type = float64"
        "                }"
        "            }"
        "        }"
        "    }"
        "    +States = {"
        "        Class = ReferenceContainer"
        "        +State1 = {"
        "            Class = RealTimeState"
        "            +Threads = {"
        "                Class = ReferenceContainer"
        "                +Thread1 = {"
        "                    Class = RealTimeThread"
        "                    Functions = {GAM1}"
        "                }"
        "            }"
        "        }"
        "    }"
        "    +Scheduler = {"
        "        Class = FileWriterSchedulerTestHelper"
        "        TimingDataSource = Timings"
        "    }"
        "}"
        "+TestMessages = {"
        "    Class = ReferenceContainer"
        "    +MessageFlush = {"
        "        Class = Message"
        "        Destination = \"Test.Data.Drv1\""
        "        Function = FlushFile"
        "    }"
        "}";

#if 0
//Standard configuration with trigger source
static const MARTe::char8 * const config2 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            Trigger =  {0 1 0 1 0 1 0 1 1 1 1 1 1 1}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalUInt32F = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt64F = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"                SignalInt32F = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                }"
"                SignalInt64F = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                }"
"                SignalFloat32F = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                }"
"                SignalFloat64F = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16 = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalUInt32 = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt64 = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                }"
"                SignalInt16 = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"                SignalInt32 = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                }"
"                SignalInt64 = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                }"
"                SignalFloat32 = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                }"
"                SignalFloat64 = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = 1"
"            StoreOnTrigger = 1"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            NumberOfPreTriggers = 2"
"            NumberOfPostTriggers = 1"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = int32"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt32F = {"
"                    NodeName = \"SIGUINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt64F = {"
"                    NodeName = \"SIGUINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt32F = {"
"                    NodeName = \"SIGINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt64F = {"
"                    NodeName = \"SIGINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat32F = {"
"                    NodeName = \"SIGFLT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGFLT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat64F = {"
"                    NodeName = \"SIGFLT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGFLT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt16 = {"
"                    NodeName = \"SIGUINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalUInt32 = {"
"                    NodeName = \"SIGUINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalUInt64 = {"
"                    NodeName = \"SIGUINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt16 = {"
"                    NodeName = \"SIGINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt32 = {"
"                    NodeName = \"SIGINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt64 = {"
"                    NodeName = \"SIGINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalFloat32 = {"
"                    NodeName = \"SIGFLT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalFloat64 = {"
"                    NodeName = \"SIGFLT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+TestMessages = {"
"    Class = ReferenceContainer"
"    +MessageFlush = {"
"        Class = Message"
"        Destination = \"Test.Data.Drv1\""
"        Function = FlushSegments"
"    }"
"}";

//Standard configuration with number of elements > 0
static const MARTe::char8 * const config3 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt32F = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt64F = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt32F = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt64F = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat32F = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat64F = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt16 = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt32 = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt64 = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt16 = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt32 = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt64 = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat32 = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat64 = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = 1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt32F = {"
"                    NodeName = \"SIGUINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt64F = {"
"                    NodeName = \"SIGUINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt32F = {"
"                    NodeName = \"SIGINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt64F = {"
"                    NodeName = \"SIGINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat32F = {"
"                    NodeName = \"SIGFLT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGFLT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat64F = {"
"                    NodeName = \"SIGFLT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGFLT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt16 = {"
"                    NodeName = \"SIGUINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalUInt32 = {"
"                    NodeName = \"SIGUINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalUInt64 = {"
"                    NodeName = \"SIGUINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt16 = {"
"                    NodeName = \"SIGINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt32 = {"
"                    NodeName = \"SIGINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt64 = {"
"                    NodeName = \"SIGINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalFloat32 = {"
"                    NodeName = \"SIGFLT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalFloat64 = {"
"                    NodeName = \"SIGFLT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+TestMessages = {"
"    Class = ReferenceContainer"
"    +MessageFlush = {"
"        Class = Message"
"        Destination = \"Test.Data.Drv1\""
"        Function = FlushSegments"
"    }"
"}";

//Standard configuration with trigger source and elements > 0
static const MARTe::char8 * const config5 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            Trigger =  {0 1 0 1 0 1 0 1 1 1 1 1 1 1}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt32F = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt64F = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt32F = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt64F = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat32F = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat64F = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt16 = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt32 = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalUInt64 = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt16 = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt32 = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalInt64 = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat32 = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"                SignalFloat64 = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                    NumberOfElements = 4"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = 1"
"            StoreOnTrigger = 1"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            NumberOfPreTriggers = 2"
"            NumberOfPostTriggers = 1"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt32F = {"
"                    NodeName = \"SIGUINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt64F = {"
"                    NodeName = \"SIGUINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt32F = {"
"                    NodeName = \"SIGINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt64F = {"
"                    NodeName = \"SIGINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat32F = {"
"                    NodeName = \"SIGFLT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGFLT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat64F = {"
"                    NodeName = \"SIGFLT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGFLT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt16 = {"
"                    NodeName = \"SIGUINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalUInt32 = {"
"                    NodeName = \"SIGUINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalUInt64 = {"
"                    NodeName = \"SIGUINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt16 = {"
"                    NodeName = \"SIGINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt32 = {"
"                    NodeName = \"SIGINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalInt64 = {"
"                    NodeName = \"SIGINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalFloat32 = {"
"                    NodeName = \"SIGFLT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"                SignalFloat64 = {"
"                    NodeName = \"SIGFLT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+TestMessages = {"
"    Class = ReferenceContainer"
"    +MessageFlush = {"
"        Class = Message"
"        Destination = \"Test.Data.Drv1\""
"        Function = FlushSegments"
"    }"
"}";

//Configuration to test the OpenTree message interface
static const MARTe::char8 * const config6 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+TestMessages = {"
"    Class = ReferenceContainer"
"    +MessageFlush = {"
"        Class = Message"
"        Destination = \"Test.Data.Drv1\""
"        Function = FlushSegments"
//"        Mode = ExpectsReply"
"    }"
"    +MessageOpenTree = {"
"        Class = Message"
"        Destination = \"Test.Data.Drv1\""
//"        Mode = ExpectsReply"
"        Function = OpenTree"
"        +Parameters = {"
"            Class = ConfigurationDatabase"
"            param1 = -1"
"        }"
"    }"
"}";

//Configuration with Samples > 1
static const MARTe::char8 * const config7 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                    Samples = 10"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}";

//Configuration with more than one time signal
static const MARTe::char8 * const config8 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                Time2 = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 1"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            NumberOfPreTriggers = 1"
"            NumberOfPostTriggers = 1"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                    TimeSignal = 1"
"                }"
"                Time2 = {"
"                    Type = uint32"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}";

//Configuration with no mds plus signals
static const MARTe::char8 * const config9 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                    TimeSignal = 1"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}";

//Configuration with no time signal
static const MARTe::char8 * const config10 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 1"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            NumberOfPreTriggers = 1"
"            NumberOfPostTriggers = 1"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}";

//Configuration with time signal not uint32
static const MARTe::char8 * const config11 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 1"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            NumberOfPreTriggers = 1"
"            NumberOfPostTriggers = 1"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint64"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}";

//Configuration with more than one GAM writing data into the FileWriter
static const MARTe::char8 * const config12 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"        +GAM2 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {8 1 2 3 4 5 6 7 }"
"            Trigger =  {0 1 0 1 0 1 0 1 }"
"            OutputSignals = {"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1 GAM2}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}";

//Standard configuration with trigger source but large MakeSegmentAfterNWrites in order to trigger discontinuities that
//force the creation of a segment
static const MARTe::char8 * const config13 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =   {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            Trigger =  {0 1 0 1 0 1 0 1 1 1 1 1 1 1}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalUInt32F = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt64F = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                }"
"                SignalInt16F = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"                SignalInt32F = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                }"
"                SignalInt64F = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                }"
"                SignalFloat32F = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                }"
"                SignalFloat64F = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16 = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"                SignalUInt32 = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt64 = {"
"                    Type = uint64"
"                    DataSource = Drv1"
"                }"
"                SignalInt16 = {"
"                    Type = int16"
"                    DataSource = Drv1"
"                }"
"                SignalInt32 = {"
"                    Type = int32"
"                    DataSource = Drv1"
"                }"
"                SignalInt64 = {"
"                    Type = int64"
"                    DataSource = Drv1"
"                }"
"                SignalFloat32 = {"
"                    Type = float32"
"                    DataSource = Drv1"
"                }"
"                SignalFloat64 = {"
"                    Type = float64"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = 1"
"            StoreOnTrigger = 1"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            NumberOfPreTriggers = 2"
"            NumberOfPostTriggers = 1"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = int32"
"                    TimeSignal = 1"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt32F = {"
"                    NodeName = \"SIGUINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGUINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt64F = {"
"                    NodeName = \"SIGUINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGUINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt16F = {"
"                    NodeName = \"SIGINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt32F = {"
"                    NodeName = \"SIGINT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGINT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalInt64F = {"
"                    NodeName = \"SIGINT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGINT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat32F = {"
"                    NodeName = \"SIGFLT32F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGFLT32D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalFloat64F = {"
"                    NodeName = \"SIGFLT64F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                    DecimatedNodeName = \"SIGFLT64D\""
"                    MinMaxResampleFactor = 4"
"                }"
"                SignalUInt16 = {"
"                    NodeName = \"SIGUINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalUInt32 = {"
"                    NodeName = \"SIGUINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalUInt64 = {"
"                    NodeName = \"SIGUINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalInt16 = {"
"                    NodeName = \"SIGINT16\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalInt32 = {"
"                    NodeName = \"SIGINT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalInt64 = {"
"                    NodeName = \"SIGINT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalFloat32 = {"
"                    NodeName = \"SIGFLT32\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"                SignalFloat64 = {"
"                    NodeName = \"SIGFLT64\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 8"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+TestMessages = {"
"    Class = ReferenceContainer"
"    +MessageFlush = {"
"        Class = Message"
"        Destination = \"Test.Data.Drv1\""
"        Function = FlushSegments"
"    }"
"}";

//Configuration with messages
static const MARTe::char8 * const config14 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = -1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"            +Messages = {"
"                Class = ReferenceContainer"
"                +TreeOpenedOK = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenOK"
"                    Mode = ExpectsReply"
"                }"
"                +TreeOpenedFail = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenFail"
"                    Mode = ExpectsReply"
"                }"
"                +TreeFlushed = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"                +TreeRuntimeError = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+FileWriterTestHelper = {"
"    Class = FileWriterTestHelper"
"}";

//Configuration with messages that will fail to open the tree
static const MARTe::char8 * const config15 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2testbad\""
"            PulseNumber = -1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"            +Messages = {"
"                Class = ReferenceContainer"
"                +TreeOpenedOK = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenOK"
"                    Mode = ExpectsReply"
"                }"
"                +TreeOpenedFail = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenFail"
"                    Mode = ExpectsReply"
"                }"
"                +TreeFlushed = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+FileWriterTestHelper = {"
"    Class = FileWriterTestHelper"
"}";

//Configuration with messages that cannot be sent
static const MARTe::char8 * const config16 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = -1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"            +Messages = {"
"                Class = ReferenceContainer"
"                +TreeOpenedOK = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelperDoesNotExist"
"                    Function = HandleOpenOK"
"                    Mode = ExpectsReply"
"                }"
"                +TreeOpenedFail = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelperDoesNotExist"
"                    Function = HandleOpenFail"
"                    Mode = ExpectsReply"
"                }"
"                +TreeFlushed = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelperDoesNotExist"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+FileWriterTestHelper = {"
"    Class = FileWriterTestHelper"
"}";

//Configuration with messages that will fail to open the tree and that is sent to an object that does not exists
static const MARTe::char8 * const config17 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2testbad\""
"            PulseNumber = -1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"            +Messages = {"
"                Class = ReferenceContainer"
"                +TreeOpenedOK = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelperDoesNotExist"
"                    Function = HandleOpenOK"
"                    Mode = ExpectsReply"
"                }"
"                +TreeOpenedFail = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelperDoesNotExist"
"                    Function = HandleOpenFail"
"                    Mode = ExpectsReply"
"                }"
"                +TreeFlushed = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelperDoesNotExist"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+FileWriterTestHelper = {"
"    Class = FileWriterTestHelper"
"}";

//Configuration with with an invalid message name
static const MARTe::char8 * const config18 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = -1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"            +Messages = {"
"                Class = ReferenceContainer"
"                +TreeOpened = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenOK"
"                    Mode = ExpectsReply"
"                }"
"                +TreeOpenedFail = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenFail"
"                    Mode = ExpectsReply"
"                }"
"                +TreeFlushed = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+FileWriterTestHelper = {"
"    Class = FileWriterTestHelper"
"}";

//Configuration with with an invalid message type
static const MARTe::char8 * const config19 = ""
"$Test = {"
"    Class = RealTimeApplication"
"    +Functions = {"
"        Class = ReferenceContainer"
"        +GAM1 = {"
"            Class = FileWriterGAMTriggerTestHelper"
"            Signal =  {0 1 2 3 4 5 6 7 8 9 8 7 6 5}"
"            OutputSignals = {"
"                Trigger = {"
"                    Type = uint8"
"                    DataSource = Drv1"
"                }"
"                Time = {"
"                    Type = uint32"
"                    DataSource = Drv1"
"                }"
"                SignalUInt16F = {"
"                    Type = uint16"
"                    DataSource = Drv1"
"                }"
"            }"
"        }"
"    }"
"    +Data = {"
"        Class = ReferenceContainer"
"        DefaultDataSource = DDB1"
"        +Timings = {"
"            Class = TimingDataSource"
"        }"
"        +Drv1 = {"
"            Class = FileWriter"
"            NumberOfBuffers = 10"
"            CPUMask = 15"
"            StackSize = 10000000"
"            TreeName = \"mds_m2test\""
"            PulseNumber = -1"
"            StoreOnTrigger = 0"
"            EventName = \"updatejScope\""
"            TimeRefresh = 5"
"            Signals = {"
"                Trigger = {"
"                    Type = uint8"
"                }"
"                Time = {"
"                    Type = uint32"
"                }"
"                SignalUInt16F = {"
"                    NodeName = \"SIGUINT16F\""
"                    Period = 2"
"                    MakeSegmentAfterNWrites = 4"
"                    DecimatedNodeName = \"SIGUINT16D\""
"                    MinMaxResampleFactor = 4"
"                }"
"            }"
"            +Messages = {"
"                Class = ReferenceContainer"
"                +TreeOpenedOK = {"
"                    Class = ReferenceContainer"
"                }"
"                +TreeOpenedFail = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleOpenFail"
"                    Mode = ExpectsReply"
"                }"
"                +TreeFlushed = {"
"                    Class = Message"
"                    Destination = FileWriterTestHelper"
"                    Function = HandleFlushSegments"
"                    Mode = ExpectsReply"
"                }"
"            }"
"        }"
"    }"
"    +States = {"
"        Class = ReferenceContainer"
"        +State1 = {"
"            Class = RealTimeState"
"            +Threads = {"
"                Class = ReferenceContainer"
"                +Thread1 = {"
"                    Class = RealTimeThread"
"                    Functions = {GAM1}"
"                }"
"            }"
"        }"
"    }"
"    +Scheduler = {"
"        Class = FileWriterSchedulerTestHelper"
"        TimingDataSource = Timings"
"    }"
"}"
"+FileWriterTestHelper = {"
"    Class = FileWriterTestHelper"
"}";

#endif
/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
bool FileWriterTest::TestConstructor() {
    using namespace MARTe;
    FileWriter test;
    return true;
}

bool FileWriterTest::TestAllocateMemory() {
    using namespace MARTe;
    FileWriter test;
    return test.AllocateMemory();
}

bool FileWriterTest::TestGetNumberOfMemoryBuffers() {
    using namespace MARTe;
    FileWriter test;
    return (test.GetNumberOfMemoryBuffers() == 1u);
}

bool FileWriterTest::TestGetSignalMemoryBuffer() {
    using namespace MARTe;
    FileWriter test;
    void *ptr = NULL;
    bool ok = !test.GetSignalMemoryBuffer(0, 0, ptr);
    if (ok) {
        ok = TestIntegratedInApplication_NoTrigger("FileWriterTest_TestGetSignalMemoryBuffer_CSV", true);
    }
    return ok;
}

bool FileWriterTest::TestGetBrokerName_InputSignals() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    bool ok = (StringHelper::Compare(test.GetBrokerName(cdb, InputSignals), "") == 0);

    return ok;
}

bool FileWriterTest::TestGetBrokerName_MemoryMapAsyncOutputBroker() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("CPUMask", 15);
    cdb.Write("StackSize", 10000000);
    cdb.Write("Filename", "TestGetBrokerName_MemoryMapAsyncOutputBroker");
    cdb.Write("FileFormat", "binary");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 0);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    bool ok = test.Initialise(cdb);
    if (ok) {
        ok = (StringHelper::Compare(test.GetBrokerName(cdb, OutputSignals), "MemoryMapAsyncOutputBroker") == 0);
    }

    return ok;
}

bool FileWriterTest::TestGetBrokerName_MemoryMapAsyncTriggerOutputBroker() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("CPUMask", 15);
    cdb.Write("StackSize", 10000000);
    cdb.Write("Filename", "TestGetBrokerName_MemoryMapAsyncOutputBroker");
    cdb.Write("FileFormat", "binary");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    bool ok = test.Initialise(cdb);
    if (ok) {
        ok = (StringHelper::Compare(test.GetBrokerName(cdb, OutputSignals), "MemoryMapAsyncTriggerOutputBroker") == 0);
    }

    return ok;
}

bool FileWriterTest::TestGetInputBrokers() {
    using namespace MARTe;
    FileWriter test;
    ReferenceContainer rc;
    return !test.GetInputBrokers(rc, "", NULL);
}

bool FileWriterTest::TestGetOutputBrokers() {
    bool ok = TestIntegratedInApplication_NoTrigger("FileWriterTest_TestGetOutputBrokers", true);
    return ok;
}

bool FileWriterTest::TestSynchronise() {
    bool ok = true;
    if (ok) {
        ok = TestIntegratedInApplication_NoTrigger("FileWriterTest_TestSynchronise_NoT_CSV", true);
    }
    if (ok) {
        ok = TestIntegratedInApplication_NoTrigger("FileWriterTest_TestSynchronise_NoT_BIN", false);
    }
    if (ok) {
        ok = TestIntegratedInApplication_NoTrigger_Array("FileWriterTest_TestSynchronise_NoT_Arr_CSV", true);
    }
    if (ok) {
        ok = TestIntegratedInApplication_NoTrigger_Array("FileWriterTest_TestSynchronise_NoT_Arr_BIN", false);
    }
    if (ok) {
        ok = TestIntegratedInApplication_Trigger("FileWriterTest_TestSynchronise_T_CSV", true);
    }
    if (ok) {
        ok = TestIntegratedInApplication_Trigger("FileWriterTest_TestSynchronise_T_BIN", false);
    }

    return ok;
}

bool FileWriterTest::TestPrepareNextState() {
    using namespace MARTe;
    FileWriter test;
    return test.PrepareNextState(NULL, NULL);
}

bool FileWriterTest::TestInitialise() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("CPUMask", 15);
    cdb.Write("StackSize", 10000000);
    cdb.Write("Filename", "FileWriterTest_TestInitialise");
    cdb.Write("FileFormat", "csv");
    cdb.Write("CSVSeparator", ",");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 3);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    bool ok = test.Initialise(cdb);
    ok &= (test.GetNumberOfBuffers() == 10);
    ok &= (test.GetCPUMask() == 15);
    ok &= (test.GetStackSize() == 10000000);
    ok &= (test.GetFilename() == "FileWriterTest_TestInitialise");
    ok &= (test.GetFileFormat() == "csv");
    ok &= (test.GetCSVSeparator() == ",");
    ok &= (test.IsOverwrite());
    ok &= (test.IsStoreOnTrigger());
    ok &= (test.GetNumberOfPreTriggers() == 2);
    ok &= (test.GetNumberOfPostTriggers() == 3);
    return ok;
}

bool FileWriterTest::TestInitialise_False_NumberOfBuffers() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("StackSize", 10000000);
    cdb.Write("Filename", "FileWriterTest_TestInitialise");
    cdb.Write("FileFormat", "csv");
    cdb.Write("CSVSeparator", ",");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 3);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_CPUMask() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("Filename", "FileWriterTest_TestInitialise");
    cdb.Write("FileFormat", "csv");
    cdb.Write("CSVSeparator", ",");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 3);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_StackSize() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("CPUMask", 15);
    cdb.Write("Filename", "FileWriterTest_TestInitialise");
    cdb.Write("FileFormat", "csv");
    cdb.Write("CSVSeparator", ",");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 3);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_Warning_Filename() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("CPUMask", 15);
    cdb.Write("StackSize", 10000000);
    cdb.Write("FileFormat", "csv");
    cdb.Write("CSVSeparator", ",");
    cdb.Write("Overwrite", "yes");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 3);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return test.Initialise(cdb);
}

#if 0
bool FileWriterTest::TestInitialise_False_EventName() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("TreeName", "mds_m2test");
    cdb.Write("PulseNumber", 1);
    cdb.Write("TimeRefresh", 5);
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_TimeRefresh() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("TreeName", "mds_m2test");
    cdb.Write("PulseNumber", 1);
    cdb.Write("EventName", "updatejScope");
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_StoreOnTrigger() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("TreeName", "mds_m2test");
    cdb.Write("PulseNumber", 1);
    cdb.Write("EventName", "updatejScope");
    cdb.Write("TimeRefresh", 5);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_NumberOfPreTriggers() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("TreeName", "mds_m2test");
    cdb.Write("PulseNumber", 1);
    cdb.Write("EventName", "updatejScope");
    cdb.Write("TimeRefresh", 5);
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_NumberOfPostTriggers() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("TreeName", "mds_m2test");
    cdb.Write("PulseNumber", 1);
    cdb.Write("EventName", "updatejScope");
    cdb.Write("TimeRefresh", 5);
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.CreateRelative("Signals");
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestInitialise_False_Signals() {
    using namespace MARTe;
    FileWriter test;
    ConfigurationDatabase cdb;
    cdb.Write("CPUMask", 15);
    cdb.Write("NumberOfBuffers", 10);
    cdb.Write("StackSize", 10000000);
    cdb.Write("TreeName", "mds_m2test");
    cdb.Write("PulseNumber", 1);
    cdb.Write("EventName", "updatejScope");
    cdb.Write("TimeRefresh", 5);
    cdb.Write("StoreOnTrigger", 1);
    cdb.Write("NumberOfPreTriggers", 2);
    cdb.Write("NumberOfPostTriggers", 1);
    cdb.MoveToRoot();
    return !test.Initialise(cdb);
}

bool FileWriterTest::TestSetConfiguredDatabase() {
    bool ok = TestIntegratedInApplication_NoTrigger();
    return ok;
}

bool FileWriterTest::TestSetConfiguredDatabase_False_NumberOfSamples() {
    return !TestIntegratedInApplication(config7, true);
}

bool FileWriterTest::TestSetConfiguredDatabase_False_MoreThanOneTimeSignal() {
    return !TestIntegratedInApplication(config8, true);
}

bool FileWriterTest::TestSetConfiguredDatabase_False_NoFileSignals() {
    return !TestIntegratedInApplication(config9, true);
}

bool FileWriterTest::TestSetConfiguredDatabase_False_NoTimeSignal() {
    return !TestIntegratedInApplication(config10, true);
}

bool FileWriterTest::TestSetConfiguredDatabase_False_TimeSignal_NotUInt32() {
    return !TestIntegratedInApplication(config11, true);
}

bool FileWriterTest::TestSetConfiguredDatabase_False_TimeSignal_MoreThanOneFunction() {
    return !TestIntegratedInApplication(config12, true);
}
#endif

bool FileWriterTest::TestIntegratedInApplication_NoTrigger(const MARTe::char8 *filename, bool csv) {
    using namespace MARTe;
    uint32 signalToGenerate[] = { 1, 2, 3, 4, 5 };
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    const uint32 N_OF_SIGNALS = 12;
    const char8 *signalNames[N_OF_SIGNALS] = { "Trigger", "Time", "SignalUInt8", "SignalUInt16", "SignalUInt32", "SignalUInt64", "SignalInt8", "SignalInt16",
            "SignalInt32", "SignalInt64", "SignalFloat32", "SignalFloat64" };
    const uint16 signalTypes[N_OF_SIGNALS] = { UnsignedInteger8Bit.all, UnsignedInteger32Bit.all, UnsignedInteger8Bit.all, UnsignedInteger16Bit.all,
            UnsignedInteger32Bit.all, UnsignedInteger64Bit.all, SignedInteger8Bit.all, SignedInteger16Bit.all, SignedInteger32Bit.all, SignedInteger64Bit.all,
            Float32Bit.all, Float64Bit.all };
    const uint32 signalElements[N_OF_SIGNALS] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    const uint32 numberOfBuffers = 16;
    const float32 period = 2;
    const char8 * expectedFileContent = NULL;
    if (csv) {
        expectedFileContent =
                ""
                        "#Trigger (uint8)[1];Time (uint32)[1];SignalUInt8 (uint8)[1];SignalUInt16 (uint16)[1];SignalUInt32 (uint32)[1];SignalUInt64 (uint64)[1];SignalInt8 (int8)[1];SignalInt16 (int16)[1];SignalInt32 (int32)[1];SignalInt64 (int64)[1];SignalFloat32 (float32)[1];SignalFloat64 (float64)[1]\n"
                        "0;0;1;1;1;1;1;1;1;1;1.000000;1.000000\n"
                        "0;2000000;2;2;2;2;-2;-2;-2;-2;-2.000000;-2.000000\n"
                        "0;4000000;3;3;3;3;3;3;3;3;3.000000;3.000000\n"
                        "0;6000000;4;4;4;4;-4;-4;-4;-4;-4.000000;-4.000000\n"
                        "0;8000000;5;5;5;5;5;5;5;5;5.000000;5.000000\n";
    }
    else {
        uint32 cycleWriteSize = sizeof(uint8); //trigger
        cycleWriteSize += sizeof(uint32); //time
        cycleWriteSize += sizeof(uint8); //signalUInt8
        cycleWriteSize += sizeof(uint16); //signalUInt16
        cycleWriteSize += sizeof(uint32); //signalUInt32
        cycleWriteSize += sizeof(uint64); //signalUInt64
        cycleWriteSize += sizeof(int8); //signalInt8
        cycleWriteSize += sizeof(int16); //signalInt16
        cycleWriteSize += sizeof(int32); //signalInt32
        cycleWriteSize += sizeof(int64); //signalInt64
        cycleWriteSize += sizeof(float32); //signalFloat32
        cycleWriteSize += sizeof(float64); //signalFloat64

        const uint32 SIGNAL_NAME_SIZE = 32;
        uint32 headerSize = sizeof(uint32) + N_OF_SIGNALS * (sizeof(uint16) + SIGNAL_NAME_SIZE + sizeof(uint32));
        uint32 memorySize = headerSize + (numberOfElements * cycleWriteSize); //5 writes
        expectedFileContent = static_cast<char8 *>(GlobalObjectsDatabase::Instance()->GetStandardHeap()->Malloc(memorySize));

        uint32 n;
        //Header
        char8 *header = const_cast<char8 *>(&expectedFileContent[0]);
        MemoryOperationsHelper::Copy(header, reinterpret_cast<const char8 *>(&N_OF_SIGNALS), sizeof(uint32));
        header += sizeof(uint32);
        for (n = 0u; n < N_OF_SIGNALS; n++) {
            MemoryOperationsHelper::Copy(header, &signalTypes[n], sizeof(uint16));
            header += sizeof(uint16);
            MemoryOperationsHelper::Set(header, '\0', SIGNAL_NAME_SIZE);
            MemoryOperationsHelper::Copy(header, signalNames[n], StringHelper::Length(signalNames[n]));
            header += SIGNAL_NAME_SIZE;
            MemoryOperationsHelper::Copy(header, &signalElements[n], sizeof(uint32));
            header += sizeof(uint32);
        }

        for (n = 0u; n < numberOfElements; n++) {
            uint8 *triggerPointer = const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&expectedFileContent[headerSize + (n * cycleWriteSize)]));
            uint32 *timerPointer = reinterpret_cast<uint32 *>(triggerPointer + 1);
            uint8 *signalUInt8Pointer = reinterpret_cast<uint8 *>(timerPointer + 1);
            uint16 *signalUInt16Pointer = reinterpret_cast<uint16 *>(signalUInt8Pointer + 1);
            uint32 *signalUInt32Pointer = reinterpret_cast<uint32 *>(signalUInt16Pointer + 1);
            uint64 *signalUInt64Pointer = reinterpret_cast<uint64 *>(signalUInt32Pointer + 1);
            int8 *signalInt8Pointer = reinterpret_cast<int8 *>(signalUInt64Pointer + 1);
            int16 *signalInt16Pointer = reinterpret_cast<int16 *>(signalInt8Pointer + 1);
            int32 *signalInt32Pointer = reinterpret_cast<int32 *>(signalInt16Pointer + 1);
            int64 *signalInt64Pointer = reinterpret_cast<int64 *>(signalInt32Pointer + 1);
            float32 *signalFloat32Pointer = reinterpret_cast<float32 *>(signalInt64Pointer + 1);
            float64 *signalFloat64Pointer = reinterpret_cast<float64 *>(signalFloat32Pointer + 1);

            *triggerPointer = 0u;
            *timerPointer = static_cast<uint32>(period * 1e6) * n;
            *signalUInt8Pointer = signalToGenerate[n];
            *signalUInt16Pointer = signalToGenerate[n];
            *signalUInt32Pointer = signalToGenerate[n];
            *signalUInt64Pointer = signalToGenerate[n];
            int32 multiplier = -1;
            if ((n % 2) == 0) {
                multiplier = 1;
            }
            *signalInt8Pointer = multiplier * signalToGenerate[n];
            *signalInt16Pointer = multiplier * signalToGenerate[n];
            *signalInt32Pointer = multiplier * signalToGenerate[n];
            *signalInt64Pointer = static_cast<int64>(multiplier) * signalToGenerate[n];
            *signalFloat32Pointer = static_cast<float32>(multiplier) * signalToGenerate[n];
            *signalFloat64Pointer = static_cast<float64>(multiplier) * signalToGenerate[n];
        }
    }

    bool ok = TestIntegratedExecution(config1, signalToGenerate, numberOfElements, NULL, 1u, numberOfBuffers, 0, 0, period, filename, expectedFileContent, csv);
    if (!csv) {
        if (expectedFileContent != NULL) {
            char8 *mem = const_cast<char8 *>(&expectedFileContent[0]);
            GlobalObjectsDatabase::Instance()->GetStandardHeap()->Free(reinterpret_cast<void *&>(mem));
        }
    }
    return ok;
}

bool FileWriterTest::TestIntegratedInApplication_NoTrigger_Array(const MARTe::char8 *filename, bool csv) {
    using namespace MARTe;
    uint32 signalToGenerate[] = { 1, 2, 3, 4, 5 };
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    const uint32 N_OF_SIGNALS = 12;
    const char8 *signalNames[N_OF_SIGNALS] = { "Trigger", "Time", "SignalUInt8", "SignalUInt16", "SignalUInt32", "SignalUInt64", "SignalInt8", "SignalInt16",
            "SignalInt32", "SignalInt64", "SignalFloat32", "SignalFloat64" };
    const uint16 signalTypes[N_OF_SIGNALS] = { UnsignedInteger8Bit.all, UnsignedInteger32Bit.all, UnsignedInteger8Bit.all, UnsignedInteger16Bit.all,
            UnsignedInteger32Bit.all, UnsignedInteger64Bit.all, SignedInteger8Bit.all, SignedInteger16Bit.all, SignedInteger32Bit.all, SignedInteger64Bit.all,
            Float32Bit.all, Float64Bit.all };
    const uint32 signalElements[N_OF_SIGNALS] = { 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    const uint32 numberOfBuffers = 16;
    const float32 period = 2;
    const char8 * expectedFileContent = NULL;
    const uint32 ARRAY_SIZE = 2u;

    if (csv) {
        expectedFileContent =
                ""
                        "#Trigger (uint8)[1];Time (uint32)[1];SignalUInt8 (uint8)[2];SignalUInt16 (uint16)[2];SignalUInt32 (uint32)[2];SignalUInt64 (uint64)[2];SignalInt8 (int8)[2];SignalInt16 (int16)[2];SignalInt32 (int32)[2];SignalInt64 (int64)[2];SignalFloat32 (float32)[2];SignalFloat64 (float64)[2]\n"
                        "0;0;{ 1 1 } ;{ 1 1 } ;{ 1 1 } ;{ 1 1 } ;{ 1 1 } ;{ 1 1 } ;{ 1 1 } ;{ 1 1 } ;{ 1.000000 1.000000 } ;{ 1.000000 1.000000 } \n"
                        "0;4000000;{ 2 2 } ;{ 2 2 } ;{ 2 2 } ;{ 2 2 } ;{ -2 -2 } ;{ -2 -2 } ;{ -2 -2 } ;{ -2 -2 } ;{ -2.000000 -2.000000 } ;{ -2.000000 -2.000000 } \n"
                        "0;8000000;{ 3 3 } ;{ 3 3 } ;{ 3 3 } ;{ 3 3 } ;{ 3 3 } ;{ 3 3 } ;{ 3 3 } ;{ 3 3 } ;{ 3.000000 3.000000 } ;{ 3.000000 3.000000 } \n"
                        "0;12000000;{ 4 4 } ;{ 4 4 } ;{ 4 4 } ;{ 4 4 } ;{ -4 -4 } ;{ -4 -4 } ;{ -4 -4 } ;{ -4 -4 } ;{ -4.000000 -4.000000 } ;{ -4.000000 -4.000000 } \n"
                        "0;16000000;{ 5 5 } ;{ 5 5 } ;{ 5 5 } ;{ 5 5 } ;{ 5 5 } ;{ 5 5 } ;{ 5 5 } ;{ 5 5 } ;{ 5.000000 5.000000 } ;{ 5.000000 5.000000 } \n";
    }
    else {
        uint32 cycleWriteSize = sizeof(uint8); //trigger
        cycleWriteSize += sizeof(uint32); //time
        cycleWriteSize += sizeof(uint8) * ARRAY_SIZE; //signalUInt8
        cycleWriteSize += sizeof(uint16) * ARRAY_SIZE; //signalUInt16
        cycleWriteSize += sizeof(uint32) * ARRAY_SIZE; //signalUInt32
        cycleWriteSize += sizeof(uint64) * ARRAY_SIZE; //signalUInt64
        cycleWriteSize += sizeof(int8) * ARRAY_SIZE; //signalInt8
        cycleWriteSize += sizeof(int16) * ARRAY_SIZE; //signalInt16
        cycleWriteSize += sizeof(int32) * ARRAY_SIZE; //signalInt32
        cycleWriteSize += sizeof(int64) * ARRAY_SIZE; //signalInt64
        cycleWriteSize += sizeof(float32) * ARRAY_SIZE; //signalFloat32
        cycleWriteSize += sizeof(float64) * ARRAY_SIZE; //signalFloat64

        const uint32 SIGNAL_NAME_SIZE = 32;
        uint32 headerSize = sizeof(uint32) + N_OF_SIGNALS * (sizeof(uint16) + SIGNAL_NAME_SIZE + sizeof(uint32));
        uint32 memorySize = headerSize + (numberOfElements * cycleWriteSize); //5 writes
        expectedFileContent = static_cast<char8 *>(GlobalObjectsDatabase::Instance()->GetStandardHeap()->Malloc(memorySize));

        uint32 n;
        //Header
        char8 *header = const_cast<char8 *>(&expectedFileContent[0]);
        MemoryOperationsHelper::Copy(header, reinterpret_cast<const char8 *>(&N_OF_SIGNALS), sizeof(uint32));
        header += sizeof(uint32);
        for (n = 0u; n < N_OF_SIGNALS; n++) {
            MemoryOperationsHelper::Copy(header, &signalTypes[n], sizeof(uint16));
            header += sizeof(uint16);
            MemoryOperationsHelper::Set(header, '\0', SIGNAL_NAME_SIZE);
            MemoryOperationsHelper::Copy(header, signalNames[n], StringHelper::Length(signalNames[n]));
            header += SIGNAL_NAME_SIZE;
            MemoryOperationsHelper::Copy(header, &signalElements[n], sizeof(uint32));
            header += sizeof(uint32);
        }

        for (n = 0u; n < numberOfElements; n++) {
            uint8 *triggerPointer = const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&expectedFileContent[headerSize + (n * cycleWriteSize)]));
            uint32 *timerPointer = reinterpret_cast<uint32 *>(triggerPointer + 1);
            uint8  *signalUInt8Pointer = reinterpret_cast<uint8 *>(timerPointer + 1);
            uint16 *signalUInt16Pointer = reinterpret_cast<uint16 *>(signalUInt8Pointer + ARRAY_SIZE);
            uint32 *signalUInt32Pointer = reinterpret_cast<uint32 *>(signalUInt16Pointer + ARRAY_SIZE);
            uint64 *signalUInt64Pointer = reinterpret_cast<uint64 *>(signalUInt32Pointer + ARRAY_SIZE);
            int8 *signalInt8Pointer = reinterpret_cast<int8 *>(signalUInt64Pointer + ARRAY_SIZE);
            int16 *signalInt16Pointer = reinterpret_cast<int16 *>(signalInt8Pointer + ARRAY_SIZE);
            int32 *signalInt32Pointer = reinterpret_cast<int32 *>(signalInt16Pointer + ARRAY_SIZE);
            int64 *signalInt64Pointer = reinterpret_cast<int64 *>(signalInt32Pointer + ARRAY_SIZE);
            float32 *signalFloat32Pointer = reinterpret_cast<float32 *>(signalInt64Pointer + ARRAY_SIZE);
            float64 *signalFloat64Pointer = reinterpret_cast<float64 *>(signalFloat32Pointer + ARRAY_SIZE);

            *triggerPointer = 0u;
            *timerPointer = static_cast<uint32>(period * 1e6) * n * ARRAY_SIZE;

            int32 multiplier = -1;
            if ((n % 2) == 0) {
                multiplier = 1;
            }

            uint32 i;
            for (i = 0; i < ARRAY_SIZE; i++) {
                *(signalUInt8Pointer + i) = signalToGenerate[n];
                *(signalUInt16Pointer + i) = signalToGenerate[n];
                *(signalUInt32Pointer + i) = signalToGenerate[n];
                *(signalUInt64Pointer + i) = signalToGenerate[n];
                *(signalInt8Pointer + i) = static_cast<int8>(multiplier) * signalToGenerate[n];
                *(signalInt16Pointer + i) = static_cast<int16>(multiplier) * signalToGenerate[n];
                *(signalInt32Pointer + i) = static_cast<int32>(multiplier) * signalToGenerate[n];
                *(signalInt64Pointer + i) = static_cast<int64>(multiplier) * signalToGenerate[n];
                *(signalFloat32Pointer + i) = static_cast<float32>(multiplier) * signalToGenerate[n];
                *(signalFloat64Pointer + i) = static_cast<float64>(multiplier) * signalToGenerate[n];
            }
        }
    }

    bool ok = TestIntegratedExecution(config1, signalToGenerate, numberOfElements, NULL, ARRAY_SIZE, numberOfBuffers, 0, 0, period, filename, expectedFileContent, csv);
    if (!csv) {
        if (expectedFileContent != NULL) {
            char8 *mem = const_cast<char8 *>(&expectedFileContent[0]);
            GlobalObjectsDatabase::Instance()->GetStandardHeap()->Free(reinterpret_cast<void *&>(mem));
        }
    }
    return ok;
}

bool FileWriterTest::TestIntegratedInApplication_Trigger(const MARTe::char8 *filename, bool csv) {
    using namespace MARTe;
    uint32 signalToGenerate[] = { 1, 2, 3, 4, 5 };
    uint8 triggerToGenerate[] = { 0, 1, 0, 1, 1 };
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    const uint32 N_OF_SIGNALS = 12;
    const char8 *signalNames[N_OF_SIGNALS] = { "Trigger", "Time", "SignalUInt8", "SignalUInt16", "SignalUInt32", "SignalUInt64", "SignalInt8", "SignalInt16",
            "SignalInt32", "SignalInt64", "SignalFloat32", "SignalFloat64" };
    const uint16 signalTypes[N_OF_SIGNALS] = { UnsignedInteger8Bit.all, UnsignedInteger32Bit.all, UnsignedInteger8Bit.all, UnsignedInteger16Bit.all,
            UnsignedInteger32Bit.all, UnsignedInteger64Bit.all, SignedInteger8Bit.all, SignedInteger16Bit.all, SignedInteger32Bit.all, SignedInteger64Bit.all,
            Float32Bit.all, Float64Bit.all };
    const uint32 signalElements[N_OF_SIGNALS] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    const uint32 numberOfBuffers = 16;
    const float32 period = 2;
    const char8 * expectedFileContent = NULL;
    if (csv) {
        expectedFileContent =
                ""
                        "#Trigger (uint8)[1];Time (uint32)[1];SignalUInt8 (uint8)[1];SignalUInt16 (uint16)[1];SignalUInt32 (uint32)[1];SignalUInt64 (uint64)[1];SignalInt8 (int8)[1];SignalInt16 (int16)[1];SignalInt32 (int32)[1];SignalInt64 (int64)[1];SignalFloat32 (float32)[1];SignalFloat64 (float64)[1]\n"
                        "1;2000000;2;2;2;2;-2;-2;-2;-2;-2.000000;-2.000000\n"
                        "1;6000000;4;4;4;4;-4;-4;-4;-4;-4.000000;-4.000000\n"
                        "1;8000000;5;5;5;5;5;5;5;5;5.000000;5.000000\n";
    }
    else {
        uint32 cycleWriteSize = sizeof(uint8); //trigger
        cycleWriteSize += sizeof(uint32); //time
        cycleWriteSize += sizeof(uint8); //signalUInt8
        cycleWriteSize += sizeof(uint16); //signalUInt16
        cycleWriteSize += sizeof(uint32); //signalUInt32
        cycleWriteSize += sizeof(uint64); //signalUInt64
        cycleWriteSize += sizeof(int8); //signalInt8
        cycleWriteSize += sizeof(int16); //signalInt16
        cycleWriteSize += sizeof(int32); //signalInt32
        cycleWriteSize += sizeof(int64); //signalInt64
        cycleWriteSize += sizeof(float32); //signalFloat32
        cycleWriteSize += sizeof(float64); //signalFloat64

        const uint32 SIGNAL_NAME_SIZE = 32;
        uint32 headerSize = sizeof(uint32) + N_OF_SIGNALS * (sizeof(uint16) + SIGNAL_NAME_SIZE + sizeof(uint32));
        uint32 memorySize = headerSize + (numberOfElements * cycleWriteSize); //3 writes (as per trigger)
        expectedFileContent = static_cast<char8 *>(GlobalObjectsDatabase::Instance()->GetStandardHeap()->Malloc(memorySize));

        uint32 n;
        //Header
        char8 *header = const_cast<char8 *>(&expectedFileContent[0]);
        MemoryOperationsHelper::Copy(header, reinterpret_cast<const char8 *>(&N_OF_SIGNALS), sizeof(uint32));
        header += sizeof(uint32);
        for (n = 0u; n < N_OF_SIGNALS; n++) {
            MemoryOperationsHelper::Copy(header, &signalTypes[n], sizeof(uint16));
            header += sizeof(uint16);
            MemoryOperationsHelper::Set(header, '\0', SIGNAL_NAME_SIZE);
            MemoryOperationsHelper::Copy(header, signalNames[n], StringHelper::Length(signalNames[n]));
            header += SIGNAL_NAME_SIZE;
            MemoryOperationsHelper::Copy(header, &signalElements[n], sizeof(uint32));
            header += sizeof(uint32);
        }

        uint32 z = 0u;
        for (n = 0u; n < numberOfElements; n++) {
            if (triggerToGenerate[n] != 1) {
                continue;
            }
            uint8 *triggerPointer = const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&expectedFileContent[headerSize + (z * cycleWriteSize)]));
            z++;

            uint32 *timerPointer = reinterpret_cast<uint32 *>(triggerPointer + 1);
            uint8 *signalUInt8Pointer = reinterpret_cast<uint8 *>(timerPointer + 1);
            uint16 *signalUInt16Pointer = reinterpret_cast<uint16 *>(signalUInt8Pointer + 1);
            uint32 *signalUInt32Pointer = reinterpret_cast<uint32 *>(signalUInt16Pointer + 1);
            uint64 *signalUInt64Pointer = reinterpret_cast<uint64 *>(signalUInt32Pointer + 1);
            int8 *signalInt8Pointer = reinterpret_cast<int8 *>(signalUInt64Pointer + 1);
            int16 *signalInt16Pointer = reinterpret_cast<int16 *>(signalInt8Pointer + 1);
            int32 *signalInt32Pointer = reinterpret_cast<int32 *>(signalInt16Pointer + 1);
            int64 *signalInt64Pointer = reinterpret_cast<int64 *>(signalInt32Pointer + 1);
            float32 *signalFloat32Pointer = reinterpret_cast<float32 *>(signalInt64Pointer + 1);
            float64 *signalFloat64Pointer = reinterpret_cast<float64 *>(signalFloat32Pointer + 1);

            *triggerPointer = 1u;
            *timerPointer = static_cast<uint32>(period * 1e6) * n;
            *signalUInt8Pointer = signalToGenerate[n];
            *signalUInt16Pointer = signalToGenerate[n];
            *signalUInt32Pointer = signalToGenerate[n];
            *signalUInt64Pointer = signalToGenerate[n];
            int32 multiplier = -1;
            if ((n % 2) == 0) {
                multiplier = 1;
            }
            *signalInt8Pointer = multiplier * signalToGenerate[n];
            *signalInt16Pointer = multiplier * signalToGenerate[n];
            *signalInt32Pointer = multiplier * signalToGenerate[n];
            *signalInt64Pointer = static_cast<int64>(multiplier) * signalToGenerate[n];
            *signalFloat32Pointer = static_cast<float32>(multiplier) * signalToGenerate[n];
            *signalFloat64Pointer = static_cast<float64>(multiplier) * signalToGenerate[n];
        }
    }

    bool ok = TestIntegratedExecution(config1, signalToGenerate, numberOfElements, triggerToGenerate, 1u, numberOfBuffers, 0, 0, period, filename,
                                      expectedFileContent, csv);
    if (!csv) {
        if (expectedFileContent != NULL) {
            char8 *mem = const_cast<char8 *>(&expectedFileContent[0]);
            GlobalObjectsDatabase::Instance()->GetStandardHeap()->Free(reinterpret_cast<void *&>(mem));
        }
    }
    return ok;
}

#if 0

bool FileWriterTest::TestIntegratedInApplication_Trigger() {
    using namespace MARTe;
    uint32 signalToGenerate[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    uint32 signalToVerify[] = {1, 2, 3, 4, 6, 7, 8, 9};
    uint32 timeToVerify[] = {0, 2, 4, 6, 10, 12, 14, 16};
    uint8 triggerToGenerate[] = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    uint32 numberOfElementsToVerify = sizeof(signalToVerify) / sizeof(uint32);
    const char8 * const treeName = "mds_m2test";
    const uint32 numberOfBuffers = 16;
    const uint32 numberOfPreTriggers = 2;
    const uint32 numberOfPostTriggers = 1;
    const uint32 pulseNumber = 3;
    const uint32 numberOfSegments = 2;
    const float32 period = 2;
    return TestIntegratedExecution(config2, signalToGenerate, numberOfElements, triggerToGenerate, signalToVerify, timeToVerify, numberOfElementsToVerify, numberOfBuffers, numberOfPreTriggers,
            numberOfPostTriggers, period, treeName, pulseNumber, numberOfSegments, false);
}

bool FileWriterTest::TestIntegratedInApplication_Trigger_Discontinuity() {
    using namespace MARTe;
    uint32 signalToGenerate[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    uint32 signalToVerify[] = {1, 2, 3, 4, 6, 7, 8, 9};
    uint32 timeToVerify[] = {0, 2, 4, 6, 10, 12, 14, 16};
    uint8 triggerToGenerate[] = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    uint32 numberOfElementsToVerify = sizeof(signalToVerify) / sizeof(uint32);
    const char8 * const treeName = "mds_m2test";
    const uint32 numberOfBuffers = 16;
    const uint32 numberOfPreTriggers = 2;
    const uint32 numberOfPostTriggers = 1;
    const uint32 pulseNumber = 6;
    const uint32 numberOfSegments = 1;
    const float32 period = 2;
    return TestIntegratedExecution(config13, signalToGenerate, numberOfElements, triggerToGenerate, signalToVerify, timeToVerify, numberOfElementsToVerify, numberOfBuffers, numberOfPreTriggers,
            numberOfPostTriggers, period, treeName, pulseNumber, numberOfSegments, false);
}

bool FileWriterTest::TestIntegratedInApplication_NoTrigger_Elements() {
    using namespace MARTe;
    uint32 signalToGenerate[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint32 signalToVerify[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8};
    uint32 timeToVerify[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    uint32 numberOfElementsToVerify = sizeof(signalToVerify) / sizeof(uint32);
    const char8 * const treeName = "mds_m2test";
    const uint32 numberOfBuffers = 16;
    const uint32 writeAfterNSegments = 4;
    const uint32 numberOfSegments = numberOfElements / writeAfterNSegments;
    const float32 period = 2;
    const uint32 pulseNumber = 4;
    return TestIntegratedExecution(config3, signalToGenerate, numberOfElements, NULL, signalToVerify, timeToVerify, numberOfElementsToVerify, numberOfBuffers, 0, 0, period, treeName, pulseNumber,
            numberOfSegments, false);
}

bool FileWriterTest::TestIntegratedInApplication_Trigger_Elements() {
    using namespace MARTe;
    uint32 signalToGenerate[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    uint32 signalToVerify[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9};
    uint32 timeToVerify[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70};
    uint8 triggerToGenerate[] = {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};
    uint32 numberOfElements = sizeof(signalToGenerate) / sizeof(uint32);
    uint32 numberOfElementsToVerify = sizeof(signalToVerify) / sizeof(uint32);
    const char8 * const treeName = "mds_m2test";
    const uint32 numberOfBuffers = 16;
    const uint32 numberOfPreTriggers = 2;
    const uint32 numberOfPostTriggers = 1;
    const uint32 pulseNumber = 5;
    const uint32 numberOfSegments = 2;
    const float32 period = 2;
    return TestIntegratedExecution(config5, signalToGenerate, numberOfElements, triggerToGenerate, signalToVerify, timeToVerify, numberOfElementsToVerify, numberOfBuffers, numberOfPreTriggers,
            numberOfPostTriggers, period, treeName, pulseNumber, numberOfSegments, false);
}

bool FileWriterTest::TestOpenTree() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config6, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    //Get the current pulse number
    Fileplus::Tree *tree = NULL;
    int32 lastPulseNumber = -1;
    StreamString treeName = "mds_m2test";
    try {
        tree = new Fileplus::Tree(treeName.Buffer(), lastPulseNumber);
        lastPulseNumber = tree->getCurrent(treeName.Buffer());
        delete tree;
    }
    catch (Fileplus::MdsException &exc) {
        delete tree;
        tree = NULL_PTR(Fileplus::Tree *);
        ok = false;
    }

    ReferenceT<FileWriter> msdWriter;
    if (ok) {
        msdWriter = godb->Find("Test.Data.Drv1");
        ok = msdWriter.IsValid();
    }
    ReferenceT<Message> messageOpenTree = ObjectRegistryDatabase::Instance()->Find("TestMessages.MessageOpenTree");
    if (ok) {
        ok = messageOpenTree.IsValid();
    }
    if (ok) {
        MessageI::SendMessage(messageOpenTree, NULL);
    }

    int32 currentPulseNumber = -1;
    try {
        tree = new Fileplus::Tree(treeName.Buffer(), currentPulseNumber);
        currentPulseNumber = tree->getCurrent(treeName.Buffer());
        delete tree;
    }
    catch (Fileplus::MdsException &exc) {
        delete tree;
        tree = NULL_PTR(Fileplus::Tree *);
        ok = false;
    }
    if (ok) {
        ok = ((currentPulseNumber - lastPulseNumber) == 1);
    }

    //Check that data can be successfully stored in the new pulse number
    ReferenceT<RealTimeApplication> application;
    ReferenceT<FileWriterSchedulerTestHelper> scheduler;
    ReferenceT<FileWriterGAMTriggerTestHelper> gam;

    if (ok) {
        application = godb->Find("Test");
        ok = application.IsValid();
    }
    if (ok) {
        scheduler = application->Find("Scheduler");
        ok = scheduler.IsValid();
    }
    if (ok) {
        gam = application->Find("Functions.GAM1");
        ok = gam.IsValid();
    }
    //Open the tree and check if the data was correctly stored.
    //Create a pulse. It assumes that the tree template is already created!!
    if (ok) {
        try {
            tree = new Fileplus::Tree(treeName.Buffer(), currentPulseNumber);
        }
        catch (Fileplus::MdsException &exc) {
            delete tree;
            tree = NULL;
            ok = false;
        }
    }
    Fileplus::TreeNode *sigUInt16F;
    if (ok) {
        try {
            sigUInt16F = tree->getNode("SIGUINT16F");
            sigUInt16F->deleteData();
        }
        catch (Fileplus::MdsException &exc) {
            REPORT_ERROR_STATIC(ErrorManagement::ParametersError, "Failed opening node");
            ok = false;
        }
    }
    if (ok) {
        ok = application->PrepareNextState("State1");
    }
    if (ok) {
        ok = application->StartNextStateExecution();
    }

    uint32 i;
    if (ok) {
        for (i = 0; (i < gam->numberOfExecutes); i++) {
            scheduler->ExecuteThreadCycle(0);
            Sleep::MSec(10);
        }
    }
    if (ok) {
        ok = application->StopCurrentStateExecution();
    }
    const int32 numberOfSegments = 2;
    if (ok) {
        const uint64 maxTimeoutSeconds = 2;
        uint64 maxTimeout = HighResolutionTimer::Counter() + maxTimeoutSeconds * HighResolutionTimer::Frequency();
        while ((sigUInt16F->getNumSegments() != numberOfSegments) && (ok)) {
            Sleep::MSec(10);
            ok = (HighResolutionTimer::Counter() < maxTimeout);
        }
    }
    if (ok) {
        uint32 signalToVerify[] = {8, 1, 2, 3, 4, 5, 6, 7};
        uint32 timeToVerify[] = {0, 2, 4, 6, 8, 10, 12, 14};
        ok &= CheckSegmentData<uint16>(numberOfSegments, sigUInt16F, signalToVerify, timeToVerify);
    }
    if (tree != NULL) {
        delete tree;
    }

    //Reopen the tree
    if (ok) {
        ok = messageOpenTree.IsValid();
    }
    if (ok) {
        MessageI::SendMessage(messageOpenTree, NULL);
    }
    currentPulseNumber++;
    if (ok) {
        try {
            tree = new Fileplus::Tree(treeName.Buffer(), currentPulseNumber);
        }
        catch (Fileplus::MdsException &exc) {
            delete tree;
            tree = NULL;
            ok = false;
        }
    }
    if (ok) {
        try {
            sigUInt16F = tree->getNode("SIGUINT16F");
            sigUInt16F->deleteData();
        }
        catch (Fileplus::MdsException &exc) {
            REPORT_ERROR_STATIC(ErrorManagement::ParametersError, "Failed opening node");
            ok = false;
        }
    }
    if (ok) {
        gam->numberOfExecutes = 8;
        gam->counter = 0;
    }
    if (ok) {
        ok = application->PrepareNextState("State1");
    }
    if (ok) {
        ok = application->StartNextStateExecution();
    }

    if (ok) {
        for (i = 0; (i < gam->numberOfExecutes); i++) {
            scheduler->ExecuteThreadCycle(0);
            Sleep::MSec(10);
        }
    }
    if (ok) {
        ok = application->StopCurrentStateExecution();
    }
    if (ok) {
        const uint64 maxTimeoutSeconds = 2;
        uint64 maxTimeout = HighResolutionTimer::Counter() + maxTimeoutSeconds * HighResolutionTimer::Frequency();
        while ((sigUInt16F->getNumSegments() != numberOfSegments) && (ok)) {
            Sleep::MSec(10);
            ok = (HighResolutionTimer::Counter() < maxTimeout);
        }
    }
    if (ok) {
        uint32 signalToVerify[] = {8, 1, 2, 3, 4, 5, 6, 7};
        uint32 timeToVerify[] = {0, 2, 4, 6, 8, 10, 12, 14};
        ok &= CheckSegmentData<uint16>(numberOfSegments, sigUInt16F, signalToVerify, timeToVerify);
    }

    if (tree != NULL) {
        delete tree;
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestGetCPUMask() {
    return TestInitialise();
}

bool FileWriterTest::TestGetEventName() {
    return TestInitialise();
}

bool FileWriterTest::TestGetNumberOfBuffers() {
    return TestInitialise();
}

bool FileWriterTest::TestGetNumberOfMdsSignals() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config2, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    if (ok) {
        mdsWriter = godb->Find("Test.Data.Drv1");
        ok = mdsWriter.IsValid();
    }
    if (ok) {
        ok = (mdsWriter->GetNumberOfMdsSignals() == 16);
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestGetNumberOfPostTriggers() {
    return TestInitialise();
}

bool FileWriterTest::TestGetNumberOfPreTriggers() {
    return TestInitialise();
}

bool FileWriterTest::TestGetPulseNumber() {
    return TestInitialise();
}

bool FileWriterTest::TestGetTreeName() {
    return TestInitialise();
}

bool FileWriterTest::TestGetRefreshEveryCounts() {
    return TestInitialise();
}

bool FileWriterTest::TestGetStackSize() {
    return TestInitialise();
}

bool FileWriterTest::TestIsStoreOnTrigger() {
    return TestInitialise();
}

bool FileWriterTest::TestGetTimeSignalIdx() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config2, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    if (ok) {
        mdsWriter = godb->Find("Test.Data.Drv1");
        ok = mdsWriter.IsValid();
    }
    if (ok) {
        ok = (mdsWriter->GetTimeSignalIdx() == 1);
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestOpenTreeOKMessage() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config14, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    ReferenceT<FileWriterTestHelper> helper;
    if (ok) {
        mdsWriter = godb->Find("Test.Data.Drv1");
        ok = mdsWriter.IsValid();
    }
    if (ok) {
        helper = godb->Find("FileWriterTestHelper");
        ok = helper.IsValid();
    }
    if (ok) {
        ok = (helper->pulseNumber == mdsWriter->GetPulseNumber());
        REPORT_ERROR_STATIC(ErrorManagement::Information, "helper->pulseNumber = %d", helper->pulseNumber);
    }
    //Force the sending of a second message
    if (ok) {
        ok = mdsWriter->OpenTree(-1);
    }
    if (ok) {
        ok = (helper->pulseNumber == mdsWriter->GetPulseNumber());
        REPORT_ERROR_STATIC(ErrorManagement::Information, "helper->pulseNumber = %d", helper->pulseNumber);
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestFlushSegmentsMessage() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config14, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    ReferenceT<FileWriterTestHelper> helper;
    if (ok) {
        mdsWriter = godb->Find("Test.Data.Drv1");
        ok = mdsWriter.IsValid();
    }
    if (ok) {
        helper = godb->Find("FileWriterTestHelper");
        ok = helper.IsValid();
    }
    if (ok) {
        mdsWriter->FlushSegments();
    }
    if (ok) {
        ok = (helper->flushSegmentsFunctionCalled);
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestOpenTreeFailMessage() {
    using namespace MARTe;
    TestIntegratedInApplication(config15, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    ReferenceT<FileWriterTestHelper> helper;
    mdsWriter = godb->Find("Test.Data.Drv1");
    bool ok = mdsWriter.IsValid();

    if (ok) {
        helper = godb->Find("FileWriterTestHelper");
        ok = helper.IsValid();
    }
    if (ok) {
        ok = (helper->openFailFunctionCalled);
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestOpenTreeOKMessage_Fail() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config16, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    ReferenceT<FileWriterTestHelper> helper;
    if (ok) {
        mdsWriter = godb->Find("Test.Data.Drv1");
        ok = mdsWriter.IsValid();
    }
    if (ok) {
        helper = godb->Find("FileWriterTestHelper");
        ok = helper.IsValid();
    }

    godb->Purge();
    return ok;
}

bool FileWriterTest::TestFlushSegmentsMessage_Fail() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config16, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    ReferenceT<FileWriterTestHelper> helper;
    if (ok) {
        mdsWriter = godb->Find("Test.Data.Drv1");
        ok = mdsWriter.IsValid();
    }
    if (ok) {
        helper = godb->Find("FileWriterTestHelper");
        ok = helper.IsValid();
    }
    if (ok) {
        mdsWriter->FlushSegments();
    }

    godb->Purge();
    return ok;
}

bool FileWriterTest::TestOpenTreeFailMessage_Fail() {
    using namespace MARTe;
    TestIntegratedInApplication(config17, false);
    ObjectRegistryDatabase *godb = ObjectRegistryDatabase::Instance();

    ReferenceT<FileWriter> mdsWriter;
    ReferenceT<FileWriterTestHelper> helper;
    mdsWriter = godb->Find("Test.Data.Drv1");
    bool ok = mdsWriter.IsValid();

    if (ok) {
        helper = godb->Find("FileWriterTestHelper");
        ok = helper.IsValid();
    }
    godb->Purge();
    return ok;
}

bool FileWriterTest::TestInvalidMessageName() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config18, true);
    return !ok;
}

bool FileWriterTest::TestInvalidMessageType() {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config19, true);
    return !ok;
}
#endif
