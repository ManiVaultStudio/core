#pragma once

#include "WidgetAction.h"

#include <QStringListModel>

class QWidget;
class QComboBox;
class QPushButton;
class QAbstractListModel;

namespace hdps {

namespace gui {

/**
 * Option widget action class
 *
 * Stores options and creates widgets to interact with these
 *
 * @author Thomas Kroes
 */
class OptionAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Combobox widget class for option action
     */
    class ComboBoxWidget : public WidgetActionWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionAction Pointer to option action
         */
        ComboBoxWidget(QWidget* parent, OptionAction* optionAction);

    public:

        /** Gets the combobox widget */
        QComboBox* getComboBox() { return _comboBox; }

    protected:
        QComboBox*      _comboBox;      /** Combobox widget */

        friend class OptionAction;
    };

protected:

    /**
     * Get widget representation of the option action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) {
        return new OptionAction::ComboBoxWidget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param options Options
     * @param currentOption Current option
     * @param defaultOption Default option
     */
    OptionAction(QObject* parent, const QString& title = "", const QStringList& options = QStringList(), const QString& currentOption = "", const QString& defaultOption = "");

    /**
     * Initialize the option action
     * @param options Options
     * @param currentOption Current option
     * @param defaultOption Default option
     */
    void initialize(const QStringList& options = QStringList(), const QString& currentOption = "", const QString& defaultOption = "");

    /** Gets the options */
    QStringList getOptions() const;

    /** Determines whether there are any options */
    bool hasOptions() const;

    /**
     * Sets the options
     * @param options Options
     */
    void setOptions(const QStringList& options);

    /**
     * Sets a custom item model for more advanced display of options
     * @param itemModel Pointer to custom item model
     */
    void setCustomModel(QAbstractItemModel* itemModel);

    /** Determines whether the option action has a custom item model */
    bool hasCustomModel() const;

    /** Gets the current option index */
    std::int32_t getCurrentIndex() const;

    /**
     * Sets the current option index
     * @param currentIndex Current option index
     */
    void setCurrentIndex(const std::int32_t& currentIndex);

    /** Gets the default option index */
    std::int32_t getDefaultIndex() const;

    /**
     * Sets the default option index
     * @param defaultIndex Default option index
     */
    void setDefaultIndex(const std::int32_t& defaultIndex);

    /** Gets the current option index */
    QString getCurrentText() const;

    /**
     * Sets the current option text
     * @param currentText Current option text
     */
    void setCurrentText(const QString& currentText);

    /** Gets the default option text */
    QString getDefaultText() const;

    /**
     * Sets the default option text
     * @param defaultText Default option text
     */
    void setDefaultText(const QString& defaultText);

    /** Determines whether the current value can be reset to its default */
    bool canReset() const;

    /** Reset the current value to the default value */
    void reset();

    /** Determines whether an option has been selected */
    bool hasSelection() const;

    /** Gets the used item model */
    const QAbstractItemModel* getModel() const;

signals:

    /**
     * Signals that the options changed
     * @param options Options
     */
    void optionsChanged(const QStringList& options);

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
     * Signals that the default index changed
     * @param defaultIndex Default index
     */
    void defaultIndexChanged(const std::int32_t& defaultIndex);

    /**
     * Signals that the current text changed
     * @param currentText Current text
     */
    void currentTextChanged(const QString& currentText);

protected:
    QStringListModel        _defaultModel;          /** Default simple string list model */
    QAbstractItemModel*     _customModel;           /** Custom item model for enriched (combobox) ui */
    std::int32_t            _currentIndex;          /** Currently selected index */
    std::int32_t            _defaultIndex;          /** Default index */
};

}
}
