# 📚 Snow Programming Language Tutorial

## Table of Contents

1. [Introduction](#introduction)
2. [The Dodecagram System](#dodecagram-system)
3. [Basic Syntax](#basic-syntax)
4. [Variables and Types](#variables-and-types)
5. [Control Flow](#control-flow)
6. [Functions](#functions)
7. [Durations and Time](#durations-and-time)
8. [Derivatives](#derivatives)
9. [Advanced Features](#advanced-features)
10. [Best Practices](#best-practices)

---

## Introduction

Welcome to Snow! This tutorial will teach you the fundamentals of programming in Snow, a language designed around the beautiful mathematics of base-12 (dodecagram) and temporal intelligence.

### Your First Program

```snow
Fn = [main];
Say("Welcome to Snow!");
```

Save as `welcome.sno` and compile:
```bash
snow welcome.sno
```

---

## Dodecagram System

### Understanding Base-12

Snow uses **base-12** (dodecagram) by default. Here's why it's amazing:

| Decimal | Dodecagram | Meaning |
|---------|------------|---------|
| 10 | a | Ten |
| 11 | b | Eleven |
| 12 | 10 | One dozen |
| 24 | 20 | Two dozen |
| 144 | 100 | One gross |

### Why Base-12?

1. **Better divisibility**: 12 divides evenly by 1, 2, 3, 4, 6, 12
2. **Natural fractions**: 1/3 = 0.4, 1/4 = 0.3, 1/6 = 0.2
3. **Human-scale**: Dozens, gross, hours, months

### Writing Numbers

```snow
Fn = [main];

# Base-12 literals (default)
let dozen = 10;        # = 12 decimal
let two_dozen = 20;    # = 24 decimal
let gross = 100;       # = 144 decimal

# Using digits a and b
let ten = a;# = 10 decimal
let eleven = b;     # = 11 decimal
let forty_seven = 3b;  # = 3*12 + 11 = 47 decimal

# Explicit decimal (use 10# prefix)
let decimal_ten = 10#10;    # = 10 decimal

# Explicit dodecagram (use 12# prefix)
let dode_value = 12#1a;     # = 22 decimal

Say("Dozen: ", dozen);
Say("Gross: ", gross);
Say("Forty-seven: ", forty_seven);
```

---

## Basic Syntax

### Comments

```snow
# Single-line comment

## Multi-line
   comment
   block ##

let x = 5;  # Inline comment
```

### Case Insensitivity

Snow is NOT case-sensitive:

```snow
Say("Hello");
say("Hello");   # Same
SAY("Hello");   # Also same
sAy("Hello");   # Still same
```

### Statements

All statements end with semicolons:

```snow
let x = 5;
Say("Value: ", x);
```

Blocks use colons and keywords:

```snow
if x > 0:
    Say("Positive");
end;
```

---

## Variables and Types

### Declaration

```snow
let name = value;
```

### Examples

```snow
Fn = [main];

# Numbers (base-12)
let count = 10;         # Dozen
let value = 3b;         # Forty-seven

# Strings
let message = "Hello, Snow!";
let name = "Dodecagram";

# Expressions
let sum = 5 + 3;
let product = 10 * 2;
let division = 100 / 10;  # gross / dozen = dozen

Say("Count: ", count);
Say("Message: ", message);
Say("Sum: ", sum);
```

### Arithmetic

```snow
let a = 10;
let b = 5;

let sum = a + b;     # 10 + 5 = 15 (base-12)
let diff = a - b;       # 10 - 5 = 5
let prod = a * b;       # 10 * 5 = 50 (base-12)
let quot = a / b;       # 10 / 5 = 2
```

### Comparison

```snow
let x = 10;
let y = 5;

x == y    # Equal
x != y    # Not equal
x < y     # Less than
x > y     # Greater than
x <= y    # Less than or equal
x >= y    # Greater than or equal
```

---

## Control Flow

### If-Else

```snow
Fn = [main];

let value = 10;

if value > 5:
    Say("Value is greater than 5");
else:
    Say("Value is 5 or less");
```

### Nested Conditions

```snow
let score = 85;

if score >= 90:
    Say("Grade: A");
else:
    if score >= 80:
        Say("Grade: B");
    else:
        if score >= 70:
   Say("Grade: C");
        else:
    Say("Grade: D or F");
```

---

## Functions

### Function Declaration Style 1 (Compact)

```snow
Fn = [greet];
Say("Hello!");
```

### Function Declaration Style 2 (Traditional)

```snow
Fn greet()
    Say("Hello from function!");

# Call it
greet();
```

### Functions with Parameters

```snow
Fn add(a, b)
    ret a + b;

Fn multiply(x, y)
    ret x * y;

# Call functions
let sum = add(10, 5);
let product = multiply(10, 5);

Say("Sum: ", sum);
Say("Product: ", product);
```

### Multiple Parameters

```snow
Fn calculate(a, b, c)
    let temp = a + b;
    let result = temp * c;
    ret result;

let answer = calculate(2, 3, 4);  # (2+3)*4 = 20 (base-12)
Say("Answer: ", answer);
```

---

## Durations and Time

### Duration Literals

Snow has first-class duration support:

```snow
Fn = [main];

let short_time = 100ms;     # 100 milliseconds
let one_second = 1s;  # 1 second
let one_minute = 1m;        # 1 minute
let one_hour = 1h;          # 1 hour

# Base-12 durations
let frame = 3bms;          # 47 milliseconds
let cycle = 10s;      # 12 seconds
```

### Wait Statement

```snow
Fn = [main];

Say("Starting...");
wait 100ms;
Say("After 100ms");
wait 1s;
Say("After 1 second");
```

### Every Statement (Temporal Loops)

```snow
Fn = [main];

let counter = 10;

Say("Starting countdown...");

every 100ms:
    Say("Count: ", counter);
    counter = counter - 1;
    
    if counter == 0:
        Say("Blast off!");
        break;
end;

Say("Countdown complete!");
```

### Real-Time Animation Example

```snow
Fn = [main];

let frame = 0;
let max_frames = 100;

# 60 FPS in dodecagram
every 16ms:
    Say("Frame ", frame);
    frame = frame + 1;
    
    if frame >= max_frames:
        break;
end;

Say("Animation complete!");
```

---

## Derivatives

### Basic Derivatives

Derivatives represent **change over time**:

```snow
Fn = [main];

let position = 0;
derive velocity = d(position);

Say("Position: ", position);
Say("Velocity (change): ", velocity);
```

### Temporal Derivatives

```snow
Fn = [main];

let sensor_value = 100;

derive rate over 10ms:
    Say("Sampling rate of change");
    Say("Sensor: ", sensor_value);
end;
```

### Physics Simulation

```snow
Fn = [main];

let position = 0;
let velocity = 0;
let acceleration = 2;

every 10ms:
    # Update physics
    velocity = velocity + acceleration;
    position = position + velocity;
    
    # Derive rate of change
    derive delta_v = d(velocity);
    
    Say("Position: ", position);
    Say("Velocity: ", velocity);
    Say("Acceleration: ", delta_v);

    if position > 1000:
        break;
end;
```

---

## Advanced Features

### Complex Calculations

```snow
Fn fibonacci(n)
    if n <= 1:
      ret n;
    
    let a = fibonacci(n - 1);
    let b = fibonacci(n - 2);
    ret a + b;

let fib_10 = fibonacci(10);
Say("Fibonacci(10): ", fib_10);
```

### Base Conversion Example

```snow
Fn = [main];

# Work in base-12
let dozen = 10;
let gross = 100;

Say("Dozen (base-12): 10");
Say("Dozen (decimal): ", dozen);

Say("Gross (base-12): 100");
Say("Gross (decimal): ", gross);

# Explicit bases
let dec_val = 10#144; # 144 in decimal
let dod_val = 12#100;   # 100 in dodecagram = 144 decimal

Say("Both represent gross: ", dec_val, " and ", dod_val);
```

---

## Best Practices

### 1. Use Base-12 Thinking

```snow
# Good: Natural base-12 divisions
let quarter = 100 / 4;    # gross / 4 = 30 (base-12) = 36 decimal

# Good: Dozen-based iteration
let items = 10;           # One dozen
```

### 2. Leverage Temporal Features

```snow
# Good: Frame-accurate timing
every 16ms:
    updateGame();
end;

# Good: Precise delays
wait 3bms;  # Exactly 47ms
```

### 3. Use Derivatives for Change

```snow
# Good: Track rate of change
derive speed = d(distance);
derive acceleration = d(speed);
```

### 4. Comment Your Code

```snow
## Main game loop
   Runs at 60 FPS ##

every 16ms:
    # Update game state
    updatePhysics();
    
    # Render frame
    render();
end;
```

### 5. Descriptive Function Names

```snow
# Good
Fn calculateDistance(x1, y1, x2, y2)
    # ...

# Less clear
Fn calc(a, b, c, d)
    # ...
```

---

## Complete Example Programs

### Example 1: Temperature Converter

```snow
Fn = [main];

Fn convertToFahrenheit(celsius)
    let temp = celsius * 2;
  temp = temp + 30;  # Simplified formula
    ret temp;

let celsius = 20;
let fahrenheit = convertToFahrenheit(celsius);

Say("Celsius: ", celsius);
Say("Fahrenheit: ", fahrenheit);
```

### Example 2: Countdown Timer

```snow
Fn = [main];

let seconds = 10;  # 12 seconds in decimal

Say("Countdown starting...");

every 1s:
    Say(seconds);
    seconds = seconds - 1;
    
    if seconds == 0:
Say("Time's up!");
        break;
end;
```

### Example 3: Dodecagram Calculator

```snow
Fn = [main];

Fn add(a, b)
    ret a + b;

Fn multiply(a, b)
    ret a * b;

let x = 10;  # dozen
let y = 5;

Say("x (10 base-12) = ", x, " decimal");
Say("y (5 base-12) = ", y, " decimal");
Say("");

let sum = add(x, y);
let product = multiply(x, y);

Say("x + y = ", sum);
Say("x * y = ", product);
```

---

## Next Steps

1. **Practice**: Try modifying the example programs
2. **Experiment**: Create your own Snow programs
3. **Explore**: Read the full language specification
4. **Build**: Create useful utilities in Snow

---

## Quick Reference

### Syntax Cheat Sheet

```snow
# Variables
let name = value;

# Functions
Fn name(param1, param2)
    ret result;

# Conditionals
if condition:
    # code
else:
    # code

# Loops
every duration:
    # code
    break;  # exit loop
end;

# Output
Say("text");
Say("text: ", value);

# Time
wait duration;

# Derivatives
derive var = d(expression);
```

---

**Congratulations!** You now know the fundamentals of Snow programming! ❄️

*"Rinse and Reuse — Count in Dozens, Not Digits."*
