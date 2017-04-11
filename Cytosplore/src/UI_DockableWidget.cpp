#include "UI_DockableWidget.h"

#include <QLayout>
#include <QFile>
#include <QTextStream>
#include <QObject>
#include <QAction>

UI_DockableWidget::UI_DockableWidget(QWidget *parent) :
	QDockWidget(parent),
	_isVisible(false),
	_mainLayout(NULL),
	_mainWidget(NULL)
{
	_mainWidget = new QWidget(this);
	_mainLayout = new QGridLayout(_mainWidget);
	_mainLayout->setMargin(0);
	setWidget(_mainWidget);

	QObject::connect(toggleViewAction(), &QAction::toggled, this, &UI_DockableWidget::setVisibility);
}

QGridLayout* UI_DockableWidget::mainLayout()
{
	return _mainLayout;
}

bool UI_DockableWidget::setVisibility(bool visible)
{
	_isVisible = visible;
	setVisible(_isVisible);
    
    return _isVisible;
}

bool UI_DockableWidget::toggleVisibility()
{
	return setVisibility(!_isVisible);
}

UI_DockableWidget::~UI_DockableWidget()
{
	if (_mainLayout) delete _mainLayout;
	if (_mainWidget) delete _mainWidget;
}
