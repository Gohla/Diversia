/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Log/QtLogger.h"
#include "UI/MainWindow.h"
#include <QListWidgetItem>
#include <boost/lambda/lambda.hpp>

namespace Diversia
{
namespace QtOgreEditor 
{

//------------------------------------------------------------------------------
LogFormatter LogMessage::msLogFormatter = boost::log::formatters::format( "[%1%] [%2%] %3%" )
    % boost::log::formatters::date_time( "TimeStamp", "%H:%M:%S" )
    % boost::log::formatters::attr< String >( "Channel" )
    % boost::log::formatters::message();

LogMessage::LogMessage( boost::log::basic_record<char> const& rRecord )
{
    // Format log message
    StringStream ss;
    msLogFormatter( ss, rRecord );
    mMessage = ss.str();

    // Get attributes
    boost::log::extract<String>( "Channel", rRecord.attribute_values(), 
        boost::lambda::var( mChannel ) = boost::lambda::_1 );
    boost::log::extract<LogLevel>( "Severity", rRecord.attribute_values(), 
        boost::lambda::var( mLogLevel ) = boost::lambda::_1 );
}

//------------------------------------------------------------------------------

QtLogger::QtLogger():
    boost::log::sinks::basic_formatting_sink_backend<char>()
{
    GlobalsBase::mLateUpdateSignal->connect( sigc::mem_fun( this, &QtLogger::update ) );
}

void QtLogger::do_consume( record_type const& rRecord, target_string_type const& rMessage )
{
    boost::mutex::scoped_lock lock( mLogRecordMutex );
    mLogRecords.push_back( LogMessage( rRecord ) );
} 

void QtLogger::update()
{
    if( !mLogRecords.size() ) return;
    boost::mutex::scoped_lock lock( mLogRecordMutex );
    QListWidgetItem* item = 0;
    for( LogRecords::const_iterator i = mLogRecords.begin();i != mLogRecords.end(); ++i )
    {
        const LogMessage& entry = *i;

        // Send log message to log list widget.
        item = new QListWidgetItem( QString::fromStdString( entry.mMessage ), 
            EditorGlobals::mMainWindow->mUI.logListWidget );
        item->setData( Qt::UserRole, entry.mLogLevel ); // Log level
        item->setData( Qt::UserRole + 1, QString::fromStdString( entry.mChannel ) ); // Log channel
        item->setData( Qt::UserRole + 2, EditorGlobals::mMainWindow->isSeverityChecked( 
            entry.mLogLevel ) ); // Shown because of severity?
        item->setData( Qt::UserRole + 3, EditorGlobals::mMainWindow->isSourceChecked( 
            entry.mChannel ) ); // Shown because of source?
        EditorGlobals::mMainWindow->checkLogItem( 
            EditorGlobals::mMainWindow->mUI.logListWidget->model()->index( 
            EditorGlobals::mMainWindow->mUI.logListWidget->row( item ), 0 ) );

        // Select color
        switch( entry.mLogLevel )
        {
            case LOG_CRITICAL: item->setForeground( QBrush( Qt::darkRed ) ); break;
            case LOG_ERROR: item->setForeground( QBrush( Qt::red ) ); break;
            case LOG_WARNING: item->setForeground( QBrush( Qt::darkYellow ) ); break;
            case LOG_INFO: item->setForeground( QBrush( Qt::black ) ); break;
            case LOG_DEBUG: item->setForeground( QBrush( Qt::darkGray ) ); break;
            case LOG_ENTRYEXIT: item->setForeground( QBrush( Qt::lightGray ) ); break;
        }

        // Select icon
        switch( entry.mLogLevel )
        {
            case LOG_CRITICAL: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-error.png" ) ); break;
            case LOG_ERROR: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-error.png" ) ); break;
            case LOG_WARNING: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-warning.png" ) ); break;
            case LOG_INFO: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-info.png" ) ); break;
            case LOG_DEBUG: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-question.png" ) );break;
            case LOG_ENTRYEXIT: item->setIcon( QIcon( ":/Icons/Icons/status/dialog-question.png" ) ); break;
        }

        // Send errors to status bar.
        if( entry.mLogLevel >= LOG_ERROR )
        {
            QPalette palette; palette.setColor( QPalette::WindowText, Qt::red );
            EditorGlobals::mMainWindow->mUI.statusBar->setPalette( palette );
            EditorGlobals::mMainWindow->mUI.statusBar->showMessage( 
                QString::fromStdString( entry.mMessage ), 8000 );
        }
    }

    // Scroll to last item
    if( item ) EditorGlobals::mMainWindow->mUI.logListWidget->scrollToItem( item, 
        QAbstractItemView::PositionAtBottom );
    mLogRecords.clear();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia