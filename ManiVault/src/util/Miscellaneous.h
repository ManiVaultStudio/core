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
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <QRegularExpression>

#include <algorithm>

class QAction;

namespace mv::util
{

/**
 * Returns a human readable string of an integer count
 * @param count Integer count
 * @return Human readable string of an integer count
 */
CORE_EXPORT QString getIntegerCountHumanReadable(const double& count);

/**
 * Convert a byte size string to a number of bytes
 * @return Number of bytes
 */
CORE_EXPORT std::uint64_t parseByteSize(const QString& input);

/**
 * Returns a human-readable string of a byte count
 * @param noBytes Number of bytes
 * @param useIEC Whether to use IEC (base 1024) or SI (base 1000) units
 * @return Human-readable string of a byte count
 */
CORE_EXPORT QString getNoBytesHumanReadable(std::uint64_t noBytes, bool useIEC = true);

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

/**
 * Replace all occurrences of \p from with \p to in \p inputString and return the result
 * @param inputString Input string
 * @param from String to replace
 * @param to Replace from with this
 * @return Replaced string
 */
CORE_EXPORT std::string replaceAll(std::string inputString, const std::string& from, const std::string& to);

/**
 * Remove CR/LF from a base64 string (some encoders insert line breaks)
 * @param inputString Input string
 * @return Curated resulting string
 */
CORE_EXPORT std::string stripNewLines(std::string inputString);

/**
 * Escape for a CSS double-quoted string (inside url("..."))
 * @param inputString Input string
 * @return Curated resulting string
 */
CORE_EXPORT std::string escapeCssDq(std::string inputString);

/**
 * Convert the contents of \p pixmap to base46 encoded string and form a CSS src string
 * @param pixmap Input pixmap
 * @return String ('')
 */
CORE_EXPORT std::string pixmapToCssSrc(const QPixmap& pixmap);

/**
 * Determine the MIME format based on \p byteArray
 * @param byteArray Input byte array
 * @return MIME type
 */
CORE_EXPORT QString mimeForFormat(const QByteArray& byteArray);

/**
 * Normalize image format from file \p path suffix (e.g. jpg -> JPEG)
 * @param path Input file path
 * @return Normalized format (PNG, JPEG, etc.)
 */
CORE_EXPORT QByteArray normalizeFormatFromSuffix(const QString& path);

/**
 * Choose encoding format for \p img, optionally using \p hinted format (if the source format is unknown, pick PNG when alpha is present, else JPEG)
 * @param img Input image
 * @param hinted Hinted format (may be empty)
 * @return Chosen format (PNG, JPEG, etc.)
 */
CORE_EXPORT QByteArray chooseFormatForImage(const QImage& img, const QByteArray& hinted);

/**
 * Convert the contents of \p pixmap to base64 encoded data URL
 * @param pixmap Input pixmap
 * @param fmt Encoding format (PNG, JPEG, etc.)
 * @param quality Encoding quality (-1 is default)
 * @return Data URL string
 */
CORE_EXPORT QString pixmapToDataUrl(const QPixmap& pixmap, const QByteArray& fmt = "PNG", int quality = -1);

/**
 * Replace \p token in \p css with a CSS data URL formed from \p pixmap encoded as \p fmt with \p quality
 * @param css Input CSS
 * @param pixmap Input pixmap
 * @param fmt Encoding format (PNG, JPEG, etc.)
 * @param quality Encoding quality (-1 is default)
 * @param token Token to replace in the CSS (default {{BACKGROUND_IMAGE}})
 * @return CSS with token replaced by data URL
 */
CORE_EXPORT QString applyPixmapToCss(QString css, const QPixmap& pm, const QByteArray& fmt = "PNG", int quality = -1, const QString& token = QStringLiteral("{{BACKGROUND_IMAGE}}"));

/**
 * Replace \p token in \p css with a CSS data URL formed from image at \p pathOrResource encoded as \p quality
 * @param css Input CSS
 * @param pathOrResource Path to image file or resource path (e.g. ":/images/background.png")
 * @param token Token to replace in the CSS (default {{BACKGROUND_IMAGE}})
 * @param quality Encoding quality (-1 is default)
 * @return CSS with token replaced by data URL
 */
CORE_EXPORT QString applyResourceImageToCss(QString css, const QString& pathOrResource, const QString& token = QStringLiteral("{{BACKGROUND_IMAGE}}"), int quality = 90);
}
