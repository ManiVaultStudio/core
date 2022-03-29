#pragma once

#include <actions/StringAction.h>
#include <actions/OptionAction.h>

#include <ActionsFilterModel.h>

#include <QWidget>
#include <QTreeView>
#include <QLabel>
#include <QGraphicsOpacityEffect>

namespace hdps
{

namespace gui
{

/**
 * Actions viewer widget class
 *
 * Widget class for interacting with shared actions (parameters)
 *
 * @author Thomas Kroes
 */
class ActionsViewerWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * No shared parameters overlay widget class
     *
     * @author Thomas Kroes
     */
    class OverlayWidget : public QWidget
    {
    public:
        
        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        OverlayWidget(QWidget* parent);

        /**
         * Respond to target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

        /**
         * Set icon character (Font Awesome)
         * @param iconCharacter Icon character
         */
        void setIconCharacter(const QString& iconCharacter);

        /**
         * Set title
         * @param title Title
         */
        void setTitle(const QString& title);

        /**
         * Set description
         * @param description Description
         */
        void setDescription(const QString& description);

    protected:
        QLabel                      _iconLabel;             /** Icon label */
        QLabel                      _titleLabel;            /** Overlay title label */
        QLabel                      _descriptionLabel;      /** Overlay description label */
        QGraphicsOpacityEffect      _opacityEffect;         /** Effect for modulating opacity */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    ActionsViewerWidget(QWidget* parent);

protected:

    /** Invoked when the number of rows changed (shows/hides overlay widget) */
    void numberOfRowsChanged();

protected:
    QTreeView               _treeView;              /** Tree view that contains the actions viewer */
    OverlayWidget           _overlayWidget;         /** Overlay help widget which is shown when there are no shared parameters */
    ActionsFilterModel      _filterModel;           /** Actions filter model */
    StringAction            _nameFilterAction;      /** String action for filtering based on parameter name */
    OptionAction            _typeFilterAction;      /** Option action for filtering based on parameter type */
};

}
}
