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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Resource/URLArchive.h"
#include "Util/XML/rapidxml.hpp"
#include "Util/XML/rapidxml_print.hpp"

#include <camp-xml/rapidxml.hpp>

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

using namespace boost::filesystem;

path URLArchive::mCachePath = "";
bool URLArchive::mCache = false;
std::list<URLArchive*> URLArchive::mURLArchives = std::list<URLArchive*>();

static size_t urlArchiveMemory( void *ptr, size_t size, size_t nmemb, void *data )
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = ( struct MemoryStruct * )data;

    assert( mem->memory != NULL || mem->size == 0 );
    mem->memory = ( unsigned char * )realloc( mem->memory, mem->size + realsize + 1 );
    if ( mem->memory )
    {
        memcpy( &( mem->memory[mem->size] ), ptr, realsize );
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

static size_t urlArchiveString( char* data, size_t size, size_t nmemb, String* buffer )
{
    // What we will return
    int result = 0;

    // Is there anything in the buffer?
    if ( buffer != NULL )
    {
        // Append the data to the buffer
        buffer->append( data, size * nmemb );

        // How much did we write?
        result = size * nmemb;
    }

    return result;
}

static size_t urlArchiveCharVector( char* data, size_t size, size_t nmemb, std::vector<char>* buffer )
{
    // What we will return
    int result = 0;

    // Is there anything in the buffer?
    if ( buffer != NULL )
    {
        // Append the data to the buffer
        buffer->assign( size * nmemb, *data );

        // How much did we write?
        result = size * nmemb;
    }

    return result;
}

int URLArchive::wildcmp( const char *wild, const char *string )
{
    // Written by Jack Handy - jakkhandy@hotmail.com

    const char *cp = NULL, *mp = NULL;

    while ( ( *string ) && ( *wild != '*' ) )
    {
        if ( ( *wild != *string ) && ( *wild != '?' ) )
        {
            return 0;
        }
        wild++;
        string++;
    }

    while ( *string )
    {
        if ( *wild == '*' )
        {
            if ( !*++wild )
            {
                return 1;
            }
            mp = wild;
            cp = string + 1;
        }
        else if ( ( *wild == *string ) || ( *wild == '?' ) )
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }

    while ( *wild == '*' )
    {
        wild++;
    }
    return !*wild;
}

CURLcode URLArchive::fetchUrl( void* curl, void *& buffer, size_t& size ) const
{
    MemoryStruct data = { 0 };
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, urlArchiveMemory );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &data );

    CURLcode res = curl_easy_perform( curl );
    buffer = data.memory;
    size = data.size;

    return res;
}

URLArchive::URLArchive( const String& rName, const String& rArchiveType ):
    Archive( rName, rArchiveType )
{
    mURLArchives.push_back( this );
    mURL = mName + String( "/" );
}

URLArchive::~URLArchive()
{
    mURLArchives.remove( this );
    unload();
}

void URLArchive::setCaching( const String& rPath, bool enabled )
{
    mCachePath = rPath;
    mCache = enabled;
}

void URLArchive::parseServerParams( const rapidxml::xml_document<>& rXMLDocument )
{
    // Parse XML into archive parameters.
    mServerArchiveParams.clear();
    URLArchiveParamsHolder holder;
    rapidxml::xml_node<>* node = rXMLDocument.first_node( "Root" );
    camp::xml::deserialize( holder, node );

    // Copy vector to map.
    for( URLArchiveParamsVector::const_iterator i = holder.mVector.begin(); 
        i != holder.mVector.end(); ++i )
    {
        mServerArchiveParams.insert( std::make_pair( i->mDirectory + i->mFilename, *i ) );
    }
}

void URLArchive::parseCacheParams( const Path& rFile )
{
    // Load XML file from cache.
    std::vector<char> buffer;
    rapidxml::xml_document<> xmlDocument;

    std::ifstream inputFile( rFile.file_string().c_str() );

    if( !inputFile )
        return;

    String line;
    String lines;
    while( std::getline( inputFile, line ) )
        lines += line;

    inputFile.close();

    // Copy data into an array and parse it.
    buffer = std::vector<char>( lines.begin(), lines.end() );
    buffer.push_back('\0');
    xmlDocument.parse<rapidxml::parse_no_data_nodes>( &buffer[0] );

    // Deserialize archive params.
    mCacheArchiveParams.clear();
    URLArchiveParamsHolder holder;
    rapidxml::xml_node<>* node = xmlDocument.first_node( "Root" );
    camp::xml::deserialize( holder, node );

    // Copy vector to map.
    for( URLArchiveParamsVector::const_iterator i = holder.mVector.begin(); 
        i != holder.mVector.end(); ++i )
    {
        mCacheArchiveParams.insert( std::make_pair( i->mDirectory + i->mFilename, *i ) );
    }
}

void URLArchive::saveServerParams()
{
    // Copy map to vector.
    URLArchiveParamsHolder holder;

    for( URLArchiveParamsMap::const_iterator i = mServerArchiveParams.begin(); 
        i != mServerArchiveParams.end(); ++i )
    {
        holder.mVector.push_back( i->second );
    }

    rapidxml::xml_document<> xmlDocument;
    char* node_name = xmlDocument.allocate_string( "Root" ); 
    rapidxml::xml_node<>* node = xmlDocument.allocate_node( rapidxml::node_element, node_name );
    xmlDocument.append_node( node );

    // Deserialize the server parameters.
    camp::xml::serialize( holder, node );

    // Save file to disk.
    Path params = mThisCachePath / "URLArchiveParams.xml";
    std::ofstream outputFile( params.file_string().c_str() );

    if( !outputFile )
        return;

    outputFile << xmlDocument;
    outputFile.close();
}

void URLArchive::load()
{
    // Create a new CURL handle.
    void* curl = curl_easy_init();

    // Get list from server.
    String buffer;
    std::stringstream ss;
    ss << mURL << "list.php";

    curl_easy_setopt( curl, CURLOPT_URL, ss.str().c_str() );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, urlArchiveString );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &buffer );

    CURLcode result = curl_easy_perform( curl );

    if ( result == CURLE_OK )
    {
        std::vector<char> data = std::vector<char>( buffer.begin(), buffer.end() );
        data.push_back('\0');

        // Parse XML into archive parameters.
        rapidxml::xml_document<> xmlDocument;
        xmlDocument.parse<rapidxml::parse_no_data_nodes>( &data[0] );
        URLArchive::parseServerParams( xmlDocument );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Resource list was not found on the server.", 
            "URLArchive::fetchUrl" );
    }

    // Clean up CURL handle.
    curl_easy_cleanup( curl );
}

void URLArchive::unload()
{
    // Save the archive parameters from server so the modification dates can be checked next time.
    saveServerParams();
}

Ogre::DataStreamPtr URLArchive::open( const String& rFilename, bool readOnly /*= true*/ ) const
{
    // Create a new CURL handle
    void* curl = curl_easy_init();

    void * buffer;
    size_t size;

    // File URL.
    std::string url = mURL + rFilename;
    // Cache path and file
    path cachePath = mThisCachePath / rFilename;
    std::string cacheFile = cachePath.file_string();
    cachePath.remove_filename();

    // Download the file.
    curl_easy_setopt( curl, CURLOPT_URL, url.c_str() );
    CURLcode res = fetchUrl( curl, buffer, size );
    long statLong = 0;

    // Get the status code.
    if ( CURLE_OK == res )
    {
        curl_easy_getinfo( curl, CURLINFO_HTTP_CODE, &statLong );
    }

    // Was anything useful downloaded?
    if ( statLong != 200 || buffer == NULL )
    {
        //OGRE_EXCEPT( Exception::ERR_FILE_NOT_FOUND,
        //            "Could not open resource: " + rFilename, "URLArchive::open" );
    }
    else
    {
        // Write file to disk if caching is enabled.
        // HACK: Don't cache particle files, or else they get loaded twice and ogre errors out...
        if ( mCache && Path( rFilename ).extension() != ".particle" )
        {
            // Create the directory if it does not exist yet
            create_directories( cachePath );
            FILE* fp = fopen( cacheFile.c_str(), "wb" );
            if ( fp != NULL )
            {
                // Write to file.
                fwrite( buffer, 1, size, fp );
                fclose( fp );
            }
        }
    }

    // Clean up CURL handle.
    curl_easy_cleanup( curl );

    // Create DataStream and free the old buffer.
    Ogre::MemoryDataStream stream( rFilename, buffer, size, false );
    Ogre::DataStreamPtr ptr( new Ogre::MemoryDataStream( stream, true ) );
    free( buffer );
    return ptr;
}

Ogre::StringVectorPtr URLArchive::list( bool recursive, bool dirs )
{
    Ogre::StringVector* data = new Ogre::StringVector();

    URLArchiveParamsMap::const_iterator i;
    for ( i = mServerArchiveParams.begin(); i != mServerArchiveParams.end(); i++ )
    {
        data->push_back( i->second.mDirectory + i->second.mFilename );
    }
    return Ogre::StringVectorPtr( data );
}

Ogre::FileInfoListPtr URLArchive::listFileInfo( bool recursive, bool dirs )
{
    Ogre::FileInfoList* data = new Ogre::FileInfoList();

    URLArchiveParamsMap::const_iterator i;
    for ( i = mServerArchiveParams.begin(); i != mServerArchiveParams.end(); i++ )
    {
        Ogre::FileInfo fi;
        fi.archive = this;
        fi.filename = i->second.mDirectory  + i->second.mFilename;
        fi.basename = i->second.mFilename;
        fi.path = i->second.mDirectory ;
        fi.compressedSize = i->second.mSize;
        fi.uncompressedSize = i->second.mSize;
        data->push_back( fi );
    }
    return Ogre::FileInfoListPtr( data );
}

Ogre::StringVectorPtr URLArchive::find( const String& rPattern, bool recursive, bool dirs )
{
    if ( rPattern == "*" )
    {
        return list( recursive, dirs );
    }

    Ogre::StringVector* data = new Ogre::StringVector();

    URLArchiveParamsMap::const_iterator i;
    for ( i = mServerArchiveParams.begin(); i != mServerArchiveParams.end(); i++ )
    {
        String file = i->second.mDirectory + i->second.mFilename;

        if ( wildcmp( rPattern.c_str(), file.c_str() ) )
            data->push_back( file );
    }
    return Ogre::StringVectorPtr( data );
}

Ogre::FileInfoListPtr URLArchive::findFileInfo( const String& rPattern, bool recursive, bool dirs ) const
{
    if ( rPattern == "*" )
    {
        // Remove const because ogre devs fail..
        return const_cast<URLArchive*>( this )->listFileInfo( recursive, dirs );
    }

    Ogre::FileInfoList* data = new Ogre::FileInfoList();

    URLArchiveParamsMap::const_iterator i;
    for ( i = mServerArchiveParams.begin(); i != mServerArchiveParams.end(); i++ )
    {
        String file = i->second.mDirectory + i->second.mFilename;

        if ( wildcmp( rPattern.c_str(), file.c_str() ) )
        {
            Ogre::FileInfo fi;
            fi.archive = this;
            fi.filename = i->second.mDirectory  + i->second.mFilename;
            fi.basename = i->second.mFilename;
            fi.path = i->second.mDirectory ;
            fi.compressedSize = i->second.mSize;
            fi.uncompressedSize = i->second.mSize;
            data->push_back( fi );
        }
    }
    return Ogre::FileInfoListPtr( data );
}

bool URLArchive::exists( const String& rFilename )
{
    URLArchiveParamsMap::const_iterator i;
    for ( i = mServerArchiveParams.begin(); i != mServerArchiveParams.end(); i++ )
    {
        String file = i->second.mDirectory + i->second.mFilename;
        if ( file.find( rFilename ) != String::npos )
            return true;
    }

    return false;
}

time_t URLArchive::getModifiedTime( const String& rFilename )
{
    return 0;
}

String URLArchive::setCacheDirectory( const String& rName, const String& rCacheDirectory )
{
    std::list<URLArchive*>::const_iterator i;
    for ( i = mURLArchives.begin(); i != mURLArchives.end(); i++ )
    {
        if ( ( *i )->getName() == rName )
        {
            return ( *i )->setCacheDirectory( rCacheDirectory );
        }
    }

    return String();
}

const String& URLArchive::setCacheDirectory( const String& rCacheDirectory )
{
    mThisCachePath = mCachePath / Path( rCacheDirectory );

    // Load last params list if it exists.
    Path params = mThisCachePath / "URLArchiveParams.xml";
    if( boost::filesystem::exists( params ) )
    {
        // Parse cache xml into cache parameters.
        URLArchive::parseCacheParams( params );

        // Compare lists and unload resources that have new files on the resource server.
        URLArchiveParamsMap::iterator i;
        for( i = mCacheArchiveParams.begin(); i != mCacheArchiveParams.end(); ++i )
        {
            // Look for the resource in the current params map.
            URLArchiveParamsMap::iterator j = mServerArchiveParams.find( i->first );

            if( j != mServerArchiveParams.end() )
            {
                // If new modified date is newer than last modified date, unload the resource.
                if( j->second.mDate > i->second.mDate )
                {
                    Path deletePath = mThisCachePath / j->second.mDirectory / 
                        j->second.mFilename;

                    if( boost::filesystem::exists( deletePath ) && 
                        !boost::filesystem::is_directory( deletePath ) )
                    {
                        boost::filesystem::remove( deletePath );
                    }
                }
            }
        }
    }

    return mThisCachePath.string();
}

const path&  URLArchive::getCachePath()
{
    return mCachePath;
}

const String& URLArchiveFactory::getType( void ) const
{
    static String name = "URL";
    return name;
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia