/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_CONNECTDIALOG_H
#define DIVERSIA_QTOGREEDITOR_CONNECTDIALOG_H

#include "Platform/Prerequisites.h"

#include "ui_ConnectDialog.h"

#include "Shared/Communication/ServerInfo.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog( QWidget* pParent = 0, Qt::WFlags flags = 0 );
    ~ConnectDialog();

    /**
    Initializes the connect dialog, loading the settings from last time.
    **/
    void init();
    
private slots:
    void accept();
    void reject();
    void textChanged();

private:
    friend class Bindings::CampBindings;    ///< Allow private access for camp bindings.

    void storeLastInfo();

	Ui::ConnectToServer mUI;

    // Settings
    ServerInfo  mLastServer;
    QString     mLastUsername;
    QString     mLastNickname;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::QtOgreEditor::ConnectDialog, 
    &Diversia::QtOgreEditor::Bindings::CampBindings::bindConnectDialog );

#endif // DIVERSIA_QTOGREEDITOR_CONNECTDIALOG_H