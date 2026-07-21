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
 * @brief Replaces all occurrences of a substring.
 * @param inputString Input string.
 * @param from Substring to replace.
 * @param to Replacement substring.
 * @return String with replacements applied.
 */
CORE_EXPORT std::string replaceAll(std::string inputString, const std::string& from, const std::string& to);

/**
 * @brief Removes CR/LF characters from a string.
 * @param inputString Input string.
 * @return String without newline characters.
 */
CORE_EXPORT std::string stripNewLines(std::string inputString);

/**
 * @brief Escapes text for use inside a CSS double-quoted string.
 * @param inputString Input string.
 * @return CSS-escaped string.
 */
CORE_EXPORT std::string escapeCssDq(std::string inputString);

/**
 * @brief Converts a pixmap to a CSS data source string.
 * @param pixmap Pixmap to encode.
 * @return CSS url(...) data source string.
 */
CORE_EXPORT std::string pixmapToCssSrc(const QPixmap& pixmap);

/**
 * @brief Detects an image MIME type from encoded bytes.
 * @param byteArray Encoded image bytes.
 * @return MIME type, when detected.
 */
CORE_EXPORT QString mimeForFormat(const QByteArray& byteArray);

/**
 * @brief Normalizes an image format from a file suffix.
 * @param path File path whose suffix should be inspected.
 * @return Normalized image format such as PNG or JPEG.
 */
CORE_EXPORT QByteArray normalizeFormatFromSuffix(const QString& path);

/**
 * @brief Chooses an encoding format for an image.
 *
 * If the hinted format is empty, PNG is preferred for images with alpha and
 * JPEG otherwise.
 *
 * @param img Image to encode.
 * @param hinted Optional hinted format.
 * @return Chosen image format.
 */
CORE_EXPORT QByteArray chooseFormatForImage(const QImage& img, const QByteArray& hinted);

/**
 * @brief Converts a pixmap to a base64 data URL.
 * @param pixmap Pixmap to encode.
 * @param fmt Encoding format.
 * @param quality Encoding quality, or -1 for the Qt default.
 * @return Data URL string.
 */
CORE_EXPORT QString pixmapToDataUrl(const QPixmap& pixmap, const QByteArray& fmt = "PNG", int quality = -1);

/**
 * @brief Replaces a CSS token with an encoded pixmap data URL.
 * @param css CSS text to update.
 * @param pixmap Pixmap to encode.
 * @param format Encoding format.
 * @param quality Encoding quality, or -1 for the Qt default.
 * @param token Token to replace in the CSS.
 * @return CSS with the token replaced by a data URL.
 */
CORE_EXPORT QString applyPixmapToCss(QString css, const QPixmap& pixmap, const QByteArray& format = "PNG", int quality = -1, const QString& token = QStringLiteral("{{BACKGROUND_IMAGE}}"));

/**
 * @brief Replaces a CSS token with an encoded image data URL.
 * @param css CSS text to update.
 * @param pathOrResource Image file path or Qt resource path.
 * @param token Token to replace in the CSS.
 * @param scaleFactor Optional image scale factor, or 0 to keep the original size.
 * @param quality Encoding quality, or -1 for the Qt default.
 * @return CSS with the token replaced by a data URL.
 */
CORE_EXPORT QString applyResourceImageToCss(QString css, const QString& pathOrResource, const QString& token, float scaleFactor = 0, int quality = 90);

/** Predicate that detects common UTF-16 byte-order markers and JSON-like UTF-16 prefixes. */
inline auto isLikelyUtf16 = [](const QByteArray& byteArray) {
    if (byteArray.size() < 2)
        return false;
    
    return static_cast<uchar>(byteArray[0]) == 0xFF && static_cast<uchar>(byteArray[1]) == 0xFE
        || static_cast<uchar>(byteArray[0]) == 0xFE && static_cast<uchar>(byteArray[1]) == 0xFF
        || (byteArray[0] == 0x00 && byteArray[1] == '{') || (byteArray[0] == '{' && byteArray[1] == 0x00);
};

/**
 * @brief Ensures that a byte array is UTF-8 encoded.
 * @param byteArray Input byte array.
 * @return UTF-8 byte array, converting from UTF-16 when detected.
 */
CORE_EXPORT QByteArray ensureUtf8(QByteArray byteArray);

/**
 * @brief Removes whitespace outside JSON strings.
 * @param utf8 JSON text encoded as UTF-8.
 * @return Sanitized UTF-8 JSON text.
 */
CORE_EXPORT QByteArray sanitizeJsonWhitespaceOutsideStrings(const QByteArray& utf8);

/**
 * @brief Extracts a filename from a Content-Disposition header.
 *
 * RFC 5987 filename* values are preferred when present.
 *
 * @param contentDispositionRaw Raw Content-Disposition header value.
 * @return Filename when found, otherwise an empty string.
 */
CORE_EXPORT QString getFilenameFromContentDisposition(const QByteArray& contentDispositionRaw);

/**
 * @brief Extracts a filename from a URL path.
 * @param effectiveUrl URL to inspect.
 * @return Filename from the URL path.
 */
CORE_EXPORT QString getFilenameFromUrlPath(const QUrl& effectiveUrl);

/**
 * @brief Extracts a filename from OSF Waterbutler metadata JSON.
 * @param raw Raw Waterbutler metadata JSON.
 * @return Filename when found, otherwise an empty string.
 */
CORE_EXPORT QString getFilenameFromWaterButlerMetadata(const QByteArray& raw);

/**
 * @brief Prints a key-value pair with optional indentation.
 * @param key Key to print.
 * @param value Value to print.
 * @param indent Number of spaces to indent.
 * @param colonColumn Column at which to align the colon.
 */
CORE_EXPORT void printLine(const QString& key, const QVariant& value = {}, int indent = 0, int colonColumn = 24);

/**
 * @brief Prints a QVariantMap in a readable format.
 * @param variantMap Map to print.
 */
CORE_EXPORT void prettyPrintVariantMap(const QVariantMap& variantMap);

/**
 * @brief Converts a QVariantMap to a pretty-printed string with aligned colons.
 * @param variantMap Map to convert.
 * @return Pretty-printed string representation of the map.
 */
CORE_EXPORT QString prettyVariantMapString(const QVariantMap& variantMap);

/**
 * @brief Converts a QVariantMap to an indented pretty-printed string.
 * @param variantMap Map to convert.
 * @param indent Number of spaces to indent.
 * @return Pretty-printed string representation of the map.
 */
CORE_EXPORT QString variantMapToPrettyString(const QVariantMap& variantMap, int indent = 0);

/**
 * @brief Converts a QVariant to HTML.
 * @param value QVariant to convert.
 * @return HTML representation of the QVariant.
 */
CORE_EXPORT QString variantToHtml(const QVariant& value);

/**
 * @brief Converts a QVariantMap to HTML.
 * @param map Map to convert.
 * @param depth Current recursion depth.
 * @param maxDepth Maximum recursion depth for nested maps and lists.
 * @return HTML representation of the map.
 */
CORE_EXPORT QString variantMapToHtml(const QVariantMap& map, int depth = 0, int maxDepth = 2);

/**
 * @brief Converts a QVariantList to HTML.
 * @param list List to convert.
 * @param depth Current recursion depth.
 * @param maxDepth Maximum recursion depth for nested maps and lists.
 * @return HTML representation of the list.
 */
CORE_EXPORT QString variantListToHtml(const QVariantList& list, int depth, int maxDepth);

/**
 * @brief Converts a QVariant to HTML with explicit recursion controls.
 * @param value QVariant to convert.
 * @param depth Current recursion depth.
 * @param maxDepth Maximum recursion depth for nested maps and lists.
 * @return HTML representation of the QVariant.
 */
CORE_EXPORT QString variantToHtml(const QVariant& value, int depth, int maxDepth);

/**
 * @brief Logs memory usage of the current process.
 * @param label Optional label to include in the log message.
 */
CORE_EXPORT void logMemory(const QString& label);

/**
 * @brief Finds a nested value in a QVariantMap by path.
 * @param root Root map to search.
 * @param path Path elements to follow.
 * @return Value at the path, or an invalid QVariant if not found.
 */
CORE_EXPORT QVariant findNested(const QVariantMap& root, const QStringList& path);

/**
 * @brief Describes the keys of a QVariantMap.
 * @param map Map whose keys should be described.
 * @param maxKeys Maximum number of keys to show.
 * @return Human-readable key summary.
 */
CORE_EXPORT QString describeVariantMapKeys(const QVariantMap& map, qsizetype maxKeys = 20);

/**
 * @brief Dumps bytes as a hexadecimal string.
 * @param bytes Byte array to dump.
 * @param count Maximum number of bytes to include.
 * @return Hexadecimal byte string.
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

/**
 * @brief Extracts function names from a stack trace.
 * @param stackTrace Stack trace frames to inspect.
 * @return QVariantList containing the function name from each frame.
 */
CORE_EXPORT QVariantList stackTraceFunctionList(const StackTrace& stackTrace);
}
