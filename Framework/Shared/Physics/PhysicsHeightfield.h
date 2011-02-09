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

#ifndef DIVERSIA_SHARED_PHYSICSHEIGHTMAP_H
#define DIVERSIA_SHARED_PHYSICSHEIGHTMAP_H

#include "Shared/Platform/Prerequisites.h"

#include <FreeImage.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

namespace Diversia
{
//------------------------------------------------------------------------------

class PhysicsHeightfield
{
public:
    PhysicsHeightfield( const Path& rFile, unsigned int terrainSize ):
        mHeightfieldFile( rFile ), 
        mTerrainSize( terrainSize ),
        mMinHeight( std::numeric_limits<float>::max() ),
        mMaxHeight( std::numeric_limits<float>::min() ),
        mHeightfieldFloats( 0 ),
        mHeightfieldShape( 0 )
    {

    }
    ~PhysicsHeightfield()
    {
        if( mHeightfieldFloats ) delete[] mHeightfieldFloats;
        if( mHeightfieldShape ) delete mHeightfieldShape;
    }

    btCollisionShape* init()
    {
        FILE* fp = fopen( mHeightfieldFile.string().c_str(), "rb" );
        if( fp != NULL )
        {
            unsigned int totalSize = mTerrainSize * mTerrainSize;
            mHeightfieldFloats = new float[totalSize];
            if( fread( mHeightfieldFloats, sizeof(float), totalSize, fp ) != totalSize )
            {
                fclose( fp );
                DIVERSIA_EXCEPT( Exception::ERR_INTERNAL_ERROR, 
                    "Error reading from file " + mHeightfieldFile.string(), 
                    "PhysicsHeightfield::init" );
            }

            for( unsigned int i = 0; i < totalSize; ++i )
            {
                mMinHeight = std::min( mMinHeight, mHeightfieldFloats[i] );
                mMaxHeight = std::max( mMaxHeight, mHeightfieldFloats[i] );
            }
            mOffset = ( mMaxHeight - mMinHeight ) / 2;

            mHeightfieldShape = new btHeightfieldTerrainShape
            (
                mTerrainSize,
                mTerrainSize,
                mHeightfieldFloats,
                1,                  // Ignore height scale
                mMinHeight,
                mMaxHeight,
                1,                  // Y-axis up
                PHY_FLOAT,
                true
            );

            mHeightfieldShape->setUseDiamondSubdivision( true );
            mHeightfieldShape->setLocalScaling( btVector3( 1, 1, 1 ) );

            fclose( fp );

            SLOGI << "Loaded physics heightfield: ";
            SLOGI << "File: " << mHeightfieldFile;
            SLOGI << "Side size: " << mTerrainSize;
            SLOGI << "Total size: " << totalSize;
            SLOGI << "Min/max height: " << mMinHeight << "/" << mMaxHeight;
            SLOGI << "Height offset: " << mOffset;

            return mHeightfieldShape;
        }
        else
        {
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE, 
                "Cannot open file " + mHeightfieldFile.string(), 
                "PhysicsHeightfield::init" );
        }

        return 0;
    }

    const Path          mHeightfieldFile;
    unsigned int        mTerrainSize;

    float                       mMinHeight;
    float                       mMaxHeight;
    float                       mOffset;
    float*                      mHeightfieldFloats;
    btHeightfieldTerrainShape*  mHeightfieldShape;
};

//------------------------------------------------------------------------------
} // Namespace Diversia

#endif // DIVERSIA_SHARED_PHYSICSHEIGHTMAP_H