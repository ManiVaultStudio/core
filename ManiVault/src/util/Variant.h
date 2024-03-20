// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QVariant>

namespace mv::util {
    class Serializable;
}

namespace mv {

/**
 * TODO
 * 
 * @author Thomas Kroes
 */
class Variant : public QVariant
{
public:
    Variant();
    Variant(const QVariant& other);

    Variant operator =(const QVariant& other) {
        QVariant::operator=(other);

        return *this;
    }

    operator QVariant () const {
        return *this;
    }

protected:

private:

private:
    friend class mv::util::Serializable;
};

}
