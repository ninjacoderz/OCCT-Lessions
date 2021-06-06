//-----------------------------------------------------------------------------
// Creation date: 04 March 2019
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

#ifndef OcafEx_Application_HeaderFile
#define OcafEx_Application_HeaderFile

// OcafExLib includes
#include <OcafEx.h>

// OCCT includes
#include <TDocStd_Application.hxx>

//-----------------------------------------------------------------------------

//#define OcafExBinFormat "BinOcaf"
#define OcafExBinFormat "OcafExBin"

// Convenience macro to gracefully suppress C4100 "unreferenced formal parameter"
// compiler warning.
#define OcafEx_NotUsed(x) x

//-----------------------------------------------------------------------------

//! The single instance of Application class is used to manipulate
//! OCAF Documents which in turn contain the actual application data.
//! OCAF Application is responsible for managing CAF documents, i.e. saving,
//! opening, etc.
class OcafEx_Application : public TDocStd_Application
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(OcafEx_Application, TDocStd_Application)

public:

  //! Accessor for the static instance of OCAF Application. If does not
  //! exist, the instance will be created.
  //! \return instance of OCAF Application.
  OcafExLib_EXPORT static Handle(OcafEx_Application)
    Instance();

public:

  //! Enumerates the formats which to be used for persistence.
  //! \param[out] formats collection of accepted formats to be populated
  //!                     by this method.
  OcafExLib_EXPORT virtual void
    Formats(TColStd_SequenceOfExtendedString& formats);

  //! Name of the resources file containing descriptions of the accepted
  //! formats. The filename is set relatively to `CSF_ResourcesDefaults`
  //! environment variable.
  //! \return filename.
  OcafExLib_EXPORT virtual const char*
    ResourcesName();

private:

  //! Default ctor which is publicly inaccessible as the Application should
  //! be created and accessed via `Instance()` method.
  //! \sa Instance() method to construct and access the Application.
  OcafEx_Application();

};

#endif
