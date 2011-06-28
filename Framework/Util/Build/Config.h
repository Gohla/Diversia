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

#ifndef DIVERSIA_CONFIG_H
#define DIVERSIA_CONFIG_H

/** There are three modes for handling asserts in Diversia:
0 - STANDARD - Standard asserts in debug builds, nothing in release builds
1 - RELEASE_EXCEPTIONS - Standard asserts in debug builds, exceptions in release builds
2 - EXCEPTIONS - Exceptions in debug builds, exceptions in release builds
*/
#define DIVERSIA_ASSERT_MODE 1

/** If set to 1, Real is typedef'ed to double. Otherwise, Real is typedef'ed
    to float. Setting this allows you to perform mathematical operations in the
    CPU (Quaternion, Vector3 etc) with more precision, but bear in mind that the
    GPU still operates in single-precision mode.
*/
#ifndef DIVERSIA_DOUBLE_PRECISION
    #define DIVERSIA_DOUBLE_PRECISION 0
#endif

// Diversia server size, in meters, so a server's surface would have size DIVERSIA_SERVER_SIZE^2.
#define DIVERSIA_SERVER_SIZE 1024

// Define if all dependencies are linked in statically.
//#define DIVERSIA_DEPENDENCIES_STATIC

// Define if framework should be built as static libraries.
#define DIVERSIA_FRAMEWORK_STATIC

// Link camp statically
#define CAMP_STATIC

// Link all dependencies statically
#ifdef DIVERSIA_DEPENDENCIES_STATIC
#   define CAMP_STATIC
#endif

// Link framework statically
#ifdef DIVERSIA_FRAMEWORK_STATIC
#   define DIVERSIA_UTIL_STATIC
#   define DIVERSIA_OBJECT_STATIC
#   define DIVERSIA_SHARED_STATIC
#   define DIVERSIA_CLIENT_STATIC
#   define DIVERSIA_OGRECLIENT_STATIC
#endif

// Copy strings inside camp::Value.
#define CAMP_COPY_STRING

// Link to boost dll's
#ifndef DIVERSIA_DEPENDENCIES_STATIC
#   define BOOST_ALL_DYN_LINK
#endif

// Boost parameter max arity
#define BOOST_PARAMETER_MAX_ARITY 7

#endif // DIVERSIA_CONFIG_H