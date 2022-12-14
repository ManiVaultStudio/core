#include "WorkspaceManager.h"

#include <Application.h>

#include <util/Exception.h>
#include <util/Icon.h>

#include <QMenu>
#include <QPainter>

#ifdef _DEBUG
    #define WORKSPACE_MANAGER_VERBOSE
#endif

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

WorkspaceManager::WorkspaceManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _workspace(nullptr),
    _loadWorkspaceAction(this, "Load"),
    _saveWorkspaceAction(this, "Save"),
    _saveWorkspaceAsAction(this, "Save As..."),
    _icon()
{
    _loadWorkspaceAction.setShortcut(QKeySequence("Ctrl+Shift+O"));
    _loadWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _loadWorkspaceAction.setToolTip("Open workspace from disk");

    _saveWorkspaceAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    _saveWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAction.setToolTip("Save workspace to disk");

    _saveWorkspaceAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAsAction.setToolTip("Save workspace under a new file to disk");

    connect(&_loadWorkspaceAction, &QAction::triggered, [this](bool) {
        loadWorkspace();
    });

    connect(&_saveWorkspaceAction, &QAction::triggered, [this](bool) {
        saveWorkspace(_workspace->getFilePath());
    });

    connect(&_saveWorkspaceAsAction, &QAction::triggered, [this](bool) {
        saveWorkspaceAs();
    });

    createIcon();
}

void WorkspaceManager::loadWorkspace(QString filePath /*= ""*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        emit workspaceAboutToBeLoaded(*(_workspace.get()));
        {
            if (!_workspace.isNull()) {
                const auto workspaceId = _workspace->getId();

                emit workspaceAboutToBeDestroyed(*(_workspace.get()));
                {
                    createWorkspace();
                }
                emit workspaceDestroyed(workspaceId);
            }
            else {
                createWorkspace();
            }
        }
        emit workspaceLoaded(*(_workspace.get()));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load workspace");
    }
}

void WorkspaceManager::saveWorkspace(QString filePath /*= ""*/)
{
    try
    {
#ifdef WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        emit workspaceSaved(*(_workspace.get()));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save workspace", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save workspace");
    }
}

void WorkspaceManager::saveWorkspaceAs()
{
#ifdef WORKSPACE_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_workspace.isNull())
        return;

}

void WorkspaceManager::createWorkspace()
{
    _workspace.reset(new Workspace(this));

    emit workspaceCreated(*(_workspace.get()));
}

QMenu* WorkspaceManager::getMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Workspace", parent);

    menu->setTitle("Workspace");
    menu->setIcon(_icon);
    menu->setToolTip("Workspace operations");

    menu->addAction(&_loadWorkspaceAction);
    menu->addAction(&_saveWorkspaceAction);
    menu->addAction(&_saveWorkspaceAsAction);

    return menu;
}

void WorkspaceManager::createIcon()
{
    const auto size             = 128;
    const auto halfSize         = size / 2;
    const auto margin           = 12;
    const auto spacing          = 14;
    const auto halfSpacing      = spacing / 2;
    const auto lineThickness    = 7.0;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, size, size);

    const auto drawWindow = [&](QRectF rectangle) -> void {
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rectangle);
    };

    drawWindow(QRectF(QPointF(margin, margin), QPointF(halfSize - halfSpacing, size - margin)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, margin), QPointF(size - margin, halfSize - halfSpacing)));
    drawWindow(QRectF(QPointF(halfSize + halfSpacing, halfSize + halfSpacing), QPointF(size - margin, size - margin)));

    _icon = hdps::gui::createIcon(pixmap);
}
