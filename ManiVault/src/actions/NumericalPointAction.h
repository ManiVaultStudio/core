// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"

#include <QPoint>
#include <QVector2D>

namespace mv::gui {

/**
 * Numerical point action base class
 *
 * Stores a two-dimensional point value.
 *
 * @author Thomas Kroes
 */
template<typename NumericalType, typename NumericalActionType>
class NumericalPointAction : public GroupAction
{
public:

    /** Axis enum  */
    enum class Axis {
        X = 0,      /** X axis */
        Y,          /** Y axis */

        Count
    };

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param minimum Minimum value
     * @param maximum Maximum value
     */
    NumericalPointAction(QObject* parent, const QString& title, NumericalType minimum, NumericalType maximum) :
        GroupAction(parent, title),
        _elementActions{
        	NumericalActionType(this, "X", minimum, maximum),
        	NumericalActionType(this, "Y", minimum, maximum)
        }
    {
        setText(title);
        setDefaultWidgetFlags(WidgetFlag::Default);

        addAction(&getXAction());
        addAction(&getYAction());
    }

    /**
     * Retrieves the X coordinate value
     * @return The X coordinate
     */
    NumericalType getX() const { return getXAction().getValue(); }

    /**
     * Set the X coordinate value
     * @param x X coordinate value
     */
    void setX(const NumericalType& x) {
	    getXAction().setValue(x);
    }

    /**
     * Set the Y coordinate value
     * @param y Y coordinate value
     */
    void setY(const NumericalType& y) {
        getYAction().setValue(y);
    }

    /**
     * Retrieves the Y coordinate value
     * @return The Y coordinate
     */
    NumericalType getY() const { return getYAction().getValue(); }

    /**
     * Set the X and Y coordinate values
     * @param x X coordinate value
     * @param y Y coordinate value
     */
    void set(const NumericalType& x, const NumericalType& y) {
        setX(x);
        setY(y);
    }

    /**
     * Retrieves the X and Y coordinate values as a QPoint
     * @return QPoint value
     */
    QPoint getPoint() const {
        return QPoint(static_cast<int>(getX()), static_cast<int>(getY()));
    }

    /**
     * Set the X and Y coordinate values from a QPoint
     * @param point QPoint value
     */
    void set(const QPoint& point) {
        set(static_cast<NumericalType>(point.x()), static_cast<NumericalType>(point.y()));
    }

    /**
     * Retrieves the X and Y coordinate values as a QPointF
     * @return QPointF value
     */
    QPointF getPointF() const {
        return QPointF(getX(), getY());
    }

    /**
     * Set the X and Y coordinate values from a QPointF
     * @param point QPointF value
     */
    void set(const QPointF& point) {
        set(static_cast<NumericalType>(point.x()), static_cast<NumericalType>(point.y()));
    }

    /**
     * Retrieves the X and Y coordinate values as a std::pair
     * @return Pair of X and Y coordinate values
     */
    std::pair<NumericalType, NumericalType> get() const {
        return std::make_pair(getX(), getY());
    }

    /**
     * Set the X and Y coordinate values from a std::pair
     * @param pair Pair of X and Y coordinate values
     */
    void set(const std::pair<NumericalType, NumericalType>& pair) {
        set(pair.first, pair.second);
    }

    /**
     * Retrieves the X and Y coordinate values as a QVector2D
     * @return QVector2D value
     */
    QVector2D getVector() const {
        return QVector2D(getX(), getY());
    }

    /**
     * Set the X and Y coordinate values from a Qt vector
     * @param vector QVector2D value
     */
    void set(const QVector2D& vector) {
        set(static_cast<NumericalType>(vector.x()), static_cast<NumericalType>(vector.y()));
    }

public: // Serialization

    /**
     * Load numerical point action from variant map
     * @param variantMap Variant map representation of the numerical point action
     */
    void fromVariantMap(const QVariantMap& variantMap) override
    {
        WidgetAction::fromVariantMap(variantMap);

        for (int axisIndex = 0; axisIndex < static_cast<int>(Axis::Count); ++axisIndex)
			_elementActions[axisIndex].fromParentVariantMap(variantMap);
    }

    /**
     * Save numerical point action to variant map
     * @return Variant map representation of the numerical point action
     */
    QVariantMap toVariantMap() const override
    {
        auto variantMap = WidgetAction::toVariantMap();

        for (int axisIndex = 0; axisIndex < static_cast<int>(Axis::Count); ++axisIndex)
            _elementActions[axisIndex].insertIntoVariantMap(variantMap);

        return variantMap;
    }

public: // Action getters

    const NumericalActionType& getAction(const Axis& axis) const { return _elementActions[static_cast<int>(axis)]; }
    const NumericalActionType& getXAction() const { return getAction(Axis::X); }
    const NumericalActionType& getYAction() const { return getAction(Axis::Y); }

    NumericalActionType& getAction(const Axis& axis) { return _elementActions[static_cast<int>(axis)]; }
    NumericalActionType& getXAction() { return getAction(Axis::X); }
    NumericalActionType& getYAction() { return getAction(Axis::Y); }

private:
    NumericalActionType     _elementActions[static_cast<int>(Axis::Count)];     /** Elements actions */
};

} 
