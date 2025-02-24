// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageActionDelegateEditorWidget.h"

#include "PageAction.h"

#include <Application.h>
#include <QBuffer>

#ifdef _DEBUG
    #define PAGE_ACTION_DELEGATE_EDITOR_WIDGET_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

PageActionDelegateEditorWidget::PageActionDelegateEditorWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _previewIconLabel(util::StyledIcon("image")),
    _metaDataIconLabel(util::StyledIcon("file")),
    _tagsIconLabel(util::StyledIcon("tags")),
    _downloadUrls(util::StyledIcon("download")),
    _contributorsIconLabel(util::StyledIcon("user"))
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

    _infoLayout.setContentsMargins(0, 0, 0, 0);
    _infoLayout.setSpacing(5);

    _iconLabel.setStyleSheet("padding-top: 3px;");
    _titleLabel.setStyleSheet("font-weight: bold;");

    if (PageAction::isCompactView()) {
        _textLayout.addLayout(&_primaryTextLayout);
        _textLayout.addLayout(&_secondaryTextLayout);

        _iconLayout.addWidget(&_iconLabel);

        _primaryTextLayout.addWidget(&_titleLabel, 1);
        _primaryTextLayout.addWidget(&_metaDataLabel);
        _primaryTextLayout.addWidget(&_infoWidget);

        _subtitleLabel.hide();
        
        _infoLayout.addWidget(&_previewIconLabel);
        _infoLayout.addWidget(&_metaDataIconLabel);
        _infoLayout.addWidget(&_tagsIconLabel);
        _infoLayout.addWidget(&_downloadUrls);
        _infoLayout.addWidget(&_contributorsIconLabel);
        _infoLayout.addStretch(1);

        _infoWidget.setLayout(&_infoLayout);
    }
    else {
        _textLayout.addLayout(&_primaryTextLayout);
        _textLayout.addLayout(&_secondaryTextLayout);
        _iconLayout.addWidget(&_iconLabel);

        _primaryTextLayout.addWidget(&_titleLabel, 1);
        _primaryTextLayout.addWidget(&_metaDataLabel);

        _secondaryTextLayout.addWidget(&_subtitleLabel, 1);
        _secondaryTextLayout.addWidget(&_infoWidget);

        _infoLayout.addWidget(&_previewIconLabel);
        _infoLayout.addWidget(&_metaDataIconLabel);
        _infoLayout.addWidget(&_tagsIconLabel);
        _infoLayout.addWidget(&_downloadUrls);
        _infoLayout.addWidget(&_contributorsIconLabel);
        _infoLayout.addStretch(1);

        _infoWidget.setLayout(&_infoLayout);
    }

    const auto getTooltipHtml = [](const QString& tooltipTextHtml) -> QString {
        return QString(" \
            <html> \
                <head> \
                    <style> \
                        body { \
                        } \
                    </style> \
                </head> \
                <body> \
                    <div style='width: 300px;'>%1</div> \
                </body> \
            </html> \
        ").arg(tooltipTextHtml);
    };

    _previewIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return {};

        PageAction pageAction(_index);

        QString tooltipTextHtml;

        const auto previewImage = pageAction.getPreviewImage();

        if (!previewImage.isNull()) {
            QBuffer buffer;

            buffer.open(QIODevice::WriteOnly);

            QPixmap::fromImage(previewImage).save(&buffer, "JPG");

            auto image = buffer.data().toBase64();

            tooltipTextHtml = QString("<img style='padding: 100px;'src='data:image/jpg;base64,%1'></p>").arg(image);
        }

        return getTooltipHtml(tooltipTextHtml);
    });

    _metaDataIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return {};

        PageAction pageAction(_index);

        auto metaDataHtml = QString(" \
            <table> \
                <tr> \
                    <td><b>Title:&nbsp;</b></td> \
                    <td>%1</td> \
                </tr> \
                <tr> \
                    <td><b>Description:&nbsp;</b></td> \
                    <td>%2</td> \
                </tr> \
                <tr> \
                    <td><b>Comments:&nbsp;</b></td> \
                    <td>%3</td> \
                </tr> \
            </table> \
        ").arg(pageAction.getTitle(), pageAction.getDescription(), pageAction.getComments());

        return getTooltipHtml(metaDataHtml);
    });

    _tagsIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return {};

        PageAction pageAction(_index);

        auto tagsHtml = QString("<p><b>Tags:</b>&nbsp;%1</p>").arg(pageAction.getTags().join(", "));

        return getTooltipHtml(tagsHtml);
    });

    _downloadUrls.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return {};

        PageAction pageAction(_index);

        const auto downloadUrls = pageAction.getDownloadUrls();

        QStringList downloads;

        for (const auto& downloadUrl : downloadUrls)
            downloads << QString("<p>%1</p>").arg(downloadUrl);

        if (downloadUrls.size() == 1)
            return getTooltipHtml(QString("<p><b>One download from: </b></p>%1").arg(downloads.first()));

        if (downloadUrls.size() > 1)
            return getTooltipHtml(QString("<p><b>%1 downloads from: </b></p>%2").arg(QString::number(downloadUrls.size()), downloadUrls.join("</br>")));
            
        return {};
    });

    _contributorsIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return {};

        PageAction pageAction(_index);

        auto tagsHtml = QString("<p><b>Contributors:</b>&nbsp;%1</p>").arg(pageAction.getContributors().join(", "));

        return getTooltipHtml(tagsHtml);
    });

    setLayout(&_mainLayout);

    _subtitleLabel.installEventFilter(this);
    _titleLabel.installEventFilter(this);
    _metaDataLabel.installEventFilter(this);
    
    updateCustomStyle();

    updateInfoWidgetVisibility();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &PageActionDelegateEditorWidget::updateCustomStyle);
}

void PageActionDelegateEditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    PageAction pageAction(_index);

    updateTextLabels();

    _previewIconLabel.setVisible(!pageAction.getPreviewImage().isNull());
    _tagsIconLabel.setVisible(!pageAction.getTags().isEmpty());
    _downloadUrls.setVisible(!pageAction.getDownloadUrls().isEmpty());
    _contributorsIconLabel.setVisible(!pageAction.getContributors().isEmpty());
}

bool PageActionDelegateEditorWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
            updateTextLabels();
            break;

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void PageActionDelegateEditorWidget::enterEvent(QEnterEvent* enterEvent)
{
    QWidget::enterEvent(enterEvent);

    updateInfoWidgetVisibility();
}

void PageActionDelegateEditorWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    updateInfoWidgetVisibility();
}

void PageActionDelegateEditorWidget::updateTextLabels()
{
    PageAction pageAction(_index);

    QFontMetrics titleMetrics(_titleLabel.font()), descriptionMetrics(_subtitleLabel.font());

    _titleLabel.setText(titleMetrics.elidedText(pageAction.getTitle(), Qt::ElideMiddle, _titleLabel.width() - 2));
    _subtitleLabel.setText(descriptionMetrics.elidedText(pageAction.getSubtitle(), Qt::ElideMiddle, _subtitleLabel.width() - 2));
    _metaDataLabel.setText(pageAction.getMetaData());

    updateCustomStyle();
}

void PageActionDelegateEditorWidget::updateInfoWidgetVisibility()
{
    if (PageAction::isCompactView())
        _metaDataLabel.setVisible(!QWidget::underMouse());

    _infoWidget.setVisible(QWidget::underMouse());
}

void PageActionDelegateEditorWidget::updateCustomStyle()
{
    //_metaDataLabel.setStyleSheet(stylesheet);

    PageAction pageAction(_index);

    const auto styledIcon = StyledIcon(pageAction.getIcon());

    _iconLabel.setPixmap(QIcon(styledIcon).pixmap(PageAction::isCompactView() ? QSize(14, 14) : QSize(24, 24)));
    _titleLabel.setStyleSheet(QString("color: %1; font-weight: bold;").arg(qApp->palette().text().color().name()));
    _subtitleLabel.setStyleSheet(QString("color: %1; font-size: 7pt;").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text).name()));
}
