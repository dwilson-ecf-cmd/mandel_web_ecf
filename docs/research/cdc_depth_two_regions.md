# Registered depth-two regions

The only registered tree is:

```
R = [2, 9/4] x [0, 1/4]
├── A = [2, 17/8] x [0, 1/4]
│   ├── C = [2, 33/16] x [0, 1/4]
│   └── D = [33/16, 17/8] x [0, 1/4]
└── B = [17/8, 9/4] x [0, 1/4]
```

`split_1` bisects the real axis at `17/8` (`2.125`, binary64 bits `0x4001000000000000`); `split_2` bisects A at `33/16` (`2.0625`, bits `0x4000800000000000`). All endpoints are exactly representable dyadics. Nodes use deterministic identities `R,A,B,C,D` and lineage `A<-R`, `B<-R`, `C<-A`, `D<-A`.

Every box is closed for conventional inequality checking. For set ownership/serialization, the child on the split's left owns `[min,split)` and the right owns `[split,max]`; their union is the closed parent and their interiors are disjoint. Every width and height is positive: widths are `1/4, 1/8, 1/8, 1/16, 1/16`, height is `1/4`. No third split is registered or implied. Geometry and coverage are conventional mathematics, not CDC facts.
