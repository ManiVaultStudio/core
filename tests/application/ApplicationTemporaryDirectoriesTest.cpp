#include <Application.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QTemporaryDir>
#include <QVariantMap>

#include <cstdio>
#include <initializer_list>
#include <utility>

namespace
{
constexpr auto temporaryDirectorySettingPath = "GlobalSettings/Temporary/ApplicationTemporaryDirectory";

bool require(bool condition, const QString& message)
{
    if (condition)
        return true;

    std::fprintf(stderr, "FAIL: %s\n", message.toLocal8Bit().constData());
    return false;
}

bool isWithin(const QString& path, const QString& root)
{
    const auto normalizedPath = QDir::cleanPath(QDir(path).absolutePath());
    const auto normalizedRoot = QDir::cleanPath(QDir(root).absolutePath());

    return normalizedPath.compare(normalizedRoot, Qt::CaseInsensitive) == 0 || normalizedPath.startsWith(normalizedRoot + "/", Qt::CaseInsensitive);
}

void initializeApplicationAttributes()
{
    int bootstrapArgc = 1;
    char bootstrapName[] = "MV_Application_TemporaryDirectoriesTest";
    char* bootstrapArgv[] = {bootstrapName, nullptr};
    QCoreApplication bootstrapApplication(bootstrapArgc, bootstrapArgv);
    mv::Application::initializeAttributes();
}

class TemporaryDirectorySettingGuard
{
public:
    TemporaryDirectorySettingGuard()
        : _settings(), _hadValue(_settings.contains(temporaryDirectorySettingPath)), _previousValue(_settings.value(temporaryDirectorySettingPath))
    {
    }

    ~TemporaryDirectorySettingGuard()
    {
        if (_hadValue)
            _settings.setValue(temporaryDirectorySettingPath, _previousValue);
        else
            _settings.remove(temporaryDirectorySettingPath);

        _settings.sync();
    }

    void setRoot(const QString& root)
    {
        _settings.setValue(temporaryDirectorySettingPath, QVariantMap{{"Value", root}});
        _settings.sync();
    }

private:
    QSettings _settings;
    bool _hadValue;
    QVariant _previousValue;
};

bool runConfiguredRootTest()
{
    QTemporaryDir configuredRoot;
    if (!require(configuredRoot.isValid(), "Could not create the configured-root fixture"))
        return false;

    const auto configuredPath = configuredRoot.filePath("application-temp");

    initializeApplicationAttributes();
    TemporaryDirectorySettingGuard settingGuard;
    settingGuard.setRoot(configuredPath);

    int argc = 1;
    char applicationName[] = "MV_Application_TemporaryDirectoriesTest";
    char* argv[] = {applicationName, nullptr};
    mv::Application application(argc, argv);

    bool success = true;
    success &= require(QDir(configuredPath).exists(), "The configured application temporary root was not created");
    success &= require(QDir::cleanPath(application.getTemporaryDirRoot()) == QDir::cleanPath(QDir(configuredPath).absolutePath()), "The application did not use the configured temporary root");

    const auto sessionPath = application.getTemporaryDir().path();
    success &= require(isWithin(sessionPath, configuredPath), "The session temporary directory is outside the configured root");

    const std::initializer_list<std::pair<mv::Application::TemporaryDirectoryType, const char*>> categories = {
        {mv::Application::TemporaryDirectoryType::Open, "Open"},
        {mv::Application::TemporaryDirectoryType::Save, "Save"},
        {mv::Application::TemporaryDirectoryType::Publish, "Publish"},
        {mv::Application::TemporaryDirectoryType::Staging, "Staging"},
        {mv::Application::TemporaryDirectoryType::Other, "Other"}};

    for (const auto& [type, name] : categories)
    {
        const auto categoryPath = application.getTemporaryDirectoryPath(type);
        success &= require(QFileInfo(categoryPath).isDir(), QString("The %1 temporary directory was not created").arg(name));
        success &= require(QDir::cleanPath(categoryPath) == QDir(sessionPath).filePath(name), QString("The %1 temporary directory has an unexpected path").arg(name));
        success &= require(isWithin(categoryPath, sessionPath), QString("The %1 temporary directory is outside the session directory").arg(name));
        success &= require(categoryPath == application.getTemporaryDirectoryPath(type), QString("The %1 temporary directory path is not stable").arg(name));
    }

    return success;
}

bool runFallbackRootTest()
{
    QTemporaryDir invalidRootContainer;
    if (!require(invalidRootContainer.isValid(), "Could not create the invalid-root fixture"))
        return false;

    const auto invalidRoot = invalidRootContainer.filePath("not-a-directory");
    QFile invalidRootFile(invalidRoot);
    if (!require(invalidRootFile.open(QIODevice::WriteOnly), "Could not create the invalid configured root"))
        return false;
    invalidRootFile.close();

    initializeApplicationAttributes();
    TemporaryDirectorySettingGuard settingGuard;
    settingGuard.setRoot(invalidRoot);

    int argc = 1;
    char applicationName[] = "MV_Application_TemporaryDirectoriesTest";
    char* argv[] = {applicationName, nullptr};
    mv::Application application(argc, argv);

    const auto expectedFallback = QDir::cleanPath(QDir::tempPath());
    bool success = true;
    success &= require(QDir::cleanPath(application.getTemporaryDirRoot()) == expectedFallback, "An invalid configured root did not fall back to the OS temporary directory");
    success &= require(isWithin(application.getTemporaryDir().path(), expectedFallback), "The fallback session directory is outside the OS temporary directory");
    return success;
}
}

int main(int argc, char* argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    if (argc == 2 && QString::fromLocal8Bit(argv[1]) == "--invalid-root")
        return runFallbackRootTest() ? 0 : 1;

    return runConfiguredRootTest() ? 0 : 1;
}
