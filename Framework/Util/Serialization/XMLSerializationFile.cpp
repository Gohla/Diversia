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

#include "Util/Serialization/XMLSerializationFile.h"
#include "Util/XML/rapidxml_print.hpp"

#include <camp-xml/common.hpp>
#include <camp-xml/rapidxml.hpp>

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

XMLSerializationFile::XMLSerializationFile( const Path& rFile ):
    SerializationFile( camp::Args::empty, false ),
    mFile( rFile ),
    mSerializeOneDocument( false )
{

}

XMLSerializationFile::XMLSerializationFile( const Path& rFile, 
    const camp::Value& rTag, bool include /*= false*/, bool serializeOneDocument /*= false*/ ):
    SerializationFile( rTag, include ),
    mFile( rFile ),
    mSerializeOneDocument( serializeOneDocument )
{

}

XMLSerializationFile::XMLSerializationFile( const Path& rFile, 
    const camp::Args& rTags, bool include /*= false*/, bool serializeOneDocument /*= false*/ ):
    SerializationFile( rTags, include ),
    mFile( rFile ),
    mSerializeOneDocument( serializeOneDocument )
{

}

XMLSerializationFile::~XMLSerializationFile()
{

}

void XMLSerializationFile::deserialize( const camp::UserObject& rObject, 
    bool throwExceptions /*= true*/ )
{
    // Get the node for this object by using its metaclass name.
    rapidxml::xml_node<>* node = mXMLDocument.first_node( rObject.getClass().name().c_str() );

    // Serialize the object using data from the XML node.
    const camp::Value& tag = mTags.count() ? mTags[0] : camp::Value::nothing;
    if( node )
        camp::xml::deserialize( rObject, node, tag, mInclude, throwExceptions );
    else
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
            "No configuration found for object " + rObject.getClass().name(), 
        	"XMLSerializationFile::deserialize" );
}

void XMLSerializationFile::serialize( const camp::UserObject& rObject,
    bool throwExceptions /*= true*/ )
{
    rapidxml::xml_document<>* doc;
    if( mSerializeOneDocument ) doc = &mXMLDocument;
    else doc = &mXMLSaveDocument;

    // Create a node for this object by using its metaclass name.
    char* node_name = doc->allocate_string( rObject.getClass().name().c_str() ); 
    rapidxml::xml_node<>* node = doc->allocate_node( rapidxml::node_element, 
        node_name );

    doc->append_node( node );

    // Deserialize the object to XML.
    camp::xml::serialize( rObject, node, mTags, mInclude, throwExceptions );
}

bool XMLSerializationFile::load()
{
    std::ifstream inputFile( mFile.file_string().c_str() );

    if( !inputFile )
        return false;

    String line;
    String lines;
    while( std::getline( inputFile, line ) )
        lines += line;

    inputFile.close();

    // Clear data.
    mXMLData.clear();
    mXMLDocument.clear();

    // Copy data into an array and parse it.
    mXMLData = std::vector<char>( lines.begin(), lines.end() );
    mXMLData.push_back('\0');
    mXMLDocument.parse<rapidxml::parse_no_data_nodes>( &mXMLData[0] );

    return true;
}

bool XMLSerializationFile::save( bool backup /*= false*/ )
{
    if( backup && boost::filesystem::exists( mFile ) )
    {
        Path backupPath = mFile; backupPath.replace_extension( ".xml.bak" );
        if( boost::filesystem::exists( backupPath ) ) boost::filesystem::remove( backupPath );
        boost::filesystem::copy_file( mFile, backupPath );
    }

    std::ofstream outputFile( mFile.file_string().c_str() );

    if( !outputFile )
        return false;

    outputFile << mXMLSaveDocument;
    outputFile.close();

    return true;
}

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia