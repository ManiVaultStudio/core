#include "IconFonts.h"

#include <QDebug>
#include <QMessageBox>

namespace hdps {

IconFonts::IconFonts() :
    _iconFonts()
{
}

const hdps::IconFont& IconFonts::getIconFont(const QString& name, const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/) const
{
    if (majorVersion < 0) {
        if (minorVersion >= 0)
            throw IconFontNotFoundException(name, majorVersion, minorVersion);

        if (!_iconFonts.contains(name))
            throw IconFontNotFoundException(name, majorVersion, minorVersion);

        return *(_iconFonts[name].last().get());
    }
    else {
        if (minorVersion < 0) {
            Fonts filtered;

            std::copy_if(_iconFonts[name].begin(), _iconFonts[name].end(), std::back_inserter(filtered), [&majorVersion](auto iconFont) {
                return iconFont->getMajorVersion() == majorVersion;
            });

            if (filtered.isEmpty())
                throw IconFontNotFoundException(name, majorVersion, minorVersion);

            return *(filtered.last().get());
        }
        else {
            Fonts filtered;

            std::copy_if(_iconFonts[name].begin(), _iconFonts[name].end(), std::back_inserter(filtered), [&majorVersion, &minorVersion](auto iconFont) {
                return iconFont->getMajorVersion() == majorVersion && iconFont->getMinorVersion() == minorVersion;
            });

            if (filtered.isEmpty())
                throw IconFontNotFoundException(name, majorVersion, minorVersion);

            return *(filtered.last().get());
        }
    }   
}

QIcon IconFonts::getIcon(const QString& fontName, const QString& iconName, const QSize& iconSize /*= QSize(24, 24)*/, const QColor& iconForegroundGolor/*= QColor(0, 0, 0, 255)*/, const QColor& iconBackgroundColor/*= Qt::transparent*/) const
{
    return getIconFont(fontName).getIcon(iconName, iconSize, iconForegroundGolor, iconBackgroundColor);
}

QStringList IconFonts::getFontNames() const
{
    return _iconFonts.keys();
}

hdps::IconFonts::Fonts IconFonts::getIconFonts(const QString& fontName) const
{
    try
    {
        return _iconFonts[fontName];
    }
    catch (...) {
        QMessageBox::critical(nullptr, "An icon fonts error occurred", "Failed to fetch icon font due to an unhandled exception");

        return Fonts();
    }
}

void IconFonts::add(const QSharedPointer<IconFont>& iconFont)
{
    try
    {
        _iconFonts[iconFont->getName()].append(iconFont);

        std::sort(_iconFonts[iconFont->getName()].begin(), _iconFonts[iconFont->getName()].end(), [] (auto iconFontA, auto iconFontB) {
            if (iconFontA->getMajorVersion() == iconFontB->getMajorVersion() && iconFontA->getMinorVersion() < iconFontB->getMinorVersion())
                return true;

            if (iconFontA->getMajorVersion() < iconFontB->getMajorVersion())
                return true;

            return false;
        });
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An icon fonts error occurred", QString("Failed to add icon font: %1").arg(e.what()));
    }
    catch (...) {
        QMessageBox::critical(nullptr, "An icon fonts error occurred", "Failed to add icon font due to unhandled exception");
    }
}

}