// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionToolButtonMenu.h"
#include "WidgetActionToolButton.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QEvent>

namespace mv::gui {

WidgetActionToolButtonMenu::WidgetActionToolButtonMenu(WidgetActionToolButton& widgetActionToolButton) :
    QMenu(&widgetActionToolButton),
    _widgetActionToolButton(widgetActionToolButton),
    _widgetAction(widgetActionToolButton),
    _widgetConfigurationFunction(),
    _widget(nullptr)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    addAction(&_widgetAction);
    //addAction(new QAction("asdasd"));

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        if (auto currentAction = _widgetActionToolButton.getAction())
            _widgetAction.getWidget()->layout()->addWidget(currentAction->createWidget(this));


        //_widget->setFixedSize(QSize(400, 400));

        //_widget->show();

        //QResizeEvent re(QSize(1000, 1000), size());
        //qApp->sendEvent(this, &re);
    });
}

WidgetConfigurationFunction WidgetActionToolButtonMenu::getWidgetConfigurationFunction()
{
    return _widgetConfigurationFunction;
}

void WidgetActionToolButtonMenu::showEvent(QShowEvent* showEvent)
{
    //updateGeometry();
    //adjustSize();

    QWidget::showEvent(showEvent);

    //QResizeEvent re(QSize(10, 10), size());
    //qApp->sendEvent(this, &re);

    //
    
    //QActionEvent e(QEvent::ActionChanged, &_widgetAction);

    //qApp->sendEvent(this, &e);
}

WidgetActionToolButtonMenu::DeferredWidgetAction::DeferredWidgetAction(WidgetActionToolButton& widgetActionToolButton) :
    QWidgetAction(&widgetActionToolButton),
    _widgetActionToolButton(widgetActionToolButton),
    _widget(nullptr)
{
    /*
    connect(&_widgetActionToolButton, &WidgetActionToolButton::actionChanged, this, [this](WidgetAction* previousAction, WidgetAction* currentAction)-> void {
        _action = currentAction;
    });
    */
}

QWidget* WidgetActionToolButtonMenu::DeferredWidgetAction::createWidget(QWidget* parent)
{
    //_widgetActionToolButton.getMenu().getWidget()->setParent(parent);
    _widget = new DeferredWidget(parent, _widgetActionToolButton);
    return _widget;
}

WidgetActionToolButtonMenu::DeferredWidgetAction::DeferredWidget::DeferredWidget(QWidget* parent, WidgetActionToolButton& widgetActionToolButton) :
    QWidget(parent),
    _widgetActionToolButton(widgetActionToolButton),
    _widget(nullptr)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    //layout->addWidget(currentAction->createWidget(this));

    setLayout(layout);

    //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    //layout->setSizeConstraint(QLayout::SetFixedSize);

    //connect(&_widgetActionToolButton, &WidgetActionToolButton::actionChanged, this, [this, layout](WidgetAction* previousAction, WidgetAction* currentAction)-> void {
    //    if (!currentAction)
    //        return;

    //    auto widget = currentAction->createWidget(this);

    //    widget->setVisible(false);

    //    layout->addWidget(widget);
    //});

    
}

void WidgetActionToolButtonMenu::DeferredWidgetAction::DeferredWidget::showEvent(QShowEvent* showEvent)
{
    
    QWidget::showEvent(showEvent);

    //layout()->addWidget(_widgetActionToolButton.getMenu().getWidget());
    /*

    //setFixedSize(QSize(600, 400));
    if (auto action = _widgetActionToolButton.getAction()) {
        if (!_widget) {
                _widget = action->createWidget(this);
                
                _widget->installEventFilter(this);

                layout()->addWidget(_widget);

                

                _widget->updateGeometry();
                _widget->adjustSize();

                //QActionEvent e(QEvent::ActionChanged, this);
                //qApp->sendEvent(&_widgetActionToolButton.getMenu(), &e);

                //QResizeEvent re(QSize(10000, 10), _widgetActionToolButton.size());
                //qApp->sendEvent(&_widgetActionToolButton.getMenu(), &re);
                //_widget->updateGeometry();
                //_widget->adjustSize();
            
        }
    }

    //_widgetActionToolButton.getMenu().setFixedSize(QSize(600, 200));// _widget->size());

    _widgetActionToolButton.getMenu().updateGeometry();
    _widgetActionToolButton.getMenu().adjustSize();
    //
    */
    
    
    

    
    //QResizeEvent resize_event(QSize(), _widgetActionToolButton.size());

    //parentWidget()->adjustSize();
    //qApp->sendEvent(&_widgetActionToolButton.getMenu(), &resize_event);
    

    //_widgetActionToolButton.getMenu().adjustSize();

    //Application::processEvents();

    //QResizeEvent resize_event(QSize(), parentWidget()->size());

    //parentWidget()->adjustSize();
    //qApp->sendEvent(parentWidget(), &resize_event);

    //auto size = _widget->size();
    //adjustSize();

    //update();

    //QWidget::showEvent(showEvent);

    //parentWidget()->updateGeometry();
    //parentWidget()->adjustSize();
    //

    ////adjustSize();

    
    //QActionEvent e(QEvent::ActionChanged, &_widgetActionToolButton.getMenu().getDeferredWidgetAction());
    //qApp->sendEvent(&_widgetActionToolButton.getMenu(), &e);
}

QSize WidgetActionToolButtonMenu::DeferredWidgetAction::DeferredWidget::sizeHint() const
{
    if (_widget)
        return _widget->sizeHint();

    return QSize(0, 0);
}

bool WidgetActionToolButtonMenu::DeferredWidgetAction::DeferredWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        // Force QMenu to recalculate the geometry of this item
        QActionEvent e(QEvent::ActionChanged, &_widgetActionToolButton.getMenu().getDeferredWidgetAction());
        qApp->sendEvent(&_widgetActionToolButton.getMenu(), &e);
    }

    return QWidget::eventFilter(watched, event);
}

}
