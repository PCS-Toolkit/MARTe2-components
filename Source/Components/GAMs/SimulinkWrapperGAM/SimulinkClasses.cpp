/**
 * @file SimulinkClasses.cpp
 * @brief Source file for classes SimulinkClasses
 * @date 27/05/2019
 * @author Galperti Cristian
 *
 * @copyright Copyright 2019 Swiss Plasma Center, EPFL Lausanne Switzerland
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
 * the class SimulinkWrapperGAM (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "SimulinkClasses.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe
{

/*---------------------------------------------------------------------------*/
/*                              SimulinkDataI                                */
/*---------------------------------------------------------------------------*/
    
SimulinkDataI::SimulinkDataI() {
    
    fullName = "";
    dataClass = "Data";
    
    numberOfDimensions = 0u;
    for (uint16 dimIdx = 0u; dimIdx < MAXDIMS; dimIdx++) {
        numberOfElements[dimIdx] = 1u;
    }
    totalNumberOfElements = 0u;
    orientation = rtwCAPI_SCALAR;
    
    byteSize     = 0u;
    dataTypeSize = 0u;
    
    cTypeName     = "void";
    MARTeTypeName = "void";
    type = InvalidType;
    
    address = NULL_PTR(void*);
    
    verbosity = 0u;
}

SimulinkDataI::~SimulinkDataI() {
}

void SimulinkDataI::PrintData(uint32 maxNameLength /* = 0u */, StreamString additionalText /* = "" */) {
    
    // Adds spaces at the end of the name until it reaches maxNameLength
    StreamString nameWithSpacesAtTheEnd = fullName;
    while (nameWithSpacesAtTheEnd.Size() < maxNameLength) {
        nameWithSpacesAtTheEnd.Printf("%s", " ");
    }
    
    REPORT_ERROR_STATIC(ErrorManagement::Information,
        "%s %s │ dims (%-3u %-3u %-3u) │ elems %-5u │ type %-7s │ bytesize %-6u │ %s @%p",
        dataClass.Buffer(),
        nameWithSpacesAtTheEnd.Buffer(),
        numberOfElements[0u],
        numberOfElements[1u],
        numberOfElements[2u],
        totalNumberOfElements,
        MARTeTypeName.Buffer(),
        byteSize,
        additionalText.Buffer(),
        address
        );
}

/*---------------------------------------------------------------------------*/
/*                            SimulinkParameter                              */
/*---------------------------------------------------------------------------*/

bool SimulinkParameter::Actualise(AnyType& sourceParameter) {
    
//     bool ok = sourceParameter.IsValid();
//     if (!ok) {
//         REPORT_ERROR_STATIC(ErrorManagement::ParametersError,
//             "Parameter actualization failed: invalid reference.");
//     }
    
    bool ok = true;
    
    if (ok) {
        
        // with this reference the name of the class can be retrieved if needed
        //ReferenceT<ReferenceContainer> sourceParamAsContainer = sourceParameter;
        //const char8* paramName = sourceParamAsContainer->GetClassProperties()->GetName();
        
//         ok = sourceParameter.IsValid();
//         
//         if (!ok) {
//             REPORT_ERROR_STATIC(ErrorManagement::Warning,
//                 "Parameter %s: invalid reference. Is the parameter loader class ok?",
//                 fullName.Buffer());
//         }
        
        // Before importing value, coherence checks between model parameter
        // and imported value are performed.
                
        // Type coherence check
        if (ok) {
            
            TypeDescriptor slkType  = TypeDescriptor::GetTypeDescriptorFromTypeName(MARTeTypeName.Buffer());
            TypeDescriptor mdsType = sourceParameter.GetTypeDescriptor();
            
            ok = (mdsType == slkType);
            
            if (!ok) {
                REPORT_ERROR_STATIC(ErrorManagement::Warning,
                    "Parameter %s data type not matching (MDSplus: %s, model: %s)",
                    fullName.Buffer(),
                    TypeDescriptor::GetTypeNameFromTypeDescriptor(mdsType),
                    MARTeTypeName.Buffer());
            }
        }
        
        // Orientation check is not necessary: an AnyType is passed by, so
        // whatever generated this AnyType is in charge for granting that
        // the AnyType data buffer is in standard row-major format.
        
        // Type size coherence check
        if (ok) {
            
            uint32 slkTypeSize = dataTypeSize;
            uint32 mdsTypeSize = sourceParameter.GetByteSize();
            
            ok = (mdsTypeSize == slkTypeSize);
            
            if (!ok) {
                REPORT_ERROR_STATIC(ErrorManagement::Warning,
                    "Parameter %s data type size not matching (MDSplus: %d, model: %d)",
                    fullName.Buffer(), mdsTypeSize, slkTypeSize);
            }
        }
        
        // Number of dimensions check (scalar, vector or matrix)
        if(ok) {
            
            ok = (sourceParameter.GetNumberOfDimensions() == numberOfDimensions);
            
            if (!ok) {
                REPORT_ERROR_STATIC(ErrorManagement::Warning,
                    "Parameter %s number of dimensions not matching (MDSplus: %d, model: %d)",
                    fullName.Buffer(), sourceParameter.GetNumberOfDimensions(), numberOfDimensions);
            }
        }
        
        // Dimensionality check
        if(ok)
        {
            uint32 mdsDim1 = sourceParameter.GetNumberOfElements(0u);
            uint32 mdsDim2 = sourceParameter.GetNumberOfElements(1u);
            uint32 mdsDim3 = sourceParameter.GetNumberOfElements(2u);
            
            uint32 slkDim1 = numberOfElements[0u];
            uint32 slkDim2 = numberOfElements[1u];
            uint32 slkDim3 = numberOfElements[2u];
            
            // On the model side, scalar are [1,1], vectors are [1,N] or [N,1],
            // matrices are [N,M]. AnyType instead can be respectively [1], [N], [N,M].
            switch (sourceParameter.GetNumberOfDimensions()) {
                
                case 0u: { // Scalar
                    
                    ok = (mdsDim1 == 1u);
                    break;
                }
                
                case 1u: { // Vector
                    
                    // Model vectors can be [1,N] or [N,1], both are ok
                    bool orientOk1 = ( (mdsDim1 == slkDim1) && (mdsDim2 == 1u)      );
                    bool orientOk2 = ( (mdsDim1 == 1u)      && (mdsDim2 == slkDim1) );
                    bool orientOk3 = ( (mdsDim1 == slkDim2) && (mdsDim2 == 1u)      );
                    
                    ok = ( orientOk1 || orientOk2 || orientOk3 );
                    break;
                }
                
                case 2u: { // Matrix
                    
                    ok = ( mdsDim1 = slkDim1 && mdsDim2 == slkDim2 && mdsDim3 == 1u );
                    break;
                }
                
                case 3u: { // 3D matrix
                    
                    ok = ( mdsDim1 = slkDim1 && mdsDim2 == slkDim2 && mdsDim3 == slkDim3 );
                    break;
                }
                
                default:
                    
                    REPORT_ERROR_STATIC(ErrorManagement::ParametersError,
                        "Parameter %s: number of dimensions > 3.",
                        fullName.Buffer());
                    break;
                
            }
            if (!ok) {
                REPORT_ERROR_STATIC(ErrorManagement::ParametersError,
                    "Parameter %s: dimensions not matching (MDSplus: [%u, %u, %u], model: [%u, %u, %u]).",
                    fullName.Buffer(),
                    mdsDim1, mdsDim2, mdsDim3,
                    slkDim1, slkDim2, slkDim3);
            }
        }

        // Total data size check
        if (ok)
        {
            ok = (sourceParameter.GetDataSize() == byteSize);
            
            if(!ok) {
                REPORT_ERROR_STATIC(ErrorManagement::Warning,
                    "Parameter %s: total data size not matching (MDSplus: %d, model: %d)",
                    fullName.Buffer(), sourceParameter.GetDataSize(), byteSize);
            }
        }
        
        // Checks passed, data buffer can be copied from the input AnyType to the model memory
        if (ok) {
            MemoryOperationsHelper::Copy(address, sourceParameter.GetDataPointer(), sourceParameter.GetDataSize());
        }
    }
    
    return ok;
}

/*---------------------------------------------------------------------------*/
/*                              SimulinkSignal                               */
/*---------------------------------------------------------------------------*/

SimulinkSignal::SimulinkSignal() {
    
    dataClass = "Signal";
    
    MARTeAddress = NULL_PTR(void*);
    offset       = 0u;
}

void SimulinkSignal::PrintSignal(uint32 maxNameLength /* = 0u */ ) {
    
    SimulinkDataI::PrintData(maxNameLength);
}

/*---------------------------------------------------------------------------*/
/*                               SimulinkPort                                */
/*---------------------------------------------------------------------------*/

SimulinkPort::SimulinkPort() {
    
    isValid      = true;
    isTyped      = false;
    isContiguous = true;
    hasHomogeneousType = true;
    
    runningOffset = 0u;
    typeBasedSize = 0u;
    CAPISize      = 0u;
    offsetBasedSize = 0u;
    baseAddress       = NULL_PTR(void*);
    lastSignalAddress = NULL_PTR(void*);
    
    mode = ROOTSIG_INPUTS;
    
    dataClass = "Port";
}

SimulinkPort::~SimulinkPort() {
    
    uint32 numberOfSignalsInThisPort = carriedSignals.GetSize();
    for (uint32 signalIdx = 0U; signalIdx < numberOfSignalsInThisPort; signalIdx++) {
        SimulinkSignal* toDelete;
        if(carriedSignals.Extract(0U, toDelete)) {
            delete toDelete;
        }
    }
}

bool SimulinkPort::AddSignal(SimulinkSignal* signalIn) {
    
    bool ok = false;
    ok = carriedSignals.Add(signalIn);
    
    if (ok) {
        uint32 totalNumOfElems = 1u;
        for (uint32 elemIdx = 0u; elemIdx < MAXDIMS; elemIdx++) {
            totalNumOfElems *= signalIn->numberOfElements[elemIdx];
        }
        offsetBasedSize = signalIn->offset + signalIn->dataTypeSize*totalNumOfElems;
    }
    
    return ok;
}

void SimulinkPort::PrintPort(uint32 maxNameLength) {
    
    StreamString typeStr = "";
    if (hasHomogeneousType) {
        typeStr = "homogeneous type";
    }
    else {
        typeStr = "mixed types     ";
    }
    
    StreamString additionalText = "";
    additionalText.Seek(0u);
    additionalText.Printf(
        "type-based size: %-6u │ offset-based size %-6u │ %-16s │",
        typeBasedSize, offsetBasedSize, typeStr.Buffer()
    );
    
    SimulinkDataI::PrintData(maxNameLength, additionalText);
    
}

SimulinkInputPort::SimulinkInputPort() {
    
    dataClass = "IN  port";
}

SimulinkOutputPort::SimulinkOutputPort() {
    
    dataClass = "OUT port";
}

SimulinkInputPort::~SimulinkInputPort() {

}

SimulinkOutputPort::~SimulinkOutputPort() {

}

void SimulinkInputPort::CopyData() {
    MemoryOperationsHelper::Copy(address, MARTeAddress, CAPISize);
}

void SimulinkOutputPort::CopyData() {
    MemoryOperationsHelper::Copy(MARTeAddress, address, CAPISize);
}


} /* namespace MARTe */