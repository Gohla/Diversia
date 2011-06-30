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

#include "Util/Platform/StableHeaders.h"

#include "Util/Config/ConfigManager.h"
#include "Util/Serialization/SerializationFile.h"

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

ConfigManager::ConfigManager( SerializationFile* pFile ):
    mFile( pFile )
{
    mFile->setTag( "Configurable" );
    mFile->setInclude( true );
}

ConfigManager::~ConfigManager()
{

}

void ConfigManager::registerObject( const camp::UserObject& rObject )
{
    std::set<camp::UserObject>::iterator i = mObjects.find( rObject );
    if( i == mObjects.end() )
    {
        mObjects.insert( rObject );
    }
        
    // Deserialize the configuration into the object.
    try
    {
        mFile->deserialize( rObject );
    }
    catch( Exception e )
    {
        ULOGW << "No settings found for " + rObject.getClass().name() + ", using defaults.";
    }
    catch( camp::Error e )
    {
        ULOGW << "Could not deserialize configuration for " + rObject.getClass().name() + ": " <<
            e.what();
    }
}

bool ConfigManager::load()
{
    if( !mFile->load() )
        return false;

    // Deserialize all objects
    for( std::set<camp::UserObject>::iterator i = mObjects.begin(); i != mObjects.end(); ++i )
    {
        try
        {
            mFile->deserialize( *i );
        }
        catch( Exception e )
        {
            ULOGW << "No settings found for " + (*i).getClass().name() + ", using defaults.";
        }
        catch( camp::Error e )
        {
            ULOGW << "Could not deserialize configuration for " + (*i).getClass().name() + ": " <<
                e.what();
        }
    }

    return true;
}

bool ConfigManager::save()
{
    // Serialize all objects
    for( std::set<camp::UserObject>::iterator i = mObjects.begin(); i != mObjects.end(); ++i )
    {
        mFile->serialize( *i );
    }

    return mFile->save( true );
}

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia