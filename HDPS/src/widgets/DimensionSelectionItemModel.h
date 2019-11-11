#pragma once
#include <QAbstractItemModel>
#include <memory> // For unique_ptr

namespace hdps
{
    class DimensionSelectionHolder;

    class DimensionSelectionItemModel final : public QAbstractItemModel
    {
    private:
        DimensionSelectionHolder& _holder;

    public:
        enum class ColumnEnum: int
        {
            Name,
            Mean,
            StandardDeviation,
            count
        };

        explicit DimensionSelectionItemModel(DimensionSelectionHolder&);

    private:
        // Implementation of pure virtual member functions of QAbstractItemModel
        int rowCount(const QModelIndex &) const override;
        int columnCount(const QModelIndex &) const noexcept override;
        QModelIndex index(int, int, const QModelIndex &) const  override;
        QModelIndex parent(const QModelIndex &) const noexcept override;
        QVariant data(const QModelIndex &, int) const override;

        // Overrides of non-pure virtual member function of QAbstractItemModel
        QVariant headerData(int, Qt::Orientation, int) const override;
        Qt::ItemFlags flags(const QModelIndex&) const noexcept override;
        bool setData(const QModelIndex &, const QVariant&, int) override;
    };

}

