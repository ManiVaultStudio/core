// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginShortcutsDialog.h"

#include "Application.h"
#include "util/ShortcutMap.h"
#include "ViewPlugin.h"

#include <QDebug>

#ifdef _DEBUG
    #define VIEW_PLUGIN_SHORTCUTS_DIALOG_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ViewPluginShortcutsDialog::ViewPluginShortcutsDialog(plugin::ViewPlugin* viewPlugin, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _shortcutMap(viewPlugin->getShortcuts().getMap())
{
    setAutoFillBackground(true);
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("keyboard"));
    setWindowTitle(QString("%1 shortcuts").arg(viewPlugin->getLearningCenterAction().getPluginTitle()));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_textScrollArea);

    setLayout(layout);

    QString categories;

    const auto createShortcutMapCategoryTable = [this](const QString& category) -> QString {
        const auto header = QString("<p style='font-size: 12pt;'><b>%1</b></p>").arg(category);

        QString rows;

        for (const auto& shortcut : _shortcutMap.getShortcuts({ category }))
            rows += QString("<tr><td width='50'><b>%1</b></td><td>%2</td></tr>").arg(shortcut._keySequence.toString(), shortcut._title);

        const auto table = QString("<table>%1</table>").arg(rows);

        return header + table;
    };

    for (const auto& category : _shortcutMap.getCategories())
        categories += createShortcutMapCategoryTable(category);

    _textScrollArea.setWidgetResizable(true);
    _textScrollArea.setWidget(&_textWidget);
    _textScrollArea.setObjectName("Shortcuts");
    _textScrollArea.setStyleSheet("QScrollArea#Shortcuts { border: none; }");

    _textWidgetLayout.setContentsMargins(0, 0, 0, 0);
    _textWidgetLayout.addWidget(&_textBodyLabel);
    _textWidgetLayout.setAlignment(Qt::AlignTop);

    _textWidget.setLayout(&_textWidgetLayout);

    _textBodyLabel.setText(categories);
}

}