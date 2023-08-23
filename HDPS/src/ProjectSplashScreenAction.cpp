// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSplashScreenAction.h"
#include "ProjectSplashScreenDialog.h"
#include "Application.h"

//#include "CoreInterface.h"

namespace hdps::gui {

ProjectSplashScreenAction::ProjectSplashScreenAction(ProjectMetaAction* projectMetaAction) :
    AbstractSplashScreenAction(projectMetaAction),
    _projectMetaAction(projectMetaAction),
    _editAction(this, "Edit"),
    _projectImageAction(this, "Project Image"),
    _affiliateLogosImageAction(this, "Affiliate Logos")
{
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _editAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIcon(fontAwesome.getIcon("cog"));
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(350, 0));

    //_editAction.addAction(&_durationAction);
    //_editAction.addAction(&_closeManuallyAction);
    //_editAction.addAction(&_projectImageAction);
    //_editAction.addAction(&_affiliateLogosImageAction);
    //_editAction.addAction(&_backgroundColorAction);

    _projectImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _projectImageAction.setIcon(fontAwesome.getIcon("image"));
    _projectImageAction.setToolTip("Project image");

    _affiliateLogosImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _affiliateLogosImageAction.setIcon(fontAwesome.getIcon("image"));
    _affiliateLogosImageAction.setToolTip("Affiliate logos image");

    //_loadTaskAction.setTask(&hdps::projects().getCurrentProject()->getTask());
    
    addAction(&_editAction);
}

void ProjectSplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    AbstractSplashScreenAction::fromVariantMap(variantMap);

    _projectImageAction.fromParentVariantMap(variantMap);
    _affiliateLogosImageAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectSplashScreenAction::toVariantMap() const
{
    auto variantMap = AbstractSplashScreenAction::toVariantMap();

    _projectImageAction.insertIntoVariantMap(variantMap);
    _affiliateLogosImageAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

SplashScreenDialog* ProjectSplashScreenAction::getSplashScreenDialog()
{
    return new ProjectSplashScreenDialog(_projectMetaAction);
}

/*
ProjectSplashScreenAction::Dialog::Dialog(ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent) :
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
    //setWindowFlags(Qt::Window | Qt::Popup | Qt::WindowStaysOnTopHint); // | Qt::Popup
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint); // | Qt::Popup
    //setAttribute(Qt::WA_StyledBackground);
    setFixedSize(QSize(640, 480));
    //setModal(true);

    

    

    const auto updateDuration = [this]() -> void {
        const auto animationDuration = std::min(_projectSplashScreenAction.getAnimationDurationAction().getValue(), _projectSplashScreenAction.getDurationAction().getValue() / 2);

        _opacityAnimation.setDuration(animationDuration);
        _positionAnimation.setDuration(animationDuration);
    };
    
    updateDuration();

    connect(&_projectSplashScreenAction.getDurationAction(), &IntegralAction::valueChanged, this, updateDuration);
    connect(&_projectSplashScreenAction.getAnimationDurationAction(), &IntegralAction::valueChanged, this, updateDuration);

    const auto margin = 10;

    _mainLayout.setContentsMargins(margin, margin, margin, margin);

    createTopContent();
    createCenterContent();
    createBottomContent();

    setLayout(&_mainLayout);

    _mainLayout.addWidget(new QPushButton());
}

void ProjectSplashScreenAction::Dialog::open()
{
    _projectSplashScreenAction.getShowSplashScreenAction().setEnabled(false);
    _closeToolButton.lower();

    if (!_projectSplashScreenAction.getCloseManuallyAction().isChecked()) { //  && !_projectSplashScreenAction.getLoadTaskAction().getTask()->isIdle()
        QTimer::singleShot(_projectSplashScreenAction.getDurationAction().getValue() - _projectSplashScreenAction.getAnimationDurationAction().getValue(), this, [this]() -> void {
            if (_opacityAnimation.currentValue().toFloat() == 1.0f)
                fadeOut();
        });
    }

    fadeIn();

    QDialog::open();
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
    //_mainLayout.addWidget(_projectSplashScreenAction.getLoadTaskAction().createWidget(this));
}
*/

}