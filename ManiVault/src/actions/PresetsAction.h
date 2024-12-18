// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include "widgets/HierarchyWidget.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QCompleter>

namespace mv::gui {

/**
 * Presets action class
 *
 * Action class for interacting with widget action presets.
 * 
 * This action allows you to manage the presets for a particular widget action. More precisely it allows to:
 * - Save the widget action state to application settings
 * - Load the widget action state from application settings
 * - Save widget action default preset to application settings
 * - Load widget action default preset  from application settings
 * - Export widget action state to preset file
 * - Import widget action state from preset file
 * - Manage presets for the widget action (remove presets etc.)
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT PresetsAction final : public WidgetAction
{
    Q_OBJECT

public:

    /** Model columns */
    enum class Column {
        Name,       /** Name of the preset */
        DateTime    /** Date and time when the preset was created */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columnInfo;

    /** Filter model for presets model */
    class CORE_EXPORT FilterModel : public QSortFilterProxyModel
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

    /** Dialog for choosing a preset name */
    class CORE_EXPORT ChoosePresetNameDialog final : public QDialog
    {
    protected:

        /**
         * Construct a dialog with \p parent
         * @param presetsAction Pointer to presets action
         * @param parent Pointer to parent widget
         */
        ChoosePresetNameDialog(PresetsAction* presetsAction, QWidget* parent = nullptr);

        /** Get preferred size */
        QSize sizeHint() const override;

        /** Get minimum size hint*/
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

        /**
         * Get preset name action
         * @return Reference to preset name action
         */
        StringAction& getPresetNameAction() {
            return _presetNameAction;
        }

    private:
        StringAction    _presetNameAction;  /** Action for picking the preset name */
        QCompleter      _completer;         /** Complete with existing preset names */
        TriggerAction   _okAction;          /** Trigger action for accepting the preset name */
        TriggerAction   _cancelAction;      /** Trigger action for canceling the creation of the preset */

        friend class PresetsAction;
    };

    /** Dialog for managing presets */
    class CORE_EXPORT ManagePresetsDialog final : public QDialog
    {
    protected:

        /**
         * Construct a dialog with owning \p presetsAction
         * @param presetsAction Pointer to presets action
         */
        ManagePresetsDialog(PresetsAction* presetsAction);

        /** Get preferred size */
        QSize sizeHint() const override;

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
    Q_INVOKABLE PresetsAction(QObject* parent, WidgetAction* sourceAction, const QString& settingsKey = "", const QString& presetType = "", const QIcon& icon = QIcon());

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
     * Get icon
     * @return Icon for the preset type
     */
    QIcon getIcon() const;

    /**
     * Set icon to \p icon
     * @param icon Icon for the preset type
     */
    void setIcon(const QIcon& icon);

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

    /**
     * Get presets
     * @return Variant map containing the presets
     */
    const QVariantMap& getPresets() const;

    /** Load presets variant map from application settings */
    void loadPresetsFromApplicationSettings();

    /** Save presets variant map to application settings */
    void savePresetsToApplicationSettings() const;

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
    void loadPreset(const QString& name);

    /**
     * Save preset to settings with \p name and update application settings
     * @param name Name of the preset
     */
    void savePreset(const QString& name);

    /**
     * Remove preset with \p name and update application settings
     * @param name Name of the preset to remove
     */
    void removePreset(const QString& name);

    /**
     * Remove presets with \p names and update application settings
     * @param names Names of the preset to remove
     */
    void removePresets(const QStringList& names);

    /** Load default preset from settings */
    void loadDefaultPreset();

    /** Save default preset to settings */
    void saveDefaultPreset();

    /** Import preset from file */
    void importPreset();

    /** Export preset to file */
    void exportPreset();

private:

    /** Update the model with presets names and date time */
    void updateModel();

signals:

    /**
     * Signals that preset with \p name is about to be loaded
     * @param name Name of the preset that is about to be loaded
     */
    void presetAboutToBeLoaded(const QString& name);

    /**
     * Signals that preset with \p name is loaded
     * @param name Name of the preset that was loaded
     */
    void presetLoaded(const QString& name);

    /**
     * Signals that preset with \p name is saved
     * @param name Name of the preset that was saved
     */
    void presetSaved(const QString& name);

    /**
     * Signals that preset with \p name is about to be removed
     * @param name Name of the preset that is about to be removed
     */
    void presetAboutToBeRemoved(const QString& name);

    /**
     * Signals that preset with \p name is removed
     * @param name Name of the preset that was removed
     */
    void presetRemoved(const QString& name);

    /**
     * Signals that preset with \p filePath is imported
     * @param filePath File path of the preset that was imported
     */
    void presetImported(const QString& filePath);

    /**
     * Signals that preset with \p filePath is exported
     * @param filePath File path of the preset that was exported
     */
    void presetExported(const QString& filePath);

private:
    WidgetAction*       _sourceAction;      /** Non-owning pointer to widget action of which to save presets */
    QString             _settingsKey;       /** Settings key where the preset will be stored */
    QString             _presetType;        /** Preset type */
    QIcon               _icon;              /** Icon for the preset type */
    QStandardItemModel  _model;             /** Model for storing preset names and icons */
    FilterModel         _filterModel;       /** Sort/filter model */
    TriggerAction       _editAction;        /** Action which triggers a dialog in which the presets can be managed */
    QVariantMap         _presets;           /** Cached presets */
};

}

Q_DECLARE_METATYPE(mv::gui::PresetsAction)

inline const auto presetsActionMetaTypeId = qRegisterMetaType<mv::gui::PresetsAction*>("PresetsAction");
