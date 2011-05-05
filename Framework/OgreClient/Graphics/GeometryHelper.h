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
    static Ogre::ManualObject* createAxisAlignedBox(const String& materialName, const Ogre::Vector3& center, const Ogre::Vector3& size);
    static Ogre::Entity* createAxisAlignedBoxEntity(const String& materialName, const Ogre::Vector3& center, const Ogre::Vector3& size);
    static Ogre::ManualObject* createCircle(const String& materialName, float radius, int sides, const Ogre::Vector3& normal);
    inline static Ogre::ManualObject* createCircleXY(const String& materialName, float radius, int sides)
    {
        return createCircle(materialName, radius, sides, Ogre::Vector3::UNIT_Z);
    }
    inline static Ogre::ManualObject* createCircleXZ(const String& materialName, float radius, int sides)
    {
        return createCircle(materialName, radius, sides, Ogre::Vector3::UNIT_Y);
    }
    inline static Ogre::ManualObject* createCircleYZ(const String& materialName, float radius, int sides)
    {
        return createCircle(materialName, radius, sides, Ogre::Vector3::UNIT_X);
    }
    static Ogre::ManualObject* createCone(const String& materialName, const Ogre::Vector3& center, const Ogre::Vector3& direction, float radius, int sides, float height);
    static Ogre::ManualObject* createCylinder(const String& materialName, const Ogre::Vector3& center, float radius, float halfHeight, int sides, const Ogre::Vector3& normal);
    static Ogre::Entity* createCylinderEntity(const String& entityName, const String& materialName, const Ogre::Vector3& center, float radius, float halfHeight, int sides, const Ogre::Vector3& normal);
    static Ogre::ManualObject* createLine(const String& materialName, Ogre::Vector3 startPoint, Ogre::Vector3 endPoint);
    static Ogre::ManualObject* createSphere(const String& materialName, float radius, int sides);
    static Ogre::Entity* createSphereEntity(const String& entityName, const String& materialName, float radius, int sides);
    static Ogre::ManualObject* createTorus(const String& materialName, const Ogre::Vector3& center, float radius1, float radius2, int sides1, int sides2, const Ogre::Vector3& normal);
    static Ogre::Entity* createTorusEntity(const String& entityName, const String& materialName, const Ogre::Vector3& center, float radius1, float radius2, int sides1, int sides2, const Ogre::Vector3& normal);
    inline static Ogre::Entity* createTorusEntityXY(const String& entityName, const String& materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorusEntity(entityName, materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Z);
    }
    inline static Ogre::Entity* createTorusEntityXZ(const String& entityName, const String& materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorusEntity(entityName, materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Y);
    }
    inline static Ogre::Entity* createTorusEntityYZ(const String& entityName, const String& materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorusEntity(entityName, materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_X);
    }
    inline static Ogre::ManualObject* createTorusXY(const String& materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorus(materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Z);
    }
    inline static Ogre::ManualObject* createTorusXZ(const String& materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorus(materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_Y);
    }
    inline static Ogre::ManualObject* createTorusYZ(const String& materialName, float radius1, float radius2, int sides1, int sides2)
    {
        return createTorus(materialName, Ogre::Vector3::ZERO, radius1, radius2, sides1, sides2, Ogre::Vector3::UNIT_X);
    }
    static std::vector<Ogre::Vector3> getCircleNormals(const Ogre::Vector3& center, float radius, int sides, const Ogre::Vector3& normal, bool close);
    static std::vector<Ogre::Vector3> getCirclePoints(const Ogre::Vector3& center, float radius, int sides, const Ogre::Vector3& normal, bool close);
    static std::vector<Ogre::Vector3> getCircleTangents(int sides, const Ogre::Vector3& normal, bool close);

};

//------------------------------------------------------------------------------
} // Namespace OgreClient
} // Namespace Diversia

#endif // DIVERSIA_OGRECLIENT_GEOMETRYHELPER_H