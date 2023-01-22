#include "StartPageActionDelegateEditorWidget.h"
#include "StartPageActionsModel.h"
#include "StartPageActionsFilterModel.h"

#include <Application.h>
#include <QBuffer>
#include <QPainter>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE_EDITOR_WIDGET_VERBOSE
#endif

using namespace hdps;

StartPageActionDelegateEditorWidget::StartPageActionDelegateEditorWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _index(),
    _mainLayout(),
    _iconLayout(),
    _textLayout(),
    _primaryTextLayout(),
    _titleLabel(),
    _commentsLabel(),
    _secondaryTextLayout(),
    _descriptionLabel(),
    _infoLayout(),
    _previewIconLabel(Application::getIconFont("FontAwesome").getIcon("image")),
    _commentsIconLabel(Application::getIconFont("FontAwesome").getIcon("file-alt")),
    _tagsIconLabel(Application::getIconFont("FontAwesome").getIcon("tags"))
{
    setObjectName("StartPageActionDelegateEditorWidget");
    setMouseTracking(true);

    _mainLayout.setContentsMargins(1, 1, 1, 1);
    _mainLayout.setSpacing(0);

    _mainLayout.addLayout(&_iconLayout);
    _mainLayout.addLayout(&_textLayout);

    _iconLayout.setContentsMargins(5, 3, 5, 3);
    _textLayout.setContentsMargins(3, 3, 3, 3);

    _iconLayout.setAlignment(Qt::AlignTop | Qt::AlignCenter);

    _textLayout.setAlignment(Qt::AlignTop);
    _textLayout.setSpacing(1);
    _textLayout.addLayout(&_primaryTextLayout);
    _textLayout.addLayout(&_secondaryTextLayout);

    _iconLayout.addWidget(&_iconLabel);
    _iconLabel.setStyleSheet("padding-top: 3px;");

    _primaryTextLayout.addWidget(&_titleLabel, 1);
    _primaryTextLayout.addWidget(&_commentsLabel);

    _titleLabel.setStyleSheet("font-weight: bold;");
    _commentsLabel.setStyleSheet("color: dark-gray;");

    _secondaryTextLayout.addWidget(&_descriptionLabel, 1);
    _secondaryTextLayout.addLayout(&_infoLayout);

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
        const auto previewImage = _index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::PreviewImage)).data(Qt::UserRole + 1).value<QImage>();

        QString tooltipTextHtml;

        if (!previewImage.isNull()) {
            QBuffer buffer;

            buffer.open(QIODevice::WriteOnly);

            QPixmap::fromImage(previewImage).save(&buffer, "JPG");

            auto image = buffer.data().toBase64();

            tooltipTextHtml = QString("<img style='padding: 100px;'src='data:image/jpg;base64,%1'></p>").arg(image);
        }

        return getTooltipHtml(tooltipTextHtml);
    });

    _commentsIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        return getTooltipHtml(_index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Comments)).data(Qt::EditRole).toString());
    });

    _tagsIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        return getTooltipHtml(_index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Tags)).data(Qt::EditRole).toStringList().join(", "));
    });

    _infoLayout.setSpacing(5);
    _infoLayout.addWidget(&_previewIconLabel);
    _infoLayout.addWidget(&_commentsIconLabel);
    _infoLayout.addWidget(&_tagsIconLabel);
    _infoLayout.addStretch(1);
    
    setLayout(&_mainLayout);
}

void StartPageActionDelegateEditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    StartPageAction startPageAction(_index);

    _iconLabel.setPixmap(startPageAction.getIcon().pixmap(QSize(24, 24)));

    QFontMetrics titleMetrics(_titleLabel.font()), descriptionMetrics(_descriptionLabel.font());

    _titleLabel.setText(titleMetrics.elidedText(startPageAction.getTitle(), Qt::ElideMiddle, _titleLabel.width() - 2));
    _descriptionLabel.setText(descriptionMetrics.elidedText(startPageAction.getDescription(), Qt::ElideMiddle, _descriptionLabel.width() - 2));
    _commentsLabel.setText(startPageAction.getComments());

    _previewIconLabel.setVisible(!startPageAction.getPreviewImage().isNull());
    _commentsIconLabel.setVisible(!startPageAction.getComments().isEmpty());
    _tagsIconLabel.setVisible(!startPageAction.getTags().isEmpty());
}

StartPageActionDelegateEditorWidget::IconLabel::IconLabel(const QIcon& icon, QWidget* parent /*= nullptr*/) :
    QLabel(parent),
    _opacityEffect(this),
    _tooltipCallback()
{
    setPixmap(icon.pixmap(QSize(14, 14)));
    setGraphicsEffect(&_opacityEffect);

    updateOpacityEffect();
}

void StartPageActionDelegateEditorWidget::IconLabel::setTooltipCallback(const TooltipCallback& tooltipCallback)
{
    _tooltipCallback = tooltipCallback;
}

void StartPageActionDelegateEditorWidget::IconLabel::enterEvent(QEnterEvent* enterEvent)
{
    QLabel::enterEvent(enterEvent);

    updateOpacityEffect();

    if (toolTip().isEmpty() && _tooltipCallback)
        setToolTip(_tooltipCallback());
}

void StartPageActionDelegateEditorWidget::IconLabel::leaveEvent(QEvent* event)
{
    QLabel::leaveEvent(event);

    updateOpacityEffect();
}

void StartPageActionDelegateEditorWidget::IconLabel::updateOpacityEffect()
{
    _opacityEffect.setOpacity(QWidget::underMouse() ? 0.5 : 1.0);
}
