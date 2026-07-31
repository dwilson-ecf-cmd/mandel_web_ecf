# CDC region guards

| Guard | Exact predicate/evidence | Output | Failure | Category / role |
|---|---|---|---|---|
| enclosure beyond radius | valid `Z_n` and `inf |Z_n|>2` | `REGION_ESCAPES` | enclosure invalid or lower bound not strict | conventional |
| Study-0 real lower bound | exact box identity and `x_min^2-y_max^2+x_min=95/16>2` | escaped at `n=2` | changed bounds, rounding ambiguity | conventional analytic guard |
| analytic exclusion | an identified theorem whose premises cover all of `R` | theorem's claim | missing premise | conventional; not used |
| subdivision required | no classification guard and a sound partition | child obligations | overlap/gap, no progress | obligation-management guard, inferred |
| numeric insufficiency | invalid/non-finite/unsupported arithmetic evidence | `NUMERICALLY_UNCERTAIN` | treating uncertainty as classification | conventional safety guard |

For Study 0 the composite transition has state `(R, prove REGION_ESCAPES(R), evidence-set)`. Its guard verifies the exact conventional witness; its update installs that evidence and marks the one typed obligation discharged. The guard's truth does not alone establish CDC descent. The separately defined deficit potential changes `1 -> 0`, giving a token inequality with `delta=1`; soundness of verified claims is preserved. Subdivision is not used or certified.
