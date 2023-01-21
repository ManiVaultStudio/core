#include "StartPageActionDelegateEditorWidget.h"
#include "StartPageActionsModel.h"
#include "StartPageActionsFilterModel.h"

#include <Application.h>
#include <QToolButton>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE_EDITOR_WIDGET_VERBOSE
#endif

using namespace hdps;

StartPageActionDelegateEditorWidget::StartPageActionDelegateEditorWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _iconLayout(),
    _textLayout(),
    _titleLabel(),
    _descriptionLabel(),
    _commentsLabel(),
    _infoLayout()
{
    setObjectName("StartPageActionDelegateWidget");
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
    _textLayout.setColumnStretch(0, 1);

    _iconLayout.addWidget(&_iconLabel);

    _iconLabel.setStyleSheet("padding-top: 3px;");
    _titleLabel.setStyleSheet("font-weight: bold;");

    _textLayout.addWidget(&_titleLabel, 0, 0);
    _textLayout.addWidget(&_commentsLabel, 0, 1);
    _textLayout.addWidget(&_descriptionLabel, 1, 0, 1, 2);
    _textLayout.addLayout(&_infoLayout, 2, 0, 1, 2);

    {
        _infoLayout.addWidget(new QPushButton("A"));
        _infoLayout.addWidget(new QPushButton("B"));
        _infoLayout.addWidget(new QPushButton("C"));
        _infoLayout.addStretch(1);
    }

    setLayout(&_mainLayout);
}

void StartPageActionDelegateEditorWidget::setEditorData(const QModelIndex& index)
{
    StartPageAction startPageAction(index);

    _iconLabel.setPixmap(startPageAction.getIcon().pixmap(QSize(24, 24)));

    QFontMetrics titleMetrics(_titleLabel.font()), descriptionMetrics(_descriptionLabel.font());

    _titleLabel.setText(titleMetrics.elidedText(startPageAction.getTitle(), Qt::ElideMiddle, _titleLabel.width() - 2));
    _descriptionLabel.setText(descriptionMetrics.elidedText(startPageAction.getDescription(), Qt::ElideMiddle, _descriptionLabel.width() - 2));
    _commentsLabel.setText(startPageAction.getComments());

    //nonConstThis->_widget.setStyleSheet(QString("QWidget#StartPageActionDelegateWidget { background-color: %1; }").arg(option.state & QStyle::State_MouseOver ? "rgba(0, 0, 0, 100)" : "transparent"));
    //nonConstThis->_widget.setFixedWidth(option.rect.size().width());
}
