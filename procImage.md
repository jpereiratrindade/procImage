# procImage

**procImage** is a comprehensive C-based toolkit for Image Processing. Initially developed for applying Local Binary Pattern (LBP) filters to images, the project aims to evolve into a robust suite for various image processing tasks. The toolkit leverages the GDAL library for handling diverse image formats and operations.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Overview

`procImage` started as a tool for applying the Local Binary Pattern (LBP) filter to images, converting them into grayscale and saving the results in the PGM format. The project is now expanding into a full-fledged image processing library, capable of handling various tasks such as filtering, transformation, and analysis of images.

## Features

- **Image Reading**: Load images from various formats using the GDAL library.
- **Grayscale Conversion**: Convert RGB images to grayscale.
- **Local Binary Pattern (LBP)**: Apply LBP filtering to images.
- **Output Formats**: Save processed images in the PGM format (with plans to support more formats).
- **Extensible Architecture**: The codebase is structured to allow easy addition of new image processing algorithms.

## Getting Started

### Prerequisites

Before compiling and running `procImage`, ensure the following dependencies are installed:

- **GDAL (Geospatial Data Abstraction Library)**: For image input/output operations.
- **C Compiler**: GCC is recommended.
- **Make**: Optional but useful for simplified compilation.

### Installation

#### GDAL Installation

- On Ubuntu/Debian:
  ```bash
  sudo apt-get install libgdal-dev gdal-bin

