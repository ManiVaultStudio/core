# ![AppIcon32](https://github.com/ManiVaultStudio/core/assets/58806453/5968d5a1-ff7c-4a95-84d8-aade5aff6409) ManiVault Core

[![License: LGPL v3.0](https://img.shields.io/badge/License-LGPL%20v3.0-blue.svg)](LICENSE)
![Build Status](https://github.com/ManiVaultStudio/core/actions/workflows/build.yml/badge.svg?branch=master)
[![GitHub contributors](https://img.shields.io/github/contributors/ManiVaultStudio/core?color=ffdf00)](https://github.com/ManiVaultStudio/core/graphs/contributors)
[![Webpage](https://img.shields.io/badge/webpage-manivault.studio-blue)](https://www.manivault.studio)

ManiVault is a flexible and extensible visual analytics framework for high-dimensional data.

The system allows for creating custom visualization applications tailored to your specific field, data, and preferences. Applications are composed of several modular plugins, that can be combined to fit your needs. The plugins are fully self-encapsulated but can communicate through ManiVault's core.
For more information on the system, see our [documentation]([https://github.com/ManiVaultStudio/PublicWiki/wiki](https://manivault.readthedocs.io/)).

The best starting point is the [example plugins](https://github.com/ManiVaultStudio/ExamplePlugins) which showcases each possible plugin type.

<p align="middle">
  <img align="top" src="https://github.com/ManiVaultStudio/core/assets/58806453/701ed1e3-ec5b-42ff-93f0-a9de0d60538f" width="32%" />
  <img align="top" src="https://github.com/ManiVaultStudio/core/assets/58806453/1957eb15-af49-4e2a-bea5-752a6f1fab5c" width="32%" /> 
  <img align="top" src="https://github.com/ManiVaultStudio/core/assets/58806453/9b70fa18-da9d-4d36-9a80-feaeb9dc906e" width="32%" />
</p>

The three projects above have all been built using ManiVault ([Vieth et al. 2022](https://doi.org/10.1109/PacificVis53943.2022.00010): [plugin](https://github.com/ManiVaultStudio/SpidrPlugin), [Popa et al. 2022](https://doi.org/10.2312/gch.20221233): [plugin](https://github.com/ManiVaultStudio/SpectralViewPlugin), [Li et al. 2023](https://doi.org/10.1101/2023.03.20.532934): [plugin](https://github.com/ManiVaultStudio/SpaceWalker)).

## Installation & Building
The [install guide](https://github.com/ManiVaultStudio/core/wiki/Installation) gives a complete overview of ManiVault's dependencies and setup.

Before building ManiVault, be sure to set up [Qt6](https://doc.qt.io/qt-6/get-and-install-qt.html), then clone this repo:
```
git clone git@github.com:ManiVaultStudio/core.git
```

<details closed>
   
<summary>Automated using DevBundle</summary>

The preferred approach to building the example plugins is by using our [DevBundle](https://github.com/ManiVaultStudio/DevBundle) tool. This cross-platform tool creates self-contained development environments using build configurations in JSON format. Since the build environments are self-contained, multiple build environments can exist side-by-side. The major advantage of using DevBundle is that it will remove much of the configuration overhead by:
- Cloning repositories from the build configuration (with the branch specified in the build configuration)
- Downloading related binary dependencies from our [Artifactory](https://lkeb-artifactory.lumc.nl/ui/login/) server (and adding/configuring paths in the `CMakeLists.txt`)
- Setting up an umbrella `CMakeLists.txt` which consists of all projects from the build configuration

Note: the `allmain` build config in the DevBundle [config.json](https://github.com/ManiVaultStudio/DevBundle/blob/master/config.json) contains an example of how to add the core and plugins to a build configuration.

</details>

## References
ManiVault was first presented at [IEEE VIS 2023](https://ieeevis.org/year/2023/info/papers-sessions) in the paper **[ManiVault: A Flexible and Extensible Visual Analytics Framework for High-Dimensional Data](https://doi.org/10.1109/TVCG.2023.3326582)** (2023). A preprint is available on [arXiv](https://arxiv.org/abs/2308.01751), other supplemental material are available [here](https://graphics.tudelft.nl/Publications-new/2024/VKTLEBEVHL24/) or on [OSF](https://osf.io/9k6jw/).

```
@article{vkt2023manivault,
	title        = {ManiVault: A Flexible and Extensible Visual Analytics Framework for High-Dimensional Data},
	author       = {Vieth, Alexander and Kroes, Thomas and Thijssen, Julian and van Lew, Baldur and Eggermont, Jeroen and Basu, Soumyadeep and Eisemann, Elmar and Vilanova, Anna and Höllt, Thomas and Lelieveldt, Boudewijn},
	year         = 2024,
	journal      = {IEEE Transactions on Visualization and Computer Graphics},
	volume       = 30,
	number       = 1,
	pages        = {175--185},
	doi          = {10.1109/TVCG.2023.3326582}
}
```

## License

ManiVault is licensed under LGPL v3.0, refer to the [LICENSE file](https://github.com/ManiVaultStudio/core/blob/master/LICENSE) in the top level directory.

Copyright © 2024 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)
