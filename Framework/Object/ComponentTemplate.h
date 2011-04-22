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

#ifndef DIVERSIA_OBJECT_COMPONENTTEMPLATE_H
#define DIVERSIA_OBJECT_COMPONENTTEMPLATE_H

#include "Object/Platform/Prerequisites.h"

#include "Object/ObjectTemplate.h"
#include "Object/ObjectTemplateManager.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

class DIVERSIA_OBJECT_API ComponentTemplate : public RakNet::Replica3, public boost::noncopyable
{
public:
    /**
    Sets the component template to be local even if the object template is a remote object.

    @param  localOverride   True to override to local mode, false to remove override.
    **/
    virtual void setLocalOverride( bool localOverride );
    /**
    Gets if the component template is local by an override.

    @return True if component is local by an override, false if not.
    **/
    inline bool getLocalOverride() const { return mLocalOverride; }
    /**
    Gets the networking (local/remote) type.
    **/
    inline NetworkingType getNetworkingType() const { return mNetworkingType; }
    /**
    Gets the name of the component template.
    **/
    inline const String& getName() const { return mName; }
    /**
    Gets the mode
    **/
    inline Mode getMode() const { return mMode; }
    /**
    Gets the source
    **/
    inline Source getSource() const { return mSource; }
    /**
    Gets the object template that owns this component template.
    **/
    inline ObjectTemplate& getObjectTemplate() const { return mObjectTemplate; }
    /**
    Gets the type for this component template.
    **/
    inline ComponentType getType() const { return mType; }
    /**
    Gets the type name for this component template.
    **/
    String getTypeName() const;
    /**
    Gets our own unique identifier.
    **/
    inline RakNet::RakNetGUID getOwnGUID() const { return mObjectTemplate.getOwnGUID(); }
    /**
    Gets the server unique identifier.
    **/
    inline RakNet::RakNetGUID getServerGUID() const { return mObjectTemplate.getServerGUID(); }
    /**
    Query if this component template is created by our own GUID. Uses source GUID, not 
    CreatingSystemGUID.

    @return True if created by our own GUID, false if not.
    **/
    inline bool isCreatedByOwnGUID() const { return mSourceGUID == getOwnGUID(); }
    /**
    Query if this component template is created by the server. Uses source GUID, not 
    CreatingSystemGUID.

    @return True if created by the server, false if not.
    **/
    inline bool isCreatedByServer() const { return mSourceGUID == getServerGUID(); }
    /**
    Query if this component template is created by a given GUID.

    @return True if created by the given GUID, false if not.
    **/
    inline bool isCreatedBy( RakNet::RakNetGUID guid ) const { return Replica3::GetCreatingSystemGUID() == guid; }
    /**
    Query if this component template is created by a given GUID. Uses source GUID, not 
    CreatingSystemGUID.

    @return True if created by the given GUID, false if not.
    **/
    inline bool isCreatedBySource( RakNet::RakNetGUID guid ) const { return mSourceGUID == guid; }
    /**
    Gets the source GUID of this component template. On the client the source GUID will only be the 
    client's own GUID or the server GUID. Component templates created by other clients have the 
    server GUID as source. This should only be used in situations where CreatingSystemGUID has not 
    been set yet.
    **/
    inline RakNet::RakNetGUID getSourceGUID() const { return mSourceGUID; }
    /**
    Gets the network unique identifier as a string.
    **/
    inline String getGUIDString() { return boost::lexical_cast<String>( GetNetworkID() ); }

    /**
    Sets a template property. 
    
    @param  rPropertyName   Name of the property. 
    @param  rValue          The value to set the property to.
    **/
    void setTemplateProperty( const String& rPropertyName, const camp::Value& rValue );
    /**
    Sets template properties using properties from given component.
    
    @param  rComponent  The component. 
    **/
    void setTemplateProperties( const Component& rComponent );

    /**
    Convenience function for destroying this component template.
    **/
    inline void destroyComponentTemplate() { mObjectTemplate.destroyComponentTemplate( mName ); }
    /**
    Convenience function for destroying this component template locally.
    **/
    void destroyComponentTemplateLocally();
    /**
    Connects a slot to the destruction signal.

    @param [in,out] rSlot   The slot (signature: void func(ComponentTemplate&)) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectDestruction( const sigc::slot<void, ComponentTemplate&>& rSlot )
    {
        return mDestructionSignal.connect( rSlot );
    }

protected:
    friend class ObjectTemplate;	///< Used for delayed destruction and networking type calls.

    /**
    Constructor.

    @param  rName                   The name of the component template.
    @param  mode                    The mode (Client/Server) the component template will run in.
    @param  networkingType          If the component template should be a remote or local object.
    @param  type                    Component type.
    @param  source                  The source of this component template.
    @param  localOverride           True to override component template to local mode even if the 
                                    object is a remote object.
    @param [in,out] rObjectTemplate The object that created this component.
    **/
    ComponentTemplate( const String& rName, Mode mode, NetworkingType networkingType, 
        ComponentType type, RakNet::RakNetGUID source, bool localOverride, 
        ObjectTemplate& rObjectTemplate );
    /**
    Destructor.
    **/
    virtual ~ComponentTemplate();

    /**
    Sets this component template to local or remote mode.

    @param  type    Local or remote.

    @throws Exception   When networking type cannot be set.
    **/
    virtual void setNetworkingTypeImpl( NetworkingType type ) = 0;
    /**
    Query if this component template can be set to local or remote mode.

    @param  type    Local or remote.

    @throws Exception   When networking type cannot be set.
    **/
    virtual void querySetNetworkingTypeImpl( NetworkingType type ) = 0;
    /**
    Cleanup the changes made by querySetNetworkingTypeImpl.

    @param  type    Local or remote.
    **/
    virtual void cleanupQuerySetNetworkingType( NetworkingType type ) = 0;

    /**
    Provide implementation for these functions based on permissions in the client/server.
    **/
    virtual RakNet::RM3ConstructionState QueryConstruction(
        RakNet::Connection_RM3* pDestinationConnection,
        RakNet::ReplicaManager3* pReplicaManager3 ) = 0;
    virtual bool QueryRemoteConstruction( RakNet::Connection_RM3* pSourceConnection ) = 0;
    virtual RakNet::RM3QuerySerializationResult QuerySerialization(
        RakNet::Connection_RM3* pDestinationConnection ) = 0;

    /**
    Provide implementation for these functions in specialized component template classes.
    **/
    virtual void SerializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) = 0;
    virtual bool DeserializeConstruction( RakNet::BitStream* pConstructionBitstream,
        RakNet::Connection_RM3* pSourceConnection ) = 0;
    inline virtual void SerializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pDestinationConnection ) {}
    inline virtual bool DeserializeDestruction( RakNet::BitStream* pDestructionBitstream,
        RakNet::Connection_RM3* pSourceConnection ) { return true; }
    virtual RakNet::RM3SerializationResult Serialize(
        RakNet::SerializeParameters* pSerializeParameters ) = 0;
    virtual void Deserialize( RakNet::DeserializeParameters* pDeserializeParameters ) = 0;

    /**
    Not used in ComponentTemplate.
    **/
    inline virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(
        RakNet::Connection_RM3* pDroppedConnection ) const { return RakNet::RM3AOPC_DO_NOTHING; }

    ComponentType mType;

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.
    friend void camp::detail::destroy<ComponentTemplate>( const UserObject& object );  ///< Allow private access for camp.

    /**
    Sets this component template to local or remote mode. Use setLocalOverride to set a component 
    to be local even if the object template is remote.

    @param  type    Local or remote.

    @throws Exception   When networking type cannot be set.
    **/
    void setNetworkingType( NetworkingType type );
	/**
    Broadcasts this component template to the network.
    **/
    void broadcastConstruction();
    /**
    Query if the component template should be destroyed on the network before being destroyed locally.
    **/
    bool queryBroadcastDestruction();
	/**
	Destroys this component template from the network.
	**/
	void broadcastDestruction();

    /**
    Implemented by ComponentTemplate.
    **/
    void WriteAllocationID( RakNet::Connection_RM3* pConnection,
        RakNet::BitStream* pAllocationIdBitstream ) const;
    void DeallocReplica( RakNet::Connection_RM3* pSourceConnection );

    String                                  mName;
    Mode			                        mMode;
    Source		                            mSource;
    RakNet::RakNetGUID                      mSourceGUID;
    NetworkingType 	                        mNetworkingType;
    bool                                    mLocalOverride;
    sigc::signal<void, ComponentTemplate&>  mDestructionSignal;
    bool                                    mBroadcastingDestruction;
    ObjectTemplate&                         mObjectTemplate;
    ComponentFactory&                       mFactory;

    typedef std::map<String, camp::Value> PropertyValueMap;
    const camp::Class&                                      mComponentClass;
    PropertyValueMap                                        mProperties;
    sigc::signal<void, const String&, const camp::Value&>   mPropertySignal;

    CAMP_RTTI()
};

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::ObjectSystem::ComponentTemplate,
    &Diversia::ObjectSystem::Bindings::CampBindings::bindComponentTemplate );

#endif