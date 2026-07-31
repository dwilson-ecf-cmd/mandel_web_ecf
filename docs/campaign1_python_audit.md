# Campaign 1 Python audit

The correction compared the Campaign 1 commit with its parent and classified every
Python path involved:

| Path | Origin/classification | Disposition |
| --- | --- | --- |
| `server.py` | Collaborator-original HTTP launcher | Preserved byte-for-byte as legacy/reference functionality. |
| `shared/__init__.py` | Campaign 1 production package | Removed; native public headers are authoritative. |
| `shared/render_spec/__init__.py` | Campaign 1 production package | Removed. |
| `shared/render_spec/model.py` | Campaign 1 production model | Replaced by `shared/include/fractal/render_spec.h` and `shared/src/render_spec.c`. |
| `shared/protocol/__init__.py` | Campaign 1 production package | Removed. |
| `shared/protocol/models.py` | Campaign 1 production model | Replaced by native model headers and sources. |
| `runtime/__init__.py` | Campaign 1 production package | Removed. |
| `runtime/memory_backend.py` | Campaign 1 production backend | Replaced by the native scoped-memory contract and system backend. |
| `server/__init__.py` | Campaign 1 production package | Removed. |
| `server/renderer/__init__.py` | Campaign 1 production package | Removed. |
| `server/renderer/backend.py` | Campaign 1 production renderer seam | Replaced by the C ABI renderer interface and C++ adapter. |
| `tests/test_spine.py` | Campaign 1 Python validation utility | Removed; native C tests cover the production spine without Python. |

No Campaign 1 Python production or test implementation was retained or demoted, as it
would create a competing authoritative model. The explanatory Markdown files and
verified discovery evidence remain documentation-only support.
