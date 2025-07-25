// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageActionDelegateEditorWidget.h"

#include "AbstractPageActionsModel.h"
#include "PageAction.h"

#include "CoreInterface.h"

#include <Application.h>
#include <QBuffer>

#ifdef _DEBUG
    #define PAGE_ACTION_DELEGATE_EDITOR_WIDGET_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

PageActionDelegateEditorWidget::PageActionDelegateEditorWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent)
{
    setObjectName("PageActionDelegateEditorWidget");
    setMouseTracking(true);

    _mainLayout.setContentsMargins(1, 1, 1, 1);
    _mainLayout.setSpacing(0);

    _mainLayout.addLayout(&_iconLayout);
    _mainLayout.addLayout(&_textLayout);

    _iconLayout.setContentsMargins(5, 3, 5, 3);
    _textLayout.setContentsMargins(3, 3, 3, 3);

    _iconLayout.setAlignment(Qt::AlignTop | Qt::AlignCenter);

    _textLayout.setAlignment(Qt::AlignTop);
    _textLayout.setSpacing(3);

    _subActionsLayout.setContentsMargins(0, 0, 0, 0);
    _subActionsLayout.setSpacing(5);

    _iconLabel.setStyleSheet("padding-top: 3px;");
    _titleLabel.setStyleSheet("font-weight: bold;");

    if (PageAction::isCompactView()) {
        _textLayout.addLayout(&_primaryTextLayout);
        _textLayout.addLayout(&_secondaryTextLayout);

        _iconLayout.addWidget(&_iconLabel);

        _primaryTextLayout.addWidget(&_titleLabel, 1);
        _primaryTextLayout.addWidget(&_metaDataLabel);
        _primaryTextLayout.addWidget(&_subActionsWidget);

        _subtitleLabel.hide();
        
        _subActionsWidget.setLayout(&_subActionsLayout);
    }
    else {
        _textLayout.addLayout(&_primaryTextLayout);
        _textLayout.addLayout(&_secondaryTextLayout);
        _iconLayout.addWidget(&_iconLabel);

        _primaryTextLayout.addWidget(&_titleLabel, 1);
        _primaryTextLayout.addWidget(&_metaDataLabel);

        _secondaryTextLayout.addWidget(&_subtitleLabel, 1);
        _secondaryTextLayout.addWidget(&_subActionsWidget);

        _subActionsWidget.setLayout(&_subActionsLayout);
    }

    setLayout(&_mainLayout);

    _subtitleLabel.installEventFilter(this);
    _titleLabel.installEventFilter(this);
    _metaDataLabel.installEventFilter(this);
    
    updateCustomStyle();

    updateOverlayWidgetVisibility();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &PageActionDelegateEditorWidget::updateCustomStyle);
}

void PageActionDelegateEditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    
    if (auto proxyModel = dynamic_cast<const QSortFilterProxyModel*>(_index.model())) {
        if (auto pageActionsModel = dynamic_cast<const QStandardItemModel*>(proxyModel->sourceModel())) {
            if (auto item = dynamic_cast<AbstractPageActionsModel::Item*>(pageActionsModel->itemFromIndex(proxyModel->mapToSource(_index)))) {
                _pageAction = item->getPageAction();

                connect(_pageAction.get(), &PageAction::iconChanged, this, &PageActionDelegateEditorWidget::updateIcon);
                connect(_pageAction.get(), &PageAction::titleChanged, this, &PageActionDelegateEditorWidget::updateTitle);
                connect(_pageAction.get(), &PageAction::subtitleChanged, this, &PageActionDelegateEditorWidget::updateSubtitle);
                connect(_pageAction.get(), &PageAction::tooltipChanged, this, &PageActionDelegateEditorWidget::updateTooltip);
                connect(_pageAction.get(), &PageAction::metadataChanged, this, &PageActionDelegateEditorWidget::updateMetadata);
                connect(_pageAction.get(), &PageAction::subActionsChanged, this, &PageActionDelegateEditorWidget::updateSubActions);

                updateIcon();
                updateTitle();
                updateSubtitle();
                updateTooltip();
                updateMetadata();
                updateSubActions();
            }
        }
    }
}

bool PageActionDelegateEditorWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        updateTitle();
        updateSubtitle();
    }

    return QWidget::eventFilter(target, event);
}

void PageActionDelegateEditorWidget::enterEvent(QEnterEvent* enterEvent)
{
    QWidget::enterEvent(enterEvent);

    updateOverlayWidgetVisibility();
}

void PageActionDelegateEditorWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    updateOverlayWidgetVisibility();
}

void PageActionDelegateEditorWidget::updateIcon()
{
    _iconLabel.setPixmap(_pageAction->getIcon().pixmap(PageAction::isCompactView() ? QSize(14, 14) : QSize(24, 24)));

    updateCustomStyle();
}

void PageActionDelegateEditorWidget::updateTitle()
{
    if (!_pageAction)
        return;

    _titleLabel.setText(QFontMetrics(_titleLabel.font()).elidedText(_pageAction->getTitle(), Qt::ElideMiddle, _titleLabel.width() - 2));

    updateCustomStyle();
}

void PageActionDelegateEditorWidget::updateSubtitle()
{
    if (!_pageAction)
        return;

    _subtitleLabel.setText(QFontMetrics(_subtitleLabel.font()).elidedText(_pageAction->getSubtitle(), Qt::ElideMiddle, _subtitleLabel.width() - 2));

    updateCustomStyle();
}

void PageActionDelegateEditorWidget::updateTooltip()
{
    if (_pageAction) {
        _iconLabel.setToolTip(_pageAction->getTooltip());//_pageAction->getTooltip());
    }
}

void PageActionDelegateEditorWidget::updateMetadata()
{
    if (_pageAction) {
    	_metaDataLabel.setText(_pageAction->getMetaData());

    	updateCustomStyle();
    }
}

void PageActionDelegateEditorWidget::updateSubActions()
{
    clearLayout(&_subActionsLayout, true);

    if (_pageAction) {
        for (auto& subAction : _pageAction->getSubActions())
            _subActionsLayout.addWidget(subAction->getIconLabel());

        updateCustomStyle();
    }
}

void PageActionDelegateEditorWidget::updateOverlayWidgetVisibility()
{
    if (PageAction::isCompactView())
        _metaDataLabel.setVisible(!QWidget::underMouse());

    _subActionsWidget.setVisible(QWidget::underMouse());
}

void PageActionDelegateEditorWidget::updateCustomStyle()
{
    if (_pageAction) {
        _metaDataLabel.setStyleSheet(QString("color: %1; font-weight: bold;").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text).name()));
        _titleLabel.setStyleSheet(QString("color: %1; font-weight: bold;").arg(qApp->palette().text().color().name()));
        _subtitleLabel.setStyleSheet(QString("color: %1; font-size: 7pt;").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text).name()));
    }
    
}
