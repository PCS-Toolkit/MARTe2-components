/**
 * @file MDSObjLoader.cpp
 * @brief Source file for class MDSObjLoader
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
 * the class MDSObjLoader (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "MDSObjLoader.h"
#include <mdsobjects.h>

#include <StreamString.h>
#include "MDSClientsTypes.h"
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe
{

    MDSObjLoader::MDSObjLoader() : ParObjLoader()
    {
        hasglobalshot = false;
    }

    MDSObjLoader::~MDSObjLoader()
    {
    }

    bool MDSObjLoader::Initialise(StructuredDataI &data)
    {
        MDSplus::Tree *currenttree;
        MDSplus::Connection *currentconnection;

        StreamString basename = this->GetName();
        bool status = ParObjLoader::Initialise(data);

        if (status)
        {
            // Read "Shot" parameter, which will set up the shot for all the MDSObjConnection objects if they don't define it
            if (data.Read("Shot", shot))
            {
                hasglobalshot = true;
            }

            // Loop over all the children of this node (which are supposed to be MDSObjConnection objects)
            for (uint32 i = 0; i < Size(); i++)
            {
                // Get a reference to the i-th child
                ReferenceT<MDSObjConnection> ref = Get(i);

                if (!ref.IsValid())
                {
                    // MDSObjLoader populates itself with AnyObjects, so references to AnyObject are ignored (no warning issued).
                    ReferenceT<AnyObject> objRef = Get(i);
                    if (!objRef.IsValid())
                    {
                        REPORT_ERROR(ErrorManagement::InitialisationError, "Node %u is of incompatible class. Invalid reference.", i);
                    }
                }
                else
                {
                    // If the connection has a local shot defined, override the one defined in the MDSObjLoader main object
                    int32 shottoopen;
                    if (ref->hasLocalShot())
                    {
                        shottoopen = ref->getLocalShot();
                    }
                    else
                    {
                        // If not defined, use the global port
                        if (hasglobalshot)
                        {
                            shottoopen = shot;
                        }
                        else
                        {
                            // Error. No shots defined anywhere.
                            status = false;
                            REPORT_ERROR(ErrorManagement::ParametersError, "Neither %s nor %s have defined any Shot.", this->GetName(), ref->GetName());
                        }
                    }

                    REPORT_ERROR(ErrorManagement::Information, "%s connecting to server: %s, tree: %s, shot: %d", this->GetName(), ref->getServer().Buffer(), ref->getTree().Buffer(), shottoopen);

                    // Start the connection with the server
                    MDSClientTypes clienttype = ref->getClientType();
                    try
                    {
                        switch (clienttype)
                        {
                            case MDSClientType_Distributed:
                            {
                                currentconnection = NULL;
                                currenttree = new MDSplus::Tree(ref->getTree().Buffer(), shottoopen, "NORMAL");
                                break;
                            }
                            case MDSClientType_Thin:
                            default:
                            {
                                currenttree = NULL;
                                currentconnection = new MDSplus::Connection((char *)(ref->getServer().Buffer()));
                                currentconnection->openTree((char *)(ref->getTree().Buffer()), (int)shottoopen);
                                break;
                            }
                        }
                    }
                    catch (MDSplus::MdsException ex)
                    {
                        REPORT_ERROR(ErrorManagement::InitialisationError, "Error opening the tree: %s\n", ex.what());
                        return false;
                    }

                    // ... loop over all children of the connection, that are supposed to be MDSParameters
                    for (uint32 j = 0; j < ref->Size(); j++)
                    {
                        //Get reference to the next MDSParameter
                        ReferenceT<MDSParameter> refPar = ref->Get(j);
                        if (refPar.IsValid())
                        {
                            // Actualisation (data from MDSplus are copied into this MDSParameter)
                            status = refPar->Actualize(privatecdb, clienttype, currentconnection, currenttree, basename);

                            // Also insert an AnyObject copy of this parameter at the root level of this ReferenceContainer
                            ReferenceT<AnyObject> paramObject("AnyObject", GlobalObjectsDatabase::Instance()->GetStandardHeap());

                            if (status && paramObject.IsValid())
                            {
                                if (refPar->IsStaticDeclared())
                                {
                                    status = paramObject->Serialise(*(refPar.operator->())); // required since we can't do Serialise(*refPar), * is not overloaded for ReferenceT
                                }
                                else
                                {
                                    // If the source parameter is unlinked a placeholder AnyType is Serialised instead
                                    AnyType unlinkedPar = AnyType(0u);
                                    unlinkedPar.SetStaticDeclared(false);

                                    status = paramObject->Serialise(unlinkedPar);
                                }

                                if (status)
                                {
                                    paramObject->SetName(refPar->GetName());
                                    status = Insert(paramObject);
                                }
                            }

                            if (!status)
                            {
                                goto actualizeerrorexit;
                            }
                        }
                    }

                    // Close the connection
                    switch (clienttype)
                    {
                        case MDSClientType_Distributed:
                        {
                            currenttree = NULL;
                            break;
                        }
                        case MDSClientType_Thin:
                        default:
                        {
                            currentconnection->closeTree((char *)(ref->getTree().Buffer()), (int)shottoopen);
                            currentconnection = NULL;
                            break;
                        }
                    }

                    REPORT_ERROR(ErrorManagement::Information, "%s disconnecting", this->GetName());
                }
            }
        }

    actualizeerrorexit:

        return status;
    }

    CLASS_REGISTER(MDSObjLoader, "1.0")
}
