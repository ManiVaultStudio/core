#include "DockableWidget.h"

#include <QLayout>
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QAction>

namespace hdps {

namespace gui {

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

	QObject::connect(toggleViewAction(), &QAction::toggled, this, &DockableWidget::setVisibility);
}

QGridLayout* DockableWidget::mainLayout()
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

DockableWidget::~DockableWidget()
{
	if (_mainLayout) delete _mainLayout;
	if (_mainWidget) delete _mainWidget;
}

} // namespace gui

} // namespace hdps
