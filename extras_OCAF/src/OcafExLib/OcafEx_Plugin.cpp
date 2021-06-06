//-----------------------------------------------------------------------------
// Creation date: 14 March 2019
// Author:        Sergey Slyadnev
//-----------------------------------------------------------------------------
// Copyright (c) 2019, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// OcafEx include
#include <OcafEx_BinStorageDriver.h>
#include <OcafEx_BinRetrievalDriver.h>

// OCCT includes
#include <BinLDrivers.hxx>
#include <Plugin_Macro.hxx>
#include <Standard_GUID.hxx>

#undef COUT_DEBUG

#pragma warning(disable: 4190)

//! Class exporting the entry point into OcafEx for dynamic
//! loading of storage/retrieval Drivers.
class OcafExDrivers 
{
public:

  OcafExLib_EXPORT static Handle(Standard_Transient)
    Factory(const Standard_GUID& guid);

};

static Standard_GUID BinStorageDriverGUID   ("7397208C-5D82-48C3-9C35-53274CEA5D73");
static Standard_GUID BinRetrievalDriverGUID ("7397208C-5D82-48C3-9C35-53274CEA5D74");

//! Entry point for Plugin. Returns Storage/Retrieval Driver by the passed
//! GUID.
//! \param[in] guid Driver's GUID.
//! \return Driver instance.
Handle(Standard_Transient) OcafExDrivers::Factory(const Standard_GUID& guid)
{
  if ( guid == BinStorageDriverGUID )
  {
    static Handle(OcafEx_BinStorageDriver) DRV_Storage = new OcafEx_BinStorageDriver();
    return DRV_Storage;
  }

  if ( guid == BinRetrievalDriverGUID )
  {
    static Handle(OcafEx_BinRetrievalDriver) DRV_Retrieval = new OcafEx_BinRetrievalDriver();
    return DRV_Retrieval;
  }

  return BinLDrivers::Factory(guid);
}

// Declare entry point
PLUGIN(OcafExDrivers)

#pragma warning(default: 4190)
