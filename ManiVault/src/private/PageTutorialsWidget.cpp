// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageTutorialsWidget.h"
#include "PageContentWidget.h"
#include "PageAction.h"

#include <util/FileDownloader.h>

#include <QEvent>

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

        PageAction tutorialAction(Application::getIconFont("FontAwesome").getIcon(tutorial->getIconName()), tutorial->getTitle(), tutorial->getSummary(), "", "", [this, tutorial]() -> void {
			try {
                if (tutorial->hasProject()) {
                    auto* projectDownloader = new FileDownloader(FileDownloader::StorageMode::All, Task::GuiScope::Modal);

                    connect(projectDownloader, &FileDownloader::downloaded, this, [projectDownloader]() -> void {
                        mv::projects().openProject(projectDownloader->getDownloadedFilePath());
                        projectDownloader->deleteLater();
                    });

                    connect(projectDownloader, &FileDownloader::aborted, this, [projectDownloader]() -> void {
                        qDebug() << "Download aborted by user";
					});

                    projectDownloader->download(tutorial->getProject());
                } else {
                    mv::projects().newBlankProject();

                	if (auto tutorialPlugin = mv::plugins().requestViewPlugin("Tutorial")) {
                        if (auto pickerAction = dynamic_cast<OptionAction*>(tutorialPlugin->findChildByPath("Pick tutorial")))
                            pickerAction->setCurrentText(tutorial->getTitle());

                        if (auto toolbarAction = dynamic_cast<HorizontalGroupAction*>(tutorialPlugin->findChildByPath("Toolbar")))
                            toolbarAction->setVisible(false);
                	}
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
		});

        tutorialAction.setComments(tutorial->hasProject() ? QString("A tutorial project will be downloaded from: %1.").arg(tutorial->getProject()) : "Creates a project with a tutorial plugin.");
        tutorialAction.setTags(tutorial->getTags());

        getModel().add(tutorialAction);
    }
}

void PageTutorialsWidget::updateCustomStyle()
{
}