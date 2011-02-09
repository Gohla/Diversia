/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_EDITORAPPLICATION_H
#define DIVERSIA_QTOGREEDITOR_EDITORAPPLICATION_H

#include "Platform/Prerequisites.h"

#include "OgreClient/ApplicationBase.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

typedef std::map< LogLevel, boost::log::basic_record<char> > LogRecords;
typedef boost::log::formatters::fmt_format<char> LogFormatter;

class EditorApplication : public QApplication, public ApplicationBase
{
    Q_OBJECT

public:
    EditorApplication( int argc, char* argv[] );
    ~EditorApplication();
    
    void init();
    void run();
    inline void quitSoon() { QApplication::quit(); }
    inline void quit() { QApplication::quit(); }
    void writeLogRecord( const boost::log::basic_record<char>& rRecord );

private slots:
    void update();

private:
    bool notify( QObject* pObject, QEvent* pEvent );
    void unhandledException( const String& rExceptionString );

    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    LogRecords mLogRecords;
    boost::mutex    logRecordMutex;
    LogFormatter    mLogFormatter;

    QTimer*         mUpdateTimer;
    boost::timer    mTimer;

    boost::scoped_ptr<Logger>           mLogger;
    boost::scoped_ptr<ConfigManager>    mConfigManager;
    boost::scoped_ptr<CrashReporter>    mCrashReporter;
    boost::scoped_ptr<MainWindow>       mMainWindow;
    boost::scoped_ptr<GraphicsManager>  mGraphicsManager;
    boost::scoped_ptr<PhysicsManager>   mPhysicsManager;
    boost::scoped_ptr<AudioManager>     mAudioManager;
    boost::scoped_ptr<CameraManager>    mCameraManager;
    boost::scoped_ptr<ObjectSelection>  mObjectSelection;
    boost::scoped_ptr<GridManager>      mGridManager;
    boost::scoped_ptr<StateMachine>     mStateMachine;

    sigc::signal<void>          mEarlyUpdateSignal;
    sigc::signal<void, Real>    mEarlyFrameSignal;
    sigc::signal<void>          mUpdateSignal;
    sigc::signal<void, Real>    mFrameSignal;
    sigc::signal<void>          mLateUpdateSignal;
    sigc::signal<void, Real>    mLateFrameSignal;

    // Settings
    LogLevel        mLogLevel;
    Real            mUpdateRate;

};

//------------------------------------------------------------------------------

/**
Boost logging sink backend for writing log messages to a Qt text stream.
**/
class TextEditLogger : public boost::log::sinks::basic_formatting_sink_backend<char>
{
public:
    TextEditLogger( LogLevel logLevel ): mLogLevel( logLevel ) {}
    void do_consume( record_type const& rRecord, target_string_type const& rMessage );

    LogLevel mLogLevel;
};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::QtOgreEditor::EditorApplication, 
    &Diversia::QtOgreEditor::Bindings::CampBindings::bindEditorApplication );

#endif // DIVERSIA_QTOGREEDITOR_EDITORAPPLICATION_H