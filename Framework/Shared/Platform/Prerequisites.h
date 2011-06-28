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

#ifndef DIVERSIA_SHARED_PREREQUISITES_H
#define DIVERSIA_SHARED_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"

// Logging
#include "Util/Log/Log.h"

namespace Diversia
{
// Exports
#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_WIN32
#   ifndef DIVERSIA_SHARED_STATIC
#       ifndef DIVERSIA_SHARED_API
#           if defined( DIVERSIA_SHARED_EXPORTS )
#               define DIVERSIA_SHARED_API __declspec(dllexport)
#           else
#               if defined(__MINGW32__)
#                   define DIVERSIA_SHARED_API
#               else
#                   define DIVERSIA_SHARED_API __declspec(dllimport)
#               endif
#           endif
#       endif
#   else
#       define DIVERSIA_SHARED_API
#   endif
#endif

#if DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_LINUX || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
#   if defined( DIVERSIA_SHARED_GCC_VISIBILITY )
#       define DIVERSIA_SHARED_API  __attribute__ ((visibility("default")))
#   else
#       define DIVERSIA_SHARED_API
#   endif
#endif

// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
namespace Shared
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "Shared"
);

// Easy logging defines.
#define SLOG(severity) BOOST_LOG_SEV(Diversia::Shared::Log::lg, severity)
#define SLOGC BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_CRITICAL)
#define SLOGE BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_ERROR)
#define SLOGW BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_WARNING)
#define SLOGI BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_INFO)
#define SLOGD BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_DEBUG)
#define SLOGDE BOOST_LOG_SEV(Diversia::Shared::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace Shared
} // Namespace Diversia

// Definitions
#include "Shared/Camp/CampBindings.h"
#include "Shared/Platform/Defines.h"

#endif // DIVERSIA_SHARED_PREREQUISITES_H