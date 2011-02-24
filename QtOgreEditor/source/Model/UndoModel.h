/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_UNDOMODEL_H
#define DIVERSIA_QTOGREEDITOR_UNDOMODEL_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class UndoItem : public QStandardItem
{
public:
    UndoItem( UndoCommand* pUndoCommand );
    ~UndoItem();

    void setCurrent( bool current );

private:
    UndoCommand* mUndoCommand;

};

//------------------------------------------------------------------------------

typedef std::map<UndoCommand*, UndoItem*> UndoItems;

class UndoModel : public QStandardItemModel, public sigc::trackable
{
    Q_OBJECT

public:
    UndoModel( UndoStack& rUndoStack, QObject* pParent = 0 );
    ~UndoModel();
    
private:
    void undoCommandChange( UndoCommand* pUndoCommand, bool created );
    void currentCommandChanged( UndoCommand* pUndoCommand );

    UndoStack&      mUndoStack;
    UndoItems       mUndoItems;
    UndoItem*       mCurrentItem;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_UNDOMODEL_H