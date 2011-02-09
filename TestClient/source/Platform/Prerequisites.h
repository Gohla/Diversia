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

#ifndef DIVERSIA_TESTCLIENT_PREREQUISITES_H
#define DIVERSIA_TESTCLIENT_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"
#include "Shared/Platform/Prerequisites.h"
#include "Client/Platform/Prerequisites.h"

namespace Diversia
{
namespace TestClient
{
// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;
using namespace Diversia::Client;

// Forward declarations

} // namespace TestClient
} // namespace Diversia

// Include after the configuration has been set.
#include "Util/Build/StdHeaders.h"

// Logging
#include "Util/Log/Log.h"
namespace Diversia
{
namespace TestClient
{
namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "TestClient"
);

// Easy logging defines.
#define LOG(severity) BOOST_LOG_SEV(Diversia::TestClient::Log::lg, severity)
#define LOGC BOOST_LOG_SEV(Diversia::TestClient::Log::lg, Diversia::Util::LOG_CRITICAL)
#define LOGE BOOST_LOG_SEV(Diversia::TestClient::Log::lg, Diversia::Util::LOG_ERROR)
#define LOGW BOOST_LOG_SEV(Diversia::TestClient::Log::lg, Diversia::Util::LOG_WARNING)
#define LOGI BOOST_LOG_SEV(Diversia::TestClient::Log::lg, Diversia::Util::LOG_INFO)
#define LOGD BOOST_LOG_SEV(Diversia::TestClient::Log::lg, Diversia::Util::LOG_DEBUG)
#define LOGDE BOOST_LOG_SEV(Diversia::TestClient::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace TestClient
} // Namespace Diversia

// Camp bindings
#include "Camp/CampBindings.h"

#endif // DIVERSIA_TESTCLIENT_PREREQUISITES_H