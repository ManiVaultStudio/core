#include "ResponsiveToolBar.h"
#include "WidgetState.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QFrame>
#include <QEvent>

#include <algorithm>

namespace hdps {

namespace gui {

ResponsiveToolBar::QSpacer::QSpacer(QWidget* left, QWidget* right) :
    QWidget(),
    _left(left),
    _right(right),
    _layout(new QHBoxLayout()),
    _verticalLine(new QFrame())
{
    _left->installEventFilter(this);
    _right->installEventFilter(this);

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
    //setFixedWidth(50);
    setLayout(_layout);

    _verticalLine->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _verticalLine->setFrameShape(QFrame::VLine);
    _verticalLine->setFrameShadow(QFrame::Sunken);

    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_verticalLine);
}

bool ResponsiveToolBar::QSpacer::eventFilter(QObject* target, QEvent* event)
{
    const auto widget = dynamic_cast<QWidget*>(target);

    if (widget) {
        switch (event->type()) {
            case QEvent::Hide:
            case QEvent::Show:
                //setVisible(_left->isVisible() && _right->isVisible());
                break;

            case QEvent::DynamicPropertyChange:
                //updateLayout(dynamic_cast<QWidget*>(target));
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(target, event);
}

void ResponsiveToolBar::QSpacer::updateState()
{
    /*
    const auto getState = [this](QWidget* widget) -> std::int32_t {
        const auto stateProperty = widget->property("state");

        if (stateProperty.isValid())
            return stateProperty.toInt();

        return 0;
    };

    setVisible(_left->isVisible() && _right->isVisible());
    */
}

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 5;

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _listenWidget(nullptr),
    _layout(new QHBoxLayout()),
    _sectionWidgets(),
    _modified(-1)
{
    setAutoFillBackground(true);
    setObjectName("ResponsiveToolBar");
    setStyleSheet("QWidget#ResponsiveToolBar { background-color:red; }");

    _layout->setMargin(5);
    //_layout->setSpacing(0);
    _layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(_layout);
}

void ResponsiveToolBar::setListenWidget(QWidget* listenWidget)
{
    Q_ASSERT(listenWidget != nullptr);

    _listenWidget = listenWidget;

    _listenWidget->installEventFilter(this);
}

bool ResponsiveToolBar::eventFilter(QObject* object, QEvent* event)
{
    const auto widget = dynamic_cast<QWidget*>(object);

    if (widget == nullptr)
        return QObject::eventFilter(object, event);

    switch (event->type()) {
        case QEvent::Resize:
            computeLayout(dynamic_cast<SectionWidget*>(widget));
            break;

        default:
            break;
    }

    return QObject::eventFilter(object, event);
}

void ResponsiveToolBar::addSection(StatefulWidget* statefulWidget, const QIcon& icon /*= QIcon()*/, const std::int32_t& priority /*= 0*/)
{
    Q_ASSERT(statefulWidget != nullptr);

    /*
    if (!_widgets.isEmpty()) {
        auto spacer = new QSpacer(_widgets.last(), widget);
        _layout->addWidget(spacer);
        _widgets << spacer;
    }
    */

    auto sectionWidget = new SectionWidget(statefulWidget, icon, priority);

    _sectionWidgets << sectionWidget;

    //sectionWidget->installEventFilter(this);

    _layout->addWidget(sectionWidget);

    /*
    const auto getPriority = [this](QWidget* widget) -> std::int32_t {
        const auto priorityProperty = widget->property("priority");

        if (priorityProperty.isValid())
            return priorityProperty.toInt();

        return 0;
    };

    std::sort(_sectionWidgets.begin(), _sectionWidgets.end(), [getPriority](QWidget* left, QWidget* right) {
        return getPriority(left) > getPriority(right);
    });
    */
}

void ResponsiveToolBar::addStretch(const std::int32_t& stretch /*= 0*/)
{
    _layout->addStretch(stretch);
}

/*
const auto printWidgets = [this](QList<QWidget*> widgets) {
    for (auto widget : widgets)
        qDebug() << widget->windowTitle();
};
*/

void ResponsiveToolBar::computeLayout(SectionWidget* sectionWidget /*= nullptr*/)
{
    Q_ASSERT(_listenWidget != nullptr);
    
    qDebug() << "Compute layout" << sectionWidget;

    if (sectionWidget)
        _modified = sectionWidget->getModified();
    else
        _modified++;

    auto sortedSectionWidgets = _sectionWidgets;

    std::sort(sortedSectionWidgets.begin(), sortedSectionWidgets.end(), [](SectionWidget* left, SectionWidget* right) {
        return left->getPriority() > right->getPriority();
    });

    const auto marginTotal  = 2 * _layout->margin();
    const auto spacingTotal = std::max(sortedSectionWidgets.count() - 1, 0) * _layout->spacing();

    auto availableWidth = _listenWidget->rect().width() - marginTotal;

    QMap<SectionWidget*, std::int32_t> widgetStates;

    const auto getSectionsWidth = [this, sortedSectionWidgets, spacingTotal, &widgetStates]() -> std::int32_t {
        auto sectionsWidth = spacingTotal;

        for (auto sectionWidget : sortedSectionWidgets)
            sectionsWidth += sectionWidget->getWidth(static_cast<WidgetState>(widgetStates[sectionWidget]));

        return sectionsWidth;
    };

    for (int targetState = 0; targetState < 2; targetState++) {
        for (auto sectionWidget : sortedSectionWidgets) {
            const auto oldWidgetStates = widgetStates;

            widgetStates[sectionWidget]++;

            const auto sectionsWidth = getSectionsWidth();

            if (sectionsWidth > availableWidth) {
                widgetStates = oldWidgetStates;
                break;
            }
        }
    }

    qDebug() << availableWidth << width() << getSectionsWidth();;

    for (auto sectionWidget : _sectionWidgets)
        sectionWidget->setState(static_cast<WidgetState>(widgetStates[sectionWidget]));
}

QList<QWidget*> ResponsiveToolBar::getVisibleWidgets()
{
    QList<QWidget*> visibleWidgets;

    for (auto widget : _sectionWidgets) {
        if (widget->isHidden())
            continue;

        visibleWidgets << widget;
    }

    return visibleWidgets;
}

}
}