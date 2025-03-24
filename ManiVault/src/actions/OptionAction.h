// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

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
        OptionAction*   _optionAction;  /** Pointer to owning option action */

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
        OptionAction*           _optionAction;  /** Pointer to owning option action */
        QCompleter              _completer;     /** Completer for searching and filtering */
        QSortFilterProxyModel   _proxyModel;    /** Proxy model for sorting the options */

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

    protected:

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

protected:
    QStringListModel        _defaultModel;          /** Default simple string list model */
    QAbstractItemModel*     _customModel;           /** Custom item model for enriched (combobox) ui */
    std::int32_t            _currentIndex;          /** Currently selected index */
    QString                 _placeholderString;     /** Place holder string */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::OptionAction)

inline const auto optionActionMetaTypeId = qRegisterMetaType<mv::gui::OptionAction*>("mv::gui::OptionAction");
