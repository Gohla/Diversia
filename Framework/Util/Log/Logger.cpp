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

#include "Util/Platform/StableHeaders.h"

#include "Util/Log/Logger.h"
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>

namespace Diversia
{
namespace Util 
{
//------------------------------------------------------------------------------

Logger::Logger( Util::LogLevel logLevel, bool consoleLog /*= true*/, bool fileLog /*= true*/ )
{
    boost::log::add_common_attributes();
    boost::shared_ptr< boost::log::core > pCore = boost::log::core::get();
    pCore->add_global_attribute(
        "Scope", boost::make_shared< boost::log::attributes::named_scope >());

    if( consoleLog)
        boost::log::init_log_to_console
        (
            std::cout, 
            boost::log::keywords::filter = 
            boost::log::filters::attr< LogLevel >( "Severity" ) >= logLevel,
            boost::log::keywords::format = boost::log::formatters::format( "[%1%] [%2%] <%3%> %4%" )
            % boost::log::formatters::date_time( "TimeStamp", "%d.%m-%H:%M:%S.%f" )
            % boost::log::formatters::attr< String >( "Channel" )
            % boost::log::formatters::attr< LogLevel >( "Severity" )
            % boost::log::formatters::message()
        );

    if( fileLog )
        boost::log::init_log_to_file
        (
            boost::log::keywords::file_name = "Log_%d.%m.%y.log",
            boost::log::keywords::open_mode = std::ios::app,
            boost::log::keywords::filter = 
            boost::log::filters::attr< LogLevel >( "Severity" ) >= logLevel,
            boost::log::keywords::format = boost::log::formatters::format( "[%1%] [%2%] <%3%> %4%" )
            % boost::log::formatters::date_time( "TimeStamp", "%H:%M:%S.%f" )
            % boost::log::formatters::attr< String >( "Channel" )
            % boost::log::formatters::attr< LogLevel >( "Severity" )
            % boost::log::formatters::message()
        );
}

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia