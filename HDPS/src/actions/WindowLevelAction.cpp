#include "WindowLevelAction.h"
#include "WidgetActionLabel.h"
#include "Application.h"

#include <QGridLayout>

using namespace hdps;

WindowLevelAction::WindowLevelAction(QObject* parent) :
    WidgetAction(parent, "Window Level"),
    _windowAction(this, "Window", 0.0f, 1.0f, 1.0f, 1.0f, 1),
    _levelAction(this, "Level", 0.0f, 1.0f, 0.5f, 0.5f, 1)
{
    setText("Window/level Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    // Set tooltips
    _windowAction.setToolTip("Window");
    _levelAction.setToolTip("Level");
    
    // Set decimals
    _windowAction.setNumberOfDecimals(2);
    _levelAction.setNumberOfDecimals(2);

    // Set single step
    _windowAction.setSingleStep(0.05f);
    _levelAction.setSingleStep(0.05f);

    const auto windowLevelChanged = [this]() {
        emit changed(*this);
    };

    connect(&_windowAction, &DecimalAction::valueChanged, this, windowLevelChanged);
    connect(&_levelAction, &DecimalAction::valueChanged, this, windowLevelChanged);
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
