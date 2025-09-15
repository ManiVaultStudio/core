// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenAction.h"

#include <QBuffer>

#include "Application.h"

#include "widgets/SplashScreenWidget.h"

#include "CoreInterface.h"

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
    _projectImageAction(this, "Project Image", false),
    _affiliateLogosImageAction(this, "Affiliate Logos", false),
    _htmlOverrideAction(this, "HTML Override"),
    _editAction(this, "Edit"),
    _openAction(this, "Open splash screen"),
    _closeAction(this, "Close splash screen"),
    _taskAction(this, "ManiVault")
{
    addAction(&_enabledAction);
    addAction(&_editAction);
    addAction(&_openAction);

    _editAction.addAction(&_enabledAction);
    _editAction.addAction(&_projectImageAction);
    _editAction.addAction(&_affiliateLogosImageAction);

    _taskAction.setTask(&Application::current()->getStartupTask());

    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");

    _openAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _openAction.setIconByName("eye");
    _openAction.setToolTip("Open the splash screen");

    _closeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _closeAction.setIconByName("eye-slash");
    _closeAction.setToolTip("Close the splash screen");

    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIconByName("gear");
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(350, 0));

    _projectImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _projectImageAction.setIconByName("image");
    _projectImageAction.setToolTip("Project image");

    _affiliateLogosImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _affiliateLogosImageAction.setIconByName("image");
    _affiliateLogosImageAction.setToolTip("Affiliate logos image");

    connect(&_openAction, &TriggerAction::triggered, this, &SplashScreenAction::showSplashScreenWidget);
    connect(&_closeAction, &TriggerAction::triggered, this, &SplashScreenAction::closeSplashScreenWidget);

    connect(&Application::current()->getStartupTask(), &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (mv::projects().isOpeningProject() || mv::projects().isImportingProject())
            return;

        if (previousStatus == Task::Status::Finished && status == Task::Status::Idle)
            closeSplashScreenWidget();
    });

    connect(&mv::projects().getProjectSerializationTask(), &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (previousStatus == Task::Status::Running && status == Task::Status::Finished) {
            QPointer<SplashScreenAction> splashScreenAction(this);

            QTimer::singleShot(2500, parentWidget(), [this, splashScreenAction]() -> void {
                if (!splashScreenAction.isNull())
					closeSplashScreenWidget();
			});
        }
    });

    //addAlert(Alert::info("Info message!"));
    //addAlert(Alert::debug("Debug message!"));
    //addAlert(Alert::warning("Error message!"));
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

QString SplashScreenAction::getHtml() const
{
    if (!_htmlOverrideAction.getString().isEmpty())
        return _htmlOverrideAction.getString();

    return getHtmlFromTemplate();
}

ProjectMetaAction* SplashScreenAction::getProjectMetaAction()
{
    return _projectMetaAction;
}

void SplashScreenAction::setProjectMetaAction(ProjectMetaAction* projectMetaAction)
{
    _projectMetaAction = projectMetaAction;
}

void SplashScreenAction::showSplashScreenWidget()
{
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
    QTimer::singleShot(5000, [this]() -> void {
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
    QFile splashScreenFile(":/HTML/SplashScreen");

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

    //qDebug() << htmlTemplate;

    return htmlTemplate;
}

void SplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _projectImageAction.fromParentVariantMap(variantMap);
    _affiliateLogosImageAction.fromParentVariantMap(variantMap);
    _htmlOverrideAction.fromParentVariantMap(variantMap, true);
}

QVariantMap SplashScreenAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _projectImageAction.insertIntoVariantMap(variantMap);
    _affiliateLogosImageAction.insertIntoVariantMap(variantMap);
    _htmlOverrideAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}