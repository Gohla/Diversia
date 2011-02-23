/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_UNDOLISTVIEW_H
#define DIVERSIA_QTOGREEDITOR_UNDOLISTVIEW_H

#include "Platform/Prerequisites.h"
#include <QListView>

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class UndoListView : public QListView, public sigc::trackable
{
    Q_OBJECT

public:
    UndoListView( QWidget* pParent = 0 );
    ~UndoListView();
    
private:

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_UNDOLISTVIEW_H