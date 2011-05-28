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

#ifndef DIVERSIA_DSLTYPEEXTRACTOR_PREREQUISITES_H
#define DIVERSIA_DSLTYPEEXTRACTOR_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"
#include "Shared/Platform/Prerequisites.h"
#include "Client/Platform/Prerequisites.h"
#include "OgreClient/Platform/Prerequisites.h"

namespace Diversia
{
namespace DSLTypeExtractor
{
// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;
using namespace Diversia::Client;
using namespace Diversia::OgreClient;
} // namespace DSLTypeExtractor
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace DSLTypeExtractor
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "DSLTypeExtractor"
);

// Easy logging defines.
#define LOG(severity) BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, severity)
#define LOGC BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, Diversia::Util::LOG_CRITICAL)
#define LOGE BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, Diversia::Util::LOG_ERROR)
#define LOGW BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, Diversia::Util::LOG_WARNING)
#define LOGI BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, Diversia::Util::LOG_INFO)
#define LOGD BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, Diversia::Util::LOG_DEBUG)
#define LOGDE BOOST_LOG_SEV(Diversia::DSLTypeExtractor::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace DSLTypeExtractor
} // Namespace Diversia

#endif // DIVERSIA_DSLTYPEEXTRACTOR_PREREQUISITES_H