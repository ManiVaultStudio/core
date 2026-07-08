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
#include <QStringDecoder>

#include <algorithm>

#include "StackFrame.h"

class QAction;

namespace mv::util
{

/**
 * @brief Formats an integer-like count for display.
 * @param count Count to format.
 * @return Human-readable count string.
 */
CORE_EXPORT QString getIntegerCountHumanReadable(const double& count);

/**
 * @brief Parses a human-readable byte-size string.
 * @param input Byte-size string to parse.
 * @return Number of bytes represented by the input.
 */
CORE_EXPORT std::uint64_t parseByteSize(const QString& input);

/**
 * @brief Formats a byte count for display.
 * @param noBytes Number of bytes.
 * @param useIEC Whether to use IEC base-1024 units instead of SI base-1000 units.
 * @return Human-readable byte count.
 */
CORE_EXPORT QString getNoBytesHumanReadable(std::uint64_t noBytes, bool useIEC = true);

/**
 * @brief Formats an elapsed duration for display.
 * @param ms Duration in milliseconds.
 * @param compact Whether to use compact formatting.
 * @return Human-readable elapsed time string.
 */
CORE_EXPORT QString getElapsedTimeHumanReadable(std::uint64_t ms, bool compact = true);

/**
 * @brief Sorts actions by their display text.
 * @tparam ActionType Action pointer type stored in the vector.
 * @param actions Actions to sort in place.
 */
template<typename ActionType>
void sortActions(QVector<QPointer<ActionType>>& actions)
{
    std::sort(actions.begin(), actions.end(), [](auto actionA, auto actionB) {
        return actionA->text() < actionB->text();
    });
}

/**
 * @brief Finds the first parent widget of the requested type.
 * @tparam WidgetClass Widget type to find.
 * @param widget Widget whose parents should be searched.
 * @return Matching parent widget, or nullptr when no parent matches.
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
 * @brief Adds tab indentation to a message.
 * @param message Message to indent.
 * @param tabIndex Number of tab characters to prefix.
 * @return Indented message.
 */
CORE_EXPORT inline QString getTabIndentedMessage(QString message, const std::uint32_t& tabIndex);

/**
 * @brief Converts a QColor to a CSS color string.
 * @param color Color to convert.
 * @param alpha Whether to include the alpha channel.
 * @return CSS rgb(...) or rgba(...) color string.
 */
CORE_EXPORT QString getColorAsCssString(const QColor& color, bool alpha = true);

/**
 * @brief Returns whether a URL exists.
 * @param urlString URL string to test.
 * @return True when the URL can be reached.
 */
CORE_EXPORT bool urlExists(const QString& urlString);

/**
 * @brief Replaces a widget layout.
 * @param widget Widget whose layout should be replaced.
 * @param newLayout New layout to install.
 * @param removeWidgets Whether to remove widgets from the previous layout.
 */
CORE_EXPORT void replaceLayout(QWidget* widget, QLayout* newLayout, bool removeWidgets = false);

/**
 * @brief Clears a layout.
 * @param layout Layout to clear.
 * @param removeWidgets Whether to delete contained widgets.
 */
CORE_EXPORT void clearLayout(QLayout* layout, bool removeWidgets = false);

/**
 * @brief Sets equal contents margins on a layout.
 * @param layout Layout to update.
 * @param margin Margin applied to all sides.
 */
CORE_EXPORT void setLayoutContentsMargins(QLayout* layout, std::int32_t margin);

/**
 * @brief Returns an icon representing a Qt alignment.
 * @param alignment Alignment to represent.
 * @return Alignment icon.
 */
CORE_EXPORT QIcon getAlignmentIcon(const Qt::Alignment& alignment);

/**
 * @brief Sets a nested value in a QVariant tree by path.
 * @param root Root value to update.
 * @param path Path to the nested value.
 * @param value Value to set.
 * @return Updated root value.
 */
CORE_EXPORT QVariant setValueByPath(QVariant root, const QString& path, const QVariant& value);
    
/**
 * @brief Returns a nested value from a QVariant tree by path.
 * @param root Root value to inspect.
 * @param path Path to the nested value.
 * @param valueIfNotFound Value returned when the path is not found.
 * @return Nested value, or valueIfNotFound when unavailable.
 */
CORE_EXPORT QVariant getValueByPath(const QVariant& root, const QString& path, const QVariant& valueIfNotFound = QVariant());

/**
 * @brief Converts GIF bytes to a base64 string.
 * @param gifByteArray GIF image bytes.
 * @return Base64-encoded GIF data.
 */
CORE_EXPORT QString gifToBase64(const QByteArray& gifByteArray);

/**
 * @brief Embeds a base64 GIF in an HTML image tag.
 * @param gifBase64 Base64-encoded GIF data.
 * @return HTML image tag containing the GIF.
 */
CORE_EXPORT QString embedGifFromBase64(const QString& gifBase64);

/**
 * @brief Embeds a GIF resource in an HTML image tag.
 * @param resourcePath Resource path of the GIF image.
 * @return HTML image tag containing the GIF.
 */
CORE_EXPORT QString embedGifFromResource(const QString& resourcePath);

/**
 * @brief Waits while keeping the application event loop responsive.
 * @param milliSeconds Duration to wait in milliseconds.
 */
CORE_EXPORT void waitForDuration(int milliSeconds);

/**
 * @brief Disconnects an object and all descendants.
 * @param object Root object whose signal connections should be removed.
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
 * @param format Encoding format (PNG, JPEG, etc.)
 * @param quality Encoding quality (-1 is default)
 * @param token Token to replace in the CSS (default {{BACKGROUND_IMAGE}})
 * @return CSS with token replaced by data URL
 */
CORE_EXPORT QString applyPixmapToCss(QString css, const QPixmap& pixmap, const QByteArray& format = "PNG", int quality = -1, const QString& token = QStringLiteral("{{BACKGROUND_IMAGE}}"));

/**
 * Replace \p token in \p css with a CSS data URL formed from image at \p pathOrResource encoded as \p quality
 * @param css Input CSS
 * @param pathOrResource Path to image file or resource path (e.g. ":/images/background.png")
 * @param token Token to replace in the CSS (default {{BACKGROUND_IMAGE}})
 * @param scaleFactor Scale factor (0 is default, i.e. no scaling)
 * @param quality Encoding quality (-1 is default)
 * @return CSS with token replaced by data URL
 */
CORE_EXPORT QString applyResourceImageToCss(QString css, const QString& pathOrResource, const QString& token, float scaleFactor = 0, int quality = 90);

inline auto isLikelyUtf16 = [](const QByteArray& byteArray) {
    if (byteArray.size() < 2)
        return false;
    
    return static_cast<uchar>(byteArray[0]) == 0xFF && static_cast<uchar>(byteArray[1]) == 0xFE
        || static_cast<uchar>(byteArray[0]) == 0xFE && static_cast<uchar>(byteArray[1]) == 0xFF
        || (byteArray[0] == 0x00 && byteArray[1] == '{') || (byteArray[0] == '{' && byteArray[1] == 0x00);
};

/**
 * Ensure that \p byteArray is UTF-8 encoded, converting from UTF-16 if necessary
 * @param byteArray Input byte array
 */
CORE_EXPORT QByteArray ensureUtf8(QByteArray byteArray);

/**
 * Sanitize JSON by removing all whitespace outside of strings
 * @param utf8 Input JSON as UTF-8 byte array
 * @return Sanitized JSON as UTF-8 byte array
 */
CORE_EXPORT QByteArray sanitizeJsonWhitespaceOutsideStrings(const QByteArray& utf8);

/**
 * Minimal, practical parser for Content-Disposition filenames (prefers RFC 5987 filename* with UTF-8 when present,
 * but also accepts other charsets as provided in the header without strict validation).
 * @param contentDispositionRaw Raw Content-Disposition header value
 * @return Filename if found, otherwise an empty string
 */
CORE_EXPORT QString getFilenameFromContentDisposition(const QByteArray& contentDispositionRaw);

/**
 * Extract filename from \p effectiveUrl
 * @param effectiveUrl Input URL
 * @return Filename from URL path
 */
CORE_EXPORT QString getFilenameFromUrlPath(const QUrl& effectiveUrl);

/**
 * Extract filename from OSF Waterbutler metadata JSON \p raw
 * This is used to determine the filename of files downloaded from OSF storage providers.
 * @param raw Raw Waterbutler metadata JSON
 * @return Filename if found, otherwise an empty string
 */
CORE_EXPORT QString getFilenameFromWaterButlerMetadata(const QByteArray& raw);

/**
 * Print a key-value pair with optional indentation and aligned colons
 * @param key Key to print
 * @param value Value to print (default is an invalid QVariant, which will be printed as "null")
 * @param indent Number of spaces to indent (default is 0)
 * @param colonColumn Column at which to align the colon (default is 24)
 */
CORE_EXPORT void printLine(const QString& key, const QVariant& value = {}, int indent = 0, int colonColumn = 24);

/**
 *  Pretty print a QVariantMap
 *  @param variantMap Map to print
 */
CORE_EXPORT void prettyPrintVariantMap(const QVariantMap& variantMap);

/**
 * Convert a QVariantMap to a pretty-printed string with aligned colons
 * @param variantMap Map to convert
 * @return Pretty-printed string representation of the map
 */
CORE_EXPORT QString prettyVariantMapString(const QVariantMap& variantMap);

/**
 * Convert a QVariantMap to a pretty-printed string with indentation
 * @param variantMap Map to convert
 * @param indent Number of spaces to indent (default is 0)
 * @return Pretty-printed string representation of the map
 */
CORE_EXPORT QString variantMapToPrettyString(const QVariantMap& variantMap, int indent = 0);

/**
 * Convert a QVariant to an HTML string representation, handling maps and lists recursively
 * @param value QVariant to convert
 * @return HTML string representation of the QVariant
 */
CORE_EXPORT QString variantToHtml(const QVariant& value);

/**
 * Convert a QVariantMap to an HTML string representation, showing key-value pairs in a table
 * @param map Map to convert
 * @param depth Current recursion depth (default is 0)
 * @param maxDepth Maximum depth to recurse into nested maps/lists (default is 2)
 * @return HTML string representation of the map
 */
CORE_EXPORT QString variantMapToHtml(const QVariantMap& map, int depth = 0, int maxDepth = 2);

/**
 * Convert a QVariantList to an HTML string representation, showing items in an ordered list
 * @param list List to convert
 * @param depth Current recursion depth (default is 0)
 * @param maxDepth Maximum depth to recurse into nested maps/lists (default is 2)
 * @return HTML string representation of the list
 */
CORE_EXPORT QString variantListToHtml(const QVariantList& list, int depth, int maxDepth);

/**
 * Convert a QVariant to an HTML string representation, handling maps and lists recursively
 * @param value QVariant to convert
 * @param depth Current recursion depth (default is 0)
 * @param maxDepth Maximum depth to recurse into nested maps/lists (default is 2)
 * @return HTML string representation of the QVariant
 */
CORE_EXPORT QString variantToHtml(const QVariant& value, int depth, int maxDepth);

/**
 * Log memory usage of the current process with an optional label
 * @param label Label to prefix the log message with (default is an empty string)
 */
CORE_EXPORT void logMemory(const QString& label);

/**
 * Find a nested value in a QVariantMap by a dot-separated path (e.g. "a.b.c" to find root["a"]["b"]["c"])
 * @param root Root map to search within
 * @param path Dot-separated path to the value to find
 * @return Value at the specified path, or an invalid QVariant if not found
 */
CORE_EXPORT QVariant findNested(const QVariantMap& root, const QStringList& path);

/**
 * Describe the keys of a QVariantMap in a human-readable string, showing up to \p maxKeys keys and indicating if there are more
 * @param map Map whose keys to describe
 * @param maxKeys Maximum number of keys to show (default is 20)
 * @return String describing the keys of the map
 */
CORE_EXPORT QString describeVariantMapKeys(const QVariantMap& map, qsizetype maxKeys = 20);

/**
 * Dump a byte array as a hex string, showing up to \p count bytes
 * @param bytes Byte array to dump
 * @param count Maximum number of bytes to show (default is 100)
 * @return Hex string representation of the byte array
 */
CORE_EXPORT QString dumpHex(const QByteArray& bytes, qsizetype count = 100);;

/**
 * @brief Converts stack trace frames to a QVariantList.
 *
 * Each frame is represented as a QVariantMap containing the available symbolic
 * stack-frame fields.
 *
 * @param stackTrace Stack trace frames to convert.
 * @return QVariantList representation of the stack trace.
 */
CORE_EXPORT QVariantList stackTraceToVariantList(const StackTrace& stackTrace);

/**
 * @brief Converts a QVariantList to stack trace frames.
 *
 * Each list entry is expected to contain a QVariantMap with stack-frame fields.
 * Missing fields are left at their default StackFrame values.
 *
 * @param frames QVariantList representation of stack frames.
 * @return Stack trace frames reconstructed from the list.
 */
CORE_EXPORT StackTrace stackTraceFromVariantList(const QVariantList& frames);

CORE_EXPORT QVariantList stackTraceFunctionList(const StackTrace& stackTrace);
}
