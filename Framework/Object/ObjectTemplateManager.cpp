/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

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

#include "Object/Platform/StableHeaders.h"

#include "Object/ObjectTemplateManager.h"
#include "Object/ObjectTemplate.h"

namespace Diversia
{
namespace ObjectSystem 
{
//------------------------------------------------------------------------------

ObjectTemplateManager::ObjectTemplateManager( Mode mode, RakNet::RakNetGUID ownGUID, 
    RakNet::RakNetGUID serverGUID, RakNet::ReplicaManager3& rReplicaManager, 
    RakNet::NetworkIDManager& rNetworkIDManager, bool offlineMode /*= false*/ ): 
    mMode( mode ),
    mOfflineMode( offlineMode ),
    mOwnGUID( ownGUID ),
    mServerGUID( serverGUID ),
    mReplicaManager( rReplicaManager ),
    mNetworkIDManager( rNetworkIDManager )
{

}

ObjectTemplateManager::~ObjectTemplateManager()
{
    for( ObjectTemplates::iterator i = mObjectTemplates.begin(); i != mObjectTemplates.end(); ++i )
    {
        mObjectTemplateSignal( *i->second, false );
        delete i->second;
    }
}

ObjectTemplate& ObjectTemplateManager::createObjectTemplate( const String& rName, 
    NetworkingType type /*= LOCAL*/, const String& rDisplayName /*= ""*/, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 )*/ )
{
    if( !ObjectTemplateManager::hasObjectTemplate( rName ) )
    {
        // Default to own GUID as source.
        if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

        // Set to local if in offline mode.
        if( mOfflineMode ) type = LOCAL;

        ObjectTemplate& objectTemplate = createObjectTemplateImpl( rName, type, 
            rDisplayName.empty()? rName : rDisplayName, source );
        mObjectTemplates.insert( std::make_pair( rName, &objectTemplate ) );
        mObjectTemplateSignal( objectTemplate, true );
        return objectTemplate;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Object template already exists.", 
            "ObjectTemplateManager::createObjectTemplate" );
    }
}

ObjectTemplate& ObjectTemplateManager::createObjectTemplate( const Object& rObject, 
    const String& rName, NetworkingType type /*= LOCAL */ )
{
    ObjectTemplate& objectTemplate = ObjectTemplateManager::createObjectTemplate( rName, 
        type, rObject.getDisplayName().empty()? rObject.getName() : rObject.getDisplayName(), 
        mOwnGUID );
    objectTemplate.createComponentTemplates( rObject );
    return objectTemplate;
}

ObjectTemplate& ObjectTemplateManager::getObjectTemplate( const String& rName ) const
{
    ObjectTemplates::const_iterator i = mObjectTemplates.find( rName );
    if( i != mObjectTemplates.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Object template does not exist.", 
            "ObjectTemplateManager::getObjectTemplate" );
    }
}

bool ObjectTemplateManager::hasObjectTemplate( const String& rName ) const
{
    return mObjectTemplates.find( rName ) != mObjectTemplates.end();
}

void ObjectTemplateManager::destroyObjectTemplate( ObjectTemplate& rObjectTemplate, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */ )
{
    // Default to own GUID as source.
    if( source == RakNet::RakNetGUID( 0 ) ) source = mOwnGUID;

    // Get permission to destroy the object.
    queryDestroyObjectTemplate( rObjectTemplate, source );

    mObjectTemplateSignal( rObjectTemplate, false );
    mObjectTemplates.erase( rObjectTemplate.getName() );
    if( rObjectTemplate.queryBroadcastDestruction() ) rObjectTemplate.broadcastDestruction();
    delete &rObjectTemplate;
}

void ObjectTemplateManager::destroyObjectTemplateTree( ObjectTemplate& rObjectTemplate, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */, bool redirectToChilds /*= false*/ )
{
    // Destroy given object template
    ObjectTemplateManager::destroyObjectTemplate( rObjectTemplate, source );

    // Destroy all childs
    if( !rObjectTemplate.hasParent() || redirectToChilds )
    {
        ObjectTemplateChilds childs = rObjectTemplate.getChildObjectTemplates();
        for( ObjectTemplateChilds::iterator i = childs.begin(); i != childs.end(); ++i )
        {
            // Set redirectToChilds to true so that the call is forwarded to all childs.
            ObjectTemplateManager::destroyWholeObjectTemplateTree( *i->second, source, true );
        }
    }
}

void ObjectTemplateManager::destroyWholeObjectTemplateTree( ObjectTemplate& rObjectTemplate, 
    RakNet::RakNetGUID source /*= RakNet::RakNetGUID( 0 ) */, bool redirectToChilds /*= false*/ )
{
    // Not the root parent and not redirecting to childs, let root handle it.
    if( !redirectToChilds && rObjectTemplate.hasParent() )
    {
        ObjectTemplateManager::destroyWholeObjectTemplateTree( 
            *rObjectTemplate.getRootParentObjectTemplate(), source );
        return;
    }

    // Destroy given object template
    ObjectTemplateManager::destroyObjectTemplate( rObjectTemplate, source );

    // Destroy all childs
    if( !rObjectTemplate.hasParent() || redirectToChilds )
    {
        ObjectTemplateChilds childs = rObjectTemplate.getChildObjectTemplates();
        for( ObjectTemplateChilds::iterator i = childs.begin(); i != childs.end(); ++i )
        {
            // Set redirectToChilds to true so that the call is forwarded to all childs.
            ObjectTemplateManager::destroyWholeObjectTemplateTree( *i->second, source, true );
        }
    }
}

String ObjectTemplateManager::generateName()
{
    static unsigned int counter = 0;
    return String( "ObjectTemplate" ) + boost::lexical_cast<String>( counter++ );
}

void ObjectTemplateManager::offlineModeChanged( bool offlineMode )
{
    if( mOfflineMode != offlineMode )
    {
        mOfflineMode = offlineMode;

        if( mOfflineMode )
        {
            for( ObjectTemplates::iterator i = mObjectTemplates.begin(); 
                i != mObjectTemplates.end(); ++i )
            {
                i->second->setNetworkingType( LOCAL );
            }
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia
