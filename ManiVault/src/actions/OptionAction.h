// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "models/TextOnlyProxyModel.h"

#include <QComboBox>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QItemSelection>
#include <QSortFilterProxyModel>

class QWidget;
class QAbstractListModel;

namespace mv::gui {

/**
 * Option widget action class
 *
 * Stores options and creates widgets to interact with these
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT OptionAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox            = 0x00001,      /** The widget includes a combobox widget */
        LineEdit            = 0x00002,      /** The widget includes a searchable line edit widget */
        HorizontalButtons   = 0x00004,      /** The widget includes a push button for each option in a horizontal layout */
        VerticalButtons     = 0x00008,      /** The widget includes a push button for each option in a vertical layout */
        Clearable           = 0x00010,      /** The widget includes a push button to clear the selection (current index is set to minus one) */

        Default = ComboBox
    };

public: // Model utility classes

    /** Performance oriented strings filter model class (does not use regex's etc.) */
    class CORE_EXPORT StringsFilterModel : public QSortFilterProxyModel
    {
    public:

        /** No need for custom constructor */
        using QSortFilterProxyModel::QSortFilterProxyModel;

        /**
         * Determines whether the row at \p sourceRow in \p sourceParent should be accepted
         * @param sourceRow Row in the source model to check
         * @param sourceParent Parent index in the source model to check
         * @return True if the row should be accepted, false otherwise
         */
        bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

        /**
         * Get the text filter
         * @return Text filter
         */
        QString getTextFilter() const;

        /**
         * Set the text filter to \p textFilter
         * @param textFilter Text filter to apply to the source model
         */
        void setTextFilter(const QString& textFilter);

    private:
        QString     _textFilter;    /** Text filter to apply to the source model */
    };

    /** Performance oriented last model which avoids unnecessary creation and clones of huge string lists */
    class LazyIndicesModel : public QAbstractListModel {
    public:

        /**
         * Construct with pointer to \p parent object (maybe nullptr)
         * @param parent Pointer to parent object (maybe nullptr)
         */
        explicit LazyIndicesModel(QObject* parent = nullptr);

        /**
         * Set the source model and the matches
         * @param sourceModel Pointer to source model
         * @param matches Vector of indices in the source model that matched
         */
        void setSourceAndMatches(QAbstractItemModel* sourceModel, const QVector<int>& matches);

        /**
         * Reimplemented to return the number of rows for \p index in the model
         * @param index Index in the model to check (defaults to invalid index)
         * @return
         */
        int rowCount(const QModelIndex& index = QModelIndex()) const override;

        /**
         * Get data at \p idx for the specified \p role
         * @param index Index in the model to get data for
         * @param role Role of the data to get (defaults to Qt::DisplayRole)
         * @return Data for the specified index and role, or an invalid QVariant if the index is invalid or the role is not Qt::DisplayRole
         */
        QVariant data(const QModelIndex& index, int role) const override;

        /**
         * Reimplemented to return the index for the specified \p row and \p column
         * @param index Parent index in the model (defaults to invalid index)
         * @return Boolean indicating whether more data can be fetched for the specified \p index
         */
        bool canFetchMore(const QModelIndex& index) const override;

        /**
         * Fetch more data for the specified \p index
         * @param index Index in the model to fetch more data for (defaults to invalid index)
         */
        void fetchMore(const QModelIndex& index) override;

    private:
        QAbstractItemModel* _sourceModel = nullptr;     /** Pointer to source model */
        QVector<int>            _all;                       /** source row indices that matched */
        int                     _loaded = 0;                /** number of rows loaded so far */
    };

public: // Widgets

    /** Combobox widget class for option action */
    class CORE_EXPORT ComboBoxWidget : public QComboBox {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionAction Pointer to option action
         */
        ComboBoxWidget(QWidget* parent, OptionAction* optionAction);

        /**
         * Paint event (overridden to show placeholder text)
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent) override;

    protected:
        OptionAction*       _optionAction;          /** Pointer to owning option action */

        friend class OptionAction;
    };

    /** Line edit widget (with auto-completion) class for option action */
    class CORE_EXPORT LineEditWidget : public QLineEdit {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionAction Pointer to option action
         */
        LineEditWidget(QWidget* parent, OptionAction* optionAction);

    protected:
        OptionAction*           _optionAction;          /** Pointer to owning option action */
        QCompleter              _completer;             /** Completer for searching and filtering */
        StringsFilterModel      _stringsFilterModel;    /** Strings filter model for optimized filtering of the source model */
        LazyIndicesModel        _lazyIndicesModel;      /** Lazy indices model for optimized lazy loading of the source model */

        friend class OptionAction;
    };

    /** Horizontal/vertical buttons widget for option action */
    class CORE_EXPORT ButtonsWidget : public QWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionAction Pointer to option action
         * @param orientation Orientation of the buttons widget (horizontal/vertical)
         */
        ButtonsWidget(QWidget* parent, OptionAction* optionAction, const Qt::Orientation& orientation);

        friend class OptionAction;
    };

protected:

    /**
     * Get widget representation of the option action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param options Options
     * @param currentOption Current option
     */
    Q_INVOKABLE explicit OptionAction(QObject* parent, const QString& title, const QStringList& options = QStringList(), const QString& currentOption = "");

    /**
     * Initialize the option action
     * @param options Options
     * @param currentOption Current option
     */
    void initialize(const QStringList& options = QStringList(), const QString& currentOption = "");

    /**
     * Initialize the option action with a custom model
     * @param customModel Pointer to custom model
     * @param currentOption Current option
     * @param defaultOption Default option
     */
    void initialize(QAbstractItemModel& customModel, const QString& currentOption = "", const QString& defaultOption = "");

    /** Get the options */
    QStringList getOptions() const;

    /** Get the number of options */
    std::uint32_t getNumberOfOptions() const;

    /** Determines whether an option exists */
    bool hasOption(const QString& option) const;

    /** Determines whether there are any options */
    bool hasOptions() const;

    /**
     * Set the options
     * @param options Options
     */
    void setOptions(const QStringList& options);

    /**
     * Set a custom item model for more advanced display of options
     * @param itemModel Pointer to custom item model
     */
    void setCustomModel(QAbstractItemModel* itemModel);

    /** Determines whether the option action has a custom item model */
    bool hasCustomModel() const;

    /**
     * Get the completion column
     * @return Completion column (defaults to 0)
     */
    std::int32_t getCompletionColumn() const;

    /**
     * Set the completion column to \p completionColumn
     * @param completionColumn Completion column (defaults to 0)
     */
    void setCompletionColumn(const std::int32_t& completionColumn);

    /**
     * Get the completion match mode
     * @return Match mode for the completer (defaults to Qt::MatchExactly)
     */
    Qt::MatchFlag getCompletionMatchMode() const;

    /**
     * Set the completion match mode for the completer to \p completionMatchMode
     * @param completionMatchMode Match mode for the completer (defaults to Qt::MatchExactly)
     */
    void setCompletionMatchMode(const Qt::MatchFlag& completionMatchMode);

    /** Get the current option index */
    std::int32_t getCurrentIndex() const;

    /**
     * Set the current option index
     * @param currentIndex Current option index
     */
    void setCurrentIndex(const std::int32_t& currentIndex);

    /** Get the current option index */
    QString getCurrentText() const;

    /**
     * Set the current option text
     * @param currentText Current option text
     */
    void setCurrentText(const QString& currentText);

    /** Get placeholder text (shown when no option selected) */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text (shown when no option selected)
     * @param placeholderString Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

    /** Determines whether an option has been selected */
    bool hasSelection() const;

    /** Get the used item model */
    const QAbstractItemModel* getModel() const;

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

protected:

    void updateCurrentIndex();
     
signals:

    /** Signals that the model changed */
    void modelChanged();

    /**
     * Signals that the custom model changed
     * @param customModel Custom model
     */
    void customModelChanged(QAbstractItemModel* customModel);

    /**
     * Signals that the current index changed
     * @param currentIndex Current index
     */
    void currentIndexChanged(const std::int32_t& currentIndex);

    /**
     * Signals that the current text changed
     * @param currentText Current text
     */
    void currentTextChanged(const QString& currentText);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

    /**
     * Signals that the completion column changed from \p previousCompletionColumn to \p currentCompletionColumn
     * @param previousCompletionColumn Previous completion column
     * @param currentCompletionColumn Current completion column
     */
    void completionColumnChanged(std::int32_t previousCompletionColumn, std::int32_t currentCompletionColumn);

    /**
     * Signals that the filter index changed from \p previousMatchMode to \p currentMatchMode
     * @param previousMatchMode Previous match mode
     * @param currentMatchMode Current match mode
     */
    void completionMatchModeChanged(const Qt::MatchFlag& previousMatchMode, const Qt::MatchFlag& currentMatchMode);

protected:
    QStringListModel        _defaultModel;              /** Default simple string list model */
    QAbstractItemModel*     _customModel;               /** Custom item model for enriched (combobox) ui */
    std::int32_t            _currentIndex;              /** Currently selected index */
    QString                 _placeholderString;         /** Place holder string */
    std::int32_t            _completionColumn;          /** Filter index on the custom model (if it exists, defaults to 0) */
    Qt::MatchFlag           _completionMatchMode;       /** Match mode for the completer (defaults to Qt::MatchExactly) */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::OptionAction)

inline const auto optionActionMetaTypeId = qRegisterMetaType<mv::gui::OptionAction*>("mv::gui::OptionAction");
