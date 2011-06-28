/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_PREREQUISITES_H
#define DIVERSIA_QTOGREEDITOR_PREREQUISITES_H

// Platform-specific stuff
#include "Util/Build/Platform.h"

// Include dependent libraries
#include "Util/Platform/Prerequisites.h"
#include "Object/Platform/Prerequisites.h"
#include "Shared/Platform/Prerequisites.h"
#include "Client/Platform/Prerequisites.h"
#include "OgreClient/Platform/Prerequisites.h"

// Logging
#include "Util/Log/Log.h"

namespace Diversia
{
namespace QtOgreEditor
{
// Used namespaces
using namespace Diversia::Util;
using namespace Diversia::ObjectSystem;
using namespace Diversia;
using namespace Diversia::Client;
using namespace Diversia::OgreClient;

namespace Log
{
// Static logger
static boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel > lg
(
    boost::log::keywords::severity = Diversia::Util::LOG_INFO,
    boost::log::keywords::channel = "QtOgreEditor"
);

// Easy logging defines.
#define LOG(severity) BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, severity)
#define LOGC BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, Diversia::Util::LOG_CRITICAL)
#define LOGE BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, Diversia::Util::LOG_ERROR)
#define LOGW BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, Diversia::Util::LOG_WARNING)
#define LOGI BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, Diversia::Util::LOG_INFO)
#define LOGD BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, Diversia::Util::LOG_DEBUG)
#define LOGDE BOOST_LOG_SEV(Diversia::QtOgreEditor::Log::lg, Diversia::Util::LOG_ENTRYEXIT)
} // Namespace Log
} // Namespace QTOgreEditor
} // Namespace Diversia

// Definitions
#include "EditorGlobals.h"
#include "Camp/CampBindings.h"

#endif // DIVERSIA_QTOGREEDITOR_PREREQUISITES_H