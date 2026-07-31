# Collaborator requirements

Permission to rebuild is verified. Existing repository evidence verifies Mandelbrot still BMP output, HD output, frame-sequence/video paths, a browser control/reference UI, and Python/C++ server launch paths. These observations do **not** verify which behavior the collaborator still needs.

## Requirement classes

- **Required now:** preserve every legacy source/artifact; keep conventional formulas as behavioral reference; capture requirements and reference manifests before retirement; provide explicit cancellation/error boundaries in the architecture.
- **Likely, awaiting confirmation:** Android still-image generation, interactive pan/zoom, saved artifacts, palettes and iteration controls; animation/zoom sequence based on existing artifacts.
- **Optional improvements:** modern native UI, background progress, diagnostic/export metadata, accessibility and localization.
- **Potentially retireable only after confirmation:** browser/server workflow, Python launcher, Termux execution workflow, legacy file-placement conventions.
- **Unresolved:** all product targets below. Answers are not blockers for portable boundaries, explicit backend identity, or preservation; they block workflow parity/retirement decisions.

## Focused questionnaire

1. Which fractal types are required now and later? Are still images, animation, and zoom sequences each required?
2. Which target resolutions, frame rates, and acceptable foreground/background render latencies matter?
3. What minimum Android version and device classes (phone/tablet/TV; 32/64-bit) must work?
4. Must operation be fully offline? Is rendering exclusively on-device, remote, or selectable?
5. Where should saves appear, and which image/video/manifest export formats and Android share flows are required?
6. Which palette controls and recognizable legacy visual behavior must be preserved?
7. Must users enter exact coordinates? What deep-zoom and precision expectations apply?
8. Which iteration controls and animation-path/zoom-path controls are required?
9. What cancellation responsiveness is acceptable? May rendering continue in a foreground/background service?
10. What thermal throttling, device-temperature, and battery/charging constraints are acceptable?
11. Which recognizable UI actions/layouts must remain familiar?
12. Which languages/locales and accessibility features (TalkBack, scaling, contrast, motor alternatives) are required?

## Acceptance criteria

A workflow is accepted only with a recorded collaborator answer, executable Android replacement where required, captured conventional reference output/manifest, validated formulas/palette/precision/export behavior, cancellation/error evidence where applicable, and explicit collaborator or maintainer sign-off. Unanswered optional questions remain labeled, never silently defaulted into retirement criteria.
