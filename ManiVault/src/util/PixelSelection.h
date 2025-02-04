// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QMap>
#include <QAbstractListModel>

namespace mv::util {

/** Pixel selection types enum */
enum class PixelSelectionType
{
    Rectangle,      /** Select pixels within a rectangle */
    Line,           /** Select pixels within a line */
    Brush,          /** A brush is used the paint the pixel selection */
    Lasso,          /** A lasso tool is used to select pixels */
    Polygon,        /** Select pixels in the interior of a polygon */
    Sample,         /** Probe one pixel at a time */
    ROI             /** Select all pixels in a ROI */
};

/** Pixel selection types alias */
using PixelSelectionTypes = QVector<PixelSelectionType>;

/** Default pixel selection types */
static const PixelSelectionTypes defaultPixelSelectionTypes = {
    PixelSelectionType::Rectangle,
    PixelSelectionType::Line,
    PixelSelectionType::Brush,
    PixelSelectionType::Polygon,
    PixelSelectionType::Lasso
};

/** Maps pixel selection type enum to name */
static const QMap<PixelSelectionType, QString> pixelSelectionTypes = {
    { PixelSelectionType::Rectangle, "Rectangle" },
    { PixelSelectionType::Line, "Line" },
    { PixelSelectionType::Brush, "Brush" },
    { PixelSelectionType::Lasso, "Lasso" },
    { PixelSelectionType::Polygon, "Polygon" },
    { PixelSelectionType::Sample, "Sample" },
    { PixelSelectionType::ROI, "ROI" }
};

/**
 * Get pixel selection type name from enum
 * @param pixelSelectionType Pixel selection type
 */
CORE_EXPORT inline QString getPixelSelectionTypeName(const PixelSelectionType& pixelSelectionType) {
    return pixelSelectionTypes.value(pixelSelectionType);
}

/**
 * Get pixel selection type enum from name
 * @param pixelSelectionTypeName Name of the pixel selection type
 */
CORE_EXPORT inline PixelSelectionType getPixelSelectionTypeEnum(const QString& pixelSelectionTypeName) {
    return pixelSelectionTypes.key(pixelSelectionTypeName);
}

/** Selection modifiers enum */
enum class PixelSelectionModifierType
{
    Replace,        /** Replace selection */
    Add,            /** Add to selection */
    Subtract        /** Subtract from selection */
};

/** Maps pixel selection modifier name to modifier enum and vice versa */
static const QMap<PixelSelectionModifierType, QString> pixelSelectionModifiers = {
    { PixelSelectionModifierType::Replace, "Replace" },
    { PixelSelectionModifierType::Add, "Add" },
    { PixelSelectionModifierType::Subtract, "Subtract" }
};

/**
 * Get pixel selection modifier type name
 * @param pixelSelectionModifierType Pixel selection modifier type
 */
CORE_EXPORT inline QString getPixelSelectionModifierTypeName(const PixelSelectionModifierType& pixelSelectionModifierType) {
    return pixelSelectionModifiers.value(pixelSelectionModifierType);
}

/**
 * Get pixel selection modifier type enum from name
 * @param pixelSelectionModifierTypeName Name of the pixel selection type
 */
CORE_EXPORT inline PixelSelectionModifierType getPixelSelectionModifierTypeEnum(const QString& pixelSelectionModifierTypeName) {
    return pixelSelectionModifiers.key(pixelSelectionModifierTypeName);
}

/**
 * Get the icon for the specified selection type
 * @param selectionType The type of selection e.g. brush rectangle etc.
 */
CORE_EXPORT QIcon getPixelSelectionTypeIcon(const PixelSelectionType& selectionType);

/**
 * Pixel selection type model class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PixelSelectionTypeModel : public QAbstractListModel {
public:
    
    /**
     * Construct with \p parent
     * @param parent Pointer to parent object
     */
    PixelSelectionTypeModel(QObject* parent = nullptr);
    
    /**
     * Set pixel selection types
     * @param pixelSelectionTypes Allowed pixel selection types
     */
    void setPixelSelectionTypes(const PixelSelectionTypes& pixelSelectionTypes);

    /**
     * Returns the number of pixel selection types in the model
     * @param parent Parent index
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns the the number of model columns
     * @param parent Parent index
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns model data for the given index
     * @param index Index
     * @param role The data role
     */
    QVariant data(const QModelIndex& index, int role) const;

protected:
    PixelSelectionTypes     _pixelSelectionTypes;       /** Allowed pixel selection types */
};

}
