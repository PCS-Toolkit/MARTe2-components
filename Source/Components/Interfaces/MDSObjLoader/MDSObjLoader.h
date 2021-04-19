/**
 * @file MDSObjLoader.h
 * @brief Header file for class MDSObjLoader
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

 * @details This header file contains the declaration of the class MDSObjLoaderGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef MDSOBJLOADER_H_
#define MDSOBJLOADER_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "ConfigurationDatabase.h"
#include "ReferenceContainer.h"
#include "ReferenceT.h"

#include "MDSObjConnection.h"
#include "MDSParameters.h"
#include "ParObjLoader.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

namespace MARTe
{
    /**
     * @brief   Loader class that loads parameters from MDSplus and makes
     *          them available as AnyObject.
     * @details This class is a ReferenceContainer. For each parameter
     *          declared in its configuration, this class inserts in itself
     *          an AnyObject that holds the value of the parameter as
     *          retrieved from MDSplus.
     *          The parameter is saved as an AnyObject for compatibility with
     *          the rest of the framework: by knowing the name of the instance
     *          of a MDSObjLoader class, a Find("instanceName.paramName")
     *          will be enough to retrieve the parameter from any other
     *          object of the framework.
     * 
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * 
     * The configuration syntax is (names and signal quantity are only given
     * as an example):
     * 
     * +MDSParamLoader1 = {
     *     Class = MDSObjLoader                         // Compulsory
     *     Shot = "-1"                                  // Optional if defined in MDSObjConnection
     *     
     *     +MDSConnection1 = {
     *         Class = MDSObjConnection                 // Compulsory
     *         ClientType = ( "Thin" | "Distributed" )  // Optional. Default: "Thin"
     *         Server = "localhost:8000"                // Compulsory if ClientType = "Thin"
     *         Tree = "tree_name"                       // Compulsory
     *         Shot = "-1"                              // Optional if defined in MDSObjLoader
     * 
     *         +gain1 = {
     *             Class = MDSParameter                                 // Compulsory
     *             Path  = "\\TREE_NAME::TOP:NODE1"                     // Compulsory
     *             DataOrientation = ( "RowMajor" | "ColumnMajor" )     // Only for 2D matrices. Optional. Default: "ColumnMajor"
     *         }
     *         +gainStruct-gain_a = {
     *             Class = MDSParameter                                 // Compulsory
     *             Path  = "\\TREE_NAME::TOP:NODE2"                     // Compulsory
     *             DataOrientation = ( "RowMajor" | "ColumnMajor" )     // Only for 2D matrices. Optional. Default: "ColumnMajor"
     *         }
     *         +gainStruct-gain_b = {
     *             Class = MDSParameter                                 // Compulsory
     *             Path  = "\\TREE_NAME::TOP:NODE3"                     // Compulsory
     *             DataOrientation = ( "RowMajor" | "ColumnMajor" )     // Only for 2D matrices. Optional. Default: "ColumnMajor"
     *         }
     *     }
     * 
     *     +MDSConnection2 = {
     *         ...
     *     }
     * }
     * 
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * 
     * @note Structured nodes are denoted by hyphens `-` since a dot cannot be used
     *       in node names. However, every `-` in the parameter names is converted to
     *       `.` for compatibility with the rest of the framework.
     * 
     * @warning Do not use `-` character for node names except for denoting structures
     *          (see note above).
     * 
     * Details of the configuration syntax:
     * - **MDSObjLoader**
     *    - *Shot*: Number of the shot of the MDS trees to be used globally in all the trees. If
     *          this parameter is set, Shot parameter in MDSObjConnection overwrites the value for 
     *          that connection only. This parameter is optional if it is defined in every MDSObjConnection.
     * - **MDSObjConnection**
     *    - *ClientType*: Optional. Its valid values are "Thin" or "Distributed". If not set, "Thin" is 
     *          used by default. Read more about remote accessing to MDS+ for understanding this parameter.
     *    - *Server*: Only required if ClientType = "Thin". It is a string which defines the IP address 
     *          of the remote MDS+ server, and its access port. Instead of IP address, domains can be used, 
     *          as well as "localhost" if the server is in the same machine. If ClientType = "Distributed", 
     *          a environment variable must be set in your machine, with the form: "treename_path=<IP>:<PORT>::<PATH>".
     *    - *Tree*: name of the tree to connect to.
     *    - *Shot*: Number of the shot of the MDS tree. It is optional if it was defined in MDSObjLoader. 
     *          If set here, it overwrites the value in MDSObjLoader for this connection only.
     * - **MDSParameter**
     *    - *Path*: MDS+ path to the node. The format is "\\TREENAME::TOP:PATH_TO_NODE".
     *    - *DataOrientation*: Only for nodes which contain 2D matrices. It sets the way to linealize the 
     *          data in memory. Its valid values are "RowMajor" or "ColumnMajor". If not set, the default value 
     *          is "ColumnMajor".
     *    - *Dim*: ????
     *    - *StartIdx*: ????
     *    - *StopIdx*: ????
     * 
     */
    class MDSObjLoader : public ParObjLoader
    {
    public:
        CLASS_REGISTER_DECLARATION()

        /**
         * @brief Constructor
         */
        MDSObjLoader();

        /**
         * @brief Destructor
         */
        virtual ~MDSObjLoader();

        /**
         * @see ReferenceContainer::Initialise()
         */
        virtual bool Initialise(StructuredDataI &data);

        inline ConfigurationDatabase &getCdb() { return privatecdb; }

    protected:
    private:
        int32 shot;
        bool hasglobalshot;
    };

}

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* MDSOBJLOADER_H_ */
