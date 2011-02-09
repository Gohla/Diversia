/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Diversia is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

Diversia is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Diversia. If not, see <http://www.gnu.org/licenses/>.

You may contact the author of Diversia by e-mail at: equabyte@sonologic.nl
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_SHARED_CRASHREPORTERABSTRACT_H
#define DIVERSIA_SHARED_CRASHREPORTERABSTRACT_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API CrashReporter
{
public:
    /**
    Default constructor. 
    **/
    CrashReporter() {}
    /**
    Destructor. 
    **/
    virtual ~CrashReporter() {}

    /**
    Creates a crash reporter implementation for the current OS. 
    
    @return Crash reporter implementation, or 0 if no implementation could be made.
    **/
    static CrashReporter* createCrashReporter();

    /**
    Sets if a large or small minidump should be created. Defaults to false.
    **/
    virtual void setCreateLargeMiniDump( bool large ) = 0;
    /**
    Queries if a large or small minidump should be created. Defaults to false.
    **/
    virtual bool isCreateLargeMiniDump() const = 0;
    /**
    Setter/getter functions for CAMP, static variables cannot be set/get directly.
    **/
    virtual void setEnabled( bool enabled ) = 0;
    virtual bool isEnabled() const = 0;
    virtual void setSilent( bool silent ) = 0;
    virtual bool isSilent() const = 0;
    virtual void setAppName( const String& rName ) = 0;
    virtual const String& getAppName() const = 0;
    virtual void setAppVersion( const String& rVersion ) = 0;
    virtual const String& getAppVersion() const = 0;
    
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::CrashReporter, 
    &Diversia::Shared::Bindings::CampBindings::bindCrashReporter );

#endif // DIVERSIA_SHARED_CRASHREPORTERABSTRACT_H