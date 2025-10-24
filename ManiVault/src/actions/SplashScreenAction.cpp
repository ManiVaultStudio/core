// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenAction.h"
#include "CoreInterface.h"
#include "Application.h"

#include "util/Miscellaneous.h"

#include "widgets/SplashScreenWidget.h"

#include <QBuffer>

using namespace mv::util;

namespace mv::gui {

SplashScreenAction::Alert SplashScreenAction::Alert::info(const QString& message)
{
    const auto color = QColor::fromHsv(220, 200, 150);

    return {
        Type::Info,
        "fa-solid fa-info-circle",
        message,
        color
    };
}

SplashScreenAction::Alert SplashScreenAction::Alert::debug(const QString& message)
{
    const auto color = QColor::fromHsv(125, 200, 150);

    return {
        Type::Debug,
        "fa-solid fa-bug",
        message,
        color
    };
}

SplashScreenAction::Alert SplashScreenAction::Alert::warning(const QString& message)
{
    const auto color = QColor::fromHsv(0, 200, 150);

    return {
        Type::Warning,
        "fa-solid fa-exclamation-circle",
        message,
        color
    };
}

SplashScreenAction::Alert::Type SplashScreenAction::Alert::getType() const
{
    return _type;
}

QString SplashScreenAction::Alert::getIconName() const
{
    return _iconName;
}

QString SplashScreenAction::Alert::getMessage() const
{
    return _message;
}

QColor SplashScreenAction::Alert::getColor() const
{
    return _color;
}

SplashScreenAction::SplashScreenAction(QObject* parent, bool mayClose /*= false*/) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _mayCloseSplashScreenWidget(mayClose),
    _projectMetaAction(nullptr),
    _enabledAction(this, "Enable splash screen"),
    _overrideAction(this, "Override"),
    _htmlAction(this, "HTML", getHtmlFromTemplate()),
    _editAction(this, "Edit"),
    _openAction(this, "Open splash screen"),
    _testAction(this, "Test the splash screen"),
    _closeAction(this, "Close splash screen"),
    _taskAction(this, "ManiVault"),
    _simulateStartupTask(this, "Test splash screen")
{
    addAction(&_enabledAction);
    addAction(&_editAction);
    addAction(&_testAction);

    //_editAction.addAction(&_enabledAction);
    _editAction.addAction(&_overrideAction);
    _editAction.addAction(&_htmlAction);

    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");

    _htmlAction.setDefaultWidgetFlags(StringAction::WidgetFlag::TextEdit);
    _htmlAction.setOverrideSizeHint(QSize(800, 600));

    _openAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _openAction.setIconByName("eye");
    _openAction.setToolTip("Open the splash screen");

    _testAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _testAction.setIconByName("play");
    _testAction.setToolTip("Test the splash screen");

    _closeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _closeAction.setIconByName("eye-slash");
    _closeAction.setToolTip("Close the splash screen");

    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIconByName("gear");
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(1024, 1024));

    connect(&_openAction, &TriggerAction::triggered, this, &SplashScreenAction::showSplashScreenWidget);
    connect(&_closeAction, &TriggerAction::triggered, this, &SplashScreenAction::closeSplashScreenWidget);

    _simulateTimer.setInterval(500);

	connect(&_simulateTimer, &QTimer::timeout, this, [this]() -> void {
        auto progress = _simulateStartupTask.getProgress();

		progress += .1f;

        _simulateStartupTask.setProgress(progress);

        _splashScreenWidget->update();

		QCoreApplication::processEvents();

		if (progress >= 1.f) {
            progress = 1.f;

            _simulateTimer.stop();
            _simulateStartupTask.setFinished();
        }

        _simulateStartupTask.setProgress(progress);
    });

	connect(&_testAction, &TriggerAction::triggered, this, [this]() -> void {
        _simulateStartupTask.setRunning();
        _simulateStartupTask.setProgress(.0f);

        setStartupTask(&_simulateStartupTask);

        showSplashScreenWidget();

        _simulateTimer.start();
    });

    const auto updateHtmlActionReadOnly = [this]() -> void {
        _htmlAction.setEnabled(_overrideAction.isChecked());
	};

    updateHtmlActionReadOnly();

    connect(&_overrideAction, &ToggleAction::toggled, this, updateHtmlActionReadOnly);
}

void SplashScreenAction::addAlert(const Alert& alert)
{
    _alerts << alert;
}

SplashScreenAction::Alerts SplashScreenAction::getAlerts() const
{
    return _alerts;
}

bool SplashScreenAction::getMayCloseSplashScreenWidget() const
{
    return _mayCloseSplashScreenWidget;
}

void SplashScreenAction::setMayCloseSplashScreenWidget(bool mayCloseSplashScreenWidget)
{
    _mayCloseSplashScreenWidget = mayCloseSplashScreenWidget;
}

void SplashScreenAction::setStartupTask(Task* startupTask)
{
    Q_ASSERT(startupTask);

    if (!startupTask)
        return;

    _taskAction.setTask(startupTask);

    connect(startupTask, &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (mv::projects().isOpeningProject() || mv::projects().isImportingProject())
            return;

        if (previousStatus == Task::Status::Finished && status == Task::Status::Idle)
            closeSplashScreenWidget();
	});
}

QString SplashScreenAction::getHtml() const
{
    return _htmlAction.getString();
}

QString SplashScreenAction::pixmapToBase64(const QPixmap& pixmap)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);

    buffer.open(QIODevice::WriteOnly);

    pixmap.save(&buffer, "PNG");

    return QString::fromLatin1(byteArray.toBase64());
}

ProjectMetaAction* SplashScreenAction::getProjectMetaAction() const
{
    return _projectMetaAction;
}

void SplashScreenAction::setProjectMetaAction(ProjectMetaAction* projectMetaAction)
{
    _projectMetaAction = projectMetaAction;
}

void SplashScreenAction::showSplashScreenWidget()
{
    if (!getEnabledAction().isChecked())
        return;

    if (_splashScreenWidget.isNull()) {
        _splashScreenWidget = new SplashScreenWidget(*this, nullptr);

        connect(_splashScreenWidget, &QWidget::destroyed, this, [this]() -> void {
            _splashScreenWidget.clear();
        });
    }
        
    _splashScreenWidget->showAnimated();
}

void SplashScreenAction::closeSplashScreenWidget() 
{
    const auto closeDelayMs = 1500 + _alerts.count() * 1000;

    QTimer::singleShot(closeDelayMs, [this]() -> void {
        if (_splashScreenWidget.isNull())
            return;

        _splashScreenWidget->closeAnimated();

        _splashScreenWidget.clear();
    });
}

bool SplashScreenAction::shouldDisplayProjectInfo() const
{
    return getEnabledAction().isChecked() && _projectMetaAction;
}

QString SplashScreenAction::getHtmlFromTemplate() const
{
    QFile splashScreenFile(":/HTML/SplashScreenTemplate");

    if (!splashScreenFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringLiteral("<html><body>Error: cannot load template</body></html>");

    QTextStream textStreamIn(&splashScreenFile);

    textStreamIn.setEncoding(QStringConverter::Utf8);

    QString htmlTemplate = textStreamIn.readAll();

    const auto replaceInHtml = [&htmlTemplate](const QString& placeholder, const QString& value) -> void {
        if (value.isEmpty())
            htmlTemplate.replace(placeholder, "");
        else
            htmlTemplate.replace(placeholder, value);
    };

    const auto bodyColor = qApp->palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Dark).name();

    replaceInHtml("{{BODY_COLOR}}", bodyColor);

    htmlTemplate = applyResourceImageToCss(htmlTemplate, QStringLiteral(":/Images/SplashScreenBackground"), "{{BACKGROUND_IMAGE}}", .2f);

    QPixmap logoPixmap(":/Icons/AppIcon128");

    replaceInHtml("{{LOGO}}", QString("<img src='data:image/png;base64,%1'>").arg(pixmapToBase64(logoPixmap.scaled(96, 96, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))));

    if (auto projectImageAction = dynamic_cast<const ProjectMetaAction*>(_projectMetaAction)) {
        replaceInHtml("{{TITLE}}", projectImageAction->getTitleAction().getString());
        replaceInHtml("{{VERSION}}", projectImageAction->getProjectVersionAction().getVersionStringAction().getString());
        replaceInHtml("{{SUBTITLE}}", "");
        replaceInHtml("{{COPYRIGHT_NOTICE}}", "");
    } else {
        replaceInHtml("{{TITLE}}", "ManiVault <span style='color: rgb(162, 141, 208)'>Studio<sup style='font-size: 12pt; font-weight: bold;'>&copy;</sup></span>");
        replaceInHtml("{{VERSION}}", QString("Version: %1").arg(QString::fromStdString(Application::current()->getVersion().getVersionString())));
        replaceInHtml("{{SUBTITLE}}", "An extensible open-source visual analytics framework for analyzing high-dimensional data");

        QStringList externalLinks;

        const auto addExternalLink = [&externalLinks](const QString& linkUrl, const QString& linkTitle, const QString& icon) -> void {
            externalLinks << QString("<span style='color: black; padding-bottom: 5px;'><i class='%3 fa-sm icon' style='padding-right: 5px;'></i><a href='%1' style='color: black'>%2</a></span>").arg(linkUrl, linkTitle, icon);
        };

        addExternalLink("https://www.manivault.studio/", "Visit our website", "fa-solid fa-earth-europe");
        addExternalLink("https://github.com/ManiVaultStudio", "Contribute to ManiVault on Github", "fa-brands fa-github");
        addExternalLink("https://discord.gg/pVxmC2cSzA", "Get in touch on our Discord", "fa-brands fa-discord");

        replaceInHtml("{{DESCRIPTION}}", externalLinks.join("<br>"));
        replaceInHtml("{{COPYRIGHT}}", "This software is licensed under the GNU Lesser General Public License v3.0.<br>Copyright &copy; 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)");
    }

    QStringList alertStrings;

    const auto addAlertString = [&alertStrings](const QString& iconName, const QString& alertMessage, const QColor& color) -> void {
        alertStrings << QString("<span style='color: %3'><i class='%1 fa-sm icon' style='padding-right: 5px;'></i>%2</span>").arg(iconName, alertMessage, color.name());
    };

    for (const auto& alert : getAlerts()) {
        addAlertString(alert.getIconName(), alert.getMessage(), alert.getColor());
    }

    replaceInHtml("{{ALERTS}}", alertStrings.join("<br>"));

    return htmlTemplate;
}

void SplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _overrideAction.fromParentVariantMap(variantMap, true);
    _htmlAction.fromParentVariantMap(variantMap, true);
}

QVariantMap SplashScreenAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _overrideAction.insertIntoVariantMap(variantMap);
    _htmlAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}