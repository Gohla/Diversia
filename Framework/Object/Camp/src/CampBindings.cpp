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

#include "Object/Platform/StableHeaders.h"

#include "Object/Camp/CampBindings.h"
#include "Util/Camp/ValueMapper.h"

#include "Object/ObjectManager.h"
#include "Object/Object.h"
#include "Object/Component.h"
#include "Util/Math/Node.h"

namespace Diversia
{
namespace ObjectSystem
{
namespace Bindings
{
//------------------------------------------------------------------------------

void CampBindings::bindModeEnum()
{
    camp::Enum::declare<Mode>( "Mode" )
        .value( "Client", CLIENT )
        .value( "Server", SERVER );
}

void CampBindings::bindNetworkingTypeEnum()
{
    camp::Enum::declare<NetworkingType>( "NetworkingType" )
        .value( "Local", LOCAL )
        .value( "Remote", REMOTE );
}

void CampBindings::bindRakNetGUID()
{
    camp::Class::declare<RakNet::RakNetGUID>( "RakNetGUID" )
        .property( "ID", &RakNet::RakNetGUID::g );
}

void CampBindings::bindObjectManager()
{
    camp::Class::declare<ObjectManager>( "ObjectManager" )
        .tag( "QtIcon", ":/Icons/Icons/mimetypes/gnome-package.png" )
        // Constructors
        // Member variables
        // Static member variables
        // Properties (read-only)
        .property( "Mode", &ObjectManager::getMode )
            .tag( "NoPropertyBrowser" )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
        .property( "OwnGUID", &ObjectManager::getOwnGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ServerGUID", &ObjectManager::getServerGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read/write)
        .property( "Objects", &ObjectManager::mObjects )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
            .tag( "AddFunction", "CreateLocalObject" )
        // Functions
        .function( "CreateLocalObject", boost::function<Object&(ObjectManager&, const String&)>( boost::bind( &ObjectManager::createObject, _1, _2, LOCAL, "", RakNet::RakNetGUID( 0 ) ) ) )
        .function( "CreateRemoteObject", boost::function<Object&(ObjectManager&, const String&)>( boost::bind( &ObjectManager::createObject, _1, _2, REMOTE, "", RakNet::RakNetGUID( 0 ) ) ) )
        .function( "CreateObject", boost::function<Object&(ObjectManager&, const String&, NetworkingType)>( boost::bind( &ObjectManager::createObject, _1, _2, _3, "", RakNet::RakNetGUID( 0 ) ) ) )
        .function( "GetObject", &ObjectManager::getObject )
        .function( "HasObject", &ObjectManager::hasObject )
        .function( "DestroyObject", boost::function<void (ObjectManager&, const String&)>( boost::bind( (void(ObjectManager::*)(const String&, RakNet::RakNetGUID))&ObjectManager::destroyObject, _1, _2, RakNet::RakNetGUID( 0 ) ) ) )
        .function( "DestroyObjectTree", boost::function<void (ObjectManager&, const String&)>( boost::bind( (void(ObjectManager::*)(const String&, RakNet::RakNetGUID, bool))&ObjectManager::destroyObjectTree, _1, _2, RakNet::RakNetGUID( 0 ), false ) ) )
        .function( "DestroyWholeObjectTree", boost::function<void (ObjectManager&, const String&)>( boost::bind( (void(ObjectManager::*)(const String&, RakNet::RakNetGUID, bool))&ObjectManager::destroyWholeObjectTree, _1, _2, RakNet::RakNetGUID( 0 ), false ) ) );
        // Static functions
        // Operators
}

void CampBindings::bindObject()
{
    camp::Class::declare<Object>( "Object" )
        .tag( "QtIcon", ":/Icons/Icons/mimetypes/gnome-package.png" )
        .base<Node>()
        // Constructors
        // Member variables
        // Static member variables
        // Properties (read-only)
        .property( "Name", &Object::getName )
            .tag( "NoBitStream" )
            .tag( "NoSerialization" )
        .property( "ObjectManager", &Object::getObjectManager )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsDelayedDestroying", &Object::queryDelayedDestruction )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Mode", &Object::getMode )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Source", &Object::getSource )
            .tag( "NoBitStream" )
            .tag( "NoSerialization" )
        .property( "SourceGUID", &Object::getSourceGUID )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
            .tag( "NoSerialization" )
        .property( "OwnGUID", &Object::getOwnGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ServerGUID", &Object::getServerGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsCreatedByOwnGUID", &Object::isCreatedByOwnGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsCreatedByServer", &Object::isCreatedByServer )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read/write)
        .property( "DisplayName", &Object::getDisplayName, &Object::setDisplayName )
            .tag( "NoBitStream" )
        .property( "NetworkingType", &Object::getNetworkingType, boost::bind( &Object::setNetworkingType, _1, _2, false ) )
            .tag( "NoBitStream" )
        .property( "ParentName", &Object::getParentName, (void(Object::*)(const String&))&Object::parent )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Components", &Object::mComponentsByHandle )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
            .tag( "AddFunction", "CreateComponentByHandle" )
        // Functions
        .function( "IsCreatedBy", &Object::isCreatedBy )
        .function( "IsCreatedBySource", &Object::isCreatedBySource )
        .function( "CreateChildObject", &Object::createChildObject )
        .function( "CreateComponent", boost::function<Component&(Object&, ComponentType, const String&, bool)>( boost::bind( &Object::createComponent, _1, _2, _3, _4, RakNet::RakNetGUID( 0 ) ) ) )
        .function( "CreateComponentByHandle", (Component&(Object::*)(const ComponentHandle&))&Object::createComponent )
        .function( "GetComponent", (Component&(Object::*)(const String&) const)&Object::getComponent )
        .function( "HasComponent", (bool(Object::*)(const String&) const)&Object::hasComponent )
        .function( "DestroyComponent", boost::function<void(Object&, const String&)>( boost::bind( (void(Object::*)(const String&, RakNet::RakNetGUID))&Object::destroyComponent, _1, _2, RakNet::RakNetGUID( 0 ) ) ) )
        .function( "Destroy", &Object::destroyObject );
        // Static functions
        // Operators
}

void CampBindings::bindComponent()
{
    camp::Class::declare<Component>( "Component" )
        // Constructors
        // Member variables
        // Static member variables
        // Properties (read-only)
        .property( "Type", &Component::getType )
            .tag( "NoBitStream" )
            .tag( "NoSerialization" )
        .property( "TypeName", &Component::getTypeName )
            .tag( "NoBitStream" )
            .tag( "NoSerialization" )
        .property( "Object", &Component::getObject )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Mode", &Component::getMode )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "NetworkingType", &Component::getNetworkingType )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Source", &Component::getSource )
            .tag( "NoBitStream" )
            .tag( "NoSerialization" )
        .property( "SourceGUID", &Component::getSourceGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "OwnGUID", &Component::getOwnGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "ServerGUID", &Component::getServerGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsCreatedByOwnGUID", &Component::isCreatedByOwnGUID )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "IsCreatedByServer", &Component::isCreatedByServer )
            .tag( "NoSerialization" )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        // Properties (read/write)
        .property( "LocalOverride", &Component::getLocalOverride, &Component::setLocalOverride )
            .tag( "NoBitStream" )
        // Functions
        .function( "IsCreatedBy", &Component::isCreatedBy )
        .function( "IsCreatedBySource", &Component::isCreatedBySource )
        .function( "Destroy", &Component::destroyComponent );
        // Static functions
        // Operators
}

void CampBindings::bindComponentHandle()
{
    camp::Class::declare<ComponentHandle>( "ComponentHandle" )
    	// Constructors
        .constructor0()
        .constructor2<ComponentType, const String&>()
    	// Properties (read-only)
    	// Properties (read/write)
        .property( "Type", &ComponentHandle::mType )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" )
        .property( "Name", &ComponentHandle::mName )
            .tag( "NoBitStream" )
            .tag( "NoPropertyBrowser" );
    	// Functions
    	// Static functions
    	// Operators
}

void CampBindings::bindObjectTemplate()
{

}

void CampBindings::bindObjectTemplateManager()
{

}

void CampBindings::bindComponentTemplate()
{

}

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace ObjectSystem
} // Namespace Diversia