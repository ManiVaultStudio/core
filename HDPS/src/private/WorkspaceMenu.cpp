#include "WorkspaceMenu.h"

#include <Application.h>

#include <util/Icon.h>

#include <QPainter>

using namespace hdps;
using namespace hdps::gui;

WorkspaceMenu::WorkspaceMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _openWorkspaceAction(this, "Open"),
    _saveWorkspaceAction(this, "Save"),
    _saveWorkspaceAsAction(this, "Save as")
{
    setTitle("Workspace");
    setIcon(getIcon());
    setToolTip("Workspace operations");

    _openWorkspaceAction.setShortcut(QKeySequence("Ctrl+Shift+O"));
    _openWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _openWorkspaceAction.setToolTip("Open workspace from disk");

    _saveWorkspaceAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    _saveWorkspaceAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAction.setToolTip("Save workspace to disk");

    _saveWorkspaceAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveWorkspaceAsAction.setToolTip("Save workspace under a new file to disk");

    addAction(&_openWorkspaceAction);
    addAction(&_saveWorkspaceAction);
    addAction(&_saveWorkspaceAsAction);

    connect(&_openWorkspaceAction, &QAction::triggered, [this](bool) {
        //Application::current()->getWorkspaceManager().open();
    });

    connect(&_saveWorkspaceAction, &QAction::triggered, [this](bool) {
        //Application::current()->getWorkspaceManager().save();
    });

    connect(&_saveWorkspaceAsAction, &QAction::triggered, [this](bool) {
        //Application::current()->getWorkspaceManager().saveAs();
    });
}

QIcon WorkspaceMenu::getIcon()
{
    const auto size = 128;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, 128, 128);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(QRectF(QPointF(16, 16), QPointF(55, 100)));

    //painter.setBrush(Qt::black);
    //painter.setPen(QPen(Qt::black, lineThickness));
    //painter.drawRect(rect);

    return createIcon(pixmap);
}
