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
    _metaDataLabel(),
    _secondaryTextLayout(),
    _subtitleLabel(),
    _infoWidget(),
    _infoLayout(),
    _previewIconLabel(Application::getIconFont("FontAwesome").getIcon("image")),
    _metaDataIconLabel(Application::getIconFont("FontAwesome").getIcon("file-alt")),
    _tagsIconLabel(Application::getIconFont("FontAwesome").getIcon("tags")),
    _contributorsIconLabel(Application::getIconFont("FontAwesome").getIcon("user"))
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
    _textLayout.setSpacing(3);

    _infoLayout.setContentsMargins(0, 0, 0, 0);
    _infoLayout.setSpacing(5);

    _iconLabel.setStyleSheet("padding-top: 3px;");
    _titleLabel.setStyleSheet("font-weight: bold;");
    _metaDataLabel.setStyleSheet("color: dark-gray;");

    if (StartPageAction::isCompactView()) {
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
            return QString();

        StartPageAction startPageAction(_index);

        QString tooltipTextHtml;

        const auto previewImage = startPageAction.getPreviewImage();

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
            return QString();

        StartPageAction startPageAction(_index);

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
        ").arg(startPageAction.getTitle(), startPageAction.getDescription(), startPageAction.getComments());

        return getTooltipHtml(metaDataHtml);
    });

    _tagsIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return QString();

        StartPageAction startPageAction(_index);

        auto tagsHtml = QString("<p><b>Tags:</b>&nbsp;%1</p>").arg(startPageAction.getTags().join(", "));

        return getTooltipHtml(tagsHtml);
    });

    _contributorsIconLabel.setTooltipCallback([this, getTooltipHtml]() -> QString {
        if (!_index.isValid())
            return QString();

        StartPageAction startPageAction(_index);

        auto tagsHtml = QString("<p><b>Contributors:</b>&nbsp;%1</p>").arg(startPageAction.getContributors().join(", "));

        return getTooltipHtml(tagsHtml);
    });

    

    setLayout(&_mainLayout);

    _subtitleLabel.installEventFilter(this);
    _titleLabel.installEventFilter(this);
    _metaDataLabel.installEventFilter(this);

    updateInfoWidgetVisibility();
}

void StartPageActionDelegateEditorWidget::setEditorData(const QModelIndex& index)
{
    _index = index;

    StartPageAction startPageAction(_index);

    updateTextLabels();

    _previewIconLabel.setVisible(!startPageAction.getPreviewImage().isNull());
    _tagsIconLabel.setVisible(!startPageAction.getTags().isEmpty());
    _contributorsIconLabel.setVisible(!startPageAction.getContributors().isEmpty());
}

bool StartPageActionDelegateEditorWidget::eventFilter(QObject* target, QEvent* event)
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

void StartPageActionDelegateEditorWidget::enterEvent(QEnterEvent* enterEvent)
{
    QWidget::enterEvent(enterEvent);

    updateInfoWidgetVisibility();
}

void StartPageActionDelegateEditorWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    updateInfoWidgetVisibility();
}

void StartPageActionDelegateEditorWidget::updateTextLabels()
{
    StartPageAction startPageAction(_index);

    _iconLabel.setPixmap(startPageAction.getIcon().pixmap(StartPageAction::isCompactView() ? QSize(14, 14) : QSize(24, 24)));

    QFontMetrics titleMetrics(_titleLabel.font()), descriptionMetrics(_subtitleLabel.font());

    _titleLabel.setText(titleMetrics.elidedText(startPageAction.getTitle(), Qt::ElideMiddle, _titleLabel.width() - 2));
    _subtitleLabel.setText(descriptionMetrics.elidedText(startPageAction.getSubtitle(), Qt::ElideMiddle, _subtitleLabel.width() - 2));
    _metaDataLabel.setText(startPageAction.getMetaData());
}

void StartPageActionDelegateEditorWidget::updateInfoWidgetVisibility()
{
    if (StartPageAction::isCompactView())
        _metaDataLabel.setVisible(!QWidget::underMouse());

    _infoWidget.setVisible(QWidget::underMouse());
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
