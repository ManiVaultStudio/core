// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SplashScreenDialog.h"
#include "Application.h"

#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QPainter>

#include <algorithm>

#ifdef _DEBUG
    #define SPLASH_SCREEN_DIALOG_VERBOSE
#endif

namespace hdps::gui {

SplashScreenDialog::SplashScreenDialog(SplashScreenAction& splashScreenAction, QWidget* parent /*= nullptr*/, std::int32_t autoHideTimeout /*= DEFAULT_AUTO_HIDE_TIMEOUT*/, std::uint32_t panAmount /*= DEFAULT_PAN_AMOUNT*/, std::uint32_t animationDuration /*= DEFAULT_ANIMATION_DURATION*/) :
    QDialog(parent),
    _splashScreenAction(splashScreenAction),
    _opacityAnimation(this, "windowOpacity", this),
    _positionAnimation(this, "pos", this),
    _animationState(AnimationState::Idle),
    _autoHideTimeout(),
    _panAmount(),
    _animationDuration(),
    _logoImage(":/Icons/AppIcon256"),
    _backgroundImage(":/Images/SplashScreenBackground"),
    _closeToolButton()
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    setFixedSize(QSize(640, 480));

    _autoHideTimeout    = autoHideTimeout;
    _panAmount          = panAmount;
    _animationDuration  = std::min(static_cast<std::int32_t>(animationDuration), _autoHideTimeout / 2);

    _opacityAnimation.setDuration(_animationDuration);
    _positionAnimation.setDuration(_animationDuration);
    
    _backgroundImage = _backgroundImage.scaled(_backgroundImage.size() / 5, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    createTopContent();
    createCenterContent();
    createBottomContent();

    setLayout(&_mainLayout);
}

void SplashScreenDialog::open()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    fadeIn();

    QDialog::open();

    if (_autoHideTimeout > 0) {
        QTimer::singleShot(_autoHideTimeout - _animationDuration, [this]() -> void {
            accept();
        });
    }
}

void SplashScreenDialog::close()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QDialog::close();
}

void SplashScreenDialog::accept()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_animationDuration > 0) {
        connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
            QDialog::accept();
        });
    }
    else
        QDialog::accept();
}

void SplashScreenDialog::reject()
{
#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (_animationDuration > 0) {
        connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
            QDialog::reject();
        });
    }
    else
        QDialog::reject();
}

void SplashScreenDialog::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    //painter.setBackground(QBrush(_projectSplashScreenAction.getBackgroundColorAction().getColor()));

    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = _backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget);

    painter.setOpacity(0.5);
    painter.drawPixmap(pixmapRectangle.topLeft(), _backgroundImage);
}

void SplashScreenDialog::fadeIn()
{
    if (_animationDuration <= 0)
        return;

    if (_animationState == AnimationState::FadingIn)
        return;

#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

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

    const auto primaryScreenGeometry    = QApplication::primaryScreen()->geometry();
    const auto position                 = primaryScreenGeometry.topLeft() + primaryScreenGeometry.center() - rect().center();

    _positionAnimation.setEasingCurve(QEasingCurve::OutQuad);
    _positionAnimation.setStartValue(position - QPoint(0, _panAmount));
    _positionAnimation.setEndValue(position);
    _positionAnimation.start();
}

void SplashScreenDialog::fadeOut()
{
    if (_animationDuration <= 0)
        return;

    if (_animationState == AnimationState::FadingOut)
        return;

#ifdef SPLASH_SCREEN_DIALOG_VERBOSE
    qDebug() << __FUNCTION__;
#endif

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

    const auto primaryScreenGeometry    = QApplication::primaryScreen()->geometry();
    const auto position                 = primaryScreenGeometry.topLeft() + primaryScreenGeometry.center() - rect().center();


    _positionAnimation.setEasingCurve(QEasingCurve::InQuad);
    _positionAnimation.setStartValue(_positionAnimation.currentValue().toPoint());
    _positionAnimation.setEndValue(position + QPoint(0, _panAmount));
    _positionAnimation.start();
}

void SplashScreenDialog::createTopContent()
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    _closeToolButton.setVisible(_splashScreenAction.getMayClose());
    _closeToolButton.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    _closeToolButton.setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    _closeToolButton.setAutoRaise(true);
    _closeToolButton.setToolTip("Close the splash screen");

    auto sizePolicy = _closeToolButton.sizePolicy();
    
    sizePolicy.setRetainSizeWhenHidden(true);
    
    _closeToolButton.setSizePolicy(sizePolicy);

    connect(&_closeToolButton, &QToolButton::clicked, this, &SplashScreenDialog::close);

    layout->addStretch(1);
    layout->addWidget(&_closeToolButton);

    _mainLayout.addLayout(layout);
}

void SplashScreenDialog::createCenterContent()
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(20, 5, 20, 5);
    layout->setSpacing(50);
    layout->setAlignment(Qt::AlignTop);

    auto imageLabel = new QLabel();
    auto htmlLabel  = new QLabel();

    imageLabel->setFixedSize(200, 200);
    imageLabel->setScaledContents(true);

    htmlLabel->setWordWrap(true);
    htmlLabel->setTextFormat(Qt::RichText);
    htmlLabel->setOpenExternalLinks(true);
    htmlLabel->setAlignment(Qt::AlignTop);

    layout->addWidget(imageLabel);
    layout->addWidget(htmlLabel);

    auto projectMetaAction = _splashScreenAction.getProjectMetaAction();

    if (projectMetaAction) {
        auto& splashScreenAction = projectMetaAction->getSplashScreenAction();

        imageLabel->setPixmap(QPixmap::fromImage(splashScreenAction.getProjectImageAction().getImage()));

        auto& versionAction = projectMetaAction->getProjectVersionAction();
        auto title          = projectMetaAction->getTitleAction().getString();
        auto version        = QString("%1.%2 <i>%3</i>").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), versionAction.getSuffixAction().getString().toLower());
        auto description    = projectMetaAction->getDescriptionAction().getString();
        auto comments       = projectMetaAction->getCommentsAction().getString();

        if (title.isEmpty())
            title = "Untitled Project";

        if (description.isEmpty())
            description = "No description";

        if (comments.isEmpty())
            comments = "No comments";

        htmlLabel->setText(QString(" \
            <p style='font-size: 20pt; font-weight: bold;'><span>%1</span></p> \
            <p style='font-weight: bold;'>Version: %2</p> \
            <p>%3</p> \
            <p>%4</p> \
            <p></p> \
            <p>This application is built with <a href='https://www.manivault.studio/'>ManiVault Studio</a></p> \
        ").arg(title, version, description, comments));
    }
    else {
        imageLabel->setPixmap(_logoImage);

        const auto applicationVersion   = Application::current()->getVersion();
        const auto versionString        = QString("%1.%2").arg(QString::number(applicationVersion.getMajor()), QString::number(applicationVersion.getMinor()));

        htmlLabel->setText(QString(" \
            <p style='font-size: 20pt; font-weight: bold;'><span style='color: rgb(102, 159, 178)'>ManiVault</span> <span style='color: rgb(162, 141, 208)'>Studio</span></p> \
            <p>Version: %2</p> \
            <p><i>An extensible open-source visual analytics framework for analyzing high-dimensional data</i></p> \
        ").arg(versionString));
    }

    _mainLayout.addLayout(layout);
}

void SplashScreenDialog::createBottomContent()
{
    _mainLayout.addStretch(1);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(20, 20, 20, 20);

    auto projectMetaAction = _splashScreenAction.getProjectMetaAction();

    if (projectMetaAction) {
        auto& splashScreenAction = projectMetaAction->getSplashScreenAction();

        auto affiliateLogosImageLabel = new QLabel();

        affiliateLogosImageLabel->setPixmap(QPixmap::fromImage(splashScreenAction.getAffiliateLogosImageAction().getImage().scaledToHeight(60, Qt::SmoothTransformation)));

        layout->addWidget(affiliateLogosImageLabel);
    }

    layout->addStretch(1);

    _mainLayout.addLayout(layout);

    if (Application::current()->getTask(Application::TaskType::LoadApplication)->isRunning())
        _mainLayout.addWidget(_splashScreenAction.getTaskAction().createWidget(this));
}


}
