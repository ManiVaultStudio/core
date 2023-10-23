# ![AppIcon32](https://github.com/ManiVaultStudio/core/assets/58806453/5968d5a1-ff7c-4a95-84d8-aade5aff6409) ManiVault Core

[![License: LGPL v3.0](https://img.shields.io/badge/License-LGPL%20v3.0-blue.svg)](LICENSE)
![Build Status](https://github.com/ManiVaultStudio/core/actions/workflows/build.yml/badge.svg)
[![GitHub contributors](https://img.shields.io/github/contributors/ManiVaultStudio/core?color=ffdf00)](https://github.com/ManiVaultStudio/core/graphs/contributors)
[![Webpage](https://img.shields.io/badge/webpage-manivault.stuio-blue)](https://www.manivault.studio)

ManiVault is a flexible and extensible visual analytics framework for high-dimensional data.

The system allows for creating custom visualization applications tailored to your specific field, data and preferences. Applications are composed of a number of modular plugins, that can be combined to fit your needs. The plugins are fully self-encapsulated, but can communicate through ManiVault's core.
For more information on the system, see our [wiki](https://github.com/ManiVaultStudio/PublicWiki/wiki).

The best starting point are the [example plugins](https://github.com/ManiVaultStudio/ExamplePlugins) which showcase each possible plugin type.

<p align="middle">
  <img align="top" src="https://github.com/ManiVaultStudio/core/assets/58806453/701ed1e3-ec5b-42ff-93f0-a9de0d60538f" width="32%" />
  <img align="top" src="https://github.com/ManiVaultStudio/core/assets/58806453/1957eb15-af49-4e2a-bea5-752a6f1fab5c" width="32%" /> 
  <img align="top" src="https://github.com/ManiVaultStudio/core/assets/58806453/9b70fa18-da9d-4d36-9a80-feaeb9dc906e" width="32%" />
</p>

The three projects above have all been built using ManiVault ([Vieth et al. 2022](https://doi.org/10.1109/PacificVis53943.2022.00010): [plugin](https://github.com/ManiVaultStudio/SpidrPlugin), [Popa et al. 2022](https://doi.org/10.2312/gch.20221233): [plugin](https://github.com/ManiVaultStudio/SpectralViewPlugin), [Li et al. 2023](https://doi.org/10.1101/2023.03.20.532934): [plugin](https://github.com/ManiVaultStudio/SpaceWalker)).

## Installation & Building
The [install guide](https://github.com/ManiVaultStudio/core/wiki/Installation) gives a complete overview of ManiVault's dependencies and setup.

Before building ManiVault, be sure to set up it's requirements (mainly Qt6 and zlib), then copy this repo:
```
git clone --recurse-submodule git@github.com:ManiVaultStudio/core.git
```

## References
ManiVault was first presented at [IEEE VIS 2023](https://ieeevis.org/year/2023/info/papers-sessions) in the paper **ManiVault: A Flexible and Extensible Visual Analytics Framework for High-Dimensional Data** (2023). A preprint is available on [arXiv](https://arxiv.org/abs/2308.01751), other supplemental material are available [here](https://graphics.tudelft.nl/Publications-new/2024/VKTLEBEVHL24/) or on [OSF](https://osf.io/9k6jw/).

```
@misc{vkt2023manivault,
  title        = {ManiVault: A Flexible and Extensible Visual Analytics Framework for High-Dimensional Data},
  author       = {Alexander Vieth and Thomas Kroes and Julian Thijssen and Baldur van Lew and Jeroen Eggermont and Soumyadeep Basu and Elmar Eisemann and Anna Vilanova and Thomas Höllt and Boudewijn Lelieveldt},
  year         = 2023,
  doi          = {10.48550/arXiv.2308.01751},
  note         = {arXiv preprint}
}
```

## License

ManiVault is licensed under LGPL v3.0, refer to the [LICENSE file](https://github.com/ManiVaultStudio/core/blob/master/LICENSE) in the top level directory.

Copyright © 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)
