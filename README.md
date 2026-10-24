# LionDance-iVR-Embodied-Learning
VR embodied learning system for intangible cultural heritage skill training

## About This Project
This is a research-through-design project exploring immersive embodied interaction for the dissemination of Lion Dance intangible cultural heritage (ICH).

This repository contains the core implementation of the plum blossom pile jumping interaction module. Iteratively optimized from the original three-tier difficulty training framework, the current version focuses on the core interactive experience of plum blossom pile jumping, and supports both immersive VR mode and PC preview mode. It balances introductory skill experience and cultural communication value, expanding the reach of traditional ICH inheritance beyond professional practitioners to a broader audience interested in traditional culture. The project is built upon embodied cognition theory, transforming traditional lion dance training scenarios into an immersive VR interactive experience.

## Research Context
The inheritance of traditional Lion Dance ICH faces three practical challenges:
- High entry barriers and safety risks for beginners in high-pile training
- Geographical restrictions, concentrated mainly in the Lingnan region
- Single transmission mode, limited to offline master-apprentice instruction

This project explores how immersive VR interaction can lower the threshold of public access to ICH, enable cross-regional cultural dissemination, and provide an experiential alternative for traditional skill learning.

## Core Design
### 1. Spatial Translation of ICH Scenarios
Led the spatial transformation of traditional Lion Dance plum blossom pile scenes into a VR environment, restoring the spatial layout, movement logic and cultural context of the original training format, and ensuring the cultural authenticity of the interactive experience.

### 2. Embodied Interaction Mechanism
Designed a low-latency physical interaction system aligned with OpenXR standards to achieve precise matching between scene interaction logic and users' physical movements. Multimodal feedback (visual, vibration, voice prompts) is provided to ensure an authentic embodied learning experience.

### 3. Iterative Optimization for Cultural Dissemination
Iterated from the initial three-tier difficulty training framework for professional novices to the current core plum blossom pile jumping module. The optimization expands the target audience from skill learners to the general public interested in ICH, and strengthens the function of cultural dissemination and experience popularization.

### 4. Dual-Platform Adaptation
Supports both immersive VR head-mounted mode and PC-side preview mode. The PC version retains complete interaction logic and cultural presentation, making it easy for users without VR devices to experience the content.

## Technical Implementation
- **Engine & Standard**: Developed on Unreal Engine 5, following the OpenXR standard, compatible with mainstream PC VR headsets
- **Core Capabilities**: Low-latency hierarchical physical collision system, spatial positioning error correction mechanism, multimodal feedback system
- **Platform Support**: Immersive VR mode + PC standalone preview mode

## Repository Structure
This repository contains core interaction logic, configuration files and scenario map files of the project:
- **Blueprint** – Core interaction blueprints for the plum blossom pile training scenario
- **Config** – Project configuration files, including OpenXR settings, input mapping and rendering parameters
- **Lion** – Main training scenario assets
- **LionDance** – Core C++ source code
  - `Map` / `Maps` – Scenario map logic files
  - `Private` – C++ implementation files (.cpp)
  - `Public` – C++ header files (.h)
- **main.umap** – Entry map file for the core training scenario

## Supplementary Materials
Full design process documentation, empirical study materials and runnable prototype are provided separately:
- **Design Process & Experimental Study Compendium**: [Link to your PDF document]
- **Runnable Prototype (VR + PC version)**: [Link to cloud drive download]
- **Integrated Interactive Demo Video (all 4 research projects)**: 
  [https://www.feicut.com/fv/FV28rlr7zx9y?cm=2&fc=5&p=0]
  This interactive video features core demos of all four research projects, with clickable section navigation. The Lion   Dance iVR segment is located in the top-left module of the video interface.

## Role & Contributions
**Role**: Core Researcher & Lead Designer
**Contributions**：
- Framed the design research framework and core research questions for VR-enabled ICH embodied learning
- Led the spatial translation of Lion Dance ICH and the design of immersive interaction scenarios
- Completed the design and implementation of OpenXR compatibility and low-latency physical interaction
- Conducted an end-to-end mixed-methods empirical study on the cultural dissemination effect of the system

## Notes
- Large art assets (high-poly models, textures, baked lighting) are excluded from this repository due to file size limits. The complete full project package is available upon request.
- Full experimental data, analysis results and interview materials are compiled in the supporting research documentation, and are not included in this code repository.
