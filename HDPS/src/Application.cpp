#include "Application.h"

#include "CoreInterface.h"
#include "util/FontAwesome.h"
#include "util/Exception.h"
#include "actions/WidgetAction.h"

#include <stdexcept>

#include <QDebug>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QFileDialog>
#include <QDir>

#define _VERBOSE

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

hdps::Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _iconFonts(),
    _settings(),
    _currentProjectFilePath(),
    _serializationTemporaryDirectory()
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14)));
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

hdps::CoreInterface* Application::getCore()
{
    Q_ASSERT(_core != nullptr);

    return _core;
}

void Application::setCore(CoreInterface* core)
{
    Q_ASSERT(_core != nullptr);

    _core = core;
}

hdps::CoreInterface* Application::core()
{
    return current()->getCore();
}

QVariant Application::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return _settings.value(path, defaultValue);
}

void Application::setSetting(const QString& path, const QVariant& value)
{
    _settings.setValue(path, value);
}

QString Application::getCurrentProjectFilePath() const
{
    return _currentProjectFilePath;
}

void Application::setCurrentProjectFilePath(const QString& currentProjectFilePath)
{
    if (currentProjectFilePath == _currentProjectFilePath)
        return;

    _currentProjectFilePath = currentProjectFilePath;

    // Notify others that the current project file path changed
    emit currentProjectFilePathChanged(_currentProjectFilePath);
}

void Application::addRecentProjectFilePath(const QString& recentProjectFilePath)
{
    // Get recent projects from settings
    auto recentProjects = getSetting("Projects/Recent", QVariantList()).toList();

    // Add to recent projects if not already in there
    if (!recentProjects.contains(recentProjectFilePath))
        recentProjects << recentProjectFilePath;

    // Save settings
    setSetting("Projects/Recent", recentProjects);
}

QString Application::getSerializationTemporaryDirectory()
{
    return current()->_serializationTemporaryDirectory;
}

}
