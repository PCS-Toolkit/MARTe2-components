/**
 * @file MDSWavegens.cpp
 * @brief Source file for class MDSWavegens
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
 * the class MDSWavegens (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

#include <stdio.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "MDSWavegens.h"
#include <tgmath.h>
#include "AdvancedErrorManagement.h"


/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

static uint16 DTACQ_ACQ196_phys2log[] = { 1, 17, 2, 18, 9, 25, 10, 26, 3, 19, 4, 20, 11, 27, 12, 28, 5, 21, 6, 22, 13, 29, 14, 30, 7, 23, 8, 24, 15, 31, 16, 32 };

MDSWavegen::MDSWavegen()
{
    mdspath="";
    NumberOfSignals=0;

    time.clear();
    valid=false;

    //printf("MDSWavegen constructor called\n");
}

MDSWgSigSingle::MDSWgSigSingle() : MDSWavegen()
{
    //printf("MDSWgSigSingle constructor called\n");
    data.clear();
}

MDSWgSigArray1::MDSWgSigArray1() : MDSWavegen()
{
    startidx=0;
    stopidx=0;
    numelems=0;
    data.clear();
    //printf("MDSWgSigArray1 constructor called\n");
}

MDSWgSigArray1Int16::MDSWgSigArray1Int16() : MDSWavegen()
{
    startidx=0;
    stopidx=0;
    numelems=0;
    data.clear();
    //printf("MDSWgSigArray1 constructor called\n");
}

MDSWgSigArray1Int16ACQ196Reorder::MDSWgSigArray1Int16ACQ196Reorder() : MDSWavegen()
{
    startidx=0;
    stopidx=0;
    numelems=0;
    data.clear();
    //printf("MDSWgSigArray1 constructor called\n");
}


bool MDSWavegen::Initialise(StructuredDataI &data)
{
    bool ret;
    std::string sctemp;
    ret=data.Read("Path", mdspath);

    if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWavegen %s, Path not set", this->GetName());
    //printf("MDSWavegen init called\n");

    if(!(data.Read("Timepath", mdstimepath)))
    {
         mdstimepath=mdspath;
    }


    return ret;
}

bool MDSWgSigSingle::Initialise(StructuredDataI &data)
{
    bool ret=MDSWavegen::Initialise(data);
    //printf("MDSWgSigSingle init called\n");

    return ret;
}

bool MDSWgSigArray1::Initialise(StructuredDataI &data)
{
    bool ret=MDSWavegen::Initialise(data);

    if(ret)
    {
        ret=data.Read("StartIdx", startidx);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StartIdx not set", this->GetName());
    }

    if(ret)
    {
        ret=data.Read("StopIdx", stopidx);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StopIdx not set", this->GetName());
    }

    if(ret)
    {
        ret = startidx<=stopidx;
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StartIdx must be <= that StopIdx", this->GetName());
    }
    //printf("MDSWgSigArray1 init called\n");

    return ret;
}

bool MDSWgSigArray1Int16::Initialise(StructuredDataI &data)
{
    bool ret=MDSWavegen::Initialise(data);

    if(ret)
    {
        ret=data.Read("StartIdx", startidx);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StartIdx not set", this->GetName());
    }

    if(ret)
    {
        ret=data.Read("StopIdx", stopidx);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StopIdx not set", this->GetName());
    }

    if(ret)
    {
        ret = startidx<=stopidx;
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StartIdx must be <= that StopIdx", this->GetName());
    }

    return ret;
}

bool MDSWgSigArray1Int16ACQ196Reorder::Initialise(StructuredDataI &data)
{
    bool ret=MDSWavegen::Initialise(data);

    if(ret)
    {
        ret=data.Read("StartIdx", startidx);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StartIdx not set", this->GetName());
    }

    if(ret)
    {
        ret=data.Read("StopIdx", stopidx);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StopIdx not set", this->GetName());
    }

    if(ret)
    {
        ret = startidx<=stopidx;
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, StartIdx must be <= that StopIdx", this->GetName());
    }

    if(ret)
    {
        ret=data.Read("PeriodPath", nominalperiodpath);
        if(!ret) REPORT_ERROR(ErrorManagement::ParametersError, "MDSWgSigArray1 %s, PeriodPath not set", this->GetName());
    }


    return ret;

}

bool MDSWavegen::Actualize(MDSplus::Connection *conn)
{
    return true;
}

bool MDSWavegen::CheckIncrementingTimes(void) const
{
    bool ok = true;
    for(uint16 ch=0; ch<NumberOfSignals; ch++)
    {
        if(SamplesPerSignal[ch]>1)
        for(uint32 sample=1; sample<SamplesPerSignal[ch]; sample++)
        {
            if(time[ch][sample-1]>=time[ch][sample])
            {
                REPORT_ERROR(ErrorManagement::InitialisationError, "%s group, signal %d, not incrementing time at sample %d t(s-1)=%f t(s)=%f", this->GetName(), ch+1, sample+1, time[ch][sample-1], time[ch][sample]);
                ok=false;
                break;
            }
        }
        if(!ok) break;
    }
    return ok;
}

float32 MDSWavegen::MinTime(void) const
{
    float32 mintime=0.0;
    if(NumberOfSignals>0)
    {
        mintime=time[0][0];
        for(uint16 ch=1; ch<NumberOfSignals; ch++) if(time[ch][0]<mintime) mintime=time[ch][0];
    }
    return mintime;
}

float32 MDSWavegen::MaxTime(void) const
{
    float32 maxtime=0.0;
    if(NumberOfSignals>0)
    {
        maxtime=time[0][SamplesPerSignal[0]-1];
        for(uint16 ch=1; ch<NumberOfSignals; ch++) if(time[ch][SamplesPerSignal[ch]-1]>maxtime) maxtime=time[ch][SamplesPerSignal[ch]-1];
    }
    return maxtime;
}

bool MDSWavegen::AppendTimes(std::vector<uint32> &sv, std::vector<int32 *> &tv, float32 offset) const
{
    int32 *timevector;
    if(NumberOfSignals>0)
    {
        for(uint16 ch=0; ch<NumberOfSignals; ch++)
        {
            sv.push_back(SamplesPerSignal[ch]);
            timevector=(int32 *)malloc(SamplesPerSignal[ch]*sizeof(int32));
            if(timevector==NULL)
            {
                return false;
            }
            for(uint32 sample=0; sample<SamplesPerSignal[ch]; sample++) timevector[sample]=(int32)((time[ch][sample] + offset) * 1e6);
            tv.push_back(timevector);
        }
    }
    return true;
}

bool MDSWgSigSingle::Actualize(MDSplus::Connection *conn)
{
    std::string errormsg;
    std::string mdschannel;
    const class ClassProperties* me;
    me=this->GetClassProperties();
    std::string datachannelname;
    std::string timechannelname;
    std::string backslash = "\\";
    int numElements;
    int* mdsdims;
    int  mdsnumDim;
    float *retArray;
    float *retArrayTime;
    float32 *localData;
    float32 *localTime;

    bool ret = MDSWavegen::Actualize(conn);
    if(!ret) return ret;

    valid=true;
    try {
            if(sizeof(float) != sizeof(float32))
            {
                errormsg="Data type mismatch between MDS+ and MARTe, cannot load the channel";
                throw errormsg;
            }

            // TODO: check mds data type ?

            // Get values
            datachannelname=mdspath.Buffer();
            MDSplus::Data *nodeData = conn->get(datachannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            // TODO: check dimensions
            retArray = nodeData->getFloatArray(&numElements);

            //printf("%s VALUES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            // Get time
            timechannelname="dim_of(";
            timechannelname+=mdstimepath.Buffer();
            timechannelname+=")";
            nodeData = conn->get(timechannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            // TODO: check dimensions
            retArrayTime = nodeData->getFloatArray(&numElements);

            //printf("%s TIMES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            NumberOfSignals=1;
            SamplesPerSignal.push_back(mdsdims[0]);
            localData = (float32 *)malloc(SamplesPerSignal[0] * sizeof(float32));
            if(localData==NULL)
            {
                errormsg="cannot allocate local data memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            localTime = (float32 *)malloc(SamplesPerSignal[0] * sizeof(float32));
            if(localTime==NULL)
            {
                errormsg="cannot allocate local time memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            memcpy(localData, retArray,  SamplesPerSignal[0] * sizeof(float32));
            memcpy(localTime, retArrayTime,  SamplesPerSignal[0] * sizeof(float32));
            data.push_back(localData);
            time.push_back(localTime);
            sourcechannel.push_back(datachannelname);

    }
    catch(MDSplus::MdsException &ex)
    {
        REPORT_ERROR(ErrorManagement::InitialisationError, "%s MDS+ error getting node %s: %s, wavegen invalidated.", me->GetName(), mdspath.Buffer(), ex.what());
        valid=false;
    }
    catch(std::string &localex)
    {
        REPORT_ERROR(ErrorManagement::InitialisationError, "%s local error getting node %s: %s, wavegen invalidated.", me->GetName(), mdspath.Buffer(), localex.c_str());
        valid=false;
    }

    return valid;
}

bool MDSWgSigArray1::Actualize(MDSplus::Connection *conn)
{
    std::string errormsg;
    std::string mdschannel;
    uint16 channelidx;
    const class ClassProperties* me;
    me=this->GetClassProperties();
    std::size_t strfound;
    std::string datachannelname;
    std::string timechannelname;
    std::string backslash = "\\";
    std::string sctemp;
    int numElements;
    int* mdsdims;
    int  mdsnumDim;
    float *retArray;
    float *retArrayTime;
    float32 *localData;
    float32 *localTime;

    bool ret = MDSWavegen::Actualize(conn);
    if(!ret) return ret;

    valid=true;
    try {
        if(sizeof(float) != sizeof(float32))
        {
            errormsg="Data type mismatch between MDS+ and MARTe, cannot load the channel";
            throw(errormsg);
        }
        numelems=stopidx-startidx+1;
        if(numelems<=0)
        {
            errormsg="Wrong number of elements";
            throw(errormsg);
        }

        NumberOfSignals=numelems;

        for(channelidx=startidx; channelidx<=stopidx; channelidx++)
        {
            // Building channel name
            size_t strsize = snprintf( nullptr, 0,  mdspath.Buffer(), channelidx ) + 1; // Extra space for '\0', this calculate the size
            char * csmdspath = (char *)new char[ strsize ];
            snprintf( csmdspath, strsize, mdspath.Buffer(), channelidx );
            datachannelname = csmdspath;
            strfound = datachannelname.find("\\\\", 0);
            while(strfound!=std::string::npos)
            {
                datachannelname.replace(strfound,2,backslash);
                strfound = datachannelname.find("\\\\", 0);
            }

            // TODO: check mds data type ?
            // Get values
            MDSplus::Data *nodeData = conn->get(datachannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            retArray = nodeData->getFloatArray(&numElements);

            //printf("%s VALUES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            // Get time
            strsize = snprintf( nullptr, 0,  mdstimepath.Buffer(), channelidx ) + 1; // Extra space for '\0', this calculate the size
            csmdspath = (char *)new char[ strsize ];
            snprintf( csmdspath, strsize, mdspath.Buffer(), channelidx );
            timechannelname = csmdspath;
            strfound = timechannelname.find("\\\\", 0);
            while(strfound!=std::string::npos)
            {
                timechannelname.replace(strfound,2,backslash);
                strfound = timechannelname.find("\\\\", 0);
            }
            sctemp="dim_of(";
            sctemp+=timechannelname;
            sctemp+=")";
            timechannelname=sctemp;
            nodeData = conn->get(timechannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            retArrayTime = nodeData->getFloatArray(&numElements);

            //printf("%s TIMES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            SamplesPerSignal.push_back(mdsdims[0]);
            localData = (float32 *)malloc(SamplesPerSignal.back() * sizeof(float32));
            if(localData==NULL)
            {
                errormsg="cannot allocate local data memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            localTime = (float32 *)malloc(SamplesPerSignal.back() * sizeof(float32));
            if(localTime==NULL)
            {
                errormsg="cannot allocate local time memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            memcpy(localData, retArray,  SamplesPerSignal.back() * sizeof(float32));
            memcpy(localTime, retArrayTime,  SamplesPerSignal.back() * sizeof(float32));
            data.push_back(localData);
            time.push_back(localTime);
            sourcechannel.push_back(datachannelname);
        }
    }
    catch(MDSplus::MdsException &ex)
    {
        REPORT_ERROR(ErrorManagement::Warning, "%s MDS+ error getting node %s: %s, wavegen invalidated.", me->GetName(), mdspath.Buffer(), ex.what());
        valid=false;
    }
    catch(std::string &localex)
    {
        REPORT_ERROR(ErrorManagement::Warning, "%s local error getting node %s: %s, wavegen invalidated.", me->GetName(), mdspath.Buffer(), localex.c_str());
        valid=false;
    }

    return valid;
}

bool MDSWgSigArray1Int16::Actualize(MDSplus::Connection *conn)
{
    std::string errormsg;
    std::string mdschannel;
    uint16 channelidx;
    const class ClassProperties* me;
    me=this->GetClassProperties();
    std::size_t strfound;
    std::string datachannelname;
    std::string timechannelname;
    std::string backslash = "\\";
    std::string sctemp;
    int numElements;
    int* mdsdims;
    int  mdsnumDim;
    short *retArray;
    float *retArrayTime;
    int16 *localData;
    float32 *localTime;

    bool ret = MDSWavegen::Actualize(conn);
    if(!ret) return ret;

    valid=true;
    try {
        if(sizeof(short) != sizeof(int16))
        {
            errormsg="Data type mismatch between MDS+ and MARTe, cannot load the channel";
            throw errormsg;
        }
        numelems=stopidx-startidx+1;
        if(numelems<=0)
        {
            errormsg="Wrong number of elements";
            throw(errormsg);
        }

        NumberOfSignals=numelems;

        for(channelidx=startidx; channelidx<=stopidx; channelidx++)
        {
            // Building channel name
            size_t strsize = snprintf( nullptr, 0,  mdspath.Buffer(), channelidx ) + 1; // Extra space for '\0', this calculate the size
            char * csmdspath = (char *)new char[ strsize ];
            snprintf( csmdspath, strsize, mdspath.Buffer(), channelidx );
            datachannelname = csmdspath;
            strfound = datachannelname.find("\\\\", 0);
            while(strfound!=std::string::npos)
            {
                datachannelname.replace(strfound,2,backslash);
                strfound = datachannelname.find("\\\\", 0);
            }

            // TODO: check mds data type ?
            // Get values
            MDSplus::Data *nodeData = conn->get(datachannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            retArray = nodeData->getShortArray(&numElements);

            //printf("%s VALUES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            // Get nominal period


            // Get time
            strsize = snprintf( nullptr, 0,  mdstimepath.Buffer(), channelidx ) + 1; // Extra space for '\0', this calculate the size
            csmdspath = (char *)new char[ strsize ];
            snprintf( csmdspath, strsize, mdspath.Buffer(), channelidx );
            timechannelname = csmdspath;
            strfound = timechannelname.find("\\\\", 0);
            while(strfound!=std::string::npos)
            {
                timechannelname.replace(strfound,2,backslash);
                strfound = timechannelname.find("\\\\", 0);
            }
            sctemp="dim_of(";
            sctemp+=timechannelname;
            sctemp+=")";
            timechannelname=sctemp;
            nodeData = conn->get(timechannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            retArrayTime = nodeData->getFloatArray(&numElements);

            //printf("%s TIMES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            SamplesPerSignal.push_back(mdsdims[0]);
            localData = (int16 *)malloc(SamplesPerSignal.back() * sizeof(int16));
            if(localData==NULL)
            {
                errormsg="cannot allocate local data memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            localTime = (float32 *)malloc(SamplesPerSignal.back() * sizeof(float32));
            if(localTime==NULL)
            {
                errormsg="cannot allocate local time memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            memcpy(localData, retArray,  SamplesPerSignal.back() * sizeof(int16));
            memcpy(localTime, retArrayTime,  SamplesPerSignal.back() * sizeof(float32));
            data.push_back(localData);
            time.push_back(localTime);
            sourcechannel.push_back(datachannelname);
        }
    }
    catch(MDSplus::MdsException &ex)
    {
        REPORT_ERROR(ErrorManagement::Warning, "%s MDS+ error getting node %s: %s, wavegen invalidated.", me->GetName(), mdspath.Buffer(), ex.what());
        valid=false;
    }
    catch(std::string &localex)
    {
        REPORT_ERROR(ErrorManagement::Warning, "%s local error getting node %s: %s, wavegen invalidated.", me->GetName(), mdspath.Buffer(), localex.c_str());
        valid=false;
    }

    return valid;
}

bool MDSWgSigArray1Int16ACQ196Reorder::Actualize(MDSplus::Connection *conn)
{
    std::string errormsg;
    std::string mdschannel;
    uint16 channelidx, hwchannelidx;
    const class ClassProperties* me;
    me=this->GetClassProperties();
    std::size_t strfound;
    std::string datachannelname;
    std::string timechannelname;
    std::string backslash = "\\";
    std::string sctemp;
    int numElements;
    int* mdsdims;
    int  mdsnumDim;
    short *retArray;
    float *retArrayTime;
    int16 *localData;
    float32 *localTime;
    unsigned int nominalperiod;
    float nominalperiodsecs;

    bool ret = MDSWavegen::Actualize(conn);
    if(!ret) return ret;

    valid=true;
    try {
        if(sizeof(short) != sizeof(int16))
        {
            errormsg="Data type mismatch between MDS+ and MARTe, cannot load the channel";
            throw errormsg;
        }
        numelems=stopidx-startidx+1;
        if(numelems<=0)
        {
            errormsg="Wrong number of elements";
            throw(errormsg);
        }

        NumberOfSignals=numelems;

        for(channelidx=startidx; channelidx<=stopidx; channelidx++)
        {

            hwchannelidx=((channelidx-1)/32)*32+DTACQ_ACQ196_phys2log[((channelidx-1)%32)];
            //if(channelidx>=96)

            //printf("MDSWgSigArray1Int16ACQ196Reorder map %d -> %d\n",channelidx,hwchannelidx);


            // Building channel name
            size_t strsize = snprintf( nullptr, 0,  mdspath.Buffer(), hwchannelidx ) + 1; // Extra space for '\0', this calculate the size
            char * csmdspath = (char *)new char[ strsize ];
            snprintf( csmdspath, strsize, mdspath.Buffer(), hwchannelidx );
            datachannelname = csmdspath;
            strfound = datachannelname.find("\\\\", 0);
            while(strfound!=std::string::npos)
            {
                datachannelname.replace(strfound,2,backslash);
                strfound = datachannelname.find("\\\\", 0);
            }

            // TODO: check mds data type ?
            // Get values
            MDSplus::Data *nodeData = conn->get(datachannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            retArray = nodeData->getShortArray(&numElements);

            //printf("%s VALUES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");


            // Get nominal period
            nodeData = conn->get(nominalperiodpath.Buffer());
            nominalperiod = nodeData->getIntUnsigned();
            nominalperiodsecs = ((float)nominalperiod)/1e6;

            // Get time
            strsize = snprintf( nullptr, 0,  mdstimepath.Buffer(), hwchannelidx ) + 1; // Extra space for '\0', this calculate the size
            csmdspath = (char *)new char[ strsize ];
            snprintf( csmdspath, strsize, mdspath.Buffer(), hwchannelidx );
            timechannelname = csmdspath;
            strfound = timechannelname.find("\\\\", 0);
            while(strfound!=std::string::npos)
            {
                timechannelname.replace(strfound,2,backslash);
                strfound = timechannelname.find("\\\\", 0);
            }
            sctemp="dim_of(";
            sctemp+=timechannelname;
            sctemp+=")";
            timechannelname=sctemp;
            nodeData = conn->get(timechannelname.c_str());
            mdsdims=nodeData->getShape(&mdsnumDim);
            retArrayTime = nodeData->getFloatArray(&numElements);

            // Regularizing time
            // MATLAB version: timebase = round(timebase.*1/d_time)*d_time-d_time/1000;

            // This is the nearest timebase reguilarization I found to make simulation data = reporcess data
            // however there are some discrepancies, probably we have to switch to integer timebases on MDS+ in the
            // next version of node 02
            for(int timeelem=0; timeelem<numElements; timeelem++)
            {
                if(retArrayTime[timeelem]<=0)
                    retArrayTime[timeelem]=roundf(retArrayTime[timeelem]/nominalperiodsecs)*nominalperiodsecs-nominalperiodsecs/100;
                else
                    retArrayTime[timeelem]=roundf(retArrayTime[timeelem]/nominalperiodsecs+1)*nominalperiodsecs-nominalperiodsecs/100;

                //retArrayTime[timeelem]=floorf(retArrayTime[timeelem]/nominalperiodsecs)*nominalperiodsecs-nominalperiodsecs/1000;
                //retArrayTime[timeelem]=floorf(retArrayTime[timeelem]/nominalperiodsecs)*nominalperiodsecs;
            }

            //printf("%s TIMES: num dims %d, values: \n", datachannelname.c_str(), mdsdims[0]);
            //for(int j=0; j<mdsdims[0]; j++) printf("%f ", retArray[j]);
            //printf("\n");

            SamplesPerSignal.push_back(mdsdims[0]);
            localData = (int16 *)malloc(SamplesPerSignal.back() * sizeof(int16));
            if(localData==NULL)
            {
                errormsg="cannot allocate local data memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            localTime = (float32 *)malloc(SamplesPerSignal.back() * sizeof(float32));
            if(localTime==NULL)
            {
                errormsg="cannot allocate local time memory for ";
                errormsg+=datachannelname;
                throw errormsg;
            }
            memcpy(localData, retArray,  SamplesPerSignal.back() * sizeof(int16));
            memcpy(localTime, retArrayTime,  SamplesPerSignal.back() * sizeof(float32));
            data.push_back(localData);
            time.push_back(localTime);
            sourcechannel.push_back(datachannelname);
        }
    }
    catch(MDSplus::MdsException &ex)
    {
        REPORT_ERROR(ErrorManagement::Warning, "%s MDS+ error getting node %s: %s, wavegen invalidated.", me->GetName(), datachannelname.c_str(), ex.what());
        valid=false;
    }
    catch(std::string &localex)
    {
        REPORT_ERROR(ErrorManagement::Warning, "%s local error getting node %s: %s, wavegen invalidated.", me->GetName(), datachannelname.c_str(), localex.c_str());
        valid=false;
    }

    return valid;
}



void MDSWavegen::PrintContent(void) const
{
    char buffer[256];

    uint32 size;
    for(unsigned int i=0; i<NumberOfSignals; i++)
    {
        size = SamplesPerSignal[i];

        if(size>=4)
        {
          snprintf(buffer, 256, "channel: %-40s, %6d points, t=[%8.4f, %8.4f ... %8.4f, %8.4f] y=[%8.4f, %8.4f ... %8.4f, %8.4f]",
                 sourcechannel[i].c_str(), size,
                 time[i][0], time[i][1], time[i][size-2], time[i][size-1],
                 GetData(i,0), GetData(i,1), GetData(i,size-2), GetData(i,size-1)
          );
        }
        else if(size>=2)
        {
            snprintf(buffer, 256, "channel: %-40s, %6d points, t=[%8.4f ... %8.4f] y=[%8.4f ... %8.4f]",
                   sourcechannel[i].c_str(), size,
                   time[i][0], time[i][size-1],
                   GetData(i,0), GetData(i,size-1)
            );
        }
        else
        {
            snprintf(buffer, 256, "channel: %-40s, %6d points",
                   sourcechannel[i].c_str(), size
            );
        }
        REPORT_ERROR(ErrorManagement::Information, buffer);

    }
}

void MDSWgSigSingle::PrintContent(void) const
{
    MDSWavegen::PrintContent();
}

void MDSWgSigArray1::PrintContent(void) const
{
    MDSWavegen::PrintContent();
}

void MDSWgSigArray1Int16::PrintContent(void) const
{
    MDSWavegen::PrintContent();
}

void MDSWgSigArray1Int16ACQ196Reorder::PrintContent(void) const
{
    MDSWavegen::PrintContent();
}

TypeDescriptor MDSWavegen::GetType(void) const
{
    return InvalidType;
}

TypeDescriptor MDSWgSigSingle::GetType(void) const
{
    return Float32Bit;
}

TypeDescriptor MDSWgSigArray1::GetType(void) const
{
    return Float32Bit;
}

TypeDescriptor MDSWgSigArray1Int16::GetType(void) const
{
    return SignedInteger16Bit;
}

TypeDescriptor MDSWgSigArray1Int16ACQ196Reorder::GetType(void) const
{
    return SignedInteger16Bit;
}

bool MDSWavegen::AppendValues(std::vector<void *> &vv) const
{
    return false;
}

bool MDSWgSigSingle::AppendValues(std::vector<void *> &vv) const
{
    float32 *valuevector;
    if(NumberOfSignals>0)
    {
        for(uint16 ch=0; ch<NumberOfSignals; ch++)
        {
            valuevector=(float32 *)malloc(SamplesPerSignal[ch]*sizeof(float32));
            if(valuevector==NULL)
            {
                return false;
            }
            for(uint32 sample=0; sample<SamplesPerSignal[ch]; sample++) valuevector[sample]=data[ch][sample];
            vv.push_back((void *)valuevector);
        }

    }
    return true;
}

bool MDSWgSigArray1::AppendValues(std::vector<void *> &vv) const
{
    float32 *valuevector;
    if(NumberOfSignals>0)
    {
        for(uint16 ch=0; ch<NumberOfSignals; ch++)
        {
            valuevector=(float32 *)malloc(SamplesPerSignal[ch]*sizeof(float32));
            if(valuevector==NULL)
            {
                return false;
            }
            for(uint32 sample=0; sample<SamplesPerSignal[ch]; sample++) valuevector[sample]=data[ch][sample];
            vv.push_back((void *)valuevector);
        }
    }
    return true;
}

bool MDSWgSigArray1Int16::AppendValues(std::vector<void *> &vv) const
{
    int16 *valuevector;
    if(NumberOfSignals>0)
    {
        for(uint16 ch=0; ch<NumberOfSignals; ch++)
        {
            valuevector=(int16 *)malloc(SamplesPerSignal[ch]*sizeof(int16));
            if(valuevector==NULL)
            {
                return false;
            }
            for(uint32 sample=0; sample<SamplesPerSignal[ch]; sample++) valuevector[sample]=data[ch][sample];
            vv.push_back((void *)valuevector);
        }
    }
    return true;
}

bool MDSWgSigArray1Int16ACQ196Reorder::AppendValues(std::vector<void *> &vv) const
{
    int16 *valuevector;
    if(NumberOfSignals>0)
    {
        for(uint16 ch=0; ch<NumberOfSignals; ch++)
        {
            valuevector=(int16 *)malloc(SamplesPerSignal[ch]*sizeof(int16));
            if(valuevector==NULL)
            {
                return false;
            }
            for(uint32 sample=0; sample<SamplesPerSignal[ch]; sample++) valuevector[sample]=data[ch][sample];
            vv.push_back((void *)valuevector);
        }
    }
    return true;
}

float MDSWavegen::GetData(unsigned int channel, uint32 at) const
{
    return 0.0;
}


float MDSWgSigSingle::GetData(unsigned int channel, uint32 at) const
{
    return (float)data[channel][at];
}

float MDSWgSigArray1::GetData(unsigned int channel, uint32 at) const
{
    return (float)data[channel][at];
}

float MDSWgSigArray1Int16::GetData(unsigned int channel, uint32 at) const
{
    return (float)data[channel][at];
}

float MDSWgSigArray1Int16ACQ196Reorder::GetData(unsigned int channel, uint32 at) const
{
    return (float)data[channel][at];
}

MDSWavegen::~MDSWavegen()
{
    //printf("1 ");
    SamplesPerSignal.clear();

    //printf("2 ");
    for(unsigned int i=0; i<time.size(); i++) delete time[i];

    //for(unsigned int i=0; i<displaydata.size(); i++) delete displaydata[i];
    //printf("3 ");
    for(unsigned int i=0; i<sourcechannel.size(); i++) sourcechannel[i].empty();

    //printf("4 ");
    time.clear();

    //printf("5\n");
    sourcechannel.clear();
}

MDSWgSigSingle::~MDSWgSigSingle()
{
    //printf("6 ");
    for(unsigned int i=0; i<data.size(); i++)
        if(data[i]!=NULL) delete data[i];
    //printf("7\n");
    data.clear();
}

MDSWgSigArray1::~MDSWgSigArray1()
{
    //printf("8 ");
    for(unsigned int i=0; i<data.size(); i++)
        if(data[i]!=NULL) delete data[i];
    //printf("9\n");
    data.clear();
}

MDSWgSigArray1Int16::~MDSWgSigArray1Int16()
{
    //printf("10 ");
    for(unsigned int i=0; i<data.size(); i++)
        if(data[i]!=NULL) delete data[i];
    //printf("11\n");
    data.clear();
}

MDSWgSigArray1Int16ACQ196Reorder::~MDSWgSigArray1Int16ACQ196Reorder()
{
    //printf("12 ");
    for(unsigned int i=0; i<data.size(); i++)
        if(data[i]!=NULL) delete data[i];
    //printf("13\n");
    data.clear();
}


CLASS_REGISTER(MDSWavegen, "1.0")

CLASS_REGISTER(MDSWgSigSingle, "1.0")
CLASS_REGISTER(MDSWgSigArray1, "1.0")
CLASS_REGISTER(MDSWgSigArray1Int16, "1.0")
CLASS_REGISTER(MDSWgSigArray1Int16ACQ196Reorder, "1.0")

}
