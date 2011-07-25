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

#include "OgreClient/Object/Mesh.h"
#include <ogre-procedural/Procedural.h>

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Mesh::Mesh( const String& rName, Mode mode, NetworkingType networkingType, 
    RakNet::RakNetGUID source, bool localOverride, ClientObject& rObject ):
    ClientComponent( rName, mode, networkingType, Mesh::getTypeStatic(), source, localOverride, 
        rObject ),
    mResourceManager( rObject.getClientObjectManager().getPluginManager().getPlugin<ResourceManager>() ),
    mShapeType( GRAPHICSSHAPE_SPHERE ),
    mShapeParameters( Vector3::UNIT_SCALE ),
    mShapeUV( Vector2::UNIT_SCALE ),
    mShapeXSegments( 4 ),
    mShapeYSegments( 4 ),
    mShapeZSegments( 4 ),
    mShapeRadius( 1.0 ),
    mShapeRadius2( 1.0 ),
    mShapeParametersSet( false ),
    mMeshName( Component::getGUIDString() ),
    mMeshCounter( 0 ),
    mCreated( false )
{
    PropertySynchronization::storeUserObject();

    Procedural::Root::getInstance()->sceneManager = GlobalsBase::mScene;
}

Mesh::~Mesh()
{

}

void Mesh::setMeshFile( const Path& rFile )
{
    if( mMeshFile == rFile ) return;

    if( rFile.empty() )
    {
        if( mShapeType == GRAPHICSSHAPE_MESH ) mShapeType = GRAPHICSSHAPE_BOX;

        Mesh::meshParametersSet();
        return;
    }

    mMeshFile = rFile;
    mShapeParameters = Vector3::UNIT_SCALE;
    mShapeParametersSet = false;
    mShapeType = GRAPHICSSHAPE_MESH;

    if( mCreated )
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
}

sigc::connection Mesh::connect( const sigc::slot<void, Mesh&>& rSlot )
{
    if( Mesh::isLoaded() ) rSlot( *this );
    return mLoadedSignal.connect( rSlot );
}

void Mesh::create()
{
    mCreated = true;
    if( mMeshFile.empty() && !mShapeParametersSet ) return;

    try
    {
        if( !mMeshFile.empty() )
        {
            // Don't add mesh name to resource list if it's already there.
            if( std::find( mResourceList.begin(), mResourceList.end(), ResourceInfo( mMeshFile ) ) == 
                mResourceList.end() )
                mResourceList.push_back( ResourceInfo( mMeshFile ) );

            mResourceManager.loadResources( mResourceList, sigc::mem_fun( this, 
                &Mesh::resourcesLoaded ) );
        }
        else
        {
            mMesh = Mesh::generateMesh();
            mLoadedSignal( *this );
        }
    }
    catch( Exception e )
    {
        CLOGE << "Could not load resource for mesh component: " << e.what();
        Component::destroyComponentLocally();
    }
}

void Mesh::resourcesLoaded()
{
    mMeshName = mMeshFile.string();
    mLoadedSignal( *this );
}

void Mesh::meshParametersSet()
{
    mMeshFile.clear();
    mShapeParametersSet = true;

    if( mCreated )
    {
        // TODO: Store and load properties when reloading the mesh so all the properties from the 
        // previous entity also get set onto the new entity.

        try
        {
            mMesh = Mesh::generateMesh();
            mLoadedSignal( *this );
        }
        catch( Exception e )
        {
            CLOGE << "Could not create mesh: " << e.what();
        }
    }
}

void Mesh::checkParameters()
{
    if( mShapeXSegments < 1 || mShapeYSegments < 1 || mShapeZSegments < 1 )
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, "Number of segments must be higher than 0", 
        	"Mesh::checkParameters" );      
    if( mShapeParameters.x <= 0 || mShapeParameters.y <= 0 || mShapeParameters.z <= 0 )
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, "Size must be higher than 0", 
            "Mesh::checkParameters" );
    if( mShapeRadius <= 0 || mShapeRadius2 <= 0 )
        DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, "Radius must be higher than 0", 
            "Mesh::checkParameters" );

    switch( mShapeType )
    {
        case GRAPHICSSHAPE_TUBE:
            if( mShapeRadius >= mShapeRadius2 )
                DIVERSIA_EXCEPT( Exception::ERR_INVALIDPARAMS, 
                    "Outer radius (ShapeRadius2) must be bigger than inner radius (ShapeRadius)", 
                    "Mesh::checkParameters" );
    }
}

Ogre::MeshPtr Mesh::generateMesh()
{
    // Remove old mesh
    Ogre::MeshManager& meshManager = Ogre::MeshManager::getSingleton();
    if( meshManager.resourceExists( mMeshName ) )
    {
        meshManager.remove( mMeshName );
    }
    
    // Generate a new name if mesh is still in use.
    if( meshManager.resourceExists( mMeshName ) )
    {
        mMeshName = Component::getGUIDString() + boost::lexical_cast<String>( mMeshCounter++ ); 
    }
    
    // Generate mesh
    switch( mShapeType )
    {
        case GRAPHICSSHAPE_BOX:
            return Procedural::BoxGenerator()
                .setSizeX( mShapeParameters.x )
                .setSizeY( mShapeParameters.y )
                .setSizeZ( mShapeParameters.z )
                .setNumSegX( mShapeXSegments )
                .setNumSegY( mShapeYSegments )
                .setNumSegZ( mShapeZSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_ROUNDEDBOX:
            return Procedural::RoundedBoxGenerator()
                .setChamferSize( mShapeRadius )
                .setSizeX( mShapeParameters.x )
                .setSizeY( mShapeParameters.y )
                .setSizeZ( mShapeParameters.z )
                .setNumSegX( mShapeXSegments )
                .setNumSegY( mShapeYSegments )
                .setNumSegZ( mShapeZSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_PLANE:
            return Procedural::PlaneGenerator()
                .setSizeX( mShapeParameters.x )
                .setSizeY( mShapeParameters.y )
                .setNumSegX( mShapeXSegments )
                .setNumSegY( mShapeYSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_SPHERE:
            return Procedural::SphereGenerator()
                 // TODO: .setNumRings()
                .setRadius( mShapeRadius )
                .setNumSegments( mShapeXSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_ICOSPHERE:
            return Procedural::IcoSphereGenerator()
                // TODO: .setNumIterations()
                .setRadius( mShapeRadius )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_CYLINDER:
            return Procedural::CylinderGenerator()
                .setHeight( mShapeParameters.x )
                .setRadius( mShapeRadius )
                .setNumSegBase( mShapeXSegments )
                .setNumSegHeight( mShapeYSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_TUBE:
            return Procedural::TubeGenerator()
                .setHeight( mShapeParameters.x )
                .setInnerRadius( mShapeRadius )
                .setOuterRadius( mShapeRadius2 )
                .setNumSegBase( mShapeXSegments )
                .setNumSegHeight( mShapeYSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_CAPSULE:
            return Procedural::CapsuleGenerator()
                .setHeight( mShapeParameters.x )
                .setRadius( mShapeRadius )
                .setNumSegments( mShapeXSegments )
                .setNumSegHeight( mShapeYSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_CONE:
            return Procedural::ConeGenerator()
                .setHeight( mShapeParameters.x )
                .setRadius( mShapeRadius )
                .setNumSegBase( mShapeXSegments )
                .setNumSegHeight( mShapeYSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_TORUS:
            return Procedural::TorusGenerator()
                .setRadius( mShapeRadius )
                .setSectionRadius( mShapeRadius2 )
                .setNumSegCircle( mShapeXSegments )
                .setNumSegSection( mShapeYSegments )
                .setUTile( mShapeUV.x )
                .setVTile( mShapeUV.y )
                .realizeMesh( mMeshName, mResourceManager.getGroup() );
        case GRAPHICSSHAPE_MESH:
            CLOGE << "Cannot generate a mesh of type mesh.";
            break;
        default:
            CLOGE << "Cannot generate a mesh of type unknown.";
            break;
    }

    return Ogre::MeshPtr();
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia