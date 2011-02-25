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

#ifndef DIVERSIA_UTIL_XMLSERIALIZATIONFILE_H
#define DIVERSIA_UTIL_XMLSERIALIZATIONFILE_H

#include "Util/Platform/Prerequisites.h"
#include "Util/Serialization/SerializationFile.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

class DIVERSIA_UTIL_API XMLSerializationFile : public SerializationFile
{
public:
    /**
    Constructor. 
    
    @param  rFile                   The file to load from and save to. 
    @param  rTag                    The tag to include or exclude properties with. Defaults to
                                    nothing. 
    @param  include                 True to include properties, false to exclude properties.
                                    Defaults to false. 
    @param  serializeOneDocument    True to serialize into one xml document instead of a separate
                                    xml document for saving.
    **/
    XMLSerializationFile( const Path& rFile, const camp::Value& rTag = camp::Value::nothing,
        bool include = false, bool serializeOneDocument = false );
    /**
    Destructor.
    **/
    virtual ~XMLSerializationFile();

	/**
    Deserializes an object from the file.

    @param  rObject         The object to deserialize.
    @param  throwExceptions True to throw exceptions, false to ignore exceptions. Defaults to true.
    **/
    void deserialize( const camp::UserObject& rObject, bool throwExceptions = true );
    /**
    Serializes an object to the file.

    @param  rObject         The object to deserialize.
    @param  throwExceptions True to throw exceptions, false to ignore exceptions. Defaults to true.
    **/
    void serialize( const camp::UserObject& rObject, bool throwExceptions = true );
    /**
    Loads from file.

    @return True if it succeeds, false if it fails.
    **/
    bool load();
    /**
    Saves to file.

    @param  backup  True to backup the existing file before saving. Defaults to false.

    @return True if it succeeds, false if it fails.
    **/
    bool save( bool backup = false );

private:
    Path                        mFile;
    bool                        mSerializeOneDocument;
    std::vector<char>           mXMLData;
    rapidxml::xml_document<>    mXMLDocument;
    rapidxml::xml_document<>    mXMLSaveDocument;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_XMLSERIALIZATIONFILE_H