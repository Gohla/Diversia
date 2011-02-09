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

#ifndef DIVERSIA_OGRECLIENT_URLARCHIVE_H
#define DIVERSIA_OGRECLIENT_URLARCHIVE_H

#include "OgreClient/Platform/Prerequisites.h"

#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreArchive.h>
#include <OGRE/OgreArchiveFactory.h>
#include <Camp/arraymapper.hpp>

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

struct URLArchiveParams
{
    URLArchiveParams( const String& rFilename = "", const String& rDirectory = "", 
        unsigned int size = 0, unsigned int date = 0 ): mFilename( rFilename ), 
        mDirectory( rDirectory ), mSize( size ), mDate( date ) {}

    std::string     mFilename;
    std::string     mDirectory;
    unsigned int    mSize;
    unsigned int    mDate;
};

typedef std::map<String, URLArchiveParams> URLArchiveParamsMap;
typedef std::vector<URLArchiveParams> URLArchiveParamsVector;

struct URLArchiveParamsHolder
{
    URLArchiveParamsVector mVector;
};

struct MemoryStruct
{
    unsigned char * memory;
    size_t size;
};

/** Specialization of the Archive class to allow reading of files from a URL.
*/
class DIVERSIA_OGRECLIENT_API URLArchive : public Ogre::Archive
{
public:
    /**
    Constructor. 
    
    @param  rName           The root URL of the archive.
    @param  rArchiveType    Type of the archive. 
    **/
    URLArchive( const String& rName, const String& rArchiveType );
    /**
    Destructor. 
    **/
    ~URLArchive();

    static int wildcmp( const char *wild, const char *string );

    CURLcode fetchUrl( void* curl, void *& buffer, size_t& size ) const;
    static void setCaching( const String& rPath, bool enabled );
    static const Path& getCachePath();

    void parseServerParams( const rapidxml::xml_document<>& rXMLDocument );
    void parseCacheParams( const Path& rFile );
    void saveServerParams();

    void load();
    void unload();

    Ogre::DataStreamPtr open( const String& rFilename, bool readOnly = true ) const;

    Ogre::StringVectorPtr list( bool recursive = true, bool dirs = false );
    Ogre::FileInfoListPtr listFileInfo( bool recursive = true, bool dirs = false );
    Ogre::StringVectorPtr find( const String& rPattern, bool recursive = true, bool dirs = false );
    Ogre::FileInfoListPtr findFileInfo( const String& rPattern, bool recursive = true, 
        bool dirs = false ) const;
    bool exists( const String& rFilename );
    time_t getModifiedTime( const String& rFilename );
    bool isCaseSensitive( void ) const { return true; }

    static String setCacheDirectory( const String& rName, const String& rCacheDirectory );
    const String& setCacheDirectory( const String& rCacheDirectory );

private:
    static Path  mCachePath;
    Path         mThisCachePath;
    static bool                     mCache;
    String                          mURL;

    static std::list<URLArchive*>   mURLArchives;
    URLArchiveParamsMap             mServerArchiveParams;
    URLArchiveParamsMap             mCacheArchiveParams;
};

/** Specialization of ArchiveFactory for URLArchive files. */
class _OgrePrivate URLArchiveFactory : public Ogre::ArchiveFactory
{
public:
    virtual ~URLArchiveFactory() {}

    const String& getType( void ) const;
    Ogre::Archive *createInstance( const String& name )
    {
        return new URLArchive( name, "URL" );
    }
    void destroyInstance( Ogre::Archive* arch ) { delete arch; }
};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE( Diversia::OgreClient::URLArchiveParams, 
    &Diversia::OgreClient::Bindings::CampBindings::bindURLArchiveParams );

CAMP_AUTO_TYPE( Diversia::OgreClient::URLArchiveParamsHolder, 
    &Diversia::OgreClient::Bindings::CampBindings::bindURLArchiveParamsHolder );

#endif // DIVERSIA_OGRECLIENT_URLARCHIVE_H