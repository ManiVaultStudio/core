#include "DockableWidget.h"

#include <QLayout>
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QAction>
#include <QUuid>

namespace hdps
{
namespace gui
{

DockableWidget::DockableWidget(QWidget *parent) :
    QDockWidget(parent),
    _isVisible(false),
    _mainLayout(NULL),
    _mainWidget(NULL)
{
    _mainWidget = new QWidget(this);
    _mainLayout = new QGridLayout(_mainWidget);
    _mainLayout->setMargin(0);
    setWidget(_mainWidget);

    // Generate a unique name for the widget to let Qt identify it and store/retrieve its state
    setObjectName(QString("Dockable Widget ") + QUuid::createUuid().toString());

    QObject::connect(toggleViewAction(), &QAction::toggled, this, &DockableWidget::setVisibility);
}

QLayout* DockableWidget::mainLayout()
{
    return _mainLayout;
}

bool DockableWidget::setVisibility(bool visible)
{
    _isVisible = visible;
    setVisible(_isVisible);

    return _isVisible;
}

bool DockableWidget::toggleVisibility()
{
    return setVisibility(!_isVisible);
}

void DockableWidget::addWidget(QWidget* widget)
{
    _mainLayout->addWidget(widget);
}

void DockableWidget::setMainLayout(QLayout* layout)
{
    // Apparently not allowed ("A layout can only have another layout as a parent")
    // layout->setParent(_mainWidget);

    // Delete the previous layout before adding another
    delete _mainLayout;

    _mainLayout = layout;
    _mainWidget->setLayout(_mainLayout);
}

DockableWidget::~DockableWidget()
{
    if (_mainLayout) delete _mainLayout;
    if (_mainWidget) delete _mainWidget;
}

} // namespace gui

} // namespace hdps
