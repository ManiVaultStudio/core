// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>
#include <QVector>
#include <QWidget>
#include <QPointer>
#include <QDebug>

#include <algorithm>

class QAction;

namespace hdps::util
{

/**
 * Returns a human readable string of an integer count
 * @param count Integer count
 * @return Human readable string of an integer count
 */
QString getIntegerCountHumanReadable(const float& count);

/**
 * Returns a human readable string of a byte count
 * @param noBytes Number of bytes
 * @return Human readable string of a byte count
 */
QString getNoBytesHumanReadable(float noBytes);

/**
 * Sort action based on their text
 * @param actions Actions to sort
 */
template<typename ActionType>
void sortActions(QVector<QPointer<ActionType>>& actions)
{
    std::sort(actions.begin(), actions.end(), [](auto actionA, auto actionB) {
        return actionA->text() < actionB->text();
    });
}

/**
 * Find parent of type \p WidgetClass
 * @param widget Pointer to widget to search for
 * @return Pointer to parent widget of type \p WidgetClass, otherwise a nullptr
 */
template <class WidgetClass>
WidgetClass* findParent(const QWidget* widget)
{
    auto parentWidget = widget->parentWidget();

    while (parentWidget)
    {
        auto parentImpl = qobject_cast<WidgetClass*>(parentWidget);

        if (parentImpl)
            return parentImpl;

        parentWidget = parentWidget->parentWidget();
    }

    return 0;
}

/**
 * Get tabbed (indented) message
 * @param message Message to prefix with tab indentation
 * @param tabIndex Number of tabs to prefix with
 * @return Message indented with tabs
 */
inline QString getTabIndentedMessage(QString message, const std::uint32_t& tabIndex) {
    static const std::uint32_t tabSize = 4;

    QString indentation;

    for (std::uint32_t i = 0; i < tabIndex * tabSize; i++)
        indentation += " ";

    message.insert(0, indentation);

    return message;
}

}
