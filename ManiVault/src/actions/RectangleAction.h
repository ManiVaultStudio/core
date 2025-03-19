// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"
#include "HorizontalGroupAction.h"
#include "NumericalRangeAction.h"

#include "graphics/Bounds.h"

#include <QVBoxLayout>

namespace mv::gui {

/**
 * Rectangle action class
 *
 * Base rectangle class for storing a rectangle and interfacing with it
 *
 * @author Thomas Kroes
 */
template<typename NumericalRangeActionType>
class RectangleAction : public GroupAction
{
    typedef typename NumericalRangeActionType::ValueType NumValType;
    using ValueRange = util::NumericalRange<NumValType>;
public:

    /** Templated classes with Q_OBJECT macro are not allowed, so use function pointers instead */
    using RectangleChangedCB = std::function<void()>;

    /** Axis enum for distinguishing between x- and y axis range of the rectangle */
    enum class Axis {
        X = 0,  /** Along x-axis */
        Y,      /** Along y-axis */

        Count
    };

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    RectangleAction(QObject* parent, const QString& title) :
        GroupAction(parent, title),
        _rangeAction{ NumericalRangeActionType(this, "X-range", ValueRange(std::numeric_limits<NumValType>::lowest(), std::numeric_limits<NumValType>::max())), NumericalRangeActionType(this, "Y-range", ValueRange(std::numeric_limits<NumValType>::lowest(), std::numeric_limits<NumValType>::max())) },
        _blockRectangleChangedCallBack(false)
    {
        setDefaultWidgetFlags(WidgetFlag::Default);
        setShowLabels(false);

        getRangeAction(Axis::X).setDefaultWidgetFlags(NumericalRangeActionType::MinimumSpinBox | NumericalRangeActionType::MaximumSpinBox);
        getRangeAction(Axis::Y).setDefaultWidgetFlags(NumericalRangeActionType::MinimumSpinBox | NumericalRangeActionType::MaximumSpinBox);

        getRangeAction(Axis::X).getRangeMinAction().setPrefix("xMin: ");
        getRangeAction(Axis::X).getRangeMaxAction().setPrefix("xMax: ");

        getRangeAction(Axis::Y).getRangeMinAction().setPrefix("yMin: ");
        getRangeAction(Axis::Y).getRangeMaxAction().setPrefix("yMax: ");

        addAction(&getRangeAction(Axis::X));
        addAction(&getRangeAction(Axis::Y));
    }

    /**
     * Get rectangle
     * @return Rectangle as a pair of numerical ranges
     */
    QPair<util::NumericalRange<NumValType>, util::NumericalRange<NumValType>> getRectangle() const {
        return {
            getRangeAction(Axis::X).getRange(),
            getRangeAction(Axis::Y).getRange()
        };
    }

    /**
     * Get bounds
     * @return Rectangle as bounds
     */
    Bounds getBounds() const {
        return {
            static_cast<float>(getLeft()),
            static_cast<float>(getRight()),
            static_cast<float>(getBottom()),
            static_cast<float>(getTop())
        };
    }

    /**
     * Set rectangle with \p left, \p right, \p bottom and \p top coordinates
     * @param left Left of the rectangle
     * @param right Right of the rectangle
     * @param bottom Bottom of the rectangle
     * @param top Top of the rectangle
     */
    void setRectangle(NumValType left, NumValType right, NumValType bottom, NumValType top) {

        if (getLeft() == left && getRight() == right && getBottom() == bottom && getTop() == top)
            return;

        blockRectangleChangedCallBack();
        {
            getRangeAction(Axis::X).setRange(ValueRange(static_cast<NumValType>(left), static_cast<NumValType>(right)));
            getRangeAction(Axis::Y).setRange(ValueRange(static_cast<NumValType>(bottom), static_cast<NumValType>(top)));
        }
        unblockRectangleChangedCallBack();

        _rectangleChanged();
    }

    /**
     * Set rectangle from \p bounds
     * @param bounds Rectangle bounds
     */
    void setBounds(const Bounds& bounds) {

        if (getLeft() == bounds.getLeft() && getRight() == bounds.getRight() && getBottom() == bounds.getBottom() && getTop() == bounds.getTop())
            return;

        blockRectangleChangedCallBack();
        {
            getRangeAction(Axis::X).setRange(ValueRange(static_cast<NumValType>(bounds.getLeft()), static_cast<NumValType>(bounds.getRight())));
            getRangeAction(Axis::Y).setRange(ValueRange(static_cast<NumValType>(bounds.getBottom()), static_cast<NumValType>(bounds.getTop())));
        }
        unblockRectangleChangedCallBack();

        _rectangleChanged();
    }

    /**
     * Get rectangle width
     * @return Width of the rectangle
     */
    NumValType getWidth() const {
        return getRangeAction(Axis::X).getLength();
    }

    /**
     * Get rectangle height
     * @return Height of the rectangle
     */
    NumValType getHeight() const {
        return getRangeAction(Axis::Y).getLength();
    }

    /**
     * Get rectangle center
     * @return Center of the rectangle
     */
    QPair<NumValType, NumValType> getCenter() const {
        return { getRangeAction(Axis::X).getCenter(), getRangeAction(Axis::Y).getCenter() };
    }

    /**
     * Get rectangle left
     * @return Left of the rectangle
     */
    NumValType getLeft() const {
        return getRangeAction(Axis::X).getMinimum();
    }

    /**
     * Set rectangle left to \p left
     * @param left Rectangle left
     */
    void setLeft(typename NumericalRangeActionType::ValueType left) {
        if (left == getRangeAction(Axis::X).getMinimum())
            return;

        getRangeAction(Axis::X).setMinimum(left);
    }

    /**
     * Get rectangle right
     * @return Right of the rectangle
     */
    NumValType getRight() const {
        return getRangeAction(Axis::X).getMaximum();
    }

    /**
     * Set rectangle right to \p right
     * @param right Rectangle right
     */
    void setRight(typename NumericalRangeActionType::ValueType right) {
        if (right == getRangeAction(Axis::X).getMaximum())
            return;

        getRangeAction(Axis::X).setMaximum(right);
    }

    /**
     * Get rectangle bottom
     * @return Bottom of the rectangle
     */
    NumValType getBottom() const {
        return getRangeAction(Axis::Y).getMinimum();
    }

    /**
     * Set rectangle bottom to \p bottom
     * @param bottom Rectangle bottom
     */
    void setBottom(typename NumericalRangeActionType::ValueType bottom) {
        if (bottom == getRangeAction(Axis::Y).getMinimum())
            return;

        getRangeAction(Axis::Y).setMinimum(bottom);
    }

    /**
     * Get rectangle top
     * @return Top of the rectangle
     */
    NumValType getTop() const {
        return getRangeAction(Axis::Y).getMaximum();
    }

    /**
     * Set rectangle top to \p top
     * @param top Rectangle top
     */
    void setTop(typename NumericalRangeActionType::ValueType top) {
        if (top == getRangeAction(Axis::Y).getMaximum())
            return;

        getRangeAction(Axis::Y).setMaximum(top);
    }

    /**
     * Translate by \p translation
     * @param translation Amount of translation
     */
    void translateBy(QPair<NumValType, NumValType> translation) {
        blockRectangleChangedCallBack();
        {
            getRangeAction(Axis::X).shiftBy(translation.first);
            getRangeAction(Axis::Y).shiftBy(translation.second);
        }
        unblockRectangleChangedCallBack();

        _rectangleChanged();
    }

    /**
     * Expand by \p factor
     * @param factor Expansion factor
     */
    void expandBy(float factor) {
        blockRectangleChangedCallBack();
        {
            getRangeAction(Axis::X).expandBy(factor);
            getRangeAction(Axis::Y).expandBy(factor);
        }
        unblockRectangleChangedCallBack();

        _rectangleChanged();
    }

protected: // Callbacks blocking

    /** Prevent RectangleAction#_rectangleChanged from being called */
    void blockRectangleChangedCallBack() {
        _blockRectangleChangedCallBack = true;
    }

    /** Allow RectangleAction#_rectangleChanged to be called */
    void unblockRectangleChangedCallBack() {
        _blockRectangleChangedCallBack = false;
    }

    /**
     * Get whether RectangleAction#_rectangleChanged callbacks are blocked
     * @return Boolean determining whether RectangleAction#_rectangleChanged calls are blocked
     */
    bool isRectangleChangedCallBackBlocked() const {
        return _blockRectangleChangedCallBack;
    }

public: // Action getters

    NumericalRangeActionType& getRangeAction(const Axis& axis) { return _rangeAction[static_cast<int>(axis)]; }

    const NumericalRangeActionType& getRangeAction(const Axis& axis) const { return _rangeAction[static_cast<int>(axis)]; }

private:
    NumericalRangeActionType    _rangeAction[static_cast<int>(Axis::Count)];    /** Range actions for the x- and y axis of the rectangle */
    bool                        _blockRectangleChangedCallBack;                 /** Whether RectangleAction#_rectangleChanged calls are blocked */

protected:
    RectangleChangedCB      _rectangleChanged;      /** Callback which is called when the rectangle changed */
};

}
