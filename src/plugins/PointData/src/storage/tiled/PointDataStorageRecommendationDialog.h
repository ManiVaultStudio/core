// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointDataStorageRecommendation.h"

#include <QDialog>
#include <QVariantMap>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace mv::pointdata::storage
{

/**
 * @brief Collects an access profile and proposes initial tiled-storage settings.
 *
 * The dialog applies a heuristic to the supplied matrix shape and intended
 * access patterns. Its result is a starting point for a conversion or import
 * configuration, not a measured performance guarantee. The parent owns the
 * dialog; the recommendation and profile are available after it has been
 * accepted.
 *
 * @author Thomas Kroes
 */
class PointDataStorageRecommendationDialog final : public QDialog
{
public:
    /**
     * Creates a recommendation wizard for the supplied matrix shape.
     * @param parent Widget that owns the dialog.
     * @param rows Logical number of matrix rows.
     * @param columns Logical number of matrix columns.
     * @param elementSize Size of one stored matrix element in bytes.
     */
    PointDataStorageRecommendationDialog(QWidget* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize);
    ~PointDataStorageRecommendationDialog() override;

    /** Returns the recommendation selected when the dialog was accepted. */
    const TiledStorageRecommendation& recommendation() const;

    /** Returns the intended-use profile to persist with the generated dataset. */
    QVariantMap profile() const;

private:
    class Private;
    std::unique_ptr<Private> _private; /**< Owns the wizard controls and selected recommendation state. */
};

}
