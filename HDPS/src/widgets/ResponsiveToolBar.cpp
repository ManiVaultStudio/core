#include "ResponsiveToolBar.h"
#include "WidgetState.h"

#include "../util/Timer.h"

#include <QDebug>
#include <QGridLayout>
#include <QFrame>
#include <QEvent>

#include <algorithm>

namespace hdps {

namespace gui {

ResponsiveToolBar::Spacer::Spacer(const Type& type /*= State::Divider*/) :
    QWidget(),
    _type(Type::Divider),
    _layout(new QHBoxLayout()),
    _verticalLine(new QFrame())
{
    _verticalLine->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _verticalLine->setFrameShape(QFrame::VLine);
    _verticalLine->setFrameShadow(QFrame::Sunken);

    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_verticalLine);

    setType(type);
}

ResponsiveToolBar::Spacer::Type ResponsiveToolBar::Spacer::getType(const ResponsiveSectionWidget::State& stateBefore, const ResponsiveSectionWidget::State& stateAfter)
{
    return stateBefore == ResponsiveSectionWidget::State::Collapsed && stateAfter == ResponsiveSectionWidget::State::Collapsed ? Spacer::Type::Spacer : Spacer::Type::Divider;
}

ResponsiveToolBar::Spacer::Type ResponsiveToolBar::Spacer::getType(const ResponsiveSectionWidget* sectionBefore, const ResponsiveSectionWidget* sectionAfter)
{
    return getType(sectionBefore->getState(), sectionAfter->getState());
}

void ResponsiveToolBar::Spacer::setType(const Type& type)
{
    _type = type;

    setLayout(_layout);
    setFixedWidth(getWidth(_type));
    
    _verticalLine->setVisible(_type == Type::Divider ? true : false);
}

std::int32_t ResponsiveToolBar::Spacer::getWidth(const Type& type)
{
    switch (type)
    {
        case Type::Divider:
            return 20;

        case Type::Spacer:
            return 8;

        default:
            break;
    }

    return 0;
}

ResponsiveToolBar::SectionsWidget::SectionsWidget(QWidget* parent /*= nullptr*/) :
    _layout(new QHBoxLayout())
{
    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(_layout);
}

void ResponsiveToolBar::SectionsWidget::addSectionWidget(QWidget* widget)
{
    _layout->addWidget(widget);
}

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 4;

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _layout(new QHBoxLayout()),
    _sectionsWidget(new SectionsWidget()),
    _sectionWidgets(),
    _spacers(),
    _dirty(false)
{
    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_sectionsWidget);
    _layout->addStretch(1);

    setLayout(_layout);

    this->installEventFilter(this);
}

bool ResponsiveToolBar::eventFilter(QObject* object, QEvent* event)
{
    const auto widget = dynamic_cast<QWidget*>(object);

    if (widget == nullptr)
        return QObject::eventFilter(object, event);

    switch (event->type()) {
        case QEvent::Resize:
            computeLayout(dynamic_cast<ResponsiveSectionWidget*>(widget));
            break;

        default:
            break;
    }
    
    return QObject::eventFilter(object, event);
}

void ResponsiveToolBar::computeLayout(ResponsiveSectionWidget* resizedSectionWidget /*= nullptr*/)
{
    return;

    // Don't compute the layout if the resized widget is on the ignore list
    /*
    if (_ignoreSectionWidgets.contains(resizedSectionWidget)) {
        _ignoreSectionWidgets.removeOne(resizedSectionWidget);
        return;
    }
    */

    if (resizedSectionWidget)
        return;

    Timer timer("Compute layout");

    if (resizedSectionWidget)
        qDebug() << resizedSectionWidget->getName() << "resized";

    const auto availableWidth = width();
    
    QMap<ResponsiveSectionWidget*, ResponsiveSectionWidget::State> states;
    
    /*
    const auto printSectionWidgets = [&states]() {
        for (auto sectionWidget : states.keys())
            if (sectionWidget->getName() == "Render mode" || sectionWidget->getName() == "Plot")
                qDebug() << sectionWidget->getName() << ResponsiveSectionWidget::getStateName(states[sectionWidget]) << sectionWidget->getSizeHints().values();
    };
    */
    
    /**/

    // Start collapsed
    for (auto sectionWidget : _sectionWidgets)
        states[sectionWidget] = ResponsiveSectionWidget::State::Collapsed;
    

    // Initialize the state for the ignored section (if any)
    //if (resizedSectionWidget != nullptr)
        //states[resizedSectionWidget] = resizedSectionWidget->getState();

    //_ignoreSectionWidgets = _sectionWidgets;

    //_ignoreSectionWidgets.removeOne(resizedSectionWidget);

    const auto getOccupiedWidth = [this, &states]() -> std::int32_t {
        auto occupiedWidth = 0;

        for (auto sectionWidget : _sectionWidgets)
            occupiedWidth += sectionWidget->getSizeHintForState(states[sectionWidget]).width();

        for (auto spacer : _spacers) {
            const auto spacerIndex  = _spacers.indexOf(spacer);
            const auto spacerType   = Spacer::getType(states[_sectionWidgets[spacerIndex]], states[_sectionWidgets[spacerIndex + 1]]);
            
            occupiedWidth += Spacer::getWidth(spacerType);
        }

        return occupiedWidth;
    };

    auto sortedSectionWidgets = _sectionWidgets;

    // Sort section widgets based on priority
    std::sort(sortedSectionWidgets.begin(), sortedSectionWidgets.end(), [](ResponsiveSectionWidget* left, ResponsiveSectionWidget* right) {
        return left->getPriority() > right->getPriority();
    });

    // Establish state for each section widget
    for (auto sectionWidget : sortedSectionWidgets + sortedSectionWidgets) {
        const auto oldWidgetStates  = states;

        states[sectionWidget] = ResponsiveSectionWidget::grow(states[sectionWidget]);

        const auto occupiedWidth = getOccupiedWidth();

        if (occupiedWidth >= availableWidth) {
            states = oldWidgetStates;
            break;
        }
    }

    /*
    for (auto sectionWidget : _sectionWidgets)
        if (sectionWidget != resizedSectionWidget)
            _ignoreSectionWidgets << sectionWidget;
    */

    auto updateSectionWidgets = _sectionWidgets;

    //updateSectionWidgets.removeOne(resizedSectionWidget);

    for (auto sectionWidget : _sectionWidgets)
        sectionWidget->setState(states[sectionWidget]);

    for (auto spacer : _spacers) {
        const auto spacerIndex  = _spacers.indexOf(spacer);
        const auto spacerType   = Spacer::getType(_sectionWidgets[spacerIndex], _sectionWidgets[spacerIndex + 1]);

        spacer->setType(spacerType);
    }

    _dirty = false;
}

}
}