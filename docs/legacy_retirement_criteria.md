# Legacy classification and independent retirement criteria

Preservation is mandatory during this milestone. There is no blanket “rewrite complete” switch.

| Component | Classification | Independent retirement gate |
|---|---|---|
| `render.cpp`, `render_engine.cpp` | Original C++ renderer reference | Required fractal/still/HD/animation behavior identified; exact specs and golden outputs/manifests captured; Android renderer validated for formula, precision, palette, export; no unresolved legacy-only behavior; explicit maintainer decision. |
| `server.cpp` | Original C++ server | Required request/job workflows captured and replaced; cancellation/errors/export operational; reference interactions recorded; explicit decision. |
| `server.py` | Original Python launcher/reference server | Required launch/API/browser workflow captured and replaced without Python in native validation/production; parity evidence and explicit decision. |
| `index.html`, `style.css` | Browser UI reference | Recognizable UI and controls inventoried with collaborator; Android equivalents accepted; accessibility/save/share flows operational; explicit decision. |
| `app_server`, `render`, `render_engine` | Checked-in Android/Termux binary reference artifacts | Provenance/build/reference behavior captured where feasible; no workflow depends exclusively on them; replacements operational; explicit decision. Never execute them in host validation. |
| `frames/*.bmp` | Checked-in frame-sequence artifacts | Originating spec/path/palette captured, needed golden samples/checksums retained elsewhere by decision, Android animation/export validated, explicit decision. |
| `render.bmp`, `render_hd.bmp`, `render.mp4` | Generated render artifacts/reference outputs | Manifest/spec and checksums captured; matching acceptance expectation defined and tested; export path works; explicit decision. |
| `shared/`, `runtime/`, `server/renderer/` C spine | Native C Fractal Studio spine | Current product foundation, not legacy retirement candidate; changes require normal compatibility/migration policy. |
