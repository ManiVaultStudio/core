#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include "widgets/HierarchyWidget.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDialog>

namespace hdps::gui {

/**
 * Presets action class
 *
 * Action class for interacting with presets.
 * 
 * TODO: Write more documentation
 *
 * @author Thomas Kroes
 */
class PresetsAction final : public WidgetAction
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        Name,       /** Name of the preset */
        Content     /** Preset content */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columnInfo;

    /** Filter model for presets model */
    class FilterModel : public QSortFilterProxyModel
    {
    public:

        /**
         * Construct the filter model with \p parent
         * @param parent Pointer to parent object
        */
        FilterModel(QObject* parent = nullptr);

        /**
         * Returns whether \p row with \p parent is filtered out (false) or in (true)
         * @param row Row index
         * @param parent Parent index
         * @return Boolean indicating whether the item is filtered in or out
         */
        bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

        /**
         * Compares two model indices plugin \p lhs with \p rhs
         * @param lhs Left-hand model index
         * @param rhs Right-hand model index
         */
        bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;
    };

    /** Dialog for managing presets */
    class Dialog final : public QDialog
    {
    protected:

        /**
         * Construct a dialog with owning \p presetsAction
         * @param presetsAction Pointer to presets action
         */
        Dialog(PresetsAction* presetsAction);

        /** Get preferred size */
        QSize sizeHint() const override {
            return QSize(640, 480);
        }

        /** Get minimum size hint*/
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

    private:
        HierarchyWidget  _hierarchyWidget;      /** Widget for displaying the presets */
        TriggerAction    _removeAction;         /** Action for removing one (or more) presets */
        TriggerAction    _okAction;             /** Action for exiting the dialog */

        friend class PresetsAction;
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param sourceAction Non-owning pointer to widget action of which to save presets
     * @param settingsKey Settings key where the presets are stored
     * @param presetType Type of preset
     * @param icon Icon in menu
     */
    PresetsAction(QObject* parent, WidgetAction* sourceAction, const QString& settingsKey = "", const QString& presetType = "", const QIcon& icon = QIcon());

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Get settings key
     * @return Settings key where the recent file paths will be stored
     */
    QString getSettingsKey() const;

    /**
     * Get preset type
     * @return Preset file type
     */
    QString getPresetType() const;

    /**
     * Get shortcut prefix
     * @return Shortcut prefix (if empty, no shortcut is created)
     */
    QString getShortcutPrefix() const;

    /**
     * Get icon
     * @return Icon for the recent file type
     */
    QIcon getIcon() const;

    /**
     * Get model
     * @return Reference to model
     */
    QStandardItemModel& getModel();

    /**
     * Get filter model
     * @return Reference to filter model
     */
    const FilterModel& getFilterModel() const;

    /**
     * Get edit action
     * @return Action which triggers a dialog in which the recent file paths can be edited
     */
    TriggerAction& getEditAction();

    /** Synchronize presets with settings */
    void updatePresetsFromSettings();

    /**
     * Get recent file paths menu
     * @return Pointer to menu
     */
    QMenu* getMenu(QWidget* parent = nullptr);

public: // Presets

    /**
     * Load preset from settings with \p name
     * @param name Name of the preset
     */
    virtual void loadPreset(const QString& name);

    /**
     * Save preset to settings with \p name
     * @param name Name of the preset
     */
    virtual void savePreset(const QString& name);

    /** Load default preset from settings */
    virtual void loadDefaultPreset();

    /** Save default preset to settings */
    virtual void saveDefaultPreset();

    /** Import preset from file */
    virtual void importPreset();

    /** Export preset to file */
    virtual void exportPreset();

signals:

    void presetsChanged();

private:
    WidgetAction*       _sourceAction;      /** Non-owning pointer to widget action of which to save presets */
    QString             _settingsKey;       /** Settings key where the preset will be stored */
    QString             _presetType;        /** Preset type */
    QString             _shortcutPrefix;    /** Shortcut prefix (if empty, no shortcut is created) */
    QIcon               _icon;              /** Icon for the recent file type */
    QStandardItemModel  _model;             /** Model for storing recent file paths */
    FilterModel         _filterModel;       /** Sort/filter model */
    TriggerAction       _editAction;        /** Action which triggers a dialog in which the recent file paths can be edited */
};

}
