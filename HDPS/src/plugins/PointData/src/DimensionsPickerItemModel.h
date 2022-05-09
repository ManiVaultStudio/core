#pragma once
#include <QAbstractItemModel>
#include <memory> // For unique_ptr

namespace hdps
{
    class DimensionsPickerHolder;

    class DimensionsPickerItemModel final : public QAbstractItemModel
    {
    private:
        DimensionsPickerHolder& _holder;

    public:
        enum class ColumnEnum: int
        {
            Name,
            Mean,
            StandardDeviation,
            count
        };

        explicit DimensionsPickerItemModel(DimensionsPickerHolder&);

        QModelIndex index(int, int, const QModelIndex &) const  override;
        int rowCount(const QModelIndex &) const override;
    private:
        // Implementation of pure virtual member functions of QAbstractItemModel
        
        int columnCount(const QModelIndex &) const noexcept override;
        
        QModelIndex parent(const QModelIndex &) const noexcept override;
        QVariant data(const QModelIndex &, int) const override;

        // Overrides of non-pure virtual member function of QAbstractItemModel
        QVariant headerData(int, Qt::Orientation, int) const override;
        Qt::ItemFlags flags(const QModelIndex&) const noexcept override;
        bool setData(const QModelIndex &, const QVariant&, int) override;
    };

}

