/**
 * @file FileWriter.cpp
 * @brief Source file for class FileWriter
 * @date 11/08/2017
 * @author Andre' Neto
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
 * the class FileWriter (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "CLASSMETHODREGISTER.h"
#include "Directory.h"
#include "FileWriter.h"
#include "MemoryMapAsyncOutputBroker.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
static const MARTe::int32 FILE_FORMAT_BINARY = 1;
static const MARTe::int32 FILE_FORMAT_CSV = 2;

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {

FileWriter::FileWriter() :
        DataSourceI(), MessageI() {
    storeOnTrigger = false;
    numberOfPreTriggers = 0u;
    numberOfPostTriggers = 0u;
    numberOfBuffers = 0u;
    dataSourceMemory = NULL_PTR(char8 *);
    offsets = NULL_PTR(uint32 *);
    cpuMask = 0xfu;
    stackSize = 0u;
    numberOfBinaryBytes = 0u;
    fileFormat = FILE_FORMAT_BINARY;
    filename = "";
    fatalFileError = false;
    overwrite = false;
    signalsAnyType = NULL_PTR(AnyType *);
    brokerAsyncTrigger = NULL_PTR(MemoryMapAsyncTriggerOutputBroker *);
    filter = ReferenceT<RegisteredMethodsMessageFilter>(GlobalObjectsDatabase::Instance()->GetStandardHeap());
    filter->SetDestination(this);
    ErrorManagement::ErrorType ret = MessageI::InstallMessageFilter(filter);
    if (!ret.ErrorsCleared()) {
        REPORT_ERROR(ErrorManagement::FatalError, "Failed to install message filters");
    }
}

/*lint -e{1551} -e{1579} the destructor must guarantee that the memory is freed and the file is flushed and closed.. The brokerAsyncTrigger is freed by the ReferenceT */
FileWriter::~FileWriter() {
    if (FlushFile() != ErrorManagement::NoError) {
        REPORT_ERROR(ErrorManagement::FatalError, "Failed to Flush the FlushFile");
    }
    if (dataSourceMemory != NULL_PTR(char8 *)) {
        GlobalObjectsDatabase::Instance()->GetStandardHeap()->Free(reinterpret_cast<void *&>(dataSourceMemory));
    }
    if (offsets != NULL_PTR(uint32 *)) {
        delete[] offsets;
    }
    if (signalsAnyType != NULL_PTR(AnyType *)) {
        delete[] signalsAnyType;
    }
    (void) CloseFile();
}

bool FileWriter::AllocateMemory() {
    return true;
}

uint32 FileWriter::GetNumberOfMemoryBuffers() {
    return 1u;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: The signalAddress is independent of the bufferIdx.*/
bool FileWriter::GetSignalMemoryBuffer(const uint32 signalIdx, const uint32 bufferIdx, void*& signalAddress) {
    bool ok = (dataSourceMemory != NULL_PTR(char8 *));
    if (ok) {
        /*lint -e{613} dataSourceMemory cannot be NULL here*/
        char8 *memPtr = &dataSourceMemory[offsets[signalIdx]];
        signalAddress = reinterpret_cast<void *&>(memPtr);
    }
    return ok;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: The brokerName only depends on the direction and on the storeOnTrigger property (which is load before).*/
const char8* FileWriter::GetBrokerName(StructuredDataI& data, const SignalDirection direction) {
    const char8* brokerName = "";
    if (direction == OutputSignals) {
        if (storeOnTrigger) {
            brokerName = "MemoryMapAsyncTriggerOutputBroker";
        }
        else {
            brokerName = "MemoryMapAsyncOutputBroker";
        }
    }
    return brokerName;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: InputBrokers are not supported. Function returns false irrespectively of the parameters.*/
bool FileWriter::GetInputBrokers(ReferenceContainer& inputBrokers, const char8* const functionName, void* const gamMemPtr) {
    return false;
}

bool FileWriter::GetOutputBrokers(ReferenceContainer& outputBrokers, const char8* const functionName, void* const gamMemPtr) {
    bool ok = true;
    if (storeOnTrigger) {
        ReferenceT<MemoryMapAsyncTriggerOutputBroker> brokerAsyncTriggerNew("MemoryMapAsyncTriggerOutputBroker");
        brokerAsyncTrigger = brokerAsyncTriggerNew.operator ->();
        ok = brokerAsyncTriggerNew->InitWithTriggerParameters(OutputSignals, *this, functionName, gamMemPtr, numberOfBuffers, numberOfPreTriggers,
                                                              numberOfPostTriggers, cpuMask, stackSize);
        if (ok) {
            ok = outputBrokers.Insert(brokerAsyncTriggerNew);
        }
    }
    else {
        ReferenceT<MemoryMapAsyncOutputBroker> brokerAsync("MemoryMapAsyncOutputBroker");
        ok = brokerAsync->InitWithBufferParameters(OutputSignals, *this, functionName, gamMemPtr, numberOfBuffers, cpuMask, stackSize);
        if (ok) {
            ok = outputBrokers.Insert(brokerAsync);
        }
    }
    return ok;
}

bool FileWriter::Synchronise() {
    bool ok = !fatalFileError;
    if (ok) {
        if (fileFormat == FILE_FORMAT_BINARY) {
            uint32 writeSize = numberOfBinaryBytes;
            ok = outputFile.Write(dataSourceMemory, writeSize);
            if (ok) {
                ok = (writeSize == numberOfBinaryBytes);
            }
        }
        else {
            ok = outputFile.PrintFormatted(csvPrintfFormat.Buffer(), signalsAnyType);
        }
        fatalFileError = !ok;
        if (fatalFileError) {
            REPORT_ERROR(ErrorManagement::FatalError, "Failed to write into file. No more attempts will be performed.");
        }
    }
    return ok;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: NOOP at StateChange, independently of the function parameters.*/
bool FileWriter::PrepareNextState(const char8* const currentStateName, const char8* const nextStateName) {
    return true;
}

bool FileWriter::Initialise(StructuredDataI& data) {
    bool ok = DataSourceI::Initialise(data);
    if (ok) {
        ok = data.Read("NumberOfBuffers", numberOfBuffers);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "NumberOfBuffers shall be specified");
        }
    }
    if (ok) {
        ok = (numberOfBuffers > 0u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "NumberOfBuffers shall be > 0u");
        }
    }
    if (ok) {
        uint32 cpuMaskIn;
        ok = data.Read("CPUMask", cpuMaskIn);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "CPUMask shall be specified");
        }
        else {
            cpuMask = cpuMaskIn;
        }
    }
    if (ok) {
        ok = data.Read("StackSize", stackSize);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "StackSize shall be specified");
        }
    }
    if (ok) {
        ok = (stackSize > 0u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "StackSize shall be > 0u");
        }
    }
    StreamString fileFormatStr;
    if (ok) {
        ok = data.Read("FileFormat", fileFormatStr);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "FileFormat shall be specified");
        }
    }
    if (ok) {
        if (fileFormatStr == "csv") {
            fileFormat = FILE_FORMAT_CSV;
        }
        else if (fileFormatStr == "binary") {
            fileFormat = FILE_FORMAT_BINARY;
        }
        else {
            ok = false;
            REPORT_ERROR(ErrorManagement::ParametersError, "Invalid FileFormat specified");
        }
    }
    if (ok) {
        if (fileFormat == FILE_FORMAT_CSV) {
            ok = data.Read("CSVSeparator", csvSeparator);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "FileFormat=csv and CSVSeparator was not specified");
            }
        }
    }
    if (ok) {
        if (!data.Read("Filename", filename)) {
            REPORT_ERROR(ErrorManagement::Warning, "The Filename was not specified. It will have to be later set using the RPC mechanism.");
        }
        else {
            REPORT_ERROR(ErrorManagement::Information, "Filename was set to %s", filename.Buffer());
        }
    }

    uint32 storeOnTriggerU = 0u;
    if (ok) {
        ok = data.Read("StoreOnTrigger", storeOnTriggerU);
        storeOnTrigger = (storeOnTriggerU == 1u);
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "StoreOnTrigger shall be specified");
    }
    if (storeOnTrigger) {
        if (ok) {
            ok = data.Read("NumberOfPreTriggers", numberOfPreTriggers);

            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "NumberOfPreTriggers shall be specified");
            }
        }
        if (ok) {
            ok = data.Read("NumberOfPostTriggers", numberOfPostTriggers);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "NumberOfPostTriggers shall be specified");
            }
        }
    }
    if (ok) {
        StreamString overwriteStr;
        ok = data.Read("Overwrite", overwriteStr);
        if (ok) {
            if (overwriteStr == "yes") {
                overwrite = true;
            }
            else if (overwriteStr == "no") {
                overwrite = false;
            }
            else {
                ok = false;
            }
        }
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Overwrite shall be specified as either yes or no");
        }
    }
    if (ok) {
        ok = data.MoveRelative("Signals");
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Could not move to the Signals section");
        }
        if (ok) {
            //Do not allow to add signals in run-time
            ok = data.Write("Locked", 1);
        }
        if (ok) {
            ok = data.MoveToAncestor(1u);
        }
    }
    if (ok) {
        //Check if there are any Message elements set
        if (Size() > 0u) {
            ReferenceT<ReferenceContainer> msgContainer = Get(0u);
            if (msgContainer.IsValid()) {
                uint32 j;
                uint32 nOfMessages = msgContainer->Size();
                for (j = 0u; (j < nOfMessages) && (ok); j++) {
                    ReferenceT<Message> msg = msgContainer->Get(j);
                    ok = msg.IsValid();
                    if (ok) {
                        StreamString msgName = msg->GetName();
                        if (msgName == "FileOpenedOK") {
                            fileOpenedOKMsg = msg;
                        }
                        else if (msgName == "FileOpenedFail") {
                            fileOpenedFailMsg = msg;
                        }
                        else if (msgName == "FileFlushed") {
                            fileFlushedMsg = msg;
                        }
                        else if (msgName == "FileRuntimeError") {
                            fileRuntimeErrorMsg = msg;
                        }
                        else {
                            REPORT_ERROR(ErrorManagement::ParametersError, "Message %s is not supported.", msgName.Buffer());
                            ok = false;
                        }
                    }
                    else {
                        REPORT_ERROR(ErrorManagement::ParametersError, "Found an invalid Message in container %s", msgContainer->GetName());
                        ok = false;
                    }

                }
            }
        }
    }
    return ok;
}

bool FileWriter::SetConfiguredDatabase(StructuredDataI& data) {
    bool ok = DataSourceI::SetConfiguredDatabase(data);
    if (ok) {
        ok = data.MoveRelative("Signals");
    }
    //Check signal properties and compute memory
    numberOfBinaryBytes = 0u;
    if (ok) {
        //Do not allow samples
        uint32 functionNumberOfSignals = 0u;
        uint32 n;
        if (GetFunctionNumberOfSignals(OutputSignals, 0u, functionNumberOfSignals)) {
            for (n = 0u; (n < functionNumberOfSignals) && (ok); n++) {
                uint32 nSamples;
                ok = GetFunctionSignalSamples(OutputSignals, 0u, n, nSamples);
                if (ok) {
                    ok = (nSamples == 1u);
                }
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError, "The number of samples shall be exactly 1");
                }
            }
        }

        offsets = new uint32[GetNumberOfSignals()];
        uint32 nOfSignals = GetNumberOfSignals();
        //Count the number of bytes
        for (n = 0u; (n < nOfSignals) && (ok); n++) {
            offsets[n] = numberOfBinaryBytes;
            uint32 nBytes = 0u;
            ok = GetSignalByteSize(n, nBytes);
            numberOfBinaryBytes += nBytes;
        }
    }
    //Allocate memory
    if (ok) {
        dataSourceMemory = reinterpret_cast<char8 *>(GlobalObjectsDatabase::Instance()->GetStandardHeap()->Malloc(numberOfBinaryBytes));
    }

    //If the type is text prepare the Printf properties in advanced
    if (fileFormat == FILE_FORMAT_CSV) {
        uint32 nOfSignals = GetNumberOfSignals();
        uint32 n;
        if (ok) {
            signalsAnyType = new AnyType[nOfSignals];
        }

        for (n = 0u; (n < nOfSignals) && (ok); n++) {
            if (n != 0u) {
                //Add the separator
                ok = csvPrintfFormat.Printf("%s", csvSeparator.Buffer());
            }
            uint8 nDimensions = 0u;
            uint32 nElements = 0u;
            if (ok) {
                ok = GetSignalNumberOfDimensions(n, nDimensions);
            }
            if (ok) {
                ok = GetSignalNumberOfElements(n, nElements);
            }
            if (ok) {
                char8 *memPtr = &dataSourceMemory[offsets[n]];
                void *signalAddress = reinterpret_cast<void *&>(memPtr);
                signalsAnyType[n] = AnyType(GetSignalType(n), 0, signalAddress);
                signalsAnyType[n].SetNumberOfDimensions(nDimensions);
                signalsAnyType[n].SetNumberOfElements(0u, nElements);
            }

            TypeDescriptor signalType = GetSignalType(n);
            bool isUnsignedInteger = (signalType.type == UnsignedInteger);
            bool isSignedInteger = (signalType.type == SignedInteger);
            bool isFloat = (signalType.type == Float);
            if (ok) {
                if (isUnsignedInteger) {
                    ok = csvPrintfFormat.Printf("%s", "%u");
                }
                else if (isSignedInteger) {
                    ok = csvPrintfFormat.Printf("%s", "%d");
                }
                else if (isFloat) {
                    ok = csvPrintfFormat.Printf("%s", "%f");
                }
                else {
                    ok = false;
                    REPORT_ERROR(ErrorManagement::ParametersError, "Unsupported signal type.");
                }
            }
        }
        if (ok) {
            ok = csvPrintfFormat.Printf("%s", "\n");
            REPORT_ERROR(ErrorManagement::Information, "CSV format is %s", csvPrintfFormat.Buffer());
        }
    }

    if (filename.Size() > 0u) {
        ok = OpenFile(filename.Buffer());
    }

    return ok;
}

ErrorManagement::ErrorType FileWriter::OpenFile(StreamString filename) {
    REPORT_ERROR(ErrorManagement::Information, "Going to open file with name %s", filename.Buffer());
    if (overwrite) {
        Directory fileToDelete(filename.Buffer());
        (void) fileToDelete.Delete();

        fatalFileError = !outputFile.Open(filename.Buffer(), (BasicFile::ACCESS_MODE_W | BasicFile::FLAG_CREAT));
    }
    else {
        fatalFileError = !outputFile.Open(filename.Buffer(), (BasicFile::ACCESS_MODE_W | BasicFile::FLAG_APPEND));
    }

    if (!fatalFileError) {
        uint32 n;
        uint32 nOfSignals = GetNumberOfSignals();
        //Write the header
        if (fileFormat == FILE_FORMAT_CSV) {
            for (n = 0u; (n < nOfSignals) && (!fatalFileError); n++) {
                if (n == 0u) {
                    fatalFileError = !outputFile.Printf("%s", "#");
                }
                else {
                    fatalFileError = !outputFile.Printf("%s", csvSeparator.Buffer());
                }
                StreamString signalName;
                TypeDescriptor signalType = GetSignalType(n);
                uint32 nOfElements;
                if (!fatalFileError) {
                    fatalFileError = !GetSignalName(n, signalName);
                }
                if (!fatalFileError) {
                    fatalFileError = !GetSignalNumberOfElements(n, nOfElements);
                }
                if (!fatalFileError) {
                    fatalFileError = !outputFile.Printf("%s (%s)[%u]", signalName.Buffer(), TypeDescriptor::GetTypeNameFromTypeDescriptor(signalType),
                                                        nOfElements);
                }
            }
            if (!fatalFileError) {
                fatalFileError = !outputFile.Printf("%s", "\n");
            }
        }
        else {
            uint32 writeSize = sizeof(uint32);
            if (!fatalFileError) {
                //Write the number of signals
                fatalFileError = !outputFile.Write(reinterpret_cast<const char8 *>(&nOfSignals), writeSize);
            }
            //TODO
            for (n = 0u; (n < nOfSignals) && (!fatalFileError); n++) {
                //Write the signal type
                writeSize = sizeof(uint16);
                uint16 signalType = GetSignalType(n).all;
                if (!fatalFileError) {
                    fatalFileError = !outputFile.Write(reinterpret_cast<const char8 *>(&signalType), writeSize);
                }
                StreamString signalName;
                uint32 nOfElements;
                if (!fatalFileError) {
                    fatalFileError = !GetSignalName(n, signalName);
                }
                if (!fatalFileError) {
                    fatalFileError = !GetSignalNumberOfElements(n, nOfElements);
                }
                if (!fatalFileError) {
                    //Write the signal name
                    const uint32 SIGNAL_NAME_MAX_SIZE = 32u;
                    char8 signalNameMemory[SIGNAL_NAME_MAX_SIZE];
                    fatalFileError = !MemoryOperationsHelper::Set(&signalNameMemory[0], '\0', SIGNAL_NAME_MAX_SIZE);
                    if (!fatalFileError) {
                        uint32 copySize = signalName.Size();
                        if (copySize > SIGNAL_NAME_MAX_SIZE) {
                            copySize = SIGNAL_NAME_MAX_SIZE;
                        }
                        fatalFileError = !MemoryOperationsHelper::Copy(&signalNameMemory[0], signalName.Buffer(), copySize);
                    }
                    if (!fatalFileError) {
                        writeSize = SIGNAL_NAME_MAX_SIZE;
                        fatalFileError = !outputFile.Write(&signalNameMemory[0], writeSize);
                    }
                }
                if (!fatalFileError) {
                    //Write the signal number of elements
                    writeSize = sizeof(uint32);
                    fatalFileError = !outputFile.Write(reinterpret_cast<const char8 *>(&nOfElements), writeSize);
                }
            }
        }

        if (fileOpenedOKMsg.IsValid()) {
            //Reset any previous replies
            fileOpenedOKMsg->SetAsReply(false);
            if (!MessageI::SendMessage(fileOpenedOKMsg, this)) {
                StreamString destination = fileOpenedOKMsg->GetDestination();
                StreamString function = fileOpenedOKMsg->GetFunction();
                REPORT_ERROR(ErrorManagement::FatalError, "Could not send FileOpenedOK message to %s [%s]", destination.Buffer(), function.Buffer());
            }
        }
    }
    else {
        if (fileOpenedFailMsg.IsValid()) {
            //Reset any previous replies
            fileOpenedFailMsg->SetAsReply(false);
            if (!MessageI::SendMessage(fileOpenedFailMsg, this)) {
                StreamString destination = fileOpenedFailMsg->GetDestination();
                StreamString function = fileOpenedFailMsg->GetFunction();
                REPORT_ERROR(ErrorManagement::FatalError, "Could not send FileOpenedFail message to %s [%s]", destination.Buffer(), function.Buffer());
            }
        }
    }
    ErrorManagement::ErrorType ret(!fatalFileError);
    return ret;
}

ErrorManagement::ErrorType FileWriter::CloseFile() {
    ErrorManagement::ErrorType err = FlushFile();
    if (err.ErrorsCleared()) {
        if (outputFile.IsOpen()) {
            err = !outputFile.Close();
        }
    }
    return err;
}

ErrorManagement::ErrorType FileWriter::FlushFile() {
    bool ok = true;
    if (brokerAsyncTrigger != NULL_PTR(MemoryMapAsyncTriggerOutputBroker *)) {
        ok = brokerAsyncTrigger->FlushAllTriggers();
    }
    if (ok) {
        if (outputFile.IsOpen()) {
            ok = outputFile.Flush();
        }
    }
    if (!ok) {
        if (fileFlushedMsg.IsValid()) {
            //Reset any previous replies
            fileFlushedMsg->SetAsReply(false);
            if (!MessageI::SendMessage(fileFlushedMsg, this)) {
                StreamString destination = fileFlushedMsg->GetDestination();
                StreamString function = fileFlushedMsg->GetFunction();
                REPORT_ERROR(ErrorManagement::FatalError, "Could not send FileFlushed message to %s [%s]", destination.Buffer(), function.Buffer());
            }
        }
    }
    ErrorManagement::ErrorType err(ok);
    return err;
    /*lint -e{1762} function cannot be constant as it is registered as an RPC for CLASS_METHOD_REGISTER*/
}

const ProcessorType& FileWriter::GetCPUMask() const {
    return cpuMask;
}

uint32 FileWriter::GetNumberOfBuffers() const {
    return numberOfBuffers;
}

uint32 FileWriter::GetNumberOfPostTriggers() const {
    return numberOfPostTriggers;
}

uint32 FileWriter::GetNumberOfPreTriggers() const {
    return numberOfPreTriggers;
}

const StreamString& FileWriter::GetFilename() const {
    return filename;
}

uint32 FileWriter::GetStackSize() const {
    return stackSize;
}

bool FileWriter::IsStoreOnTrigger() const {
    return storeOnTrigger;
}

CLASS_REGISTER(FileWriter, "1.0")
CLASS_METHOD_REGISTER(FileWriter, FlushFile)
CLASS_METHOD_REGISTER(FileWriter, OpenFile)
CLASS_METHOD_REGISTER(FileWriter, CloseFile)

}

