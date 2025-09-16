// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Miscellaneous.h"

#include "Exception.h"
#include "Icon.h"

#include <QAction>
#include <QBuffer>
#include <QResource>
#include <QEventLoop>
#include <QLayout>
#include <QLayoutItem>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QTcpSocket>
#include <QUrl>
#include <QVariant>
#include <QRegularExpression>

#include <exception>

namespace mv::util
{

QString getIntegerCountHumanReadable(const double& count)
{
    if (count >= 0 && count < 1000)
        return QString::number(count);

    if (count >= 1000 && count < 1000000)
        return QString("%1 k").arg(QString::number(count / 1000.0f, 'f', 1));

    if (count >= 1000000)
        return QString("%1 mln").arg(QString::number(count / 1000000.0f, 'f', 1));

    return "";
}

std::uint64_t parseByteSize(const QString& input)
{
    QString normalized = input.trimmed().toUpper();

    QRegularExpression regex(R"(^(\d+(?:\.\d+)?)\s*([KMGTPE]?I?B)$)");
    QRegularExpressionMatch match = regex.match(normalized);

    if (!match.hasMatch())
        throw std::invalid_argument("Invalid byte size format");

    double number = match.captured(1).toDouble();
    QString unit = match.captured(2);

    // Normalize all units to IEC (treat KB as KiB, MB as MiB, etc.)
    static const QHash<QString, std::uint64_t> binaryMultipliers{
        { "B",    1ULL },
        { "KB",   1024ULL },
        { "KIB",  1024ULL },
        { "MB",   1024ULL * 1024 },
        { "MIB",  1024ULL * 1024 },
        { "GB",   1024ULL * 1024 * 1024 },
        { "GIB",  1024ULL * 1024 * 1024 },
        { "TB",   1024ULL * 1024 * 1024 * 1024 },
        { "TIB",  1024ULL * 1024 * 1024 * 1024 },
        { "PB",   1024ULL * 1024 * 1024 * 1024 * 1024 },
        { "PIB",  1024ULL * 1024 * 1024 * 1024 * 1024 },
        { "EB",   1024ULL * 1024 * 1024 * 1024 * 1024 * 1024 },
        { "EIB",  1024ULL * 1024 * 1024 * 1024 * 1024 * 1024 }
    };

    if (binaryMultipliers.contains(unit))
        return static_cast<std::uint64_t>(number * binaryMultipliers[unit]);

    throw std::invalid_argument("Unknown byte unit: " + unit.toStdString());
}

QString getNoBytesHumanReadable(std::uint64_t byteCount, bool useIEC /*= true*/)
{
    // Units for IEC (base 1024)
    const QStringList iecUnits{ "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB" };

    // Units for SI (base 1000)
    const QStringList siUnits{ "B", "KB", "MB", "GB", "TB", "PB", "EB" };

    const auto& units = useIEC ? iecUnits : siUnits;
    const double base = useIEC ? 1024.0 : 1000.0;

    int i = 0;
    double size = static_cast<double>(byteCount);

    while (size >= base && i < units.size() - 1) {
        size /= base;
        ++i;
    }

    return QString::number(size, 'f', 2) + " " + units[i];
}


QString getTabIndentedMessage(QString message, const std::uint32_t& tabIndex)
{
	static constexpr std::uint32_t tabSize = 4;

	QString indentation;

	for (std::uint32_t i = 0; i < tabIndex * tabSize; i++)
		indentation += " ";

	message.insert(0, indentation);

	return message;
}

CORE_EXPORT QString getColorAsCssString(const QColor& color, bool alpha /*= true*/)
{
    if (alpha)
        return QString("rgba(%1, %2, %3, %4)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()), QString::number(color.alpha()));
    else
        return QString("rgb(%1, %2, %3)").arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
}

bool urlExists(const QString& urlString)
{
    QUrl url(urlString);

    QTcpSocket socket;

    socket.connectToHost(url.host(), 80);

    if (socket.waitForConnected()) {
        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\n" "Host: " + url.host().toUtf8() + "\r\n\r\n");

        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();

            if (bytes.contains("200 OK"))
                return true;
        }
    }
    return false;
}

void replaceLayout(QWidget* widget, QLayout* newLayout, bool removeWidgets)
{
    Q_ASSERT(widget && newLayout);

    try {
        if (!widget)
            throw std::runtime_error("Widget may not be a nullptr");

        if (!newLayout)
            throw std::runtime_error("New layout may not be a nullptr");

        if (auto oldLayout = widget->layout()) {
            QLayoutItem* item = nullptr;

            while ((item = oldLayout->takeAt(0)) != nullptr) {
                if (removeWidgets)
                    delete item->widget();

                delete item;
            }

            delete oldLayout;
        }

        widget->setLayout(newLayout);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to replace widget layout", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to replace widget layout");
    }
}

void clearLayout(QLayout* layout, bool removeWidgets)
{
    QLayoutItem* layoutItem;

    while ((layoutItem = layout->takeAt(0)) != nullptr) {
        if (removeWidgets)
			delete layoutItem->widget();

        delete layoutItem;
    }
}

void setLayoutContentsMargins(QLayout* layout, std::int32_t margin)
{
    Q_ASSERT(layout);

    if (!layout)
        return;

    layout->setContentsMargins(margin, margin, margin, margin);
}

QIcon getAlignmentIcon(const Qt::Alignment& alignment)
{
    constexpr auto size     = 128;
    constexpr auto halfSize = size / 2;
    constexpr auto offset   = 25;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.setWindow(0, 0, size, size);

    const auto drawCorner = [&painter, offset, size, halfSize](const Qt::Alignment& alignment, int radius = 7) -> void {
        QPoint dotPosition;

        if (alignment & Qt::AlignLeft)
            dotPosition.setX(offset);

        if (alignment & Qt::AlignHCenter)
            dotPosition.setX(halfSize);

        if (alignment & Qt::AlignRight)
            dotPosition.setX(size - offset);

        if (alignment & Qt::AlignTop)
            dotPosition.setY(offset);

        if (alignment & Qt::AlignVCenter)
            dotPosition.setY(halfSize);

        if (alignment & Qt::AlignBottom)
            dotPosition.setY(size - offset);

        painter.setBrush(QBrush(Qt::black));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(dotPosition, radius, radius);
	};

    drawCorner(Qt::AlignTop | Qt::AlignLeft);
    drawCorner(Qt::AlignTop | Qt::AlignRight);
    drawCorner(Qt::AlignBottom | Qt::AlignLeft);
    drawCorner(Qt::AlignBottom | Qt::AlignRight);

    drawCorner(alignment, 25);

    return gui::createIcon(pixmap);
}

QVariant setValueByPath(QVariant variant, const QString& path, const QVariant& value)
{
    QStringList components = path.split('/', Qt::SkipEmptyParts);

    if (components.isEmpty()) {
        qWarning() << "Path is empty. Cannot set value.";
        return {};
    }

    if (components.count() == 1) {
        if (variant.typeId() == QMetaType::QVariantMap) {
            auto map = variant.toMap();

            map[components.first()] = value;

            return map;
        }

        if (variant.typeId() == QMetaType::QVariantList) {
            return {};
        }

    	qWarning() << "Cannot set value: leaf not QVariantMap nor QVariantList";
        return {};
    }

    auto map = variant.toMap();

    const auto name = components.first();

    if (!map.contains(name)) {
        map[name] = QVariantMap();

        components.removeFirst();

        map[name] = setValueByPath(map[name], components.join(","), value);

        return map;
    }

	components.removeFirst();

    map[components.first()] = setValueByPath(map[components.first()], components.join(","), value);

    return map;
}

QVariant getValueByPath(const QVariant& root, const QString& path, const QVariant& valueIfNotFound /*= QVariant()*/)
{
	QStringList     components = path.split('/', Qt::SkipEmptyParts);
	const QVariant* current    = &root;

    QVariant foundValue;

	for (const QString& key : components) {
		if (current->typeId() == QMetaType::QVariantMap) {
			const auto map = current->toMap();

			if (map.contains(key)) {
				foundValue = map[key];
			}
			else {
				return valueIfNotFound; // Return invalid QVariant if the key doesn't exist
			}
		}
		else {
			return valueIfNotFound; // Return invalid QVariant if the current node isn't a map
		}
	}

	return foundValue; // Return the found value
}

QString gifToBase64(const QByteArray& gifByteArray)
{
    try {
        if (gifByteArray.isNull())
            throw std::runtime_error("Supplied GIF byte array is not valid");

        return QString::fromLatin1(gifByteArray.toBase64());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to convert GIF image to base64-encoded string", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to convert GIF image to base64-encoded string");
    }

    return {};
}

QString embedGifFromBase64(const QString& gifBase64)
{
    try {
        if (gifBase64.isEmpty())
            throw std::runtime_error("Supplied image is not valid");

        return QString("<img src=\"data:image/gif;base64,%1\" alt=\"Base64 GIF\" />").arg(gifBase64);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to embed GIF image", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to embed GIF image");
    }

    return {};
}

QString embedGifFromResource(const QString& resourcePath)
{
	try {
		const auto gifResource = QResource(resourcePath);

		if (!gifResource.isValid())
			throw std::runtime_error(QString("GIF resource is not valid: %1").arg(resourcePath).toStdString());

		const auto gifDataBase64 = gifToBase64(QByteArray(reinterpret_cast<const char*>(gifResource.data()), gifResource.size()));

		return embedGifFromBase64(gifDataBase64);
	}
	catch (std::exception& e)
	{
		exceptionMessageBox("Unable to embed GIF image from resource", e);
	}
	catch (...) {
		exceptionMessageBox("Unable to embed GIF image from resource");
	}

	return {};
}

void waitForDuration(int milliSeconds)
{
	QEventLoop localEventLoop;

	QTimer::singleShot(milliSeconds, &localEventLoop, &QEventLoop::quit);

	localEventLoop.exec();
}

void disconnectRecursively(const QObject* object)
{
    Q_ASSERT(object);

	if (!object)
		return;

    [[maybe_unused]] auto result = object->disconnect();

	for (auto child : object->children())
		disconnectRecursively(child);
}

std::string replaceAll(std::string inputString, const std::string& from, const std::string& to)
{
	if (from.empty())
		return inputString;

	std::size_t pos = 0;

	while ((pos = inputString.find(from, pos)) != std::string::npos) {
		inputString.replace(pos, from.length(), to);
        
		pos += to.length();
	}

	return inputString;
}

std::string stripNewLines(std::string inputString)
{
	inputString.erase(std::remove(inputString.begin(), inputString.end(), '\n'), inputString.end());
	inputString.erase(std::remove(inputString.begin(), inputString.end(), '\r'), inputString.end());

	return inputString;
}

std::string escapeCssDq(std::string inputString)
{

	std::string out; out.reserve(inputString.size() * 11 / 10);

	for (char c : inputString) {
		if (c == '\\' || c == '\"') out.push_back('\\');
			out.push_back(c);
	}

	return out;
}

}
