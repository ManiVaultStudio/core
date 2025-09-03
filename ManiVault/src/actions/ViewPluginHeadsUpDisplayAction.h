// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "models/HeadsUpDisplayTreeModel.h"

#include <QWidget>
#include <QLabel>

namespace mv::gui {

/**
 * View plugin heads up display action class
 *
 * For displaying a heads-up display action in the view plugin (as an overlay action)
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewPluginHeadsUpDisplayAction : public WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for displaying HUD entries */
    class CORE_EXPORT HeadsUpDisplayWidget : public QWidget {
    protected:

        /**
         * Construct with pointer to \p parent widget, \p viewPluginHeadsUpDisplayAction and \p widgetFlags
         * @param parent Pointer to parent widget
         * @param viewPluginHeadsUpDisplayAction Pointer to view plugin heads up display action
         * @param widgetFlags Widget flags
         */
        HeadsUpDisplayWidget(QWidget* parent, ViewPluginHeadsUpDisplayAction* viewPluginHeadsUpDisplayAction, const std::int32_t& widgetFlags);

        /**
         * Get minimum size hint for the widget
         * @return Minimum size hint for the widget
         */
        //QSize minimumSizeHint() const override;

    protected:
        ViewPluginHeadsUpDisplayAction* _viewPluginHeadsUpDisplayAction;    /** Pointer to view plugin heads up display action */
        std::int32_t                    _widgetFlags;                       /** Widget flags */
        QTreeView                       _treeView;                          /** Tree view for displaying the HUD entries */

        friend class ViewPluginHeadsUpDisplayAction;
    };

    /**
     * Get widget representation of the action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Construct with pointer to \p parent object and \p title of the action
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ViewPluginHeadsUpDisplayAction(QObject* parent, const QString& title);

    /**
     * Get heads up display tree model
     * @return Heads up display tree model
     */
    HeadsUpDisplayTreeModel& getHeadsUpDisplayTreeModel();

public: // Add/remove heads-up display items

    /**
     * Add heads-up display item
     * @param title Title
     * @param value Value
     * @param description Description
     * @param sharedParent Shared pointer to parent item (default: nullptr)
     */
    util::HeadsUpDisplayItemSharedPtr addHeadsUpDisplayItem(const QString& title, const QString& value, const QString& description, const util::HeadsUpDisplayItemSharedPtr& sharedParent = nullptr);

    /**
     * Remove \p headsUpDisplayItem
     * @param headsUpDisplayItem Heads-up display item to remove
     */
    void removeHeadsUpDisplayItem(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem);

    /** Remove all heads-up display items */
    void removeAllHeadsUpDisplayItems();

public: // Serialization

    /**
     * Load image action from variant
     * @param variantMap Variant representation of the image action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save image action to variant
     * @return Variant map representation of the image action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the content changed from \p previousContent to \p currentContent
     * @param previousContent Previous string content of the HUD action
     * @param currentContent Current string content of the HUD action
     */
    void contentChanged(const QString& previousContent, const QString& currentContent);

private:
    HeadsUpDisplayTreeModel     _headsUpDisplayTreeModel;   /** Heads up display tree model */
};

}

Q_DECLARE_METATYPE(mv::gui::ViewPluginHeadsUpDisplayAction)

inline const auto viewPluginHeadsUpDisplayActionMetaTypeId = qRegisterMetaType<mv::gui::ViewPluginHeadsUpDisplayAction*>("mv::gui::ViewPluginHeadsUpDisplayAction");
