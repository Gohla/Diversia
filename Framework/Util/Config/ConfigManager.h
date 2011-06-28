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

#ifndef DIVERSIA_UTIL_CONFIGMANAGER_H
#define DIVERSIA_UTIL_CONFIGMANAGER_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
Configuration manager.
**/
class DIVERSIA_UTIL_API ConfigManager
{
public:
    /**
    Constructor.

    @param pSerializationFile   The configuration file. This object will take ownership of this
                                object and destroy it when this object is destroyed.
    **/
    ConfigManager( SerializationFile* pFile );
    /**
    Destructor.
    **/
    ~ConfigManager();

    /**
    Register an object for serialization and deserialization of configuration. The loaded
    configuration will be loaded into the object immediately and whenever load is called. The
    object will be serialized when save is called.
    **/
    void registerObject( const camp::UserObject& rObject );
    /**
    Loads the configuration from given file.
    **/
    bool load();
    /**
    Saves the configuration to given file.
    **/
    bool save();

private:
    boost::scoped_ptr<SerializationFile>    mFile;
    std::set<camp::UserObject>              mObjects;
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_CONFIGMANAGER_H