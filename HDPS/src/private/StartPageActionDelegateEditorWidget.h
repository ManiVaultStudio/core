#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QModelIndex>
#include <QStyleOptionViewItem>

/**
 * Start page action delegate editor widget class
 *
 * Delegate editor widget class for custom start page action display in a view.
 *
 * @author Thomas Kroes
 */
class StartPageActionDelegateEditorWidget : public QWidget
{
    Q_OBJECT

public:

    /** Icon label with tooltip (obtained with tooltip callback function) */
    class IconLabel : public QLabel {
    public:

        using TooltipCallback = std::function<QString()>;    /** Callback function that is called when a tooltip is required */

    public:

        /**
         * Construct from \p icon and \p parent widget
         * @param icon Label icon
         * @param parent Pointer to parent widget
         */
        IconLabel(const QIcon& icon, QWidget* parent = nullptr);

        /**
         * Set the tooltip callback
         * @param tooltipCallback Callback function that is called when a tooltip is required
         */
        void setTooltipCallback(const TooltipCallback& tooltipCallback);

        /**
         * Triggered on mouse hover
         * @param enterEvent Pointer to enter event
         */
        void enterEvent(QEnterEvent* enterEvent) override;

        /**
         * Triggered on mouse leave
         * @param event Pointer to event
         */
        void leaveEvent(QEvent* event) override;

    private:

        /** Updates the label opacity depending on whether the mouse is hovering over the label */
        void updateOpacityEffect();

    private:
        QGraphicsOpacityEffect  _opacityEffect;         /** Effect for modulating label opacity */
        TooltipCallback         _tooltipCallback;       /** Callback function that is called when a tooltip is required */
    };

public:
    
    /**
     * Construct delegate editor widget with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageActionDelegateEditorWidget(QWidget* parent = nullptr);

    /**
     * Set editor data from model \p index
     * @param index Model index to fetch data from
     */
    void setEditorData(const QModelIndex& index);

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Update all text labels and takes care of elidation */
    void updateTextLabels();

private:
    QModelIndex     _index;                 /** Editor model index */
    QHBoxLayout     _mainLayout;            /** Main editor layout */
    QVBoxLayout     _iconLayout;            /** Left icon layout */
    QLabel          _iconLabel;             /** Left icon label */
    QVBoxLayout     _textLayout;            /** Right text layout */
    QHBoxLayout     _primaryTextLayout;     /** Primary layout with title and comments labels */
    QLabel          _titleLabel;            /** Title label */
    QLabel          _commentsLabel;         /** Comments label */
    QHBoxLayout     _secondaryTextLayout;   /** Secondary layout with description and info labels */
    QLabel          _descriptionLabel;      /** Description label */
    QHBoxLayout     _infoLayout;            /** Layout for preview, tags and meta info popups */
    IconLabel       _previewIconLabel;      /** Icon label for preview image */
    IconLabel       _commentsIconLabel;     /** Icon label for comments */
    IconLabel       _tagsIconLabel;         /** Icon label for tags */
};
