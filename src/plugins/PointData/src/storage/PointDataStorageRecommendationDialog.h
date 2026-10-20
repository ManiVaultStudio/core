// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointDataStorageRecommendation.h"

#include <QDialog>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace mv::pointdata::storage
{

/** Presents the preliminary tiled-storage recommendation wizard for a dataset shape. */
class PointDataStorageRecommendationDialog final : public QDialog
{
public:
    PointDataStorageRecommendationDialog(QWidget* parent, std::uint64_t rows, std::uint64_t columns, std::size_t elementSize);
    ~PointDataStorageRecommendationDialog() override;

    /** Returns the recommendation selected when the dialog was accepted. */
    const TiledStorageRecommendation& recommendation() const;

private:
    class Private;
    std::unique_ptr<Private> _private;
};

}
