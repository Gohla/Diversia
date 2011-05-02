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

#ifndef DIVERSIA_OGRECLIENT_GEOMETRYHELPER_H
#define DIVERSIA_OGRECLIENT_GEOMETRYHELPER_H

#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace OgreClient
{
//------------------------------------------------------------------------------

class DIVERSIA_OGRECLIENT_API GeometryHelper
{
public:
    static Ogre::ManualObject* createAxisAlignedBox(String materialName, Ogre::Vector3 center, Ogre::Vector3 size);
    static Ogre::Entity* createAxisAlignedBoxEntity(String materialName, Ogre::Vector3 center, Ogre::Vector3 size);
    static Ogre::ManualObject* createCircle(String materialName, float radius, int sides, Ogre::Vector3 normal);
    inline static Ogre::ManualObject* createCircleXY(String materialName, float radius, int sides)
    {
        return createCircle(materialName, radius, sides, Ogre::Vector3::UNIT_Z);
    }
    inline static Ogre::ManualObject* createCircleXZ(String materialName, float radius, int sides)
    {
        return createCircle(materialName, radius, sides, Ogre::Vector3::UNIT_Y);
    }
    inline static Ogre::ManualObject* createCircleYZ(String materialName, float radius, int sides)
    {
        return createCircle(materialName, radius, sides, Ogre::Vector3::UNIT_X);
    }
    static Ogre::ManualObject* createCone(String materialName, Ogre::Vector3 center, Ogre::Vector3 direction, float radius, int sides, float height);
    static Ogre::ManualObject* createCylinder(String materialName, Ogre::Vector3 center, float radius, float halfHeight, int sides, Ogre::Vector3 normal);
    static Ogre::Entity* createCylinderEntity(String entityName, String materialName, Ogre::Vector3 center, float radius, float halfHeight, int sides, Ogre::Vector3 normal);
    static Ogre::ManualObject* createLine(String materialName, Ogre::Vector3 startPoint, Ogre::Vector3 endPoint);
    static Ogre::ManualObject* createSphere(String materialName, float radius, int sides);
    static Ogre::Entity* createSphereEntity(String entityName, String materialName, float radius, int sides);
    static Ogre::ManualObject* createTorus(String materialName, Ogre::Vector3 center, float radius1, float radius2, int sides1, int sides2, Ogre::Vector3 normal);
    static Ogre::Entity* createTorusEntity(String entityName, String materialName, Ogre::Vector3 center, float radius1, float radius2, int sides1, int sides2, Ogre::Vector3 normal);
    inline static Ogre::Entity* createTorusEntityXY(String entityName, String materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorusEntity(entityName, materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Z);
    }
    inline static Ogre::Entity* createTorusEntityXZ(String entityName, String materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorusEntity(entityName, materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Y);
    }
    inline static Ogre::Entity* createTorusEntityYZ(String entityName, String materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorusEntity(entityName, materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_X);
    }
    inline static Ogre::ManualObject* createTorusXY(String materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorus(materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Z);
    }
    inline static Ogre::ManualObject* createTorusXZ(String materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorus(materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Y);
    }
    inline static Ogre::ManualObject* createTorusYZ(String materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorus(materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_X);
    }
    static std::vector<Ogre::Vector3> getCircleNormals(Ogre::Vector3 center, float radius, int sides, Ogre::Vector3 normal, bool close);
    static std::vector<Ogre::Vector3> getCirclePoints(Ogre::Vector3 center, float radius, int sides, Ogre::Vector3 normal, bool close);
    static std::vector<Ogre::Vector3> getCircleTangents(int sides, Ogre::Vector3 normal, bool close);

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_GEOMETRYHELPER_H