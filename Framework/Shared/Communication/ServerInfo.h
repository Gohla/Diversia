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

#ifndef DIVERSIA_SHARED_SERVERINFO_H
#define DIVERSIA_SHARED_SERVERINFO_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

/**
Contains general information of a server.
**/
class DIVERSIA_SHARED_API ServerInfo
{
public:
    /**
    Default constructor. 
    
    @param  rAddress        The server (ip) address. 
    @param  rPort           The server port. 
    @param  rResourceServer The resource server. 
    @param  rName           The server name. 
    **/
    ServerInfo( const String& rAddress = "", short port = 0, const String& rName = "" ):
        mAddress( rAddress ), mPort( port ), mName( rName ) {}
    /**
    Construct from a RakNet bitstream.
    
    @param [in,out] rBitStream  The bit stream. 
    **/
    ServerInfo( RakNet::BitStream& rBitStream )
    {
        rBitStream >> *this;
    }
    /** 
    Get the address and port as one string.
    **/
    String getAddressMerged() const
    {
        if ( !mAddress.empty() )
        {
            std::stringstream ss;
            ss << mAddress << ":" << mPort;
            return ss.str();
        }
        else
            return "";
    }
    /** 
    Get the address and port as one string in a safe format to use in filesystems.
    **/
    String getAddressMergedSafe() const
    {
        if ( !mAddress.empty() )
        {
            std::stringstream ss;
            ss << mAddress << "_" << mPort;
            return ss.str();
        }
        else
            return "";
    }
    /**
    Sets the address and port with a string.
    
    @param  string  The string. 
    **/
    void setAddressMerged( const String& rString )
    {
        // Split port and ip address.
        String::size_type found = rString.find_first_of( ":" );
        if ( found != String::npos )
        {
            String port = rString.substr( found + 1 );
            mPort = boost::lexical_cast<unsigned short>( port );
            mAddress = rString.substr( 0, rString.size() - ( rString.size() - found ) );
        }
    }
    /**
    Read server info from a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator>>( RakNet::BitStream& in, ServerInfo& out )
    {
        in >> out.mAddress;
        in >> out.mPort;
        in >> out.mName;

        return in;
    }
    /**
    Write server info to a RakNet bitstream.
    **/
    friend RakNet::BitStream& operator<<( RakNet::BitStream& out, ServerInfo& in )
    {
        out << in.mName;
        out << in.mPort;
        out << in.mName;

        return out;
    }
    /**
    Equality operator. 
    
    @param  rhs The right hand side. 
    
    @return True if the parameters are considered equivalent.  
    **/
    inline bool operator==( const ServerInfo& rhs ) const
    {
        return mAddress == rhs.mAddress && mPort == rhs.mPort && mName == rhs.mName;
    }

    String          mAddress;
    unsigned short  mPort;
    String          mName;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::ServerInfo, &Diversia::Shared::Bindings::CampBindings::bindServerInfo );

#endif // DIVERSIA_SHARED_SERVERINFO_H