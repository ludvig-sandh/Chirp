## Overview
Chirp is a cross-platform real-time modular synthesizer written in C++ with a focus on performance, flexibility, and clean architecture. It uses PortAudio for low-latency audio I/O and ImGui for an immediate-mode interactive graphical user interface.

The core of the system is a modular audio graph composed of nodes that generate or process audio signals. Each node implements a common interface for sample-rate updates, buffer processing, and parameter control. Typical nodes include oscillators, filters, reverb, and delay units. Nodes can be freely connected to form arbitrary effect chains, enabling complex sound design and rapid prototyping of new DSP modules.

The synthesizer supports parameter automation, where LFOs and envelopes can modulate exposed controls. Each parameter is represented by a thread-safe value container, allowing updates from both the UI and audio threads without introducing race conditions or audio dropouts.

Real-time monitoring includes a spectrogram that visualizes frequency content and meters that display overall loudness.

Chirp runs on multiple threads for better performance:

- **Audio thread** – Handles sample generation and real-time processing with strict timing guarantees. It also provides a synchronized audio stream to the analysis thread using a custom producer–consumer synchronization mechanism.

- **GUI thread** – Manages user interaction and ImGui rendering. Parameter updates from the interface are safely propagated for use by the audio thread.

- **Analysis thread** – Computes FFT data for the spectrogram and level meters, ensuring visualization never interferes with audio performance.

## Key Features

- **Modular Audio Graph**  
  Flexible node-based signal chain with components such as oscillators, filters, delays, reverbs, and gain/pan stages.  
  Nodes implement a unified processing interface for buffer handling, parameter updates, and sample-rate control.

- **Oscillators and Generators**  
  Multiple waveform generators including basic oscillators and random/periodic LFOs.  
  Frequency handling and tuning are managed through dedicated modules for stability and musical accuracy.

- **Filtering and Effects**  
  Includes biquad-based filters (low-pass, high-pass, and base filter abstractions) and time-based effects such as reverb and feedback delay lines.

- **Modulation System**  
  A modulation matrix allows routing between LFOs, envelopes, and target parameters.  
  Automation can be applied to any exposed parameter in real time.

- **Envelope and Gain Control**  
  ADSR-style envelopes shape amplitude or modulation depth, while gain and pan processors handle output balancing and spatial positioning.

- **Real-Time Visualization**  
  FFT-based spectrogram and loudness meters run on a dedicated analysis thread for continuous visual feedback without impacting audio performance.  
  Historical level data is tracked for smoother UI visualization.

- **Presets and Layout Management**  
  Built-in preset loader and synth layout management system for organizing parameter states and signal routing configurations.

- **Threaded Architecture**  
  Separate threads for audio processing, GUI rendering, and analysis.  
  Custom producer–consumer synchronization ensures safe, low-latency communication between components.

- **Cross-Platform GUI**  
  Built with ImGui for a responsive, immediate-mode interface that allows live tweaking of synthesis parameters, patch creation, and effect routing.

- **Extensible DSP Framework**  
  Designed for rapid development of new DSP modules. Developers can easily add new node types, effects, or modulation sources by extending the base processor interfaces.

TODO:
[X] Fix reverb to apply on separate channels rather than one of them
[X] Custom logarithmic sliders for Hz in the range [20, 20000]
[X] Add delay effect
[X] Allow for saving and loading presets to files
[X] Add built-in synth presets that user can select from
[X] Add LFOs that can be linked to oscillator pitch etc
[ ] Add release as part of envelope setting
[ ] Add curves for envelope settings
[ ] Add UI elements next to envelopes that visually show their shape
[ ] Add visual keyboard UI element in synth mode
[ ] Add visual UI element that shows spectrum
[ ] Organize (and stick) windows to look amazing
[ ] Test on mac and linux as well
[ ] Record a demo of the application and upload to youtube
[ ] Write extensive readme with explanations and figures