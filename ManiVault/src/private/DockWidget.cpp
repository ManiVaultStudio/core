// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DockWidget.h"

#include <util/StyledIcon.h>

#include <DockWidgetTab.h>

#include <QBoxLayout>
#include <QToolButton>

#ifdef _DEBUG
    #define DOCK_WIDGET_VERBOSE
#endif

using namespace ads;

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;

DockWidget::DockWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    CDockWidget(title, parent),
    Serializable(title),
    _settingsToolButton(nullptr)
{
    setObjectName(getId());
    setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromContent);

    _settingsToolButton = new QToolButton();

    _settingsToolButton->setToolTip("Adjust view settings");
    _settingsToolButton->setAutoRaise(true);
    _settingsToolButton->setIconSize(QSize(14, 14));
    _settingsToolButton->setFixedSize(QSize(14, 14));
    _settingsToolButton->setPopupMode(QToolButton::InstantPopup);
    _settingsToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    _settingsToolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const auto updateStyle = [this]() -> void {
        _settingsToolButton->setIcon(StyledIcon("bars"));
    };
    
    updateStyle();
    
    connect(&_themeWatcher, &ThemeWatcher::paletteChanged, this, updateStyle);

    dynamic_cast<QBoxLayout*>(tabWidget()->layout())->insertSpacing(1, 5);
    dynamic_cast<QBoxLayout*>(tabWidget()->layout())->insertWidget(2, _settingsToolButton, Qt::AlignCenter);
}

DockWidget::~DockWidget()
{
#ifdef DOCK_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << getSerializationName();
#endif
}

QString DockWidget::getTypeString() const
{
    return "DockWidget";
}

void DockWidget::showEvent(QShowEvent* showEvent)
{
    CDockWidget::showEvent(showEvent);

    if (_settingsToolButton != nullptr) {
        if (auto settingsMenu = this->getSettingsMenu())
            _settingsToolButton->setMenu(settingsMenu);
    }
}

QMenu* DockWidget::getSettingsMenu()
{
    return nullptr;
}

void DockWidget::setWidget(QWidget* widget, eInsertMode insertMode /*= AutoScrollArea*/)
{
    CDockWidget::setWidget(widget, insertMode);
}

void DockWidget::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    setObjectName(getId());
}

QVariantMap DockWidget::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "Type", QVariant::fromValue(this->getTypeString()) }
    });

    return variantMap;
}
