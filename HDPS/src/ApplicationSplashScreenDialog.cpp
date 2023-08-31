// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationSplashScreenDialog.h"
#include "ProjectMetaAction.h"
#include "Application.h"

#include "actions/ColorAction.h"

namespace hdps::gui {

ApplicationSplashScreenDialog::ApplicationSplashScreenDialog(ApplicationSplashScreenAction* applicationSplashScreenAction, QWidget* parent /*= nullptr*/) :
    SplashScreenDialog(parent),
    _applicationSplashScreenAction(applicationSplashScreenAction),
    _mainLayout(),
    _closeToolButton()
{
    Q_ASSERT(_applicationSplashScreenAction != nullptr);

    setAttribute(Qt::WA_StyledBackground);

    initialize(0, 0, 0);

    setBackgroundImage(QPixmap((":/Images/StartPageBackground")));

    /*
    const auto updateStyleSheet = [this]() -> void {
        setStyleSheet(QString("QDialog { \
            background-color: %1; \
            border: 1px solid rgb(50, 50, 50); \
         }").arg(_applicationSplashScreenAction.getBackgroundColorAction().getColor().name(QColor::HexArgb)));
    };

    updateStyleSheet();

    connect(&projectSplashScreenAction.getBackgroundColorAction(), &ColorAction::colorChanged, this, updateStyleSheet);
    */

    createCenterContent();
    createBottomContent();

    setLayout(&_mainLayout);
}

void ApplicationSplashScreenDialog::createCenterContent()
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(20, 5, 20, 5);
    layout->setSpacing(50);
    layout->setAlignment(Qt::AlignTop);

    /*
    auto projectImageLabel = new QLabel();

    projectImageLabel->setFixedSize(200, 200);
    projectImageLabel->setScaledContents(true);

    const auto updateProjectImage = [this, projectImageLabel]() -> void {
        projectImageLabel->setPixmap(QPixmap::fromImage(_projectSplashScreenAction.getProjectImageAction().getImage()));
    };

    connect(&_projectSplashScreenAction.getProjectImageAction(), &ImageAction::imageChanged, this, updateProjectImage);

    layout->addWidget(projectImageLabel);
    */

    auto htmlLabel = new QLabel();

    htmlLabel->setWordWrap(true);
    htmlLabel->setTextFormat(Qt::RichText);
    htmlLabel->setOpenExternalLinks(true);
    htmlLabel->setAlignment(Qt::AlignTop);

    layout->addWidget(htmlLabel);

    const auto updateText = [this, htmlLabel]() -> void {
        /*
        auto& project = _projectSplashScreenAction.getProject();
        auto& versionAction = project.getProjectVersionAction();

        auto title = project.getTitleAction().getString();
        auto version = QString("%1.%2 <i>%3</i>").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), versionAction.getSuffixAction().getString().toLower());
        auto description = project.getDescriptionAction().getString();
        auto comments = project.getCommentsAction().getString();

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
        */
    };

    updateText();

    _mainLayout.addLayout(layout);
}

void ApplicationSplashScreenDialog::createBottomContent()
{
    _mainLayout.addStretch(1);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(20, 20, 20, 20);

    /*
    auto affiliateLogosImageLabel = new QLabel();

    affiliateLogosImageLabel->setScaledContents(true);

    const auto updateAffiliateLogosImage = [this, affiliateLogosImageLabel]() -> void {
        affiliateLogosImageLabel->setPixmap(QPixmap::fromImage(_projectSplashScreenAction.getAffiliateLogosImageAction().getImage()));
    };

    connect(&_projectSplashScreenAction.getAffiliateLogosImageAction(), &ImageAction::imageChanged, this, updateAffiliateLogosImage);

    layout->addWidget(affiliateLogosImageLabel);
    */

    layout->addStretch(1);

    _mainLayout.addLayout(layout);
    _mainLayout.addWidget(_applicationSplashScreenAction->getTaskAction().createWidget(this));

    /*
    const auto taskAdded = [this](const QModelIndex& parent, int first, int last) -> void {
        auto& tasksModel        = _applicationSplashScreenAction->getTasksModel();
        auto& tasksFilterModel  = _applicationSplashScreenAction->getTasksFilterModel();

        for (int rowIndex = first; rowIndex < last; rowIndex++) {
            const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, 0, parent));

            auto item = static_cast<TasksModel::Item*>(_applicationSplashScreenAction->getTasksModel().itemFromIndex(sourceModelIndex));
            auto task = item->getTask();

            if (task->getName() == "Load project") {
                qDebug() << "////////////////////// Load project task added!";
            }
        }
    };

    connect(&_applicationSplashScreenAction->getTasksFilterModel(), &QSortFilterProxyModel::rowsInserted, this, taskAdded);
    */
}

}
