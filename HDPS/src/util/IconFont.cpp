#include "IconFont.h"

#include <stdexcept>

#include <QFontDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>

namespace hdps {

IconFont::IconFont(const QString& name, const std::uint32_t& majorVersion, const std::uint32_t& minorVersion) :
    _name(name),
    _majorVersion(majorVersion),
    _minorVersion(minorVersion),
    _fontResourceName(QString(":/IconFonts/%1.otf").arg(getFullName())),
    _fontFamily(),
    _characters()
{
    try
    {
        const auto result = QFontDatabase::addApplicationFont(_fontResourceName);

        if (result < 0) {
            throw std::runtime_error(QString("Unable to load %1").arg(getFullName()).toStdString().c_str());
        }
        else {
            qDebug() << "Loaded" << getFullName() << QFontDatabase::applicationFontFamilies(result);

            _fontFamily = QFontDatabase::applicationFontFamilies(result).first();
        }
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An icon font error occurred", e.what());
    }
    catch (...) {
        QMessageBox::critical(nullptr, "An icon font error occurred", "Unable to load icon font due to an unhandled exception");
    }
}

QString IconFont::getName() const
{
    return _name;
}

QString IconFont::getFullName() const
{
    return QString("%1-%2").arg(_name, getVersionString());
}

std::uint32_t IconFont::getMajorVersion() const
{
    return _majorVersion;
}

std::uint32_t IconFont::getMinorVersion() const
{
    return _minorVersion;
}

QString IconFont::getVersionString() const
{
    return QString("%1.%2").arg(QString::number(_majorVersion), QString::number(_minorVersion));
}

QString IconFont::getSearchVersionString(const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/)
{
    const auto major = majorVersion < 0 ? "x" : QString::number(majorVersion);
    const auto minor = minorVersion < 0 ? "x" : QString::number(minorVersion);

    return QString("%2.%3").arg(major, minor);
}

QFont IconFont::getFont(const int& pointSize /*= -1*/) const
{
    return QFont(_fontFamily, pointSize);
}

QIcon IconFont::getIcon(const QString& name, const QSize& size /*= QSize(24, 24)*/, const QColor& foregroundColor/*= QColor(0, 0, 0, 255)*/, const QColor& backgroundColor/*= Qt::transparent*/) const
{
    try
    {
        QPixmap pixmap(size);

        pixmap.fill(backgroundColor);

        const auto iconRectangle = QRect(0, 0, size.width(), size.height());
        const auto iconText     = getIconCharacter(name);

        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(foregroundColor);
        painter.setFont(getFont(10));

        QFontMetrics fontMetrics(painter.font());

        const auto scaleX       = iconRectangle.width() * 1.0 / fontMetrics.horizontalAdvance(iconText);
        const auto scaleY       = iconRectangle.height() * 1.0 / fontMetrics.height();
        const auto scaleOverall = std::min(scaleX, scaleY);

        painter.translate(iconRectangle.center());
        painter.scale(scaleOverall, scaleOverall);
        painter.translate(-iconRectangle.center());
        painter.drawText(iconRectangle, Qt::AlignCenter, iconText);

        return QIcon(pixmap);
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to retrieve icon", e.what());
        return QIcon();
    }
}

void IconFont::initialize()
{
    qDebug() << QString("Initializing %1").arg(getFullName());

    _characters.clear();
}

}