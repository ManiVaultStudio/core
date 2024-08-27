// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ShortcutMapOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define SHORTCUT_MAP_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ShortcutMapOverlayWidget::ShortcutMapOverlayWidget(QWidget* source, const util::ShortcutMap& shortcutMap) :
    OverlayWidget(source),
    _shortcutMap(shortcutMap),
    _widgetFader(this, this, 0.f, 0.f, 1.0f, 250, 250),
    _closeAction(this, "Close")
{
    setAutoFillBackground(true);
    setStyleSheet("background-color: white;");

    _layout.addWidget(&_label);
    _layout.addStretch(1);
    _layout.addWidget(_closeAction.createWidget(this));

    //_label.setStyleSheet("background-color: rgba(0, 0, 0, 20)");

    setLayout(&_layout);

    const auto header = QString("<p style='font-size: 16pt;'><b>%1</b> shortcuts</p>").arg(_shortcutMap.getTitle());

    QString categories;

    const auto createShortcutMapCategoryTable = [this](const QString& category) -> QString {
        const auto header = QString("<p style='font-size: 12pt;'><b>%1</b></p>").arg(category);

        QString rows;

        for (const auto& shortcut : _shortcutMap.getShortcuts({ category }))
            rows += QString("<tr><td style='width: 100px;'><b>%1</b></td><td>     %2</td></tr>").arg(shortcut._keySequence.toString(), shortcut._title);

        const auto table = QString("<table>%1</table>").arg(rows);

        return header + table;
    };

    for (const auto& category : _shortcutMap.getCategories())
        categories += createShortcutMapCategoryTable(category);

    _label.setText(header + categories);

    connect(&_closeAction, &TriggerAction::triggered, &_widgetFader, [this]() -> void {
        connect(&_widgetFader, &WidgetFader::fadedOut, this, &ShortcutMapOverlayWidget::deleteLater);
        getWidgetFader().fadeOut();
    });
}

mv::util::WidgetFader& ShortcutMapOverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

void ShortcutMapOverlayWidget::showEvent(QShowEvent* event)
{
    _widgetFader.fadeIn();

    OverlayWidget::showEvent(event);
}
}
