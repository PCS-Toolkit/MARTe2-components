/**
 * @file NI9157Device.cpp
 * @brief Source file for class NI9157Device
 * @date 11/02/2021
 * @author Giuseppe Ferro
 * @author Pedro Lourenco
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
 *
 * @details This source file contains the definition of all the methods for
 * the class NI9157Device (public, protected, and private). Be aware that some 
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
#include "CreateNI9157DeviceOperatorI.h"
#include "NI9157Device.h"
#include "NI9157DeviceOperatorDatabase.h"
#include "NI9157DeviceOperatorTI.h"
#include "RegisteredMethodsMessageFilter.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {
NI9157Device::NI9157Device() :
        ReferenceContainer(), MessageI() {
    isOpened = 0u;
    isRunning = 0u;
    run = 0u;
    status = 0;
    session = 0u;
    // Install message filter
    ReferenceT < RegisteredMethodsMessageFilter > registeredMethodsMessageFilter("RegisteredMethodsMessageFilter");
    registeredMethodsMessageFilter->SetDestination(this);
    InstallMessageFilter( registeredMethodsMessageFilter);
}

NI9157Device::~NI9157Device() {
    /* Close the session now that we're done with it. */
    //NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
    REPORT_ERROR(ErrorManagement::Information, "Closing RIO Device ...");
    if (isOpened > 0u) {
        status = NiFpga_Reset(session);
        (void) NiFpga_Close(session, 0u);
    }
    REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "RIO Device closed %d", status);
}

bool NI9157Device::Initialise(StructuredDataI & data) {
    bool ret = ReferenceContainer::Initialise(data);
    if (ret) {
        ret = data.Read("NiRioDeviceName", niRioDeviceName);
        if (!ret) {
            REPORT_ERROR(ErrorManagement::InitialisationError, "Please specify the NI-RIO Device name (NiRioDeviceName)");
        }
        if (ret) {
            ret = data.Read("NiRioGenFile", niRioGenFile);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "Please specify the NI-RIO generated bit file (NiRioGenFile)");
            }
        }
        if (ret) {
            ret = data.Read("NiRioGenSignature", niRioGenSignature);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "Please spacify the NI-RIO generated signature (NiRioGenSignature)");
            }
        }
        if (ret) {
            if (!data.Read("Open", isOpened)) {
                isOpened = 0u;
            }
        }
        if (ret) {
            if (!data.Read("Run", run)) {
                run = 0u;
            }
        }
    }
    if (ret) {
        if (isOpened > 0u) {
            bool condition = true;
            uint32 cnt = 0u;
            while (condition) {
                if (isOpened == 1u) {
                    REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "Opening %s %s %s", niRioDeviceName.Buffer(), niRioGenSignature.Buffer(),
                                            niRioGenFile.Buffer());
                    /*lint -e{930} allow conversion from enum to int*/
                    status = NiFpga_Open(niRioGenFile.Buffer(), niRioGenSignature.Buffer(), niRioDeviceName.Buffer(),
                                         static_cast<uint32_t> (NiFpga_OpenAttribute_NoRun), &session);
                    ret = (status == 0);
                    if (!ret) {
                        isOpened = 0u;
                    }
                }
                if (data.MoveRelative("Configuration")) {
                        const char8 * suffixes[] = {"Bool", "U8", "I8", "U16", "I16", "U32", "I32", "U64", "I64", NULL_PTR(const char8 *)};
                        NiFpgaEx_ResourceType resTypes[] = { NiFpgaEx_ResourceType_ControlBool, NiFpgaEx_ResourceType_ControlU8, NiFpgaEx_ResourceType_ControlI8,
                                NiFpgaEx_ResourceType_ControlU16, NiFpgaEx_ResourceType_ControlI16, NiFpgaEx_ResourceType_ControlU32,
                                NiFpgaEx_ResourceType_ControlI32, NiFpgaEx_ResourceType_ControlU64, NiFpgaEx_ResourceType_ControlI64 };
                    uint32 numberOfElements = data.GetNumberOfChildren();
                    uint32 indexStore = 0xFFFFFFFFu;
                    NI9157DeviceOperatorTI *configurator = NULL_PTR(NI9157DeviceOperatorTI *);
                    for (uint32 k = 0u; (k < numberOfElements) && (ret); k++) {
                        StreamString variable;
                        variable = data.GetChildName(k);
                        uint32 i = 0u;
                        bool found = false;
                        while ((suffixes[i] != NULL_PTR(const char8 *)) && (!found) && (ret)) {
                            const char8 *typeInName = StringHelper::SearchString(variable.Buffer(), suffixes[i]);
                            found = (typeInName != NULL_PTR(const char8*));
                            if (found) {
                                uint32 begIndex = StringHelper::Length(suffixes[i]) + 1u;
                                const char8 *varName = &typeInName[begIndex];
                                StreamString operatorName = "NI9157Device";
                                operatorName += suffixes[i];
                                REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "Try to config %s %s", operatorName.Buffer(), varName);
                                CreateNI9157DeviceOperatorI *confCreator = NI9157DeviceOperatorDatabase::GetCreateNI9157DeviceOperator(operatorName.Buffer());
                                ret = (confCreator != NULL_PTR(CreateNI9157DeviceOperatorI *));
                                if (ret) {
                                    if (i != indexStore) {
                                        if (configurator != NULL_PTR(NI9157DeviceOperatorTI *)) {
                                            delete configurator;
                                        }
                                        configurator = confCreator->Create(this);
                                        indexStore = i;
                                    }
                                    ret = (configurator != NULL_PTR(NI9157DeviceOperatorTI *));
                                    if (ret) {
                                        uint32 var = 0u;
                                        /*lint -e{661} -e{662} the index i stops increasing when (suffixes[i]==NULL)*/
                                        status = NiFpgaEx_FindResource(session, varName, resTypes[i], &var);
                                        ret = (status == 0);

                                        if (ret) {
                                            uint64 value;
                                            ret = data.Read(variable.Buffer(), value);
                                            if (ret) {
                                                /*lint -e{613} ret == true => configurator != NULL*/
                                                /*lint -e{928} value is an uint8*/
                                                status = configurator->NiWrite(var, reinterpret_cast<uint8 *> (&value));
                                                ret = (status == 0);
                                                if (!ret) {
                                                    REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "Failed writing %s=%d status=%d",
                                                                            variable.Buffer(), value, static_cast<int32> (status));
                                                }
                                                else {
                                                    REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "%s=%d status=%d", variable.Buffer(), value,
                                                                            static_cast<int32> (status));
                                                }
                                            }
                                        }
                                        else {
                                            REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "NI variable %s not found status=%d",
                                                                    variable.Buffer(), status);
                                        }
                                    }
                                    else {
                                        REPORT_ERROR(ErrorManagement::InitialisationError, "Failed the creation of NI9157DeviceOperatorTI");
                                    }

                                }
                                else {
                                    REPORT_ERROR(ErrorManagement::InitialisationError, "Failed the creation of CreateNI9157DeviceOperatorI from name %s",
                                                 operatorName.Buffer());
                                }
                            }
                            i++;
                        }
                        if (ret) {
                            ret = (found);
                            if (!ret) {
                                REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "Cannot retrieve the type from the variable name %s", variable.Buffer());
                            }
                        }

                    }
                    if (configurator != NULL_PTR(NI9157DeviceOperatorTI *)) {
                        delete configurator;
                    }
                    if (ret) {
                        ret = data.MoveToAncestor(1u);
                    }
                }
                if ((isRunning == 0u) && (run > 0u)) {
                    status = NiFpga_MergeStatus(&status, NiFpga_Run(session, 0u));
                    if (status == 0) {
                        isRunning = 1u;
                    }
                    else {
                        REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "Failed run status=%d", static_cast<int32> (status));
                        status = NiFpga_Reset(session);
                        (void) NiFpga_Close(session, 0u);
                    }
                }
                if (run > 0u) {
                    condition = ((isRunning == 0u) || (cnt > 2u));
                }
                else {
                    condition = false;
                }
                cnt++;
            }
        }
    }
    REPORT_ERROR(ErrorManagement::Information, "Initialised returning %s with NI session=%d and status=%d", ret ? "true" : "false", (int32)session, (int32)status);
    return ret;
}

NiFpga_Status NI9157Device::Open() {
    if (isOpened == 0u) {
        /*lint -e{930} allow conversion from enum to int*/
        status = NiFpga_Open(niRioGenFile.Buffer(), niRioGenSignature.Buffer(), niRioDeviceName.Buffer(), static_cast<uint32_t> (NiFpga_OpenAttribute_NoRun), &session);
        if (status == 0) {
            isOpened = 1u;
        }
        else {
            REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "Failed open status=%d", static_cast<int32> (status));
        }
    }
    return status;
}
NiFpga_Status NI9157Device::Run() {
    if (isRunning == 0u) {
        status = NiFpga_MergeStatus(&status, NiFpga_Run(session, 0u));
        if (status == 0) {
            isRunning = 1u;
        }
        else {
            REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "Failed run status=%d", static_cast<int32> (status));
        }
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const bool type, uint32 &varDescriptor) { 
//try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlBool, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorBool, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoBool, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoBool, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const uint8 type, uint32 &varDescriptor) {
    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlU8, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorU8, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoU8, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoU8, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const int8 type, uint32 &varDescriptor) {

//try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlI8, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorI8, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoI8, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoI8, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const uint16 type, uint32 &varDescriptor) {
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlU16, &varDescriptor);
    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlU16, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorU16, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoU16, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoU16, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const int16 type, uint32 &varDescriptor) {
    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlI16, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorI16, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoI16, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoI16, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const uint32 type, uint32 &varDescriptor) {
    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlU32, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorU32, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoU32, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoU32, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const int32 type, uint32 &varDescriptor) {
    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlI32, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorI32, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoI32, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoI32, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const uint64 type, uint32 &varDescriptor) {

    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlU64, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorU64, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoU64, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoU64, &varDescriptor);
    }
    return status;
}

/*lint -e{715} the parameter type is not referenced but it is nevertheless used to override the function with templates*/
NiFpga_Status NI9157Device::FindResource(const char8 * const varName, const int64 type, uint32 &varDescriptor) {
    //try all the types
    status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_ControlI64, &varDescriptor);
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_IndicatorI64, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_TargetToHostFifoI64, &varDescriptor);
    }
    if (status != 0) {
        status = NiFpgaEx_FindResource(session, varName, NiFpgaEx_ResourceType_HostToTargetFifoI64, &varDescriptor);
    }
    return status;
}

NiFpga_Status NI9157Device::Reset() {
    status = NiFpga_Reset(session);
    if (status != 0) {
        REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "Failed reset status=%d", static_cast<int32> (status));
    }
    else {
        isRunning = 0;
    }
    return status;
}

NiFpga_Status NI9157Device::Close() {
    status = NiFpga_Close(session, 0u);
    if (status != 0) {
        REPORT_ERROR_PARAMETERS(ErrorManagement::InitialisationError, "Failed close status=%d", static_cast<int32> (status));
    }
    else {
        isOpened = 0u;
    }
    return status;
}

uint8 NI9157Device::IsOpened() const {
    return isOpened;
}
uint8 NI9157Device::IsRunning() const {
    return isRunning;
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, bool &value) const {
    return NiFpga_ReadBool(session, static_cast<uint32_t> (indicator), reinterpret_cast<NiFpga_Bool*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, int8 &value) const {
    return NiFpga_ReadI8(session, static_cast<uint32_t> (indicator), reinterpret_cast<int8_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, uint8 &value) const {
    return NiFpga_ReadU8(session, static_cast<uint32_t> (indicator), reinterpret_cast<uint8_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, int16 &value) const {
    return NiFpga_ReadI16(session, static_cast<uint32_t> (indicator), reinterpret_cast<int16_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, uint16 &value) const {
    return NiFpga_ReadU16(session, static_cast<uint32_t> (indicator), reinterpret_cast<uint16_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, int32 &value) const {
    return NiFpga_ReadI32(session, static_cast<uint32_t> (indicator), reinterpret_cast<int32_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, uint32 &value) const {
    return NiFpga_ReadU32(session, static_cast<uint32_t> (indicator), reinterpret_cast<uint32_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, int64 &value) const {
    return NiFpga_ReadI64(session, static_cast<uint32_t> (indicator), reinterpret_cast<int64_t*> (&value));
}

NiFpga_Status NI9157Device::NiRead(const uint32 indicator, uint64 &value) const {
    return NiFpga_ReadU64(session, static_cast<uint32_t> (indicator), reinterpret_cast<uint64_t*> (&value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const bool value) const {
    return NiFpga_WriteBool(session, static_cast<uint32_t> (control), static_cast<NiFpga_Bool> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const int8 value) const {
    return NiFpga_WriteI8(session, static_cast<uint32_t> (control), static_cast<int8_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const uint8 value) const {
    return NiFpga_WriteU8(session, static_cast<uint32_t> (control), static_cast<uint8_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const int16 value) const {
    return NiFpga_WriteI16(session, static_cast<uint32_t> (control), static_cast<int16_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const uint16 value) const {
    return NiFpga_WriteU16(session, static_cast<uint32_t> (control), static_cast<uint16_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const int32 value) const {
    return NiFpga_WriteI32(session, static_cast<uint32_t> (control), static_cast<int32_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const uint32 value) const {
    return NiFpga_WriteU32(session, static_cast<uint32_t> (control), static_cast<uint32_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const int64 value) const {
    return NiFpga_WriteI64(session, static_cast<uint32_t> (control), static_cast<int64_t> (value));
}

NiFpga_Status NI9157Device::NiWrite(const uint32 control, const uint64 value) const {
    return NiFpga_WriteU64(session, static_cast<uint32_t> (control), static_cast<uint64_t> (value));
}

NiFpga_Status NI9157Device::NiConfigureFifo(const uint32 fifo, const uint32 requestedDepth, uint32 &actualDepth) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ConfigureFifo2(session, static_cast<uint32_t> (fifo), static_cast<size_t> (requestedDepth), reinterpret_cast<size_t*> (&actualDepth));
}

NiFpga_Status NI9157Device::NiStartFifo(const uint32 fifo) const {
    return NiFpga_StartFifo(session, static_cast<uint32_t> (fifo));
}

NiFpga_Status NI9157Device::NiStopFifo(const uint32 fifo) const {
    return NiFpga_StopFifo(session, static_cast<uint32_t> (fifo));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, bool * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoBool(session, static_cast<uint32_t> (fifo), reinterpret_cast<NiFpga_Bool *> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, int8 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoI8(session, static_cast<uint32_t> (fifo), reinterpret_cast<int8_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, uint8 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoU8(session, static_cast<uint32_t> (fifo), reinterpret_cast<uint8_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, int16 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoI16(session, static_cast<uint32_t> (fifo), reinterpret_cast<int16_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, uint16 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoU16(session, static_cast<uint32_t> (fifo), reinterpret_cast<uint16_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, int32 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoI32(session, static_cast<uint32_t> (fifo), reinterpret_cast<int32_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, uint32 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoU32(session, static_cast<uint32_t> (fifo), reinterpret_cast<uint32_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, int64 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoI64(session, static_cast<uint32_t> (fifo), reinterpret_cast<int64_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));
}

NiFpga_Status NI9157Device::NiReadFifo(const uint32 fifo, uint64 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &elementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_ReadFifoU64(session, static_cast<uint32_t> (fifo), reinterpret_cast<uint64_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&elementsRemaining));;
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const bool * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoBool(session, static_cast<uint32_t> (fifo), reinterpret_cast<const NiFpga_Bool*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const int8 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoI8(session, static_cast<uint32_t> (fifo), reinterpret_cast<const int8_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const uint8 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoU8(session, static_cast<uint32_t> (fifo), reinterpret_cast<const uint8_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const int16 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoI16(session, static_cast<uint32_t> (fifo), reinterpret_cast<const int16_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const uint16 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoU16(session, static_cast<uint32_t> (fifo), reinterpret_cast<const uint16_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const int32 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoI32(session, static_cast<uint32_t> (fifo), reinterpret_cast<const int32_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const uint32 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoU32(session, static_cast<uint32_t> (fifo), reinterpret_cast<const uint32_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const int64 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoI64(session, static_cast<uint32_t> (fifo), reinterpret_cast<const int64_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Status NI9157Device::NiWriteFifo(const uint32 fifo, const uint64 * const data, const uint32 numberOfElements, const uint32 timeout, uint32 &emptyElementsRemaining) const {
    /*lint -e{826} -e{740} pointer cast is safe since the types are compatible*/
    return NiFpga_WriteFifoU64(session, static_cast<uint32_t> (fifo), reinterpret_cast<const uint64_t*> (data), static_cast<size_t> (numberOfElements), static_cast<uint32_t> (timeout), reinterpret_cast<size_t*> (&emptyElementsRemaining));
}

NiFpga_Session NI9157Device::GetSession() const {
    return session;
}

NiFpga_Status NI9157Device::ReleaseFifoElements(const uint32 fifo, const uint32 numberOfElements) {
    return NiFpga_ReleaseFifoElements(session, static_cast<uint32_t> (fifo), static_cast<size_t> (numberOfElements));
}

ErrorManagement::ErrorType NI9157Device::CrioStart() {
    ErrorManagement::ErrorType ret;
    if (isOpened == 0u) {
        ret = (Open() != 0);
    }
    if (ret.ErrorsCleared()) {
        ret = (Run() != 0);
    }
    return ret;
}

ErrorManagement::ErrorType NI9157Device::CrioStop() {
    ErrorManagement::ErrorType ret;
    if (isRunning > 0u) {
        ret = (Reset() != 0);
    }
    return ret;
}

ErrorManagement::ErrorType NI9157Device::WriteParam(StreamString varName, uint64 value, StreamString type) {
    ErrorManagement::ErrorType err;
    if (type == "bool") {
        uint32 varId;
        bool rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteBool(session, varId, static_cast<NiFpga_Bool> (value)) != 0);
        }
    }
    else if (type == "uint8") {
        uint32 varId;
        uint8 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteU8(session, varId, static_cast<uint8_t> (value)) != 0);
        }
    }
    else if (type == "uint16") {
        uint32 varId;
        uint16 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteU16(session, varId, static_cast<uint16_t> (value)) != 0);
        }
    }
    else if (type == "uint32") {
        uint32 varId;
        uint32 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteU32(session, varId, static_cast<uint32_t> (value)) != 0);
        }
    }
    else if (type == "uint64") {
        uint32 varId;
        uint64 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteU64(session, varId, static_cast<uint64_t> (value)) != 0);
        }
    }
    else if (type == "int8") {
        uint32 varId;
        int8 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteI8(session, varId, static_cast<int8_t> (value)) != 0);
        }
    }
    else if (type == "int16") {
        uint32 varId;
        int16 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteI16(session, varId, static_cast<int16_t> (value)) != 0);
        }
    }
    else if (type == "int32") {
        uint32 varId;
        int32 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteI32(session, varId, static_cast<int32_t> (value)) != 0);
        }
    }
    else if (type == "int64") {
        uint32 varId;
        int64 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_WriteI64(session, varId, static_cast<int64_t> (value)) != 0);
        }
    }
    else {
        err.SetError(4u);
    }
    return err;
}

ErrorManagement::ErrorType NI9157Device::ReadParam(StreamString varName, uint64 &value, StreamString type) {
    ErrorManagement::ErrorType err;
    if (type == "bool") {
        uint32 varId;
        bool rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadBool(session, varId, static_cast<NiFpga_Bool*> (&value)) != 0);
        }
    }
    if (type == "uint8") {
        uint32 varId;
        uint8 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadU8(session, varId, reinterpret_cast<uint8_t*> (&value)) != 0);
        }
    }
    else if (type == "uint16") {
        uint32 varId;
        uint16 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadU16(session, varId, reinterpret_cast<uint16_t*> (&value)) != 0);
        }
    }
    else if (type == "uint32") {
        uint32 varId;
        uint32 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadU32(session, varId, reinterpret_cast<uint32_t*> (&value)) != 0);
        }
    }
    else if (type == "uint64") {
        uint32 varId;
        uint64 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadU64(session, varId, reinterpret_cast<uint64_t*> (&value)) != 0);
        }
    }
    else if (type == "int8") {
        uint32 varId;
        int8 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadI8(session, varId, reinterpret_cast<int8_t*> (&value)) != 0);
        }
    }
    else if (type == "int16") {
        uint32 varId;
        int16 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadI16(session, varId, reinterpret_cast<int16_t*> (&value)) != 0);
        }
    }
    else if (type == "int32") {
        uint32 varId;
        int32 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadI32(session, varId, reinterpret_cast<int32_t*> (&value)) != 0);
        }
    }
    else if (type == "int64") {
        uint32 varId;
        int64 rType = 0u;
        err = (FindResource(varName.Buffer(), rType, varId) != 0);
        if (err.ErrorsCleared()) {
            err = (NiFpga_ReadI64(session, varId, reinterpret_cast<int64_t*> (&value)) != 0);
        }
    }
    else {
        err.SetError(4u);
    }
    return err;
}

CLASS_REGISTER(NI9157Device, "1.0")
CLASS_METHOD_REGISTER(NI9157Device, CrioStart)
CLASS_METHOD_REGISTER(NI9157Device, CrioStop)
CLASS_METHOD_REGISTER(NI9157Device, WriteParam)
CLASS_METHOD_REGISTER(NI9157Device, ReadParam)

}
