/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_NEWGAMEDIALOG_H
#define DIVERSIA_QTOGREEDITOR_NEWGAMEDIALOG_H

#include "Platform/Prerequisites.h"

#include "ui_NewGameDialog.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    NewGameDialog( QWidget* pParent = 0, Qt::WFlags flags = 0 );
    virtual ~NewGameDialog();

private slots:
    void accept();
    void reject();
    void browseGameFile();
    void browseResources();

private:
    Ui::NewGame mUI;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_NEWGAMEDIALOG_H