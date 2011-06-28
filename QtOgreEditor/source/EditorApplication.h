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

class EditorApplication : public QApplication, public ApplicationBase
{
    Q_OBJECT

public:
    EditorApplication( int argc, char* argv[] );
    ~EditorApplication();
    
    void init( int argc, char* argv[] );
    void run();
    inline void quitSoon() { QApplication::quit(); }
    inline void quit() { QApplication::quit(); }
    inline void stopUpdates( bool stop = true ) { mStopUpdates = stop; }

private slots:
    void update();

private:
    bool notify( QObject* pObject, QEvent* pEvent );
    void unhandledException( const String& rExceptionString );

    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    QTimer*                         mUpdateTimer;
    boost::scoped_ptr<boost::timer> mTimer;
    bool                            mStopUpdates;

    boost::scoped_ptr<Logger>           mLogger;
    boost::scoped_ptr<ConfigManager>    mConfigManager;
    boost::scoped_ptr<CrashReporter>    mCrashReporter;
    boost::scoped_ptr<MainWindow>       mMainWindow;
    boost::shared_ptr<QtLogger>         mQtLogger;
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

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::QtOgreEditor::EditorApplication, 
    &Diversia::QtOgreEditor::Bindings::CampBindings::bindEditorApplication );

#endif // DIVERSIA_QTOGREEDITOR_EDITORAPPLICATION_H