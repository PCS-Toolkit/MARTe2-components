/**
 * @file MDSObjConnection.cpp
 * @brief Source file for class MDSObjConnection
 * @date 29/05/2019
 * @author Galperti Cristian
 *
 * @copyright Copyright 2019 EPFL SPC Lausanne
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
 * the class MDSObjConnection (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "MDSObjConnection.h"
#include "MDSClientsTypes.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe
{

    MDSObjConnection::MDSObjConnection() : ReferenceContainer()
    {
        haslocalshot = false;
    }

    MDSObjConnection::~MDSObjConnection()
    {
    }

    bool MDSObjConnection::Initialise(StructuredDataI &data)
    {
        bool status = ReferenceContainer::Initialise(data);

        if (status)
        {
            // Read "ClientType" parameter from config file
            StreamString clienttypeString = "";
            if (data.Read("ClientType", clienttypeString))
            {
                if (clienttypeString == "Thin")
                {
                    clienttype = MDSClientType_Thin;
                }
                else if (clienttypeString == "Distributed")
                {
                    clienttype = MDSClientType_Distributed;
                }
                else
                {
                    // Wrong parameter value
                    status = false;
                    REPORT_ERROR(ErrorManagement::ParametersError, "MDSObjConnection %s, Invalid ClientType: %s (can be Thin or Distributed).", this->GetName(), clienttypeString.Buffer());
                }
            }
            else
            {
                // If not found, set its value to "Thin"
                clienttype = MDSClientType_Thin;
                REPORT_ERROR(ErrorManagement::Information, "MDSObjConnection %s, ClientType parameter not set, by default it is set to Thin", this->GetName());
            }
        }

        if (status)
        {
            // Only read Server parameter if Thin Client is selected
            switch (clienttype) {
                case MDSClientType_Thin:
                {
                    // Read "Server" parameter from config file
                    if (!data.Read("Server", server))
                    {
                        status = false;
                        REPORT_ERROR(ErrorManagement::ParametersError, "MDSObjConnection %s, Server not set", this->GetName());
                    }
                    break;
                }
                default:
                {
                    // Set server as an empty variable
                    server = "";
                    break;
                } 
            }
        }

        if (status)
        {
            // Read "Tree" parameter from config file
            if (!data.Read("Tree", tree))
            {
                status = false;
                REPORT_ERROR(ErrorManagement::ParametersError, "MDSObjConnection %s, Tree not set", this->GetName());
            }
        }

        if (status)
        {
            // Read "Shot" parameter from config file
            if (data.Read("Shot", shot))
            {
                // Indicate the parent container that this connection is going to overwrite the global shot number.
                haslocalshot = true;
            }
            else
            {
                REPORT_ERROR(ErrorManagement::Information, "MDSObjConnection %s, Shot not set, parent Shot is used", this->GetName());
            }
        }

        REPORT_ERROR(ErrorManagement::Information, "%s init, Server: %s, Tree: %s, number of subclasses: %d", this->GetName(), server.Buffer(), tree.Buffer(), Size());

        if (status)
        {
            StreamString name = this->GetName();

            // .. loop for all the children (they are supposed to be of class MDSParameter)
            for (uint32 i = 0; i < Size(); i++)
            {
                // Get the next MDSParameter reference
                ReferenceT<MDSParameter> ref = Get(i);
                if (ref.IsValid())
                {
                    // Pass the Connection information to the MDSParameters
                    ref->SetConnection(clienttype, server, tree, name);
                }
            }
        }

        return status;
    }

    CLASS_REGISTER(MDSObjConnection, "1.0")
}
