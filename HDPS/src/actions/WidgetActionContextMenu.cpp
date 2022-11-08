#include "WidgetActionContextMenu.h"
#include "WidgetAction.h"
#include "ActionsFilterModel.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>

namespace hdps {

namespace gui {

WidgetActionContextMenu::WidgetActionContextMenu(QWidget* parent, WidgetAction* widgetAction) :
    QMenu(parent),
    _widgetAction(widgetAction),
    _publishAction(this, "Publish..."),
    _disconnectAction(this, "Disconnect...")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _publishAction.setIcon(fontAwesome.getIcon("cloud-upload-alt"));
    _disconnectAction.setIcon(fontAwesome.getIcon("unlink"));

    const auto updatePublishAction = [this]() -> void {
        _publishAction.setEnabled(!_widgetAction->isPublished());
    };

    connect(_widgetAction, &WidgetAction::isPublishedChanged, this, updatePublishAction);

    connect(&_publishAction, &TriggerAction::triggered, this, [this]() -> void {
        _widgetAction->publish();
    });

    connect(&_disconnectAction, &TriggerAction::triggered, _widgetAction, &WidgetAction::disconnectFromPublicAction);

    updatePublishAction();

    initialize();
}

void WidgetActionContextMenu::initialize()
{
    if (_widgetAction->mayPublish(WidgetAction::Gui) && !_widgetAction->isPublic())
        addAction(&_publishAction);

    if (!actions().isEmpty())
        addSeparator();

    if (_widgetAction->mayDisconnect(WidgetAction::Gui) && _widgetAction->isConnected())
        addAction(&_disconnectAction);

    if (_widgetAction->mayConnect(WidgetAction::Gui)) {
        auto connectMenu = new QMenu("Connect to:");

        connectMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("link"));

        auto actionsFilterModel = new ActionsFilterModel(this);

        actionsFilterModel->setScopeFilter(ActionsFilterModel::Public);
        actionsFilterModel->setTypeFilter(_widgetAction->getTypeString());

        const auto numberOfRows = actionsFilterModel->rowCount();

        for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
            const auto publicAction = static_cast<WidgetAction*>(actionsFilterModel->mapToSource(actionsFilterModel->index(rowIndex, 0)).internalPointer());

            auto connectAction = new QAction(publicAction->text());

            connectAction->setToolTip("Connect " + _widgetAction->text() + " to " + publicAction->text());

            connect(connectAction, &QAction::triggered, this, [this, publicAction]() -> void {
                _widgetAction->connectToPublicAction(publicAction);
            });

            connectMenu->addAction(connectAction);
        }

        connectMenu->setEnabled(!connectMenu->actions().isEmpty());

        addMenu(connectMenu);
    }
}

}
}
