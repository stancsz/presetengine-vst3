# PresetEngine

**The Declarative, Modular Audio Effect Engine.**

PresetEngine is a revolutionary VST3 audio plugin that transforms simple text configurations into professional-grade audio processing chains and user interfaces. It empowers sound designers, developers, and producers to prototype, build, and share complex audio effects without writing a single line of C++.

![PresetEngine Banner](https://via.placeholder.com/800x200?text=PresetEngine+Dynamic+VST3)

## 🚀 Why PresetEngine?

Traditional plugin development requires compiling C++ code for every small change. PresetEngine acts as a dynamic runtime:
1.  **You write text** (YAML/JSON/XML).
2.  **PresetEngine builds** the DSP graph and the UI instantly.

It's perfect for:
*   **Rapid Prototyping**: Test an EQ chain or a multi-band compressor in seconds.
*   **Education**: Learn signal flow by seeing how modules connect.
*   **Presets**: Share entire plugin structures as small text snippets.

## ✨ Key Features

*   **📄 Modular Architecture**: Build chains from a library of studio-quality blocks (EQ, Dynamics, Reverb, etc.).
*   **⚡ Hot-Swap DSP**: Change the order of effects or add new ones on the fly without restarting your DAW.
*   **🎨 Auto-GUI**: The UI creates itself. Define a parameter as `ui: Slider`, and a rotary knob appears.
*   **🔀 Advanced Routing**: Support for Series, Parallel, and Loop processing groups.
*   **🐍 Python SDK**: Generate complex, algorithmic presets programmatically using Python.

## 📦 Included Modules

| Category | Modules |
| :--- | :--- |
| **Dynamics** | Compressor, Limiter, NoiseGate |
| **Filter / EQ** | Filter (IIR), LadderFilter (Analog-style) |
| **Spatial** | Reverb, Delay, Panner |
| **Modulation** | Chorus, Phaser |
| **Distortion** | Distortion (WaveShaper), Drive |
| **Utility** | Gain |

## 🛠️ Usage Guide

### 1. The Code View
When you open the plugin, you see the Code Editor. Here you define your chain.

**Example: A "Vocal Chain" Preset**
```yaml
- type: NoiseGate
  threshold: -45.0
  ratio: 2.0

- type: Compressor
  threshold: -18.0
  ratio: 4.0
  attack: 10.0
  release: 80.0
  ui: Slider        # Exposes this control to the Visual View

- type: Filter
  type: HighPass
  frequency: 120.0

- type: Reverb
  room_size: 0.6
  wet: 0.2
```

### 2. The Visual View
Click the **"Switch to Visual View"** button to see the generated interface.
*   Modules appear as rack units.
*   Parameters defined in your YAML are interactive knobs and sliders.
*   Changes here update the sound immediately.

## 🐍 Python SDK Integration

Want to generate presets algorithmically? Use our Python SDK located in `sdk/python`.

```python
from preset_engine import Chain, Compressor, Reverb

# Create a chain
chain = Chain()
chain.add(Compressor(threshold=-20, ratio=4))
chain.add(Reverb(room_size=0.8, wet=0.4))

# Export to file
with open("my_preset.yaml", "w") as f:
    f.write(chain.to_yaml())
```

## 🏗️ Build Instructions

### Prerequisites
*   **CMake** (3.20+)
*   **C++ Compiler** (C++17 support)
*   **JUCE Dependencies** (Linux only): `libasound2-dev`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libgl1-mesa-dev`

### Compilation
1.  **Clone**:
    ```bash
    git clone https://github.com/stancsz/presets.git
    cd presets
    ```
2.  **Configure**:
    ```bash
    cmake -B build
    ```
3.  **Build**:
    ```bash
    cmake --build build --config Release
    ```
    The VST3 plugin will be in `build/PresetEngine_artefacts/Release/VST3/`.

## 📄 License

This project is open-source and available under the MIT License.
