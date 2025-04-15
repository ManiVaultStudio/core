// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopeWidget.h"

#include <Application.h>

#include <util/StyledIcon.h>

#include "SampleScopePlugin.h"

#include <QDebug>
#include <QGraphicsProxyWidget>

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;
using namespace mv::util;

SampleScopeWidget::SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleScopePlugin(sampleScopePlugin),
    _proxyWidget(nullptr),
    _currentViewWidget(nullptr),
    _noSamplesOverlayWidget(&_viewsWidget, StyledIcon("eye-dropper"), "No samples view", "There is currently no samples view available..."),
    _currentViewPlugin(nullptr)
{
}

void SampleScopeWidget::initialize()
{
    setLayout(&_mainLayout);

    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _viewsWidgetLayout.setContentsMargins(0, 0, 0, 0);

    _viewsWidget.setAutoFillBackground(true);

    _viewsWidget.setLayout(&_viewsWidgetLayout);

    _mainLayout.addWidget(&_viewsWidget);

    _widgetView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _viewsWidgetLayout.addWidget(&_htmlView);
    _viewsWidgetLayout.addWidget(&_widgetView);

	_widgetView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _widgetView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    _noSamplesOverlayWidget.show();

    connect(&_sampleScopePlugin->getSourcePluginPickerAction(), &PluginPickerAction::pluginPicked, this, [this](plugin::Plugin* plugin) -> void {
        if (_currentViewPlugin) {
            disconnect(&_currentViewPlugin->getSamplerAction(), &ViewPluginSamplerAction::viewGeneratorTypeChanged, this, nullptr);
            _previousViewPlugin = _currentViewPlugin;
        }

        _currentViewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (_previousViewPlugin) {
            qDebug() << "SourcePluginPicker: Previous plugin was:" << _previousViewPlugin->getGuiName();
        }
        if (_currentViewPlugin) {
            qDebug() << "SourcePluginPicker: Current plugin is:" << _currentViewPlugin->getGuiName();
        }

        if (_currentViewPlugin) {
            connect(&_currentViewPlugin->getSamplerAction(), &ViewPluginSamplerAction::viewGeneratorTypeChanged, this, &SampleScopeWidget::updateVisibility);

            updateVisibility();
        }
	});

    _widgetView.setScene(&_widgetViewScene);
    _widgetView.setRenderHint(QPainter::Antialiasing);

    updateVisibility();
}

void SampleScopeWidget::setViewHtml(const QString& html)
{
    _htmlView.setHtml(html);
}

void SampleScopeWidget::setViewWidget(const QWidget* widget)
{
    Q_ASSERT(widget);

    if (!widget)
        return;

    //if (widget != _currentViewWidget) 
    //{
        /*_currentViewWidget = const_cast<QWidget*>(widget);
        _widgetViewScene.clear();

        _proxyWidget = _widgetViewScene.addWidget(const_cast<QWidget*>(widget));

        _proxyWidget->setPos(0, 0);

        updateViewWidgetProxySize();*/

        // 1) Detach old widget from the proxy, so it won't be destroyed
        if (_proxyWidget)
        {
            // Get the currently attached widget (if any)
            QWidget* oldWidget = _proxyWidget->widget();
            if (oldWidget) {
                // Detach it from the proxy so it doesn't get destroyed
                _proxyWidget->setWidget(nullptr);
                oldWidget->setParent(nullptr);

                /*if (_previousViewPlugin) {
                    oldWidget->setParent(&_previousViewPlugin->getWidget());
                    qDebug() << "Previous plugin was:" << _previousViewPlugin->getGuiName();
                }
                if (_currentViewPlugin) {
                    qDebug() << "Current plugin is:" << _currentViewPlugin->getGuiName();
                }*/
            }

            // Remove _proxyWidget itself from the scene
            _widgetViewScene.removeItem(_proxyWidget);
            delete _proxyWidget;
            _proxyWidget = nullptr;
        }

        // 2) Clear the scene
        _widgetViewScene.clear();

        QString pluginName;
        if (_currentViewPlugin) {
            pluginName = _currentViewPlugin->getGuiName();
        }
        else {
            pluginName = "UnknownPlugin";
        }
        qDebug() << "SetWidget: Plugin name:" << pluginName;

        QWidget* storedWidget = _widgetMap.value(pluginName, nullptr);

        if (!storedWidget) {
            // Not in the map yet
            QWidget* newWidget = const_cast<QWidget*>(widget);

            // Store in the map
            _widgetMap[pluginName] = newWidget;
            storedWidget = newWidget;
            qDebug() << "SetWidget: Stored widget:" << storedWidget;
        }
        else {
           // ?
            qDebug() << "SetWidget: Stored widget already exists:" << storedWidget;
        }

        // 3) Store the new widget as current
        //_currentViewWidget = const_cast<QWidget*>(widget);
        _currentViewWidget = storedWidget;

        // 4) Create a new proxy for the new widget
        _proxyWidget = _widgetViewScene.addWidget(_currentViewWidget);
        _proxyWidget->setPos(0, 0);

        // 5) Update the size or layout
        updateViewWidgetProxySize();
    //}
}

InfoOverlayWidget& SampleScopeWidget::getNoSamplesOverlayWidget()
{
    return _noSamplesOverlayWidget;
}

void SampleScopeWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

    _widgetViewScene.setSceneRect(0, 0, width(), height());

    updateViewWidgetProxySize();
}

void SampleScopeWidget::updateVisibility()
{
    if (!_currentViewPlugin)
        return;

    _htmlView.hide();
    _widgetView.hide();

    auto& viewPluginSamplerAction = _currentViewPlugin->getSamplerAction();

    if (viewPluginSamplerAction.canView()) {
        switch (viewPluginSamplerAction.getViewGeneratorType()) {
	        case ViewPluginSamplerAction::ViewGeneratorType::HTML:
	        {
	            _htmlView.show();
	            _widgetView.hide();

	            break;
	        }

	        case ViewPluginSamplerAction::ViewGeneratorType::Widget:
	        {
	            _htmlView.hide();
	            _widgetView.show();

	            break;
	        }
        }
    }
}

void SampleScopeWidget::updateViewWidgetProxySize() const
{
    if (_proxyWidget)
        _proxyWidget->resize(width(), height());
}
