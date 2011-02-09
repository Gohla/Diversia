/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_CAMPBINDINGS_H
#define DIVERSIA_QTOGREEDITOR_CAMPBINDINGS_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
namespace Bindings
{
//------------------------------------------------------------------------------

class CampBindings
{
public:
    static void bindEditorApplication();
    static void bindConnectDialog();

};

//------------------------------------------------------------------------------
} // Namespace Bindings
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_CAMPBINDINGS_H