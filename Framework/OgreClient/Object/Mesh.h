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

#ifndef DIVERSIA_OGRECLIENT_MESH_H
#define DIVERSIA_OGRECLIENT_MESH_H

#include "OgreClient/Platform/Prerequisites.h"

#include "Client/Object/ClientComponent.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "Shared/Graphics/Graphics.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API Mesh : public ClientComponent, public sigc::trackable
{
public:
    Mesh( const String& rName, Mode mode, NetworkingType networkingType, 
        RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject );
    ~Mesh();

    /**
    Gets the mesh. 
    **/
    inline Ogre::MeshPtr getMesh() const { return mMesh; }
    /**
    Gets the name of the created mesh.
    **/
    inline const String& getMeshName() const { return mMeshName; }
    /**
    Query if the mesh has been loaded. 
    **/
    inline bool isLoaded() const { return !mMesh.isNull(); }
    /**
    Gets the component type.
    **/
    inline ComponentType getType() const { return COMPONENTTYPE_MESH; }
    static inline ComponentType getTypeStatic() { return COMPONENTTYPE_MESH; }
    /**
    Gets the component type name.
    **/
    inline String getTypeName() const { return COMPONENTNAME_MESH; }
    static inline String getTypeNameStatic() { return COMPONENTNAME_MESH; }

    /**
    Sets the mesh file. When executed before this component is created (next tick), 
    this will just set the mesh file. If done after the component is created, 
    this will automatically re-create the mesh.
    
    @param  rFile   Mesh file.
    **/
    void setMeshFile( const Path& rFile );

    /**
    Connects a slot to the mesh loaded signal. If the mesh is already loaded, the slot will be 
    called immediately.
    
    @param [in,out] rSlot   The slot (signature: void func(Mesh&)) to connect. 
    
    @return Connection object to block or disconnect the connection, or an empty connection when
            the entity is already loaded.
    **/
    sigc::connection connect( const sigc::slot<void, Mesh&>& rSlot );

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void create();
    inline bool delayedDestruction() { return false; }
    void resourcesLoaded();
    void meshParametersSet();
    void checkParameters();
    Ogre::MeshPtr generateMesh();

    ResourceManager& mResourceManager;

    Ogre::MeshPtr   mMesh;

    GraphicsShape   mShapeType;
    Path            mMeshFile;
    Vector3         mShapeParameters;
    Vector2         mShapeUV;
    int             mShapeXSegments;
    int             mShapeYSegments;
    int             mShapeZSegments;
    Real            mShapeRadius;
    Real            mShapeRadius2;
    bool            mShapeParametersSet;
    String          mMeshName;
    unsigned int    mMeshCounter;
    bool            mCreated;

    sigc::signal<void, Mesh&> mLoadedSignal;

    /**
    Sets the graphics shape parameters. When executed before this component is created (next tick), 
    this will just set the collision shape parameters. If done after the component is created, 
    this will automatically re-create the collision shape.
    
    @param  t The shape parameter
    **/
    template<typename DataType, typename boost::remove_const<typename boost::remove_reference<DataType>::type>::type Mesh::*MemberVar>
    void setShapeParameter( DataType t )
    {
        this->*MemberVar = t;
        Mesh::checkParameters();
        if( mShapeType == GRAPHICSSHAPE_MESH ) return;
        Mesh::meshParametersSet();
    }

    CAMP_RTTI()

};

/**
Specialization for shape type, setting this to mesh will not make the shape parameters set.
**/
template<>
inline void Mesh::setShapeParameter<GraphicsShape, &Mesh::mShapeType>( GraphicsShape t )
{
    mShapeType = t;
    Mesh::checkParameters();

    if( t == GRAPHICSSHAPE_MESH )
    {
        if( !mMeshFile.empty() && mCreated )
        {
            // Don't add mesh name to resource list if it's already there.
            if( std::find( mResourceList.begin(), mResourceList.end(), ResourceInfo( mMeshFile ) ) == 
                mResourceList.end() )
                mResourceList.push_back( ResourceInfo( mMeshFile ) );

            // TODO: Store and load properties when reloading the mesh so all the properties from the 
            // previous entity also get set onto the new entity.

            try
            {
                mResourceManager.loadResources( mResourceList, sigc::mem_fun( this, 
                    &Mesh::resourcesLoaded ) );
            }
            catch( Exception e )
            {
                CLOGE << "Could not load resource for mesh component: " << e.what();
            }
        }
        
        return;
    }

    Mesh::meshParametersSet();
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::OgreClient::Mesh, 
    &Diversia::OgreClient::Bindings::CampBindings::bindMesh );

#endif // DIVERSIA_OGRECLIENT_MESH_H