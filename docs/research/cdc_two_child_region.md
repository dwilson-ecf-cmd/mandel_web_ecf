# Two-child region

The authoritative source is `CDC.pdf`, SHA-256 `5e838e88022696fbc99deec0b67be122f9cc74770153b710d7666abf0b066e7c` (verified before editing).

Parent `R=[2,9/4] + i[0,1/4]` is split on the real axis at the exact dyadic `17/8`:

* `R_left=[2,17/8] + i[0,1/4]`;
* `R_right=[17/8,9/4] + i[0,1/4]`.

Their binary64 endpoint identities are deterministic. Claim ownership uses the half-open convention `[2,17/8)` and `[17/8,9/4]` in the real coordinate (both closed in the imaginary coordinate); conventional enclosure evidence may use the displayed closed boxes. Thus the claim domains are nonempty, disjoint, and cover `R` exactly, while the closed evidence boxes share only the split boundary and have disjoint interiors. Both widths are `1/8 < 1/4`; heights remain `1/4`; areas are `1/32 < 1/16`.

For `z2=c²+c`, `Re(z2)>=x_min²-(1/4)²+x_min`. This is `95/16` on the left and `207/32` on the right, each strictly above 2. It is conventional whole-box reasoning, independently checked for each child, not sampling and not CDC descent.
