/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Plugin/ClientPluginManager.h"
#include "Client/Communication/GridManager.h"
#include "Client/Communication/ServerAbstract.h"
#include "Client/Object/ClientObjectManager.h"
#include "Client/Object/ClientObjectTemplateManager.h"
#include "Client/Permission/PermissionManager.h"
#include "OgreClient/Resource/ResourceManager.h"
#include "State/LoadingState.h"
#include "UI/NewGameDialog.h"
#include "Util/State/StateMachine.h"
#include <QFileDialog>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

NewGameDialog::NewGameDialog( QWidget* pParent, Qt::WFlags flags ):
    QDialog( pParent, flags )
{
    mUI.setupUi( this );
}

NewGameDialog::~NewGameDialog()
{

}

void NewGameDialog::accept()
{
    EditorGlobals::mGrid->createOfflineServer();
    ClientPluginManager& pluginManager = EditorGlobals::mGrid->getActiveServer().getPluginManager();
    ResourceManager& resourceManager = pluginManager.getPlugin<ResourceManager>();
    resourceManager.set( "ResourceLocation", mUI.editResourceLocation->text() );
    resourceManager.set( "Type", mUI.comboResourceType->currentText() );
    EditorGlobals::mCurrentGame = mUI.editGameFile->text();
    EditorGlobals::mOffline = true;

    QDialog::setVisible( false );

    mUI.editResourceLocation->clear();
    mUI.editGameFile->clear();
    mUI.comboResourceType->clear();

    EditorGlobals::mState->pushState( new LoadingState() );
}

void NewGameDialog::reject()
{
    QDialog::setVisible( false );
}

void NewGameDialog::browseGameFile()
{
    mUI.editGameFile->setText( QFileDialog::getSaveFileName( this, "Game file", "", 
        tr( "Game files (*.xml)" ) ) );
}

void NewGameDialog::browseResources()
{
    mUI.editResourceLocation->setText( QFileDialog::getExistingDirectory( this, 
        "Resource directory" ) );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia
