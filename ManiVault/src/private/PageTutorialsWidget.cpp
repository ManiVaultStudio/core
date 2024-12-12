// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageTutorialsWidget.h"
#include "PageContentWidget.h"
#include "PageAction.h"

#include <util/FileDownloader.h>

#include <QEvent>
#include <QDesktopServices>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

PageTutorialsWidget::PageTutorialsWidget(QWidget* parent, const QStringList& tags /*= QStringList()*/) :
    PageActionsWidget(parent, "Tutorials"),
    _toolbarAction(this, "Toolbar")
{
    layout()->setContentsMargins(0, 0, 0, 0);

    dynamic_cast<QVBoxLayout*>(layout())->insertWidget(1, _toolbarAction.createWidget(this));

    _toolbarAction.setShowLabels(false);

    _toolbarAction.addAction(&_tutorialsFilterModel.getTextFilterAction());
    _toolbarAction.addAction(&_tutorialsFilterModel.getTagsFilterAction());

    getHierarchyWidget().getToolbarAction().setVisible(false);
    getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    getHierarchyWidget().setItemTypeName("Tutorial");
    
    _tutorialsFilterModel.setSourceModel(const_cast<LearningCenterTutorialsModel*>(&mv::help().getTutorialsModel()));
    _tutorialsFilterModel.getTextFilterColumnAction().setCurrentText("Title");
    
    if (!tags.isEmpty())
		_tutorialsFilterModel.getTagsFilterAction().setSelectedOptions(tags);

    connect(&_tutorialsFilterModel, &LearningCenterTutorialsFilterModel::layoutChanged, this, &PageTutorialsWidget::updateActions);
    connect(&_tutorialsFilterModel, &LearningCenterTutorialsFilterModel::rowsInserted, this, &PageTutorialsWidget::updateActions);
    connect(&_tutorialsFilterModel, &LearningCenterTutorialsFilterModel::rowsRemoved, this, &PageTutorialsWidget::updateActions);

    updateActions();
}

HorizontalGroupAction& PageTutorialsWidget::getToolbarAction()
{
    return _toolbarAction;
}

LearningCenterTutorialsFilterModel& PageTutorialsWidget::getTutorialsFilterModel()
{
    return _tutorialsFilterModel;
}

bool PageTutorialsWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateCustomStyle();

    return PageActionsWidget::event(event);
}

void PageTutorialsWidget::updateActions()
{
    getModel().reset();

    for (int rowIndex = 0; rowIndex < _tutorialsFilterModel.rowCount(); ++rowIndex) {
        const auto sourceRowIndex = _tutorialsFilterModel.mapToSource(_tutorialsFilterModel.index(rowIndex, 0));

        auto tutorial = dynamic_cast<LearningCenterTutorialsModel::Item*>(mv::help().getTutorialsModel().itemFromIndex(sourceRowIndex))->getTutorial();

        PageAction tutorialAction(Application::getIconFont("FontAwesome").getIcon(tutorial->getIconName()), tutorial->getTitle(), tutorial->getSummary(), tutorial->getProject(), "", [this, tutorial]() -> void {
			try {
                if (tutorial->hasProject()) {
                    auto* projectDownloader = new FileDownloader();

                    connect(projectDownloader, &FileDownloader::downloaded, this, [projectDownloader]() -> void {
                        mv::projects().openProject(projectDownloader->getDownloadedFilePath());
                        projectDownloader->deleteLater();
                    });

                    qDebug() << tutorial->getProject();

                    projectDownloader->download(tutorial->getProject());
                }
            }
            catch (std::exception& e)
	        {
	            exceptionMessageBox("Unable to load tutorial", e);

	            projects().getProjectSerializationTask().setFinished();
	        }
	        catch (...)
	        {
	            exceptionMessageBox("Unable to load tutorial");
	        }

            //QDesktopServices::openUrl(tutorial->getUrl());
		});

        //tutorialAction.setSubtitle(subtitle);
        //tutorialAction.setComments(QString("Create a new project and import data into it with the %1").arg(viewPluginFactory->getKind()));

        getModel().add(tutorialAction);
    }
}

void PageTutorialsWidget::updateCustomStyle()
{
}
