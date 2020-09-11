#include "Application.h"
#include "util/FontAwesome.h"

#include <stdexcept>

#include <QDebug>
#include <QMessageBox>

namespace hdps {

hdps::Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _settings()
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14)));

    /* Uncomment to test the icon font versioning system
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(1, 12)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 2)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 7)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(7, 0)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 6)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 1)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 11)));
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 0)));
    
    qDebug() << _iconFonts;

    auto testIconFont = [this](const QString& iconFontname, const std::int32_t& majorVersion = -1, const std::int32_t& minorVersion = -1) {
        QDebug debug = qDebug();

        debug.noquote();

        try {
            const auto& iconFont = _iconFonts.getIconFont(iconFontname, majorVersion, minorVersion);

            debug << iconFontname + "-" + IconFont::getSearchVersionString(majorVersion, minorVersion) + ":" << "\t" << IconFont::getSearchVersionString(iconFont.getMajorVersion(), iconFont.getMinorVersion());
        } catch (IconFonts::IconFontNotFoundException& e) {
            debug << iconFontname + "-" + IconFont::getSearchVersionString(majorVersion, minorVersion) + ":" << "\t" << "not found";
        }
        catch (...) {
            qDebug() << "Unhandled exception";
        }
    };

    testIconFont("FontAwesome");
    testIconFont("FontAwesome", 5);
    testIconFont("FontAwesome", 5, 5);
    testIconFont("FontAwesome", 5, 11);
    testIconFont("FontAwesome", -1, 11);
    testIconFont("FontAwesome", 3, 0);
    testIconFont("FontAwesome", 5, 14);
    */

    _settings.setValue("test", 1);
}

hdps::Application* hdps::Application::current()
{
    try
    {
        auto hdpsApplication = dynamic_cast<Application*>(instance());

        if (hdpsApplication == nullptr)
            throw std::runtime_error("Current application instance is not an HDPS application");

        return hdpsApplication;
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An application error occurred", e.what());
        return nullptr;
    }
}

const IconFont& hdps::Application::getIconFont(const QString& name, const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/)
{
    return current()->_iconFonts.getIconFont(name, majorVersion, minorVersion);
}

QVariant Application::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return _settings.value(path, defaultValue);
}

void Application::setSetting(const QString& path, const QVariant& value)
{
    _settings.setValue(path, value);
}

QVariant Application::getPluginSetting(const QString& pluginName, const QString& path, const QVariant& defaultValue /*= QVariant()*/)
{
    return current()->getSetting(QString("%1/%2").arg(pluginName, path), defaultValue);
}

void Application::setPluginSetting(const QString& pluginName, const QString& path, const QVariant& value)
{
    current()->setSetting(QString("%1/%2").arg(pluginName, path), value);
}

}