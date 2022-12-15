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
#include <QDateTime>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

hdps::Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _core(nullptr),
    _iconFonts(),
    _settings(),
    _serializationTemporaryDirectory(),
    _serializationAborted(false),
    _logger(),
    _globalSettingsAction(nullptr),
    _exitAction(nullptr)
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14)));
    
    _logger.initialize();
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
    Q_ASSERT(core != nullptr);

    _core = core;

    emit coreSet(_core);

    //connect(_core, &CoreInterface::initialized, this, [this]() -> void {
    //    _globalSettingsAction = new TriggerAction(this, "Global &Settings...");
    //    _exitAction = new TriggerAction(this, "Exit");

    //    _globalSettingsAction->setShortcut(QKeySequence("Ctrl+E"));
    //    _globalSettingsAction->setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    //    _globalSettingsAction->setToolTip("Modify global HDPS settings");

    //    _exitAction->setShortcut(QKeySequence("Alt+F4"));
    //    _exitAction->setIcon(Application::getIconFont("FontAwesome").getIcon("sign-out-alt"));
    //    _exitAction->setToolTip("Exit the HDPS application");

    //    connect(_exitAction, &TriggerAction::triggered, Application::current(), &Application::quit);
    //});
}

hdps::CoreInterface* Application::core()
{
    return current()->getCore();
}

hdps::gui::TriggerAction& Application::getGlobalSettingsAction()
{
    return *_globalSettingsAction;
}

hdps::gui::TriggerAction& Application::getExitAction()
{
    return *_exitAction;
}

QVariant Application::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return _settings.value(path, defaultValue);
}

void Application::setSetting(const QString& path, const QVariant& value)
{
    _settings.setValue(path, value);
}

Logger& Application::getLogger()
{
    return current()->_logger;
}

QString Application::getSerializationTemporaryDirectory()
{
    return current()->_serializationTemporaryDirectory;
}

void Application::setSerializationTemporaryDirectory(const QString& serializationTemporaryDirectory)
{
    current()->_serializationTemporaryDirectory = serializationTemporaryDirectory;
}

bool Application::isSerializationAborted()
{
    return current()->_serializationAborted;
}

void Application::setSerializationAborted(bool serializationAborted)
{
    current()->_serializationAborted = serializationAborted;
}

}
