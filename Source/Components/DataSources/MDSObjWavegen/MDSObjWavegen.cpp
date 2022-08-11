/**
 * @file MDSObjWavegen.cpp
 * @brief Source file for class MDSObjWavegen
 * @date 08/07/2019
 * @author Galperti Cristian
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * @copyright Copyright 2019, Swiss Plasma Center, EPFL Lausanne
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
 * the class MDSReader (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

//#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

#include "stdio.h"

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "AdvancedErrorManagement.h"
#include "MemoryMapInputBroker.h"
#include "MemoryMapSynchronisedInputBroker.h"
#include "MemoryMapOutputBroker.h"
#include "MemoryMapSynchronisedOutputBroker.h"
#include "MDSObjWavegen.h"
#include "mdsobjects.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*lint -estring(1960, "*MDSplus::*") -estring(1960, "*std::*") Ignore errors that do not belong to this DataSource namespace*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

/*MDSObjWavegen::MDSObjWavegen() :
        DataSourceI() {
*/
MDSObjWavegen::MDSObjWavegen() : DataSourceI() {

    //printf("MDSObjWavegen init called\n");

}

MDSObjWavegen::~MDSObjWavegen()
{
    // TODO: check if automatically called
    //MDSReaderSPC::~MDSReaderSPC();
}

bool MDSObjWavegen::Synchronise()
{
    //return MDSReaderSPC::Synchronise();
    //printf("%s Synchronise called, time is %d\n", this->GetName(), realtime);

    //if(realtime < -1000000) return true;

    if(signalsType==Float32Bit)
    {
        UpdateOutputsFloat32();
    }
    else if (signalsType==SignedInteger16Bit)
    {
        UpdateOutputsInt16();
    }



    //testbuffer[0] = (float32)realtime;

    return true;
}

bool MDSObjWavegen::Initialise(StructuredDataI & data)
{
    //MDSplus::Tree *currenttree;
    MDSplus::Connection *currentconnection;
    StreamString basename = this->GetName();
    const class ClassProperties *classprop;
    bool ok = DataSourceI::Initialise(data);

    if (ok) {
        ok = signalsDatabase.MoveRelative("Signals");
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Could not move to the Signals section");
        }
        if (ok) {
            //Do not allow to add signals in run-time
            ok = signalsDatabase.Write("Locked", 1u);
            //int temp;
            //if(!signalsDatabase.Read("Locked", temp)) {printf("error setting locked\n"); ok=false;} else {printf("Locked readback is %d\n",temp);}
            //unsigned int nchildren= signalsDatabase.GetNumberOfChildren();
	    //printf("NAME:%s N CHILDREN:%d\n",signalsDatabase.GetName(),nchildren);
            //for(unsigned int i=0; i<nchildren; i++) printf("CHILD NAME: %s\n", signalsDatabase.GetChildName(i));
	   
  
            }
            if (ok) {
             //printf("Signals locked\n");
             ok = signalsDatabase.MoveToAncestor(1u);
            }
    }
    //ok=false;

    if(ok)
    {
        ok=data.Read("Shot", shot);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "Shot not set");
        }
    }
    if(ok)
    {
        ok=data.Read("Frequency", frequency);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "Frequency not set");
        }
    }
    if(ok)
    {
        ok=data.Read("Server", server);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "Server not set");
        }
    }
    if(ok)
    {
        ok=data.Read("Tree", tree);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "Tree not set");
        }
    }
    if(ok)
    {
        ok=data.Read("Interpolation", interpolation);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "Interpolation not set");
        }
    }
    if(ok)
    {
        ok=data.Read("Verbosity", verbosity);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "Verbosity not set");
        }
    }
    if(ok)
    {
        ok=data.Read("TimeOffset", timeoffset);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::ParametersError, "TimeOffset not set");
        }
    }



/*
    if (ok) {
        ok = data.MoveRelative("Signals");
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Could not move to the Signals section");
        }
        if (ok) {
            ok = data.Copy(originalSignalInformation);
        }
        if (ok) {
            ok = originalSignalInformation.MoveToRoot();
        }
        //Do not allow to add signals in run-time
        if (ok) {
            ok = signalsDatabase.MoveRelative("Signals");
        }
        if (ok) {
            ok = signalsDatabase.Write("Locked", 1u);
        }
        if (ok) {
            ok = signalsDatabase.MoveToAncestor(1u);
        }
    }
    if (ok) {
        ok = data.MoveToAncestor(1u);
    }
*/

    if(ok)
    {
        if(Size()==0)
        {
            REPORT_ERROR(ErrorManagement::InitialisationError, "No MDS interface classes declared within this datasource");
            ok=false;
        }
    }

    if(ok){

        REPORT_ERROR(ErrorManagement::Information, "%s connecting to server: %s, tree: %s, shot: %d", this->GetName(), server.Buffer(), tree.Buffer(), shot);

        try{
            // Distributed client implementation, not suitable for us since
            // we have remote TDI and Signals evaluation even in the parameters
            //currenttree = new MDSplus::Tree(ref->getTree().Buffer(), shot, "NORMAL");

            // This client implementation, this is the corret way to replicate mdsvalue matlab command
            currentconnection = new MDSplus::Connection((char *)(server.Buffer()));
            currentconnection->openTree((char *)(tree.Buffer()), (int)shot);
        }
        catch(const MDSplus::MdsException &ex)
        {
            REPORT_ERROR(ErrorManagement::InitialisationError, "Error opening the tree: %s\n",ex.what());
            return false;
        }

        configuredOutputs=0;
        typeAssigned=false;
        for(uint32 i=0; i<Size(); i++){
            ReferenceT<MDSWavegen> ref=Get(i);
            if(ref.IsValid()){
                classprop = ref->GetClassProperties();
                REPORT_ERROR(ErrorManagement::Information, "actualizing group %s (class %s)", ref->GetName(), classprop->GetName());
                if(!ref->Actualize(currentconnection))
                {
                    REPORT_ERROR(ErrorManagement::InitialisationError, "Actualization failed.");
                    ok=false;
                    break;
                }
                else
                {
                    if(verbosity>0) ref->PrintContent();
                    configuredOutputs+=ref->GetNoOfSignals();
                }
                if(!typeAssigned)
                {
                    signalsType = ref->GetType();
                    typeAssigned=true;
                }
                else
                {
                    if(!(signalsType == ref->GetType()))
                    {
                        REPORT_ERROR(ErrorManagement::InitialisationError, "Mixed signals types are not supported.");
                        ok=false;
                        break;
                    }
                }
                if(!ref->CheckIncrementingTimes())
                {
                    REPORT_ERROR(ErrorManagement::InitialisationError, "At least 1 signal of the group have not incrementing timebase.");
                    ok=false;
                    break;
                }
            }
        }

        ReferenceT<MDSWavegen> ref=Get(0);
        mintime=ref->MinTime();
        maxtime=ref->MaxTime();
        //printf("ref %d, min %f max %f\n", 0, ref->MinTime(), ref->MaxTime());
        for(uint32 i=1; i<Size(); i++){
            ReferenceT<MDSWavegen> ref=Get(i);
            //printf("ref %d, min %f max %f\n", i, ref->MinTime(), ref->MaxTime());
            if(ref->MinTime()<mintime) mintime=ref->MinTime();
            if(ref->MaxTime()>maxtime) maxtime=ref->MaxTime();
        }


        REPORT_ERROR(ErrorManagement::Information, "%s disconnecting, %d signals configured, timespan [%6.3f,%6.3f]", this->GetName(), configuredOutputs, mintime, maxtime);
        //TODO: ask how to force a disconnect
        delete currentconnection;
    }

    // Check types
    if(ok)
    {
        ok = (Float32Bit ==  signalsType) || (SignedInteger16Bit == signalsType);
        if(!ok)
        {
            REPORT_ERROR(ErrorManagement::InitialisationError, "Signals type not supported.");
        }
    }

    // Build final time/data vectors
    samplesvector.clear();
    timevectors.clear();
    datavectors.clear();
    if(ok)
    {
        for(uint32 i=0; i<Size(); i++){
            ReferenceT<MDSWavegen> ref=Get(i);
            ok&=ref->AppendTimes(samplesvector, timevectors, timeoffset);
            ok&=ref->AppendValues(datavectors);
            if(!ok)
            {
                REPORT_ERROR(ErrorManagement::InitialisationError, "Error building final data vectors for MDS object %d", i+1);
                break;
            }
        }
    }


    //printf("Number of sv %d, tv %d, dv %d\n", (int)samplesvector.size(), (int)timevectors.size(), (int)datavectors.size());

    // Output buffer allocation
    if(ok)
    {
        if(signalsType==Float32Bit)
        {
            outputbuffer=malloc(configuredOutputs * sizeof(float32));
        }
        else if (signalsType==SignedInteger16Bit)
        {
            outputbuffer=malloc(configuredOutputs * sizeof(int16));
        }
        else
        {
            outputbuffer=NULL;
        }
        ok=(outputbuffer!=NULL);
        if(!ok) REPORT_ERROR(ErrorManagement::InitialisationError, "Could not allocate memory for outputbuffer");
    }

    // Iterators allocation
    iterators.clear();
    for(uint32 i=0; i<configuredOutputs; i++) iterators.push_back(0);

    /*
    for(unsigned int i=0; i<samplesvector[0]; i++)
    {
        printf("%d %d %f\n", i, timevectors[0][i], ((float32 *)datavectors[0])[i]);
    }
    */

    //printf("Locking signals\n");
                                                                      
    return ok;
}

bool MDSObjWavegen::SetConfiguredDatabase(StructuredDataI & data)
{
    bool ok = DataSourceI::SetConfiguredDatabase(data);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "DataSourceI::SetConfiguredDatabase(data) returned false");
    }

    if (ok) {
        ok = data.MoveRelative("Signals");
    }

    if (ok) { //read number of nodes per function numberOfNodeNames
        //0u (second argument) because previously it is checked
        numberofsignals = GetNumberOfSignals();        //0u (second argument) because previously it is checked
        //printf("numberofsignals = %d\n", numberofsignals); 

        ok = (numberofsignals == 2u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The number of signals must be 2 (1 input time and 1 output bus)");
        }
    }

    // TODO: add all checks as per MDSReader datasource
    // TODO: add check of time and output bus signals (declration order, types and number of elements)

    return ok;
    //return false;
}

bool MDSObjWavegen::PrepareNextState(const char8 * const currentStateName,const char8 * const nextStateName)
{
    //return MDSReaderSPC::PrepareNextState(currentStateName, nextStateName);
    return true;
}

bool MDSObjWavegen::AllocateMemory()
{
    //return MDSReaderSPC::AllocateMemory();
    return true;
}

uint32 MDSObjWavegen::GetNumberOfMemoryBuffers()
{
    //return MDSReaderSPC::GetNumberOfMemoryBuffers();
    return 0;
}

bool MDSObjWavegen::GetSignalMemoryBuffer(const uint32 signalIdx,
        const uint32 bufferIdx,
        void *&signalAddress)
{
    //return MDSReaderSPC::GetSignalMemoryBuffer(signalIdx, bufferIdx, signalAddress);
    if(signalIdx == 0)
    {
        signalAddress = (void *) &realtime;
    }
    else
    {
        signalAddress = (void *)outputbuffer;
    }

    return true;
}

const char8 *MDSObjWavegen::GetBrokerName(StructuredDataI &data,
        const SignalDirection direction)
{
    const char8 *brokerName = NULL_PTR(const char8 *);

    switch(direction)
    {
    case InputSignals:
        brokerName = "MemoryMapInputBroker";
        break;
    case OutputSignals:
        brokerName = "MemoryMapSynchronisedOutputBroker";
        break;
    default:
        brokerName = "";
        break;
    }

    return brokerName;
}
/*
bool MDSObjWavegen::GetInputBrokers(ReferenceContainer &inputBrokers,
        const char8* const functionName,
        void * const gamMemPtr)
{
    bool ok;
    ReferenceT<MemoryMapInputBroker> broker("MemoryMapInputBroker");
    //ReferenceT<MemoryMapSynchronisedInputBroker> broker("MemoryMapSynchronisedInputBroker");
    ok = broker->Init(InputSignals, *this, functionName, gamMemPtr);
    if (ok) {
        ok = inputBrokers.Insert(broker);
    }
    return ok;
}

bool MDSObjWavegen::GetOutputBrokers(ReferenceContainer &outputBrokers,
        const char8* const functionName,
        void * const gamMemPtr)
{
    bool ok;
    //ReferenceT<MemoryMapOutputBroker> broker("MemoryMapOutputBroker");
    ReferenceT<MemoryMapSynchronisedOutputBroker> broker("MemoryMapSynchronisedOutputBroker");
    ok = broker->Init(OutputSignals, *this, functionName, gamMemPtr);
    if (ok) {
        ok = outputBrokers.Insert(broker);
    }
    return ok;
}
*/
void MDSObjWavegen::UpdateOutputsFloat32()
{
    float32 *outPtr = (float32 *)outputbuffer;
    uint32 stime;

    float32 m;
    //float32 deltat;

    for(unsigned int ch=0; ch<configuredOutputs; ch++)
    {

        /*
        for(stime = iterators[ch]; stime<samplesvector[ch]; stime++)
        {
            if(realtime<=timevectors[ch][stime])
            {
                iterators[ch]=stime;
                break;
            }
        }
        */
        stime=iterators[ch];
        while((stime < samplesvector[ch]) && (realtime>timevectors[ch][stime]))
        {
            stime++;
        }
        iterators[ch]=stime;


        switch(interpolation)
        {
        case 0:
            if(stime==samplesvector[ch])
                *outPtr = ((float32 *)datavectors[ch])[stime-1];
            else
                *outPtr = ((float32 *)datavectors[ch])[stime];
            break;
        case 1:
            if(stime==0)
            {
                *outPtr = ((float32 *)datavectors[ch])[stime];
            }
            else if(stime==samplesvector[ch])
            {
                *outPtr = ((float32 *)datavectors[ch])[stime-1];
            }
            else
            {
                // TODO: this computation could be optimized precomputing all m
                m=(((float32 *)datavectors[ch])[stime]-((float32 *)datavectors[ch])[stime-1]) /
                        (float32)(timevectors[ch][stime]-timevectors[ch][stime-1]);
                *outPtr = ((float32 *)datavectors[ch])[stime-1] +
                        (float32)(realtime-timevectors[ch][stime-1]) * m;
            }
            break;
        }




        //printf("realtime %d ch %d samples %d iter %d stime %d val %f\n", realtime, ch, samplesvector[ch], iterators[ch], stime, *outPtr);
        outPtr++;
    }
}

void MDSObjWavegen::UpdateOutputsInt16()
{
    int16 *outPtr = (int16 *)outputbuffer;
    uint32 stime;

    //float32 m;
    //float32 deltat;

    for(unsigned int ch=0; ch<configuredOutputs; ch++)
    {

        /*
        for(stime = iterators[ch]; stime<samplesvector[ch]; stime++)
        {
            if(realtime<=timevectors[ch][stime])
            {
                iterators[ch]=stime;
                break;
            }
        }
        */
        stime=iterators[ch];
        while((stime < samplesvector[ch]) && (realtime>(timevectors[ch][stime])))
        {
            stime++;
        }
        iterators[ch]=stime;


        switch(interpolation)
        {
        case 0:
        case 1:
            if(stime==samplesvector[ch])
                *outPtr = ((int16 *)datavectors[ch])[stime-1];
            else
                *outPtr = ((int16 *)datavectors[ch])[stime];
            break;
        }

        //printf("realtime %d ch %d samples %d iter %d stime %d val %f\n", realtime, ch, samplesvector[ch], iterators[ch], stime, *outPtr);
        outPtr++;
    }

}


CLASS_REGISTER(MDSObjWavegen, "1.0")

}
