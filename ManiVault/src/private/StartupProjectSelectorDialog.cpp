// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartupProjectSelectorDialog.h"

#include <models/HardwareSpecTreeModel.h>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTextBrowser>
#include <QToolTip>
#include <QTreeView>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    #define STARTUP_PROJECT_SELECTOR_DIALOG_VERBOSE
#endif

class CustomTooltipPopup : public QWidget {
public:
    explicit CustomTooltipPopup(QWidget* parent = nullptr)
        : QWidget(nullptr, Qt::ToolTip)
    {
        setAttribute(Qt::WA_ShowWithoutActivating);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setWindowFlags(Qt::ToolTip);
        setFocusPolicy(Qt::NoFocus);
        setFont(QToolTip::font());
        setPalette(QToolTip::palette());
        setForegroundRole(QPalette::ToolTipText);

        // Make the background fully transparent
        _messageBrowser.setAttribute(Qt::WA_TranslucentBackground);
        _messageBrowser.setStyleSheet("background: transparent;");

        // Optional: Remove border, padding, etc.
        _messageBrowser.setFrameStyle(QFrame::NoFrame);
        _messageBrowser.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _messageBrowser.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _messageBrowser.setStyleSheet("background: transparent; color: black;");
        _messageBrowser.setMinimumHeight(10);
        _messageBrowser.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        auto layout = new QVBoxLayout(this);

        static const int margin = 2; // Set a margin for the tooltip

        layout->setContentsMargins(margin, margin, margin, margin);

        layout->addWidget(&_messageBrowser);

    	setLayout(layout);

        //setMargin(1);
        //setAlignment(Qt::AlignLeft | Qt::AlignTop);
        //setWordWrap(true);
        //setTextFormat(Qt::RichText);


    }

    void setHtml(const QString& html, int maxWidth = 1200) {
        _messageBrowser.setText(html);
        _messageBrowser.adjustSize();
        //setFixedWidth(600);
        adjustSize();
        //resize(sizeHint());
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QStyleOptionFrame opt;
        opt.initFrom(this);
        opt.rect = rect();
        opt.lineWidth = style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, nullptr, this);
        opt.midLineWidth = 0;
        opt.state |= QStyle::State_Sunken;
        opt.features = QStyleOptionFrame::None;
        opt.frameShape = QFrame::Box;

        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &p, this);

        // Call QLabel's paintEvent to draw the text
        QWidget::paintEvent(event);
    }

private:
    QTextBrowser    _messageBrowser;    /** Text browser for displaying the tooltip message */
};


class ItemViewTooltipInterceptor : public QObject {
public:
    ItemViewTooltipInterceptor(ProjectsTreeModel* projectsTreeModel, ProjectsFilterModel* projectsFilterModel, QAbstractItemView* view) :
		QObject(view),
        _projectsTreeModel(projectsTreeModel),
        _projectsFilterModel(projectsFilterModel),
		_projectsView(view),
		_popup()
    {
        _projectsView->viewport()->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::Leave) {
            _popup.hide();
        }

        const auto helpEvent = dynamic_cast<QHelpEvent*>(event);

        if (!helpEvent)
            return QObject::eventFilter(watched, event);

        const auto index    = _projectsFilterModel->mapToSource(_projectsView->indexAt(helpEvent->pos()));
        const auto column   = static_cast<mv::ProjectsTreeModel::Column>(index.column());

        if (event->type() == QEvent::ToolTip && column == ProjectsTreeModel::Column::SystemCompatibility && _popup.isHidden()) {
            const auto projectItem = dynamic_cast<ProjectsTreeModel::Item*>(_projectsTreeModel->itemFromIndex(index));

            const auto systemCompatibility = HardwareSpec::getSystemCompatibility(projectItem->getProject()->getMinimumHardwareSpec(), projectItem->getProject()->getRecommendedHardwareSpec());

			_popup.setHtml(systemCompatibility._message);
			_popup.move(helpEvent->globalPos() + QPoint(10, 20));
            _popup.show();

            return true; // block default tooltip
        }

    	
        return QObject::eventFilter(watched, event);
    }

private:
    ProjectsTreeModel*      _projectsTreeModel;     /** Pointer to the projects tree model */
    ProjectsFilterModel*    _projectsFilterModel;   /** Pointer to the projects filter model */
    QAbstractItemView*      _projectsView;          /** Pointer to the projects view */
    CustomTooltipPopup      _popup;                 /** Custom tooltip popup for displaying hardware specifications */
};

StartupProjectSelectorDialog::StartupProjectSelectorDialog(mv::ProjectsTreeModel& projectsTreeModel, mv::ProjectsFilterModel& projectsFilterModel, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectsTreeModel(projectsTreeModel),
    _projectsFilterModel(projectsFilterModel),
    _hierarchyWidget(this, "Startup project", _projectsTreeModel, &_projectsFilterModel, true),
    _loadAction(this, "Load"),
    _quitAction(this, "Quit")
{
    const auto windowIcon = StyledIcon("file-import");

    setWindowIcon(windowIcon);
    setModal(true);
    setWindowTitle("Load project");

    //new ItemViewTooltipInterceptor(&_projectsTreeModel, &_projectsFilterModel, &_hierarchyWidget.getTreeView());

    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    //auto detailsGroupbox = new QGroupBox("Project details", this);
    //auto detailsLayout = new QVBoxLayout(detailsGroupbox);
    //auto detailsLabel = new QLabel("Select a project to see its details here.", detailsGroupbox);

    //detailsLayout->addWidget(detailsLabel);

    //detailsGroupbox->setLayout(detailsLayout);

    //layout->addWidget(detailsGroupbox);

	auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_loadAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_quitAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _hierarchyWidget.getFilterGroupAction().addAction(&_projectsFilterModel.getFilterStartupOnlyAction());

	_hierarchyWidget.setWindowIcon(windowIcon);
    _hierarchyWidget.getTreeView().setRootIsDecorated(true);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    treeView.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    connect(&treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        if (getSelectedStartupProject()->isGroup())
            return; // Do not load group items

        _loadAction.trigger();
    });

    const auto updateLoadAction = [this, &treeView]() -> void {
        const auto canLoad = treeView.selectionModel()->selectedRows().isEmpty();

        _loadAction.setText(canLoad ? "Start ManiVault" : "Load Project");
        _loadAction.setToolTip(canLoad ? "Start ManiVault" : "Load the selected project");
    };

    updateLoadAction();

    connect(treeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, updateLoadAction);
    
    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(true);

    treeViewHeader->resizeSection(static_cast<int>(ProjectsTreeModel::Column::Title), 250);

    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Downloaded), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Group), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::IsGroup), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::IconName), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Tags), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Date), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Summary), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Url), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::IsStartup), true);
    //treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::RecommendedHardwareSpec), true);

#if QT_NO_DEBUG
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::MinimumCoreVersion), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::RequiredPlugins), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::MissingPlugins), true);
#endif

    connect(&_loadAction, &TriggerAction::triggered, this, [this]() -> void {
        if (auto selectedStartupProject = getSelectedStartupProject()) {
	        if (selectedStartupProject->load()) {
                mv::projects().downloadProject(Application::current()->getStartupProjectUrl());

				accept();
			}
        }
    });

    connect(&_quitAction, &TriggerAction::triggered, this, &StartupProjectSelectorDialog::reject);
}

std::int32_t StartupProjectSelectorDialog::getSelectedStartupProjectIndex()
{
    auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
        return -1;

    return selectedRows.first().row();
}

ProjectsModelProject* StartupProjectSelectorDialog::getSelectedStartupProject() const
{
    const auto selectedRows = _hierarchyWidget.getSelectedRows();

    if (selectedRows.isEmpty())
        return {};

    const auto& firstSelectedFilterRow = selectedRows.first();

	if (auto projectsModelProject = _projectsTreeModel.getProject(firstSelectedFilterRow))
		return const_cast<mv::util::ProjectsModelProject*>(projectsModelProject);

	return {};
}
