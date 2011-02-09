/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_SHARED_USERINFO_H
#define DIVERSIA_SHARED_USERINFO_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

class DIVERSIA_SHARED_API UserInfo
{
public:
    UserInfo( const String& rNickname = "", const String& rUsername = "", 
        const String& rPassword = "", const ServerInfo& rHomeServer = ServerInfo() ):
        mNickname( rNickname ), mUsername( rUsername ), mPassword( rPassword ), 
        mHomeServer( rHomeServer ) {}

    String      mNickname;
    String      mUsername;
    String      mPassword;
    ServerInfo  mHomeServer;
};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::UserInfo, &Diversia::Shared::Bindings::CampBindings::bindUserInfo );

#endif // DIVERSIA_SHARED_USERINFO_H