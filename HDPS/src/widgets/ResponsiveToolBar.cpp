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

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 5;

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _listenWidget(nullptr),
    _layout(new QGridLayout()),
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
    /**/
    const auto widget = dynamic_cast<QWidget*>(object);

    if (widget == nullptr)
        return QObject::eventFilter(object, event);

    switch (event->type()) {
        case QEvent::Resize:
        {
            if (widget == _listenWidget)
                computeLayout();
            else
                _layout->invalidate();

            break;
        }

        default:
            break;
    }
    
    
    return QObject::eventFilter(object, event);
}

void ResponsiveToolBar::computeLayout(QWidget* resizedWidget /*= nullptr*/)
{
    Q_ASSERT(_listenWidget != nullptr);

    Timer timer("Compute layout");

    /*
    if (resizedSectionWidget)
        qDebug() << QString("%1 resized").arg(resizedSectionWidget->getName());
    else
        qDebug() << "Listen widget has resized";

    if (_ignoreSectionWidgets.contains(resizedSectionWidget)) {
        _ignoreSectionWidgets.removeOne(resizedSectionWidget);
        return;
    }

    for (auto sectionWidget : _sectionWidgets) {
        if (sectionWidget != resizedSectionWidget)
            _ignoreSectionWidgets << sectionWidget;
    }
    

    //qDebug() << sectionWidget->getName() << sectionWidget->getModified();
    */

    auto sortedSectionWidgets = _sectionWidgets;

    std::sort(sortedSectionWidgets.begin(), sortedSectionWidgets.end(), [](SectionWidget* left, SectionWidget* right) {
        return left->getPriority() > right->getPriority();
    });
    
    
    const auto marginTotal  = 2 * _layout->margin();
    const auto spacingTotal = std::max(_sectionWidgets.count() - 1, 0) * _layout->spacing();
    
    auto availableWidth = _listenWidget->rect().width() - marginTotal - spacingTotal;
    
    QVector<std::int32_t> states;

    states.resize(_sectionWidgets.count());

    const auto getSectionsWidth = [this, &states, sortedSectionWidgets]() -> std::int32_t {
        auto sectionsWidth = 0;

        for (auto sectionWidget : sortedSectionWidgets)
            sectionsWidth += sectionWidget->getStateSizeHint(static_cast<WidgetState>(states[_sectionWidgets.indexOf(sectionWidget)])).width();

        return sectionsWidth;
    };

    auto sectionWidgets = sortedSectionWidgets + sortedSectionWidgets;

    for (auto sectionWidget : sectionWidgets) {
        const auto oldWidgetStates = states;

        states[_sectionWidgets.indexOf(sectionWidget)]++;

        const auto sectionsWidth = getSectionsWidth();

        if (sectionsWidth > availableWidth) {
            states = oldWidgetStates;
            break;
        }
    }

    for (auto sectionWidget : _sectionWidgets)
        sectionWidget->setState(static_cast<WidgetState>(states[_sectionWidgets.indexOf(sectionWidget)]));

    
}

}
}