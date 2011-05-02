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

#include "OgreClient/Platform/StableHeaders.h"

#include "OgreClient/Graphics/GeometryHelper.h"

namespace Diversia
{
namespace OgreClient 
{
//------------------------------------------------------------------------------

Ogre::ManualObject* GeometryHelper::createAxisAlignedBox( String materialName, Ogre::Vector3 center, Ogre::Vector3 size )
{
    Ogre::Vector3 vector(size * 0.5f);
    Ogre::Vector3 pos(center.x - vector.x, center.y + vector.y, center.z + vector.z);
    Ogre::Vector3 vector3(center.x + vector.x, center.y + vector.y, center.z + vector.z);
    Ogre::Vector3 vector4(center.x - vector.x, center.y - vector.y, center.z + vector.z);
    Ogre::Vector3 vector5(center.x + vector.x, center.y - vector.y, center.z + vector.z);
    Ogre::Vector3 vector6(center.x - vector.x, center.y + vector.y, center.z - vector.z);
    Ogre::Vector3 vector7(center.x + vector.x, center.y + vector.y, center.z - vector.z);
    Ogre::Vector3 vector8(center.x - vector.x, center.y - vector.y, center.z - vector.z);
    Ogre::Vector3 vector9(center.x + vector.x, center.y - vector.y, center.z - vector.z);
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
    obj2->position(pos);
    obj2->position(vector3);
    obj2->position(vector4);
    obj2->position(vector5);
    obj2->position(vector6);
    obj2->position(vector7);
    obj2->position(vector8);
    obj2->position(vector9);
    obj2->triangle(0, 2, 1);
    obj2->triangle(2, 3, 1);
    obj2->triangle(2, 6, 3);
    obj2->triangle(6, 7, 3);
    obj2->triangle(7, 6, 4);
    obj2->triangle(7, 4, 5);
    obj2->triangle(5, 4, 1);
    obj2->triangle(1, 4, 0);
    obj2->triangle(0, 4, 2);
    obj2->triangle(2, 4, 6);
    obj2->triangle(5, 1, 3);
    obj2->triangle(5, 3, 7);
    obj2->end();
    return obj2;
}

Ogre::Entity* GeometryHelper::createAxisAlignedBoxEntity( String materialName, Ogre::Vector3 center, Ogre::Vector3 size )
{
    if( !Ogre::ResourceGroupManager::getSingletonPtr()->resourceGroupExists( "GeometryHelper" ) )
    {
        Ogre::ResourceGroupManager::getSingletonPtr()->createResourceGroup( "GeometryHelper" );
    }

    static unsigned int counter = 0;

    String meshName = String("AxisAlignedBoxEntity") + boost::lexical_cast<String>(counter++);
    Ogre::ManualObject* obj2 = createAxisAlignedBox(materialName, center, size);
    obj2->convertToMesh(meshName, "GeometryHelper");
    GlobalsBase::mScene->destroyManualObject(obj2);
    return GlobalsBase::mScene->createEntity(meshName);
}

Ogre::ManualObject* GeometryHelper::createCircle( String materialName, float radius, int sides, Ogre::Vector3 normal )
{
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_LINE_STRIP);
    std::vector<Ogre::Vector3> list = getCirclePoints(Ogre::Vector3::ZERO, radius, sides, normal, true);
    for(std::vector<Ogre::Vector3>::iterator i = list.begin(); i != list.end(); ++i)
    {
        obj2->position(*i);
    }
    obj2->end();
    return obj2;
}

Ogre::ManualObject* GeometryHelper::createCone( String materialName, Ogre::Vector3 center, Ogre::Vector3 direction, float radius, int sides, float height )
{
    std::vector<Ogre::Vector3> list = getCirclePoints(center, radius, sides, direction, false);
    Ogre::Vector3 pos = center + direction.normalisedCopy() * height;
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
    for(std::vector<Ogre::Vector3>::iterator i = list.begin(); i != list.end(); ++i)
    {
        obj2->position(*i);
    }
    obj2->position(pos);
    obj2->position(center);
    unsigned short count = (unsigned short) list.size();
    for (unsigned short i = 0; i < count; ++i)
    {
        unsigned short num5 = (i + 1) % count;
        obj2->index(count);
        obj2->index(i);
        obj2->index(num5);
        obj2->index(count + 1);
        obj2->index(i);
        obj2->index(num5);
    }
    obj2->end();
    return obj2;
}

Ogre::ManualObject* GeometryHelper::createCylinder( String materialName, Ogre::Vector3 center, float radius, float halfHeight, int sides, Ogre::Vector3 normal )
{
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
    Ogre::Vector3 vector = Ogre::Vector3(normal.normalisedCopy() * halfHeight);
    Ogre::Vector3 vector2 = center + vector;
    Ogre::Vector3 vector3 = center - vector;
    std::vector<Ogre::Vector3> list = getCirclePoints(vector2, radius, sides, normal, false);
    std::vector<Ogre::Vector3> list2 = getCirclePoints(vector3, radius, sides, normal, false);
    for(std::vector<Ogre::Vector3>::iterator i = list.begin(); i != list.end(); ++i)
    {
        obj2->position(*i);
    }
    for(std::vector<Ogre::Vector3>::iterator i = list2.begin(); i != list2.end(); ++i)
    {
        obj2->position(*i);
    }
    obj2->position(vector2);
    obj2->position(vector3);
    unsigned short count = (unsigned short) list.size();
    unsigned short num = 2 * count;
    unsigned short num2 = num + 1;
    for (unsigned short i = 0; i < count; ++i)
    {
        unsigned short num5 = (unsigned short) ((i + 1) % count);
        unsigned short num6 = (unsigned short) (i + count);
        unsigned short num7 = (unsigned short) (num5 + count);
        obj2->triangle(num6, num5, i);
        obj2->triangle(num7, num5, num6);
        obj2->triangle(num, i, num5);
        obj2->triangle(num7, num6, num2);
    }
    obj2->end();
    return obj2;
}

Ogre::Entity* GeometryHelper::createCylinderEntity( String entityName, String materialName, Ogre::Vector3 center, float radius, float halfHeight, int sides, Ogre::Vector3 normal )
{
    String meshName = entityName + ".mesh";
    Ogre::ManualObject* obj2 = createCylinder(materialName, center, radius, halfHeight, sides, normal);
    obj2->convertToMesh(meshName, "GeometryHelper");
    GlobalsBase::mScene->destroyManualObject(obj2);
    return GlobalsBase::mScene->createEntity(entityName, meshName);
}

Ogre::ManualObject* GeometryHelper::createLine( String materialName, Ogre::Vector3 startPoint, Ogre::Vector3 endPoint )
{
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_LINE_LIST);
    obj2->position(startPoint);
    obj2->position(endPoint);
    obj2->end();
    return obj2;
}

Ogre::ManualObject* GeometryHelper::createSphere( String materialName, float radius, int sides )
{
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
    double num = 6.2831853071795862 / ((double) sides);
    double d = 0.0;
    for (int i = 0; i < sides; i++)
    {
        float num4 = ((float) Ogre::Math::Cos(d)) * radius;
        float num5 = ((float) Ogre::Math::Sin(d)) * radius;
        std::vector<Ogre::Vector3> list = getCirclePoints(Ogre::Vector3(Ogre::Vector3::UNIT_Y * num5), num4, sides, Ogre::Vector3::NEGATIVE_UNIT_Y, false);
        for(std::vector<Ogre::Vector3>::iterator i = list.begin(); i != list.end(); ++i)
        {
            obj2->position(*i);
        }
        d += num;
    }
    for (int j = 0; j < (sides - 1); j++)
    {
        int num7 = j + 1;
        for (int k = 0; k < sides; k++)
        {
            int num9 = (k + 1) % sides;
            unsigned short num10 = (unsigned short) ((j * sides) + k);
            unsigned short num11 = (unsigned short) ((j * sides) + num9);
            unsigned short num12 = (unsigned short) ((num7 * sides) + k);
            unsigned short num13 = (unsigned short) ((num7 * sides) + num9);
            obj2->triangle(num10, num11, num12);
            obj2->triangle(num12, num11, num13);
        }
    }
    obj2->end();
    return obj2;
}

Ogre::Entity* GeometryHelper::createSphereEntity( String entityName, String materialName, float radius, int sides )
{
    String meshName = entityName + ".mesh";
    Ogre::ManualObject* obj2 = createSphere(materialName, radius, sides);
    obj2->convertToMesh(meshName, "GeometryHelper");
    GlobalsBase::mScene->destroyManualObject(obj2);
    return GlobalsBase::mScene->createEntity(entityName, meshName);
}

Ogre::ManualObject* GeometryHelper::createTorus( String materialName, Ogre::Vector3 center, float radius1, float radius2, int sides1, int sides2, Ogre::Vector3 normal )
{
    Ogre::Quaternion rotationTo = Ogre::Vector3::UNIT_Y.getRotationTo(normal);
    Ogre::ManualObject* obj2 = GlobalsBase::mScene->createManualObject();
    obj2->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
    std::vector<Ogre::Vector3> list = getCirclePoints(center, radius1, sides1, Ogre::Vector3::UNIT_Y, false);
    std::vector<Ogre::Vector3> list2 = getCircleTangents(sides1, Ogre::Vector3::UNIT_Y, false);
    unsigned short num = 0;
    for (int i = 0; i < list.size(); i++)
    {
        int num3 = i;
        int num4 = (i + 1) % list.size();
        Ogre::Vector3 vector = list[num3];
        Ogre::Vector3 vector2 = list[num4];
        Ogre::Vector3 vector3 = list2[num3];
        Ogre::Vector3 vector4 = list2[num4];
        std::vector<Ogre::Vector3> list3 = getCirclePoints(vector, radius2, sides2, vector3, true);
        std::vector<Ogre::Vector3> list4 = getCirclePoints(vector2, radius2, sides2, vector4, true);
        for (int j = 0; j < list3.size(); j++)
        {
            int num6 = (j + 1) % list3.size();
            obj2->position(Ogre::Vector3(rotationTo * list3[j]));
            obj2->position(Ogre::Vector3(rotationTo * list3[num6]));
            obj2->position(Ogre::Vector3(rotationTo * list4[num6]));
            num = (unsigned short) (num + 1);
            num = (unsigned short) (num + 1);
            num = (unsigned short) (num + 1);
            obj2->triangle(num, num, num);
            obj2->position(Ogre::Vector3(rotationTo * list4[num6]));
            obj2->position(Ogre::Vector3(rotationTo * list4[j]));
            obj2->position(Ogre::Vector3(rotationTo * list3[j]));
            num = (unsigned short) (num + 1);
            num = (unsigned short) (num + 1);
            num = (unsigned short) (num + 1);
            obj2->triangle(num, num, num);
        }
    }
    obj2->end();
    return obj2;
}

Ogre::Entity* GeometryHelper::createTorusEntity( String entityName, String materialName, Ogre::Vector3 center, float radius1, float radius2, int sides1, int sides2, Ogre::Vector3 normal )
{
    String meshName = entityName + ".mesh";
    Ogre::ManualObject* obj2 = createTorus(materialName, center, radius1, radius2, sides1, sides2, normal);
    obj2->convertToMesh(meshName, "GeometryHelper");
    GlobalsBase::mScene->destroyManualObject(obj2);
    return GlobalsBase::mScene->createEntity(entityName, meshName);
}

std::vector<Ogre::Vector3> GeometryHelper::getCircleNormals( Ogre::Vector3 center, float radius, int sides, Ogre::Vector3 normal, bool close )
{
    std::vector<Ogre::Vector3> list = getCirclePoints(center, radius, sides, normal, close);
    std::vector<Ogre::Vector3> list2 = getCirclePoints(center, radius + 1.0f, sides, normal, close);
    std::vector<Ogre::Vector3> list3;
    for (int i = 0; i < list.size(); i++)
    {
        list3.push_back(list2[i] - list[i]);
    }
    return list3;
}

std::vector<Ogre::Vector3> GeometryHelper::getCirclePoints( Ogre::Vector3 center, float radius, int sides, Ogre::Vector3 normal, bool close )
{
    Ogre::Quaternion rotationTo = Ogre::Vector3::UNIT_X.getRotationTo(normal);
    // Should this be radians?
    float num2 = (float) Ogre::Math::ACos((double) (normal.dotProduct(Ogre::Vector3::NEGATIVE_UNIT_X) / normal.length())).valueRadians();
    if (num2 <= 0.001)
    {
        rotationTo.FromAngleAxis(Ogre::Radian(Ogre::Degree(180)), Ogre::Vector3::UNIT_Y);
    }
    std::vector<Ogre::Vector3> list;
    float num3 = 0.0f;
    float num4 = 6.283185f / ((float) sides);
    int num5 = sides;
    if (close)
    {
        num5++;
    }
    for (int i = 0; i < num5; i++)
    {
        Ogre::Vector3 vector2 = Ogre::Vector3(0.0f, radius * ((float) Ogre::Math::Cos((double) num3)), radius * ((float) Ogre::Math::Sin((double) num3)));
        Ogre::Vector3 item = center + (rotationTo * vector2);
        list.push_back(item);
        num3 += num4;
    }
    return list;
}

std::vector<Ogre::Vector3> GeometryHelper::getCircleTangents( int sides, Ogre::Vector3 normal, bool close )
{
    Ogre::Quaternion rotationTo = Ogre::Vector3::UNIT_Y.getRotationTo(normal);
    std::vector<Ogre::Vector3> list;
    float r = 0.0f;
    float num2 = 6.283185f / ((float) sides);
    int num3 = sides;
    if (close)
    {
        num3++;
    }
    for (int i = 0; i < num3; i++)
    {
        Ogre::Quaternion quaternion2;
        quaternion2.FromAngleAxis(Ogre::Radian(r), Ogre::Vector3::UNIT_Y);
        Ogre::Vector3 vector2 = Ogre::Vector3(quaternion2 * Ogre::Vector3::UNIT_Z);
        Ogre::Vector3 item = Ogre::Vector3(rotationTo * vector2);
        list.push_back(item);
        r += num2;
    }
    return list;
}

//------------------------------------------------------------------------------

} // Namespace OgreClient
} // Namespace Diversia