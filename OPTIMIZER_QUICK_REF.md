# ❄️ CIAM Optimizer Quick Reference

## Optimization Passes (16+)

### -O1 (Basic) - 6 Passes
```
✓ Constant Folding          - Pre-compute literals
✓ Dead Code Elimination   - Remove unused code
✓ Peephole Optimization     - Pattern matching
✓ Bounds Check Elimination  - Safety analysis
✓ Branch Chain Optimization - Control flow
✓ Footprint Compression     - Memory minimization
```

### -O2 (Aggressive) - 11 Passes
```
✓ All -O1 passes
✓ Loop Unrolling      - Iteration expansion
✓ Tail Call Optimization   - Recursion elimination
✓ Vectorization (SIMD)     - Parallel operations
✓ Look-Ahead Optimization  - Instruction reordering
✓ Loop Fusion (Curling)    - Loop merging
```

### -O3 (Maximum) - 14+ Passes
```
✓ All -O2 passes
✓ Synchronized Scheduling  - Pipeline optimization
✓ Adaptive Tuning          - Cache & branch prediction
✓ Profile-Guided Optimization - Runtime-driven
```

### Dodecagram-Specific (All Levels)
```
✓ Base-12 Arithmetic Optimization - Specialized patterns
✓ Dozisecond Timing Optimization  - Temporal tuning
```

---

## Quick Commands

```bash
# No optimization
snow program.sno -O0

# Basic (recommended for development)
snow program.sno -O1

# Aggressive (recommended for production)
snow program.sno -O2

# Maximum (recommended for release)
snow program.sno -O3

# View optimization statistics
snow program.sno -O2 -v
```

---

## Performance Impact

| Level | Speedup | Code Size | Compile Time |
|-------|---------|-----------|--------------|
| -O0   | 1.0x    | 100%      | 1.0x         |
| -O1   | 1.7x    | 85%       | 1.2x         |
| -O2   | 2.5x | 105%      | 1.5x         |
| -O3 | 3.5x    | 115%      | 2.0x         |

---

## Optimization Examples

### Constant Folding
```snow
# Before
let x = 5 + 3;

# After (-O1)
let x = 8;# Computed at compile time
```

### Dead Code Elimination
```snow
# Before
let x = 5;
let y = 10;  # Never used

# After (-O1)
let x = 5;
# y removed
```

### Loop Unrolling
```snow
# Before
let i = 0;
while i < 4:
    process(i);
    i = i + 1;
end;

# After (-O2)
process(0);
process(1);
process(2);
process(3);
```

### Tail Call Optimization
```snow
# Before
Fn factorial(n, acc)
    if n <= 1:
        ret acc;
    ret factorial(n - 1, n * acc);  # Tail call

# After (-O2)
Fn factorial(n, acc)
    loop:
     if n <= 1:
            ret acc;
        acc = n * acc;
n = n - 1;
      goto loop;  # No stack growth
```

---

## When to Use Each Level

### -O0: Development & Debugging
- Fast compilation
- Easy debugging
- Step-through friendly

### -O1: Testing & CI
- Balanced performance
- Reasonable compile time
- Good for automated testing

### -O2: Production Builds
- Excellent performance
- Moderate compile time
- Recommended default

### -O3: Final Release
- Maximum performance
- Longer compilation
- Benchmarking & shipping

---

## See OPTIMIZER_GUIDE.md for complete documentation
