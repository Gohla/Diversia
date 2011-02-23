/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "UI/UndoListView.h"
#include "Model/UndoModel.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

UndoListView::UndoListView( QWidget* pParent /*= 0*/ ):
    QListView( pParent )
{
    QListView::setModel( new UndoModel( *GlobalsBase::mUndoStack, this ) );
}

UndoListView::~UndoListView()
{

}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia