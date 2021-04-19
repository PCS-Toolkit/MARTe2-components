/**
 * @file MDSClients.h
 * @brief Header file for enum MDSClients
 * @date 19/04/2021
 * @author Hector Fernandez
 *
 * @copyright Copyright 2019 Swiss Plasma Center, EPFL Lausanne Switzerland
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing,
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This header file contains the declaration of the enum MDSClients used  
 * in the MDSObjLoader interface.
 */

#ifndef MDSCLIENTSTYPES_H_
#define MDSCLIENTSTYPES_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                            C-API interface                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

namespace MARTe
{

    /**
 * @brief Enum that specifies the type of configuration for the connection of 
 * the data client with the server for MDS+.
 */
    enum MDSClientTypes
    {
        MDSClientType_Thin,
        MDSClientType_Distributed
    };

} /* namespace MARTe */

#endif /* MDSCLIENTSTYPES_H_ */