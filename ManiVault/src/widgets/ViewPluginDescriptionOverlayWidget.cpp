// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginDescriptionOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define VIEW_PLUGIN_DESCRIPTION_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ViewPluginDescriptionOverlayWidget::ViewPluginDescriptionOverlayWidget(plugin::ViewPlugin* viewPlugin) :
    ViewPluginOverlayWidget(viewPlugin)
{
    setAutoFillBackground(true);

    getMainLayout().addLayout(&_headerLayout);
    getMainLayout().addWidget(&_shortcutsScrollArea);

    _headerLayout.addWidget(&_headerIconLabel);
    _headerLayout.addWidget(&_headerTextLabel);
    _headerLayout.addStretch(1);
    _headerLayout.setSpacing(10);

    _headerIconLabel.setPixmap(Application::getIconFont("FontAwesome").getIcon("book-reader").pixmap(QSize(24, 24)));
    _headerIconLabel.setStyleSheet("padding-top: 2px;");

    _headerTextLabel.setText(QString("<p style='font-size: 16pt;'><b>%1</b></p>").arg(viewPlugin->getKind()));

    //QString categories;

    //const auto createShortcutMapCategoryTable = [this](const QString& category) -> QString {
    //    const auto header = QString("<p style='font-size: 12pt;'><b>%1</b></p>").arg(category);

    //    QString rows;

    //    for (const auto& shortcut : _shortcutMap.getShortcuts({ category }))
    //        rows += QString("<tr><td width='50'><b>%1</b></td><td>%2</td></tr>").arg(shortcut._keySequence.toString(), shortcut._title);

    //    const auto table = QString("<table>%1</table>").arg(rows);

    //    return header + table;
    //    };

    //for (const auto& category : _shortcutMap.getCategories())
    //    categories += createShortcutMapCategoryTable(category);

    _shortcutsScrollArea.setWidgetResizable(true);
    _shortcutsScrollArea.setWidget(&_shortcutsWidget);
    _shortcutsScrollArea.setObjectName("Shortcuts");
    _shortcutsScrollArea.setStyleSheet("QScrollArea#Shortcuts { border: none; }");

    _shortcutsWidgetLayout.setContentsMargins(0, 0, 0, 0);
    _shortcutsWidgetLayout.addWidget(&_bodyLabel);
    _shortcutsWidgetLayout.setAlignment(Qt::AlignTop);

    _shortcutsWidget.setLayout(&_shortcutsWidgetLayout);

    _bodyLabel.setText(viewPlugin->getLearningCenterAction().getLongDescription());
}

}
