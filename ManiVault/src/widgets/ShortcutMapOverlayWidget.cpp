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
ShortcutMapOverlayWidget::CloseLabel::CloseLabel(ShortcutMapOverlayWidget* shortcutMapOverlayWidget) :
    QLabel(shortcutMapOverlayWidget),
    _shortcutMapOverlayWidget(shortcutMapOverlayWidget)
{
    setGraphicsEffect(&_opacityEffect);

    _opacityEffect.setOpacity(.5f);
}

void ShortcutMapOverlayWidget::CloseLabel::enterEvent(QEnterEvent* event)
{
    QLabel::enterEvent(event);

    _opacityEffect.setOpacity(1.f);
}

void ShortcutMapOverlayWidget::CloseLabel::leaveEvent(QEvent* event)
{
    QLabel::leaveEvent(event);

    _opacityEffect.setOpacity(.5f);
}

void ShortcutMapOverlayWidget::CloseLabel::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);

    _shortcutMapOverlayWidget->deleteLater();
}

ShortcutMapOverlayWidget::ShortcutMapOverlayWidget(QWidget* source, const util::ShortcutMap& shortcutMap) :
    OverlayWidget(source),
    _shortcutMap(shortcutMap),
    _closeIconLabel(this)
{
    setAutoFillBackground(true);

    _mainLayout.addLayout(&_toolbarLayout);
    _mainLayout.addLayout(&_headerLayout);
    _mainLayout.addWidget(&_shortcutsScrollArea);

    setLayout(&_mainLayout);

    _toolbarLayout.addStretch(1);
    _toolbarLayout.addWidget(&_closeIconLabel);

    _closeIconLabel.setStyleSheet("opacity: 0.5");
    _closeIconLabel.setPixmap(Application::getIconFont("FontAwesome").getIcon("times").pixmap(QSize(14, 14)));

    _headerLayout.addWidget(&_headerIconLabel);
    _headerLayout.addWidget(&_headerTextLabel);
    _headerLayout.addStretch(1);
    _headerLayout.setSpacing(10);

    _headerIconLabel.setPixmap(Application::getIconFont("FontAwesome").getIcon("keyboard").pixmap(QSize(24, 24)));
    _headerIconLabel.setStyleSheet("padding-top: 2px;");

    _headerTextLabel.setText(QString("<p style='font-size: 16pt;'><b>%1</b> shortcuts</p>").arg(_shortcutMap.getTitle()));

    QString categories;

    const auto createShortcutMapCategoryTable = [this](const QString& category) -> QString {
        const auto header = QString("<p style='font-size: 12pt;'><b>%1</b></p>").arg(category);

        QString rows;

        for (const auto& shortcut : _shortcutMap.getShortcuts({ category }))
            rows += QString("<tr><td width='60'><b>%1</b></td><td>%2</td></tr>").arg(shortcut._keySequence.toString(), shortcut._title);

        const auto table = QString("<table>%1</table>").arg(rows);

        return header + table;
    };

    for (const auto& category : _shortcutMap.getCategories())
        categories += createShortcutMapCategoryTable(category);

    _shortcutsScrollArea.setWidgetResizable(true);
    _shortcutsScrollArea.setWidget(&_shortcutsWidget);
    _shortcutsScrollArea.setObjectName("Shortcuts");
    _shortcutsScrollArea.setStyleSheet("QScrollArea#Shortcuts { border: none; }");

    _shortcutsWidgetLayout.setContentsMargins(0, 0, 0, 0);
    _shortcutsWidgetLayout.addWidget(&_bodyLabel);
    _shortcutsWidgetLayout.setAlignment(Qt::AlignTop);

    _shortcutsWidget.setLayout(&_shortcutsWidgetLayout);

    _bodyLabel.setText(categories);
}

}
