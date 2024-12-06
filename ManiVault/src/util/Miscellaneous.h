// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QIcon>
#include <QPointer>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QWidget>

#include <algorithm>

class QAction;

namespace mv::util
{

/**
 * Returns a human readable string of an integer count
 * @param count Integer count
 * @return Human readable string of an integer count
 */
CORE_EXPORT QString getIntegerCountHumanReadable(const float& count);

/**
 * Returns a human readable string of a byte count
 * @param noBytes Number of bytes
 * @return Human readable string of a byte count
 */
CORE_EXPORT QString getNoBytesHumanReadable(float noBytes);

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

    while (parentWidget) {
        if (auto parentImpl = qobject_cast<WidgetClass*>(parentWidget))
            return parentImpl;

        parentWidget = parentWidget->parentWidget();
    }

    return {};
}

/**
 * Get tabbed (indented) message
 * @param message Message to prefix with tab indentation
 * @param tabIndex Number of tabs to prefix with
 * @return Message indented with tabs
 */
CORE_EXPORT inline QString getTabIndentedMessage(QString message, const std::uint32_t& tabIndex);

/**
 * Get \p color as CSS string, either with or without \p alpha
 * @param color Input color
 * @param alpha Whether to use the alpha
 * @return Color as rgb(...) or rgba(...)
 */
CORE_EXPORT QString getColorAsCssString(const QColor& color, bool alpha = true);

/**
 * Get whether \p URL is valid
 * @param urlString Url string 
 * @return Boolean determining whether the URL is valid
 */
CORE_EXPORT bool urlExists(const QString& urlString);

CORE_EXPORT void replaceLayout(QWidget* widget, QLayout* newLayout, bool removeWidgets = false);
CORE_EXPORT void clearLayout(QLayout* layout, bool removeWidgets = false);
CORE_EXPORT void setLayoutContentsMargins(QLayout* layout, std::int32_t margin);
CORE_EXPORT QIcon getAlignmentIcon(const Qt::Alignment& alignment);

/**
 * Set child \p value of \root by \p path
 * @param root Root element
 * @param path Path to the value
 * @param value Value to set
 */
CORE_EXPORT QVariant setValueByPath(QVariant root, const QString& path, const QVariant& value);
    
/**
 * Get child value of \root by \p path
 * @param root Root element
 * @param path Path to the value
 * @param valueIfNotFound Value if not found
 * @return Value, invalid when no value was found
 */
CORE_EXPORT QVariant getValueByPath(const QVariant& root, const QString& path, const QVariant& valueIfNotFound = QVariant());

/**
 * Convert \p gifByteArray to base64 encoded string
 * @param gifByteArray GIF image raw bytes
 * @return Base64-encoded GIF string
 */
CORE_EXPORT QString gifToBase64(const QByteArray& gifByteArray);

/**
 * Embed \p gifBase64-encoded GIF in an HTML <img> tag
 * @param gifBase64 GIF image as base64-encoded string
 * @return GIF image embedded in HTML <img> tag
 */
CORE_EXPORT QString embedGifFromBase64(const QString& gifBase64);

/**
 * Embed GIF image from \p resourcePath into an HTML <img> tag
 * @param resourcePath Path of the GIF image in the resources
 * @return GIF image embedded in HTML <img> tag
 */
CORE_EXPORT QString embedGifFromResource(const QString& resourcePath);

/**
 * This method keeps the application event loop responsive while halting the current execution for n \p milliseconds
 * @param milliSeconds Milliseconds to wait
 */
CORE_EXPORT void waitForDuration(int milliSeconds);

/**
 * Remove all connections (to and from) from \p object and its descendants
 * @param object Pointer to root object
 */
CORE_EXPORT void disconnectRecursively(const QObject* object);

}
