/**
 * @file MDSObjWavegen.h
 * @brief Header file for class MDSObjWavegen
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

#ifndef DATASOURCES_MDSOBJWAVEGEN_MDSOBJWAVEGEN_H_
#define DATASOURCES_MDSOBJWAVEGEN_MDSOBJWAVEGEN_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
//#include "DataSourceI.h"

#include "MessageI.h"
#include "StreamString.h"
#include "MDSWavegens.h"


/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

namespace MARTe {

/**
 * @brief MDSObjWaveGen is a data source which allows to read data from a MDSplus tree.
 * @details MDSObjWaveGen is an input data source which takes data from MDSPlus nodes (as many as desired) and publishes it on a real time application.
 *
 */
//class MDSObjWavegen: public DataSourceI, public ReferenceContainer {
class MDSObjWavegen: public DataSourceI  {
//class MDSObjWavegen: public ReferenceContainer {

//TODO Add the macro DLL_API to the class declaration (i.e. class DLL_API MDSReader)
public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief default constructor
     */
    MDSObjWavegen();

    /**
     * @brief default destructor.
     */
    virtual ~MDSObjWavegen();

    virtual bool Synchronise();

    virtual bool Initialise(StructuredDataI & data);

    virtual bool SetConfiguredDatabase(StructuredDataI & data);

    virtual bool PrepareNextState(const char8 * const currentStateName,
            const char8 * const nextStateName);

    virtual bool AllocateMemory();

    virtual uint32 GetNumberOfMemoryBuffers();

    virtual bool GetSignalMemoryBuffer(const uint32 signalIdx,
            const uint32 bufferIdx,
            void *&signalAddress);

    virtual const char8 *GetBrokerName(StructuredDataI &data,
            const SignalDirection direction);
/*
    virtual bool GetInputBrokers(ReferenceContainer &inputBrokers,
            const char8* const functionName,
            void * const gamMemPtr);

    virtual bool GetOutputBrokers(ReferenceContainer &outputBrokers,
            const char8* const functionName,
            void * const gamMemPtr);
*/
private:

    StreamString server;
    StreamString tree;
    StreamString connectionname;

    int32 shot;
    float32 frequency;

    ConfigurationDatabase privatecdb;

    uint8 interpolation;
    uint8 verbosity;

    uint16 configuredOutputs;
    uint32 numberofsignals;



    ConfigurationDatabase originalSignalInformation;

    bool typeAssigned;
    TypeDescriptor signalsType;

    float32 mintime;
    float32 maxtime;


    std::vector<uint32> samplesvector;
    std::vector<int32 *> timevectors;
    std::vector<void *> datavectors;
    std::vector<uint32> iterators;

    float32 timeoffset;

    int32 realtime;
    void* outputbuffer;

    void UpdateOutputsFloat32();
    void UpdateOutputsInt16();

};

}

#endif /* DATASOURCES_MDSREADER_MDSREADER_H_ */

