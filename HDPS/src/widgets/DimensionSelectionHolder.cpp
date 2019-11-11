// Its own header file.
#include "DimensionSelectionHolder.h"

#include <algorithm> // For count_if.

namespace
{
    auto makeArrayOfTrueValues(const std::size_t numberOfValues)
    {
        // In C++20, this could be make_unique_default_init.
        auto result = std::make_unique<bool[]>(numberOfValues);
        std::fill_n(result.get(), numberOfValues, true);
        return result;
    }

} // End of unnamed namespace

namespace hdps
{
    DimensionSelectionHolder::DimensionSelectionHolder(const unsigned numberOfDimensions) :
        _enabledDimensions(makeArrayOfTrueValues(numberOfDimensions)),
        _numberOfDimensions{ numberOfDimensions }
    {
    }


    DimensionSelectionHolder::DimensionSelectionHolder(
        const QString* const names,
        const unsigned numberOfDimensions)
        :
        _names([&names, numberOfDimensions]
    {
        auto result = std::make_unique<QString[]>(numberOfDimensions);
        std::copy_n(names, numberOfDimensions, result.get());
        return result;
    }()),
        _enabledDimensions(makeArrayOfTrueValues(numberOfDimensions)),
        _numberOfDimensions(numberOfDimensions)
    {
    }


    unsigned DimensionSelectionHolder::getNumberOfDimensions() const noexcept
    {
        return _numberOfDimensions;
    }


    unsigned DimensionSelectionHolder::getNumberOfSelectedDimensions() const noexcept
    {
        const bool* const enabledDimensions = _enabledDimensions.get();
        return _numberOfDimensions == 0 ? 0 :
            std::count(enabledDimensions, enabledDimensions + _numberOfDimensions, true);
    }

    QString DimensionSelectionHolder::getName(const std::size_t i) const
    {
        assert(i < _numberOfDimensions);
        return (_names == nullptr) ? QString("Dim %1").arg(i) : _names[i];
    }


    bool DimensionSelectionHolder::lessThanName(const std::size_t leftIndex, const std::size_t rightIndex) const noexcept
    {
        return  (_names == nullptr) ? (leftIndex < rightIndex) : (_names[leftIndex] < _names[rightIndex]);
    }


    void DimensionSelectionHolder::disableAllDimensions()
    {
        if (_numberOfDimensions > 0)
        {
            std::fill_n(_enabledDimensions.get(), _numberOfDimensions, false);
        }
    }


    bool DimensionSelectionHolder::tryToEnableDimensionByName(const QString& name)
    {
        for (std::size_t i{}; i < _numberOfDimensions; ++i)
        {
            if (name == _names[i])
            {
                _enabledDimensions[i] = true;
                return true;
            }
        }
        return false;
    }


    bool DimensionSelectionHolder::isDimensionEnabled(const std::size_t i) const
    {
        return _enabledDimensions[i];
    }


    void DimensionSelectionHolder::toggleDimensionEnabled(const std::size_t i)
    {
        bool& isEnabled = _enabledDimensions[i];
        isEnabled = !isEnabled;
    }


    std::vector<bool> DimensionSelectionHolder::getEnabledDimensions() const
    {
        const bool* const enabledDimensions = _enabledDimensions.get();
        return std::vector<bool>(enabledDimensions, enabledDimensions + _numberOfDimensions);
    }

} // namespace hdps
