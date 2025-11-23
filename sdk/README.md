# PresetEngine SDK

This directory contains the Python Software Development Kit (SDK) for programmatically generating PresetEngine configurations.

## Overview

Instead of manually writing YAML/JSON/XML, you can use these SDKs to:
- Generate presets programmatically
- Create preset randomizers or generators
- Build algorithmic effect chains
- Integrate preset generation into your workflow tools

## Python SDK (`python/`)

**Installation:**
```bash
pip install pyyaml  # Required dependency
```

**Usage:**
```python
from preset_engine import Chain, Gain, Filter, Reverb

chain = Chain()
chain.add(Gain(db=-6.0, ui=True))
chain.add(Filter(mode="LowPass", freq=1000.0, ui=True))
chain.add(Reverb(size=0.8, wet=0.4))

# Generate YAML
print(chain.to_yaml())

# Or JSON
print(chain.to_json())
```

**Run the example:**
```bash
python sdk/python/example_generator.py
```

This will output a YAML preset that you can copy and paste directly into the PresetEngine plugin.

## Workflow

1. **Write your generator script** using the Python SDK
2. **Run the script** to generate YAML/JSON output
3. **Copy the output**
4. **Paste into PresetEngine plugin** and click "Apply"

## Example Output

Running the Python example will produce:

```yaml
- type: Gain
  gain_db:
    value: -6.0
    ui: Slider
    style: Rotary
- type: Filter
  mode: LowPass
  frequency:
    value: 1000.0
    ui: Slider
```

You can then paste this directly into the plugin's code editor.

## Advanced Usage

### Creating Custom Effect Wrappers

**Python:**
```python
def VocalChain(compression=4.0, reverb_size=0.7):
    chain = Chain()
    chain.add(Gain(db=-3.0))
    chain.add(Compressor(threshold=-12.0, ratio=compression))
    chain.add(Reverb(size=reverb_size, wet=0.3))
    chain.add(Limiter(threshold=-0.3))
    return chain

# Use it
preset = VocalChain(compression=6.0, reverb_size=0.9)
print(preset.to_yaml())
```

### Randomization

**Python:**
```python
import random

def RandomFilterChain(count=5):
    chain = Chain()
    for _ in range(count):
        freq = random.uniform(200, 5000)
        chain.add(Filter(mode="BandPass", freq=freq, q=random.uniform(0.5, 3.0)))
    return chain
```

## Contributing
