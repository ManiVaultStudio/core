// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"

#include "PointData.h"

#include "pointdata_export.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

/**
 * Dimension picker action class
 *
 * Action class for picking a dimension from a points dataset
 * Also support searching for a dimension
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT DimensionPickerAction : public WidgetAction
{
Q_OBJECT

public:

    /** Widget class for points dimension picker action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionPickerAction Smart pointer to dimension picker action
         */
        Widget(QWidget* parent, DimensionPickerAction* dimensionPickerAction);

    protected:
        friend class DimensionPickerAction;
    };

protected:

    /**
     * Get widget representation of the dimension picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE DimensionPickerAction(QObject* parent, const QString& title);

    /**
     * Set the points dataset from which the dimension will be picked
     * @param points Dataset reference to points dataset
     */
    void setPointsDataset(const Dataset<Points>& points);

    /** Get the names of the dimensions */
    QStringList getDimensionNames() const;

    /** Get the number of currently loaded dimensions */
    std::uint32_t getNumberOfDimensions() const;

    /** Get the current dimension index */
    std::int32_t getCurrentDimensionIndex() const;

    /** Get the current dimension name */
    QString getCurrentDimensionName() const;

    /**
     * Set the current dimension by index
     * @param dimensionIndex Index of the current dimension
     */
    void setCurrentDimensionIndex(const std::int32_t& dimensionIndex);

    /**
     * Set the current dimension by name
     * @param dimensionName Name of the current dimension
     */
    void setCurrentDimensionName(const QString& dimensionName);

    /** Get search threshold */
    std::uint32_t getSearchThreshold() const;

    /**
     * Set search threshold
     * @param searchThreshold Search threshold
     */
    void setSearchThreshold(const std::uint32_t& searchThreshold);

    /** Establishes whether the dimensions can be searched (number of options exceeds the search threshold) */
    bool maySearch() const;

protected: // Action publishing

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

signals:

    /**
     * Signals that the current dimension index changed
     * @param currentDimensionIndex Index of the current dimension
     */
    void currentDimensionIndexChanged(const std::int32_t& currentDimensionIndex);

    /**
     * Signals that the current dimension name changed
     * @param currentDimensionName Name of the current dimension
     */
    void currentDimensionNameChanged(const QString& currentDimensionName);

public: /** Action getters */

    OptionAction& getCurrentDimensionAction() { return _currentDimensionAction; }

protected:
    Dataset<Points>     _points;                    /** Smart pointer to points dataset from which the dimension will be picked */
    OptionAction        _currentDimensionAction;    /** Current dimension action */
    std::uint32_t       _searchThreshold;           /** Select from a drop-down below the threshold and above use a search bar */

protected:
    static constexpr std::uint32_t DEFAULT_SEARCH_THRESHOLD = 1000;     /** Default search threshold */

    friend class AbstractActionsManager;
};

Q_DECLARE_METATYPE(DimensionPickerAction)

inline const auto dimensionPickerActionMetaTypeId = qRegisterMetaType<DimensionPickerAction*>("DimensionPickerAction");
