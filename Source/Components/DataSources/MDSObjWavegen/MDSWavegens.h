/**
 * @file MDSWavegens.h
 * @brief Header file for class MDSWavegens
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

 * @details This header file contains the declaration of the class MDSObjWavegen
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef DATASOURCES_MDSWAVEGENS_H_
#define DATASOURCES_MDSWAVEGENS_H_

//#define MDSTHINCLIENT
//#define MDSDISTCLIENT


/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

#include <string>
#include <vector>

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
//#include "DataSourceI.h"


#include "ReferenceContainer.h"
#include "ConfigurationDatabase.h"
#include "StreamString.h"
#include "mdsobjects.h"
#include "TypeDescriptor.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

namespace MARTe {


/**
 *
 */
class MDSWavegen: public ReferenceContainer {
public:
    CLASS_REGISTER_DECLARATION()

    MDSWavegen();
    virtual ~MDSWavegen();

    inline uint16 GetNoOfSignals(void) {return NumberOfSignals;}


    virtual bool Initialise(StructuredDataI &data);
    virtual bool Actualize(MDSplus::Connection *conn);
    virtual float GetData(unsigned int channel, uint32 at) const;
    virtual void PrintContent(void) const;
    virtual TypeDescriptor GetType() const;

    bool CheckIncrementingTimes(void) const;
    float32 MinTime(void) const;
    float32 MaxTime(void) const;

    bool AppendTimes(std::vector<uint32> &sv, std::vector<int32 *> &tv, float32 offset) const;
    virtual bool AppendValues(std::vector<void *> &vv) const;

protected:
    StreamString mdspath;
    StreamString mdstimepath;

    uint16 NumberOfSignals;
    std::vector<uint32> SamplesPerSignal;
    std::vector<float32 *> time;
    std::vector<std::string> sourcechannel;

    bool valid;

private:

};

class MDSWgSigSingle : public MDSWavegen {
public:
    CLASS_REGISTER_DECLARATION()

    MDSWgSigSingle();
    virtual ~MDSWgSigSingle();

    virtual bool Initialise(StructuredDataI &data);
    virtual bool Actualize(MDSplus::Connection *conn);
    virtual float GetData(unsigned int channel, uint32 at) const;
    virtual void PrintContent(void) const;
    virtual TypeDescriptor GetType() const;
    virtual bool AppendValues(std::vector<void *> &vv) const;

private:

    std::vector<float32 *> data;
};

class MDSWgSigArray1 : public MDSWavegen {
public:
    CLASS_REGISTER_DECLARATION()

    MDSWgSigArray1();
    virtual ~MDSWgSigArray1();

    virtual bool Initialise(StructuredDataI &data);
    virtual bool Actualize(MDSplus::Connection *conn);
    virtual float GetData(unsigned int channel, uint32 at) const;
    virtual void PrintContent(void) const;
    virtual TypeDescriptor GetType() const;
    virtual bool AppendValues(std::vector<void *> &vv) const;

private:

    uint16 startidx;
    uint16 stopidx;
    uint16 numelems;

    std::vector<float32 *> data;
};


class MDSWgSigArray1Int16 : public MDSWavegen {
public:
    CLASS_REGISTER_DECLARATION()

    MDSWgSigArray1Int16();
    virtual ~MDSWgSigArray1Int16();

    virtual bool Initialise(StructuredDataI &data);
    virtual bool Actualize(MDSplus::Connection *conn);
    virtual float GetData(unsigned int channel, uint32 at) const;
    virtual void PrintContent(void) const;
    virtual TypeDescriptor GetType() const;
    virtual bool AppendValues(std::vector<void *> &vv) const;

private:

    uint16 startidx;
    uint16 stopidx;
    uint16 numelems;

    std::vector<int16 *> data;
};

class MDSWgSigArray1Int16ACQ196Reorder : public MDSWavegen {
public:
    CLASS_REGISTER_DECLARATION()

    MDSWgSigArray1Int16ACQ196Reorder();
    virtual ~MDSWgSigArray1Int16ACQ196Reorder();

    virtual bool Initialise(StructuredDataI &data);
    virtual bool Actualize(MDSplus::Connection *conn);
    virtual float GetData(unsigned int channel, uint32 at) const;
    virtual void PrintContent(void) const;
    virtual TypeDescriptor GetType() const;
    virtual bool AppendValues(std::vector<void *> &vv) const;

private:

    uint16 startidx;
    uint16 stopidx;
    uint16 numelems;

    std::vector<int16 *> data;

    StreamString nominalperiodpath;


};



}

#endif
