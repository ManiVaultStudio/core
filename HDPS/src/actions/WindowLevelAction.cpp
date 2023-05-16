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
    setText("Window/level Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

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
    if (recursive) {
        getWindowAction().disconnectFromPublicAction(recursive);
        getLevelAction().disconnectFromPublicAction(recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

WindowLevelAction::Widget::Widget(QWidget* parent, WindowLevelAction* windowLevelAction) :
    WidgetActionWidget(parent, windowLevelAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(windowLevelAction->getWindowAction().createLabelWidget(this), 0, 0);
    layout->addWidget(windowLevelAction->getWindowAction().createWidget(this), 0, 1);

    layout->addWidget(windowLevelAction->getLevelAction().createLabelWidget(this), 1, 0);
    layout->addWidget(windowLevelAction->getLevelAction().createWidget(this), 1, 1);

    setPopupLayout(layout);
}

}
