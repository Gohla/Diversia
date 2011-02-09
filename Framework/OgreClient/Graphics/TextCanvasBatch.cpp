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

#include "OgreClient/Graphics/TextCanvasBatch.h"
#include "OgreClient/Graphics/CameraManager.h"
#include "Canvas.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

TextCanvasBatch::TextCanvasBatch( CameraManager& rCameraManager, Atlas& rAtlas, Canvas& rCanvas, 
    Ogre::Camera& rCamera, const String& rFont ):
    mAtlas( rAtlas ),
    mCanvas( rCanvas ),
    mCamera( &rCamera ),
    mFont( rFont )
{
    rCameraManager.connect( sigc::mem_fun( this, &TextCanvasBatch::cameraChanged ) );
    GlobalsBase::mUpdateSignal->connect( sigc::mem_fun( this, &TextCanvasBatch::update ) );

    GlobalsBase::mText = this;
}

TextCanvasBatch::~TextCanvasBatch()
{
    GlobalsBase::mText = 0;

    for( TextCanvasMap::iterator i = mTextMap.begin(); i != mTextMap.end(); ++i )
    {
        delete i->second;
    }
}

TextCanvas& TextCanvasBatch::createTextCanvas( const String& rName, Ogre::MovableObject& rTarget, 
    const String& rCaption /*= ""*/, const Colour& rColour /*= Colour::White */ )
{
    if( !TextCanvasBatch::hasTextCanvas( rName ) )
    {
        TextCanvas* text = new TextCanvas( rTarget, rCaption, rColour );
        mTextMap.insert( std::make_pair( rName, text ) );
        return *text;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_DUPLICATE_ITEM, "Text already exists.", 
            "TextCanvasBatch::createText" );
    }
}

TextCanvas& TextCanvasBatch::getTextCanvas( const String& rName ) const
{
    TextCanvasMap::const_iterator i = mTextMap.find( rName );
    if( i != mTextMap.end() )
    {
        return *i->second;
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Text does not exist.", 
            "TextCanvasBatch::getText" );
    }
}

bool TextCanvasBatch::hasTextCanvas( const String& rName ) const
{
    return mTextMap.find( rName ) != mTextMap.end();
}

void TextCanvasBatch::destroyTextCanvas( const String& rName )
{
    TextCanvasMap::iterator i = mTextMap.find( rName );
    if( i != mTextMap.end() )
    {
        delete i->second;
        mTextMap.erase( i );
    }
    else
    {
        DIVERSIA_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Text does not exist.", 
            "TextCanvasBatch::destroyText" );
    }
}

void TextCanvasBatch::update()
{
    std::vector<TextCanvas*> visibleTexts;

    for( TextCanvasMap::iterator i = mTextMap.begin(); i != mTextMap.end(); i++)
    {
        if( !i->second->mTarget ) continue;
        if( !i->second->mTarget->isInScene() ) continue;

        // Derive the average point between the top-most corners of the object's bounding box
        const Ogre::AxisAlignedBox &AABB = i->second->mTarget->getWorldBoundingBox(true);
        Ogre::Vector3 point = (AABB.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_TOP)
            + AABB.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_TOP)
            + AABB.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_TOP)
            + AABB.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_TOP)) / 4;

        // Is the camera facing that point? If not, hide the overlay and return.
        Ogre::Plane cameraPlane = Ogre::Plane(Ogre::Vector3(mCamera->getDerivedOrientation().zAxis()), mCamera->getDerivedPosition());
        if(cameraPlane.getSide(point) != Ogre::Plane::NEGATIVE_SIDE)
            continue;

        Ogre::Real distance = mCamera->getDerivedPosition().distance(point);

        // scale [40, 400] to [5, 0]: (-5 / 360)x + (50 / 9) = y
        distance = Ogre::Math::Floor((-10 / 360.0)*distance + (100 / 9.0));

        if(distance < 0) distance = 0; 
        else if(distance > 10) distance = 10;

        // Derive the 2D (x,y) screen-space coordinates for that point
        point = mCamera->getProjectionMatrix() * (mCamera->getViewMatrix() * point);
        Ogre::Real x = (point.x / 2) + 0.5f;
        Ogre::Real y = 1 - ((point.y / 2) + 0.5f);

        i->second->mPosition = Ogre::Vector3(x, y, distance);

        visibleTexts.push_back( i->second ); 
    }

    mCanvas.clear();

    const Ogre::uint charPerLine = 24;
    const Ogre::uint maxCharPerLine = 36;

    for(std::vector<TextCanvas*>::iterator i = visibleTexts.begin(); i != visibleTexts.end(); i++)
    {
        int x = (*i)->mPosition.x * mCamera->getViewport()->getActualWidth();
        int y = (*i)->mPosition.y * mCamera->getViewport()->getActualHeight();

        Ogre::uint fontSize = (*i)->mPosition.z + 10;
        Ogre::Real fontHeight = mAtlas.getFontMetrics(mFont, fontSize).height;
        Ogre::Real avgAdvance = mAtlas.getGlyphInfo(mFont, fontSize, 'x').advance;
        Ogre::Real spaceAdvance = mAtlas.getGlyphInfo(mFont, fontSize, 'i').advance;
        Ogre::Real pen = x = x - (std::min((*i)->mCaption.length(), charPerLine) * avgAdvance) / 2;

        y -= ((*i)->mCaption.length() / charPerLine) * fontHeight;

        Ogre::uint lineCharCount = 0;

        for(Ogre::String::const_iterator j = (*i)->mCaption.begin(); j != (*i)->mCaption.end(); j++)
        {
            lineCharCount++;

            if(*j == ' ')
            {
                if(lineCharCount > charPerLine)
                {
                    pen = x;
                    y += fontHeight;
                    lineCharCount = 0;
                }
                else
                {
                    pen += spaceAdvance;
                }
                continue;
            }

            if(lineCharCount > maxCharPerLine)
            {
                pen = x;
                y += fontHeight;
                lineCharCount = 0;
            }

            GlyphInfo glyph = mAtlas.getGlyphInfo(mFont, fontSize, (*j));
            mCanvas.drawGlyph(glyph, glyph.bearingX + pen + 1, y - glyph.bearingY + 1, glyph.texInfo.width, glyph.texInfo.height, Ogre::ColourValue(0, 0, 0, 0.7));
            mCanvas.drawGlyph(glyph, glyph.bearingX + pen, y - glyph.bearingY, glyph.texInfo.width, glyph.texInfo.height, toColour<Ogre::ColourValue>( (*i)->mColour ));
            pen += glyph.advance;
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia