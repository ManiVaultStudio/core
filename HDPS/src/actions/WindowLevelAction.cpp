#include "WindowLevelAction.h"
#include "WidgetActionLabel.h"
#include "Application.h"

#include <QGridLayout>

using namespace hdps;

namespace hdps::gui {

WindowLevelAction::WindowLevelAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _windowAction(this, "Window", 0.0f, 1.0f, 1.0f, 1.0f, 1),
    _levelAction(this, "Level", 0.0f, 1.0f, 0.5f, 0.5f, 1)
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    _windowAction.setPrefix("win: ");
    _levelAction.setPrefix("lvl: ");

    _windowAction.setToolTip("Window");
    _levelAction.setToolTip("Level");
    
    _windowAction.setNumberOfDecimals(2);
    _levelAction.setNumberOfDecimals(2);

    _windowAction.setSingleStep(0.05f);
    _levelAction.setSingleStep(0.05f);

    const auto windowLevelChanged = [this]() {
        emit changed(*this);
    };

    connect(&_windowAction, &DecimalAction::valueChanged, this, windowLevelChanged);
    connect(&_levelAction, &DecimalAction::valueChanged, this, windowLevelChanged);
}

void WindowLevelAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicWindowLevelAction = dynamic_cast<WindowLevelAction*>(publicAction);

    Q_ASSERT(publicWindowLevelAction != nullptr);

    if (publicWindowLevelAction == nullptr)
        return;

    if (recursive) {
        getWindowAction().connectToPublicAction(&publicWindowLevelAction->getWindowAction(), recursive);
        getLevelAction().connectToPublicAction(&publicWindowLevelAction->getLevelAction(), recursive);
    }

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void WindowLevelAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        getWindowAction().disconnectFromPublicAction(recursive);
        getLevelAction().disconnectFromPublicAction(recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

void WindowLevelAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _windowAction.fromParentVariantMap(variantMap);
    _levelAction.fromParentVariantMap(variantMap);
}

QVariantMap WindowLevelAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _windowAction.insertIntoVariantMap(variantMap);
    _levelAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

WindowLevelAction::Widget::Widget(QWidget* parent, WindowLevelAction* windowLevelAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, windowLevelAction),
    _groupAction(this, "Group")
{
    _groupAction.setShowLabels(false);
    _groupAction.addAction(&windowLevelAction->getWindowAction());
    _groupAction.addAction(&windowLevelAction->getLevelAction());

    if (widgetFlags & PopupLayout) {
        auto layout = new QGridLayout();

        layout->addWidget(windowLevelAction->getWindowAction().createLabelWidget(this), 0, 0);
        layout->addWidget(windowLevelAction->getWindowAction().createWidget(this), 0, 1);

        layout->addWidget(windowLevelAction->getLevelAction().createLabelWidget(this), 1, 0);
        layout->addWidget(windowLevelAction->getLevelAction().createWidget(this), 1, 1);

        setPopupLayout(layout);
    } else {
        auto layout = new QVBoxLayout();

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(_groupAction.createWidget(this));

        setLayout(layout);
    }
}

}
