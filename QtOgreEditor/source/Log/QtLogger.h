/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_QTLOGGER_H
#define DIVERSIA_QTOGREEDITOR_QTLOGGER_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

typedef boost::log::formatters::fmt_format<char> LogFormatter;

struct LogMessage 
{
    LogMessage( boost::log::basic_record<char> const& rRecord );

    String      mMessage;
    String      mChannel;
    LogLevel    mLogLevel;

    static LogFormatter msLogFormatter;
};

//------------------------------------------------------------------------------

typedef std::vector<LogMessage> LogRecords;

class QtLogger : public boost::log::sinks::basic_formatting_sink_backend<char>
{
public:
    QtLogger();
    
private:
    void do_consume( record_type const& rRecord, target_string_type const& rMessage );
    void update();

    LogRecords      mLogRecords;
    boost::mutex    mLogRecordMutex;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_QTLOGGER_H