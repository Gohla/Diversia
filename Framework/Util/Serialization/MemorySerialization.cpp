/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#include "Util/Platform/StableHeaders.h"

#include "Util/Serialization/MemorySerialization.h"
#include "Util/Serialization/MemorySerializationImpl.h"

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

MemorySerialization::MemorySerialization( const camp::Value& rTag, bool include ):
    SerializationStream( rTag, include )
{

}

MemorySerialization::MemorySerialization( const camp::Args& rTags, bool include ):
    SerializationStream( rTags, include )
{

}

MemorySerialization::~MemorySerialization()
{

}

void MemorySerialization::deserialize( const camp::UserObject& rObject, 
    bool throwExceptions /*= true */ )
{

}

void MemorySerialization::serialize( const camp::UserObject& rObject, 
    bool throwExceptions /*= true */ )
{
    Detail::MemorySerializer serializer( rObject, mTags, mInclude, throwExceptions );
    rObject.getClass().visit( serializer );
    mMemory = serializer.mMemoryObject;
}

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia
