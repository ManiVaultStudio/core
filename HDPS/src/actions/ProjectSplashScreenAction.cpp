// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSplashScreenAction.h"
#include "Project.h"
#include "CoreInterface.h"

#include <QToolButton>
#include <QTimer>
#include <QMainWindow>
#include <QPainter>

#include <algorithm>

namespace hdps::gui {

ProjectSplashScreenAction::ProjectSplashScreenAction(QObject* parent, const Project& project) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _project(project),
    _enabledAction(this, "Splash screen"),
    _closeManuallyAction(this, "Close manually"),
    _durationAction(this, "Duration", 1000, 10000, 4000),
    _animationDurationAction(this, "Animation Duration", 10, 10000, 250),
    _animationPanAmountAction(this, "Pan Amount", 10, 100, 20),
    _backgroundColorAction(this, "Background Color", Qt::white),
    _editAction(this, "Edit"),
    _showSplashScreenAction(this, "About project..."),
    _splashScreenDialog(*this),
    _projectImageAction(this, "Project Image"),
    _affiliateLogosImageAction(this, "Affiliate Logos"),
    _loadTaskAction(this, "Load Task")
{
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");

    _closeManuallyAction.setToolTip("Close manually");

    _durationAction.setToolTip("Duration of the splash screen in milliseconds");
    _durationAction.setSuffix("ms");

    _animationDurationAction.setToolTip("Duration of the splash screen animations in milliseconds");
    _animationDurationAction.setSuffix("ms");

    _animationPanAmountAction.setToolTip("Amount of panning up/down during animation in pixels");
    _animationPanAmountAction.setSuffix("px");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _editAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIcon(fontAwesome.getIcon("cog"));
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(350, 0));

    _editAction.addAction(&_durationAction);
    _editAction.addAction(&_closeManuallyAction);
    _editAction.addAction(&_projectImageAction);
    _editAction.addAction(&_affiliateLogosImageAction);
    _editAction.addAction(&_backgroundColorAction);

    _showSplashScreenAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _showSplashScreenAction.setIcon(fontAwesome.getIcon("eye"));
    _showSplashScreenAction.setToolTip("Preview the splash screen");

    _projectImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _projectImageAction.setIcon(fontAwesome.getIcon("image"));
    _projectImageAction.setToolTip("Project image");

    _affiliateLogosImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _affiliateLogosImageAction.setIcon(fontAwesome.getIcon("image"));
    _affiliateLogosImageAction.setToolTip("Affiliate logos image");

    _loadTaskAction.setTask(hdps::projects().getFileIOTask());

    addAction(&_enabledAction);
    addAction(&_editAction);
    addAction(&_showSplashScreenAction);

    const auto updateDurationAction = [this]() -> void {
        _durationAction.setEnabled(!_closeManuallyAction.isChecked());
    };

    updateDurationAction();

    connect(&_showSplashScreenAction, &TriggerAction::triggered, this, [this]() -> void {
        _splashScreenDialog.open();
    });

    connect(&_closeManuallyAction, &ToggleAction::toggled, this, updateDurationAction);
}

const Project& ProjectSplashScreenAction::getProject() const
{
    return _project;
}

void ProjectSplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _closeManuallyAction.fromParentVariantMap(variantMap);
    _durationAction.fromParentVariantMap(variantMap);
    _animationDurationAction.fromParentVariantMap(variantMap);
    _animationPanAmountAction.fromParentVariantMap(variantMap);
    _backgroundColorAction.fromParentVariantMap(variantMap);
    _projectImageAction.fromParentVariantMap(variantMap);
    _affiliateLogosImageAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectSplashScreenAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _closeManuallyAction.insertIntoVariantMap(variantMap);
    _durationAction.insertIntoVariantMap(variantMap);
    _animationDurationAction.insertIntoVariantMap(variantMap);
    _animationPanAmountAction.insertIntoVariantMap(variantMap);
    _backgroundColorAction.insertIntoVariantMap(variantMap);
    _projectImageAction.insertIntoVariantMap(variantMap);
    _affiliateLogosImageAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ProjectSplashScreenAction::Dialog::Dialog(ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectSplashScreenAction(projectSplashScreenAction),
    _opacityAnimation(this, "windowOpacity", this),
    _positionAnimation(this, "pos", this),
    _backgroundImage(":/Images/StartPageBackground"),
    _mainLayout(),
    _closeToolButton(),
    _animationState(AnimationState::Idle)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowFlags(Qt::Window | Qt::Popup | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_StyledBackground);
    setFixedSize(QSize(640, 480));
    //setModal(true);

    const auto updateStyleSheet = [this]() -> void {
        setStyleSheet(QString("QDialog { \
            background-color: %1; \
            border: 1px solid rgb(50, 50, 50); \
         }").arg(_projectSplashScreenAction.getBackgroundColorAction().getColor().name(QColor::HexArgb)));
    };
    
    updateStyleSheet();

    connect(&_projectSplashScreenAction.getBackgroundColorAction(), &ColorAction::colorChanged, this, updateStyleSheet);

    const auto updateDuration = [this]() -> void {
        const auto animationDuration = std::min(_projectSplashScreenAction.getAnimationDurationAction().getValue(), _projectSplashScreenAction.getDurationAction().getValue() / 2);

        _opacityAnimation.setDuration(animationDuration);
        _positionAnimation.setDuration(animationDuration);
    };
    
    updateDuration();

    connect(&_projectSplashScreenAction.getDurationAction(), &IntegralAction::valueChanged, this, updateDuration);
    connect(&_projectSplashScreenAction.getAnimationDurationAction(), &IntegralAction::valueChanged, this, updateDuration);

    connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
        if (_opacityAnimation.currentValue().toFloat() == 1.0f) {
            setEnabled(true);
        }
        else {
            accept();

            _projectSplashScreenAction.getShowSplashScreenAction().setEnabled(true);
        }

        _animationState = AnimationState::Idle;
    });

    auto& project = const_cast<Project&>(_projectSplashScreenAction.getProject());

    const auto margin = 10;

    _mainLayout.setContentsMargins(margin, margin, margin, margin);

    createTopContent();
    createCenterContent();
    createBottomContent();

    setLayout(&_mainLayout);
}

void ProjectSplashScreenAction::Dialog::open()
{
    _projectSplashScreenAction.getShowSplashScreenAction().setEnabled(false);
    _closeToolButton.lower();

    if (!_projectSplashScreenAction.getCloseManuallyAction().isChecked()) {
        QTimer::singleShot(_projectSplashScreenAction.getDurationAction().getValue() - _projectSplashScreenAction.getAnimationDurationAction().getValue(), this, [this]() -> void {
            if (_opacityAnimation.currentValue().toFloat() == 1.0f)
                fadeOut();
        });
    }
    
    fadeIn();

    QDialog::open();
}

void ProjectSplashScreenAction::Dialog::fadeIn()
{
    if (_animationState == AnimationState::FadingIn)
        return;

    _animationState = AnimationState::FadingIn;

    setEnabled(false);

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();
    
    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(_opacityAnimation.currentValue().toFloat());
    _opacityAnimation.setEndValue(1.0);
    _opacityAnimation.start();

    if (_positionAnimation.state() == QPropertyAnimation::Running)
        _positionAnimation.stop();

    const auto position = getMainWindow()->frameGeometry().topLeft() + getMainWindow()->rect().center() - rect().center();

    _positionAnimation.setEasingCurve(QEasingCurve::OutQuad);
    _positionAnimation.setStartValue(position - QPoint(0, _projectSplashScreenAction.getAnimationPanAmountAction().getValue()));
    _positionAnimation.setEndValue(position);
    _positionAnimation.start();
}

void ProjectSplashScreenAction::Dialog::fadeOut()
{
    if (_animationState == AnimationState::FadingOut)
        return;

    _animationState = AnimationState::FadingOut;

#ifdef _DEBUG
    qDebug() << __FUNCTION__;
#endif

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();

    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(_opacityAnimation.currentValue().toFloat());
    _opacityAnimation.setEndValue(0.0);
    _opacityAnimation.start();

    if (_positionAnimation.state() == QPropertyAnimation::Running)
        _positionAnimation.stop();

    const auto position = getMainWindow()->frameGeometry().topLeft() + getMainWindow()->rect().center() - rect().center();

    _positionAnimation.setEasingCurve(QEasingCurve::InQuad);
    _positionAnimation.setStartValue(_positionAnimation.currentValue().toPoint());
    _positionAnimation.setEndValue(position + QPoint(0, _projectSplashScreenAction.getAnimationPanAmountAction().getValue()));
    _positionAnimation.start();
}

QMainWindow* ProjectSplashScreenAction::Dialog::getMainWindow()
{
    for (auto topLevelWidget : qApp->topLevelWidgets()) {
        auto mainWindow = qobject_cast<QMainWindow*>(topLevelWidget);
        
        if (mainWindow)
            return mainWindow;
    }
    
    return nullptr;
}

void ProjectSplashScreenAction::Dialog::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    painter.setBackground(QBrush(_projectSplashScreenAction.getBackgroundColorAction().getColor()));

    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = _backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget - QPoint(200, 0));

    painter.setOpacity(0.5);
    painter.drawPixmap(pixmapRectangle.topLeft(), _backgroundImage);
}

void ProjectSplashScreenAction::Dialog::createTopContent()
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    _closeToolButton.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    _closeToolButton.setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    _closeToolButton.setAutoRaise(true);
    _closeToolButton.setToolTip("Close the splash screen");

    connect(&_closeToolButton, &QToolButton::clicked, this, &Dialog::fadeOut);

    layout->addStretch(1);
    layout->addWidget(&_closeToolButton);

    _mainLayout.addLayout(layout);
}

void ProjectSplashScreenAction::Dialog::createCenterContent()
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(20, 5, 20, 5);
    layout->setSpacing(50);
    layout->setAlignment(Qt::AlignTop);

    auto projectImageLabel = new QLabel();

    projectImageLabel->setFixedSize(200, 200);
    projectImageLabel->setScaledContents(true);

    const auto updateProjectImage = [this, projectImageLabel]() -> void {
        projectImageLabel->setPixmap(QPixmap::fromImage(_projectSplashScreenAction.getProjectImageAction().getImage()));
    };

    connect(&_projectSplashScreenAction.getProjectImageAction(), &ImageAction::imageChanged, this, updateProjectImage);

    layout->addWidget(projectImageLabel);

    auto htmlLabel = new QLabel();

    htmlLabel->setWordWrap(true);
    htmlLabel->setTextFormat(Qt::RichText);
    htmlLabel->setOpenExternalLinks(true);
    htmlLabel->setAlignment(Qt::AlignTop);

    layout->addWidget(htmlLabel);
    
    const auto updateText = [this, htmlLabel]() -> void {
        auto& project       = _projectSplashScreenAction.getProject();
        auto& versionAction = project.getProjectVersionAction();

        auto title        = project.getTitleAction().getString();
        auto version      = QString("%1.%2 <i>%3</i>").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), versionAction.getSuffixAction().getString().toLower());
        auto description  = project.getDescriptionAction().getString();
        auto comments     = project.getCommentsAction().getString();

        if (title.isEmpty())
            title = "Untitled Project";

        if (description.isEmpty())
            description = "No description";

        if (comments.isEmpty())
            comments = "No comments";

        htmlLabel->setText(QString(" \
            <p style='font-size: 20pt; font-weight: bold;'>%1</p> \
            <p>Version: %2</p> \
            <p>%3</p> \
            <p>%4</p> \
        ").arg(title, version, description, comments));
    };

    updateText();

    connect(&_projectSplashScreenAction.getProject().getTitleAction(), &StringAction::stringChanged, this, updateText);
    connect(&_projectSplashScreenAction.getProject().getProjectVersionAction().getVersionStringAction(), &StringAction::stringChanged, this, updateText);
    connect(&_projectSplashScreenAction.getProject().getDescriptionAction(), &StringAction::stringChanged, this, updateText);
    connect(&_projectSplashScreenAction.getProject().getCommentsAction(), &StringAction::stringChanged, this, updateText);

    _mainLayout.addLayout(layout);
}

void ProjectSplashScreenAction::Dialog::createBottomContent()
{
    _mainLayout.addStretch(1);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(20, 20, 20, 20);

    auto affiliateLogosImageLabel = new QLabel();

    affiliateLogosImageLabel->setScaledContents(true);

    const auto updateAffiliateLogosImage = [this, affiliateLogosImageLabel]() -> void {
        affiliateLogosImageLabel->setPixmap(QPixmap::fromImage(_projectSplashScreenAction.getAffiliateLogosImageAction().getImage()));
    };

    connect(&_projectSplashScreenAction.getAffiliateLogosImageAction(), &ImageAction::imageChanged, this, updateAffiliateLogosImage);

    layout->addWidget(affiliateLogosImageLabel);
    layout->addStretch(1);

    _mainLayout.addLayout(layout);
    //_mainLayout.addWidget(_projectSplashScreenAction.getLoadTaskAction().createWidget(nullptr));
}

}