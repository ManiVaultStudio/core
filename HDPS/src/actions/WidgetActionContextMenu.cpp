#include "WidgetActionContextMenu.h"
#include "WidgetAction.h"
#include "models/ActionsFilterModel.h"
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
        hdps::actions().publishPrivateAction(_widgetAction);
    });

    connect(&_disconnectAction, &TriggerAction::triggered, this, [this]() -> void {
        hdps::actions().disconnectPrivateActionFromPublicAction(_widgetAction);
    });

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

        actionsFilterModel->setSourceModel(&hdps::actions().getActionsModel());
        actionsFilterModel->getScopeFilterAction().setSelectedOptions({ "Public" });
        actionsFilterModel->getTypeFilterAction().setString(_widgetAction->getTypeString(true));

        const auto numberOfRows = actionsFilterModel->rowCount();

        for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
            auto publicAction = actionsFilterModel->getAction(rowIndex);

            if (!publicAction)
                continue;

            if (publicAction == _widgetAction->getPublicAction())
                continue;

            auto connectAction = new QAction(publicAction->text());

            connectAction->setToolTip("Connect " + _widgetAction->text() + " to " + publicAction->text());

            connect(connectAction, &QAction::triggered, this, [this, publicAction]() -> void {
                hdps::actions().connectPrivateActionToPublicAction(_widgetAction, publicAction);
            });

            connectMenu->addAction(connectAction);
        }

        connectMenu->setEnabled(!connectMenu->actions().isEmpty());

        addMenu(connectMenu);
    }
}

}
}
