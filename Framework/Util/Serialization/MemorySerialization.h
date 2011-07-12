/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_MEMORYSERIALIZATION_H
#define DIVERSIA_UTIL_MEMORYSERIALIZATION_H

#include "Util/Serialization/MemorySerializationImpl.h"
#include "Util/Serialization/SerializationStream.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

class DIVERSIA_UTIL_API MemorySerialization : public SerializationStream
{
public:
    MemorySerialization( const camp::Value& rTag, bool include );
    MemorySerialization( const camp::Args& rTags, bool include );
    ~MemorySerialization();

    void deserialize( const camp::UserObject& rObject, bool throwExceptions = true );
    void serialize( const camp::UserObject& rObject, bool throwExceptions = true );
    
private:
    Detail::MemoryUserObject mMemory;

};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_MEMORYSERIALIZATION_H
