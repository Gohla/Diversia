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

#ifndef DIVERSIA_UTIL_FORWARD_H
#define DIVERSIA_UTIL_FORWARD_H

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

// Config
class ConfigManager;

// Helper
class Exception;
class ConsoleInput;

// Math
class Angle;
class BoundingBox;
class BoundingSphere;
class Colour;
class Degree;
class Math;
class Matrix3;
class Matrix4;
class Node;
class Plane;
class Quaternion;
class Radian;
class Ray;
class Vector2;
class Vector3;
class Vector4;

// Serialization
class SerializationFile;
class XMLSerializationFile;

// Job
class Job;
class Request;
class RequestManager;

// State
class State;
class StateMachine;

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_FORWARD_H
