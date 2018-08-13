# MARTe2-components

The MARTe2-components repository offers a set of components that are deemed useful to the community. 

These include general purpose GAMs, support to off-the-shelf hardware and interfaces to known supervisory control and data acquisition systems.

## GAMs

| Component | Documentation |
| --------- | ------------- |
| [BaseLib2GAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/BaseLib2GAM) | [Encapsulate and execute GAMs from BaseLib2 in MARTe2](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1BaseLib2GAM.html)|
| [ConversionGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/ConversionGAM) | [GAM which allows to convert between different signal types](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1ConversionGAM.html)|
| [ConstantGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/ConstantGAM) | [Generate constant values that can be updated with messages. ](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1ConstantGAM.html)|
| [FilterGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/FilterGAM) | [GAM which allows to implement FIR & IIR filter with float32 type](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1FilterGAM.html)|
| [Interleaved2FlatGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/Interleaved2FlatGAM) | [Allows to translate an interleaved memory region into a flat memory area (and vice-versa)..](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1Interleaved2FlatGAM.html)|
| [IOGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/IOGAM) | [GAM which copies its inputs to its outputs. Allows to plug different DataSources (e.g. driver with a DDB).](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1IOGAM.html)|
| [MuxGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/MuxGAM) | [Multiplexer GAM that allows multiplex different signals.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1MuxGAM.html)|
| [PIDGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/PIDGAM) | [A generic PID with saturation and anti-windup.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1PIDGAM.html)|
| [SSMGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/SSMGAM) | [A generic State Space model with constant matrices and float64.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1SSMGAM.html)|
| [StatisticsGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/StatisticsGAM) | [GAM which provides average, standard deviation, minimum and maximum of its input signal over a moving time window.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1StatisticsGAM.html)|
| [TimeCorrectionGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/TimeCorrectionGAM) | [GAM which allows to estimate the next time-stamp value in a continuous time stream.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1TimeCorrectionGAM.html)|
| [TriggerOnChangeGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/TriggerOnChangeGAM) | [Triggers MARTe::Message events on the basis of commands received in the input signals.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1TriggerOnChangeGAM.html)|
| [WaveformGAM](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/GAMs/WaveformGAM) | [GAM which provides average, standard deviation, minimum and maximum of its input signal over a moving time window.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1Waveform.html)|

## DataSources

| Component | Documentation |
| --------- | ------------- |
| [DAN](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/DAN) | [Allows to store signals in an ITER DAN database.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1DANSource.html)|
| [EPICSCAInput](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/EPICSCA) | [Retrieve data from any number of PVs using the EPICS channel access client protocol.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1EPICSCAInput.html)|
| [EPICSCAOutput](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/EPICSCA) | [Output data into any number of PVs using the EPICS channel access client protocol.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1EPICSCAOutput.html)|
| [FileReader](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/FileDataSource) | [Read signals from a file using different formats.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1FileReader.html)|
| [FileWriter](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/FileDataSource) | [Write signals to a file using different formats.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1FileWriter.html)|
| [LinuxTimer](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/LinuxTimer) | [Generic timing data source.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1LinuxTimer.html)|
| [LoggerDataSource](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/LoggerDataSource) | [Prints in the MARTe logger the current value of any signal.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1LoggerDataSource.html)|
| [MDSReader](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/MDSReader) | [Allows to read data from an MDSplus tree.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1MDSReader.html)|
| [MDSWriter](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/MDSWriter) | [Allows to write data into an MDSplus tree.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1MDSWriter.html)|
| [NI1588TimeStamp](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI1588) | [Circular buffer time stamp acquisition using the NI-1588 PCI-Express board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI1588TimeStamp.html)|
| [NI6259ADC](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI6259) | [Provides an input interface to the NI6259 board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI6259ADC.html)|
| [NI6259DAC](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI6259) | [Provides an analogue output interface to the NI6259 board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI6259DAC.html)|
| [NI6259DIO](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI6259) | [Provides a digital input/output interface to the NI6259 board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI6259DIO.html)|
| [NI6368ADC](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI6368) | [Provides an input interface to the NI6368 board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI6368ADC.html)|
| [NI6368DAC](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI6368) | [Provides an analogue output interface to the NI6368 board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI6368DAC.html)|
| [NI6368DIO](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/NI6368) | [Provides a digital input/output interface to the NI6368 board.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1NI6368DIO.html)|
| [RealTimeThreadAsyncBridge](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/RealTimeThreadAsyncBridge) | [Enables the asynchronous sharing of signals between multiple real-time threads.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1RealTimeThreadAsyncBridge.html)|
| [RealTimeThreadSynchronisation](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/RealTimeThreadSynchronisation) | [Enables the synchronisation of multiple real-time threads.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1RealTimeThreadSynchronisation.html)|
| [SDNSubscriber](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/SDN) | [Receive signals transported over the ITER SDN.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1SDNSubscriber.html)|
| [SDNPublisher](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/DataSources/SDN) | [Publish signals transported over the ITER SDN.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1SDNPublisher.html)|

## Interfaces

| Component | Documentation |
| --------- | ------------- |
| [BaseLib2Wrapper](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/Interfaces/BaseLib2Wrapper) | [Load BaseLib2 objects into a BaseLib2 GlobalObjectDatabase.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1BaseLib2Wrapper.html)|
| [EPICSCAClient](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/Interfaces/EPICS) | [Trigger Messages as a response to an EPICS PV value change.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1EPICSCAClient.html)|
| [SysLogger](https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/tree/master/Source/Components/Interfaces/SysLogger) | [LoggerConsumerI which outputs the log messages to a syslog.](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-Components-docs-master/doxygen/classMARTe_1_1SysLogger.html)|

# Other information

More information about the MARTe software framework can be found in: 

- [User documentation](https://vcis.f4e.europa.eu/marte2-docs/master/html/)
- [API documentation](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-docs-master/doxygen/annotated.html)
- [API documentation (components)](https://vcis-jenkins.f4e.europa.eu/job/MARTe2-components-docs-master/doxygen/annotated.html)

## How to build components and run tests

**Commands:**
```
$ export MARTe2_DIR=<marte2_dir>
$ export LD_LIBRARY_PATH=$MARTe2_DIR/Build/linux/Core/
$ make -f Makefile.linux
$ Build/linux/GTest/MainGTest.ex
```	
**Paths:**
+ marte2_dir is the root directory of MARTe2 project's source code.

**Note:** The directory marte2_dir can be a snapshot of the project or a clone of the repository itself, so it can be switched to any branch or commit.

# License

Copyright 2015 F4E | European Joint Undertaking for ITER and the Development of Fusion Energy ('Fusion for Energy').
Licensed under the EUPL, Version 1.1 or - as soon they will be approved by the European Commission - subsequent versions of the EUPL (the "Licence"). You may not use this work except in compliance with the Licence. You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl

Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence permissions and limitations under the Licence.
