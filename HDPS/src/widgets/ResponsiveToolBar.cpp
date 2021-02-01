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
    return stateBefore == ResponsiveSectionWidget::State::Popup && stateAfter == ResponsiveSectionWidget::State::Popup ? Spacer::Type::Spacer : Spacer::Type::Divider;
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
            return 14;

        case Type::Spacer:
            return 6;

        default:
            break;
    }

    return 0;
}

const std::int32_t ResponsiveToolBar::LAYOUT_MARGIN = 0;
const std::int32_t ResponsiveToolBar::LAYOUT_SPACING = 5;

ResponsiveToolBar::ResponsiveToolBar(QWidget* parent) :
    QWidget(parent),
    _listenWidget(nullptr),
    _layout(new QHBoxLayout()),
    _sections(),
    _spacers(),
    _modified(-1)
{
    setAutoFillBackground(true);
    setObjectName("ResponsiveToolBar");
    setStyleSheet("QWidget#ResponsiveToolBar { background-color:red; }");

    _layout->setMargin(5);
    _layout->setSpacing(0);
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
    switch (event->type()) {
        case QEvent::Resize:
        {
            /*
            if (dynamic_cast<QWidget*>(object) == _listenWidget)
                computeLayout();

            const auto responsiveSectionWidget = dynamic_cast<ResponsiveSectionWidget*>(object);

            if (responsiveSectionWidget)
                computeLayout(responsiveSectionWidget);
            */

            break;
        }

        default:
            break;
    }
    
    return QObject::eventFilter(object, event);
}

void ResponsiveToolBar::computeLayout()
{
    Q_ASSERT(_listenWidget != nullptr);

    const auto printIgnoredSections = [this]() {
        //for (auto ignoreSection : _ignoreSections)
            //qDebug() << QString("Ignoring %1").arg(ignoreSection->getName());
    };
/*
    if (resizedSectionWidget == nullptr) {
        qDebug() << "Toolbar resized";
    }
    else {
        qDebug() << QString("%1 resized").arg(resizedSectionWidget->getName());

        
        if (_ignoreSections.contains(resizedSectionWidget)) {
            qDebug() << QString("%1 ignored, not re-computing the layout").arg(resizedSectionWidget->getName());

            _ignoreSections.removeOne(resizedSectionWidget);

            printIgnoredSections();

            return;
        }
        
    }
*/
    //qDebug() << width();

    Timer timer("Compute layout");

    const auto marginTotal  = 2 * _layout->margin();
    
    const auto availableWidth = _listenWidget->rect().width() - marginTotal;
    
    QVector<std::int32_t> states;

    states.resize(_sections.count());

    const auto getSectionsWidth = [this, &states]() -> std::int32_t {
        auto sectionsWidth = 0;

        for (auto section : _sections)
            sectionsWidth += section->getStateSizeHint(static_cast<ResponsiveSectionWidget::State>(states[_sections.indexOf(section)])).width();

        for (auto spacer : _spacers) {
            const auto spacerIndex  = _spacers.indexOf(spacer);
            const auto spacerType   = Spacer::getType(static_cast<ResponsiveSectionWidget::State>(states[spacerIndex]), static_cast<ResponsiveSectionWidget::State>(states[spacerIndex + 1]));
            
            sectionsWidth += Spacer::getWidth(spacerType);
        }

        return sectionsWidth;
    };

    auto sortedSections = _sections;

    std::sort(sortedSections.begin(), sortedSections.end(), [](ResponsiveSectionWidget* left, ResponsiveSectionWidget* right) {
        return left->getPriority() > right->getPriority();
    });

    auto sections = sortedSections + sortedSections;

    for (auto section : sections) {
        const auto oldWidgetStates = states;

        states[_sections.indexOf(section)]++;

        const auto sectionsWidth = getSectionsWidth();

        if (sectionsWidth > availableWidth) {
            states = oldWidgetStates;
            break;
        }
    }

    /*
    if (resizedSectionWidget == nullptr) {
        

        //_ignoreSections = _sections;
    }
    else {
        for (auto section : _sections) {
            if (resizedSectionWidget != section)
                _ignoreSections << section;
        }
    }
    */

    for (auto section : _sections)
        section->setState(static_cast<ResponsiveSectionWidget::State>(states[_sections.indexOf(section)]));

    for (auto spacer : _spacers) {
        const auto spacerIndex  = _spacers.indexOf(spacer);
        const auto spacerType   = Spacer::getType(_sections[spacerIndex], _sections[spacerIndex + 1]);

        spacer->setType(spacerType);
    }
}

}
}