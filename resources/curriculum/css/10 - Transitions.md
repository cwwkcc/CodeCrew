A transition smoothly animates a CSS property from one value to another when it changes — on hover, focus, or a class toggle. It is the right tool when you have exactly two states: before and after.

---

## Basic Syntax

```css
selector {
  transition: property duration timing-function delay;
}
```

```css
.button {
  background: navy;
  transition: background 0.3s ease;
}

.button:hover {
  background: royalblue;
}
```

Declare the `transition` on the element in its **default state**, not inside `:hover`. This ensures both the forward transition (default → hover) and the reverse (hover → default) are animated.

---

## Individual Transition Properties

```css
.card {
  transition-property:        transform, box-shadow;
  transition-duration:        0.2s, 0.3s;
  transition-timing-function: ease, ease-out;
  transition-delay:           0s, 0.05s;
}
```

The shorthand for multiple properties:

```css
.card {
  transition:
    transform   0.2s ease,
    box-shadow  0.3s ease-out 0.05s;  /* property  duration  timing  delay */
}
```

### `transition-property`

```css
transition-property: color;                     /* single property */
transition-property: color, transform, opacity; /* multiple */
transition-property: all;                       /* everything — convenient but use carefully */
```

### `transition-duration`

```css
transition-duration: 300ms;   /* milliseconds */
transition-duration: 0.3s;    /* seconds — same result */
```

Typical ranges:

- Hover/focus micro-interactions: `100ms` – `200ms`
- UI state changes (open drawer, expand card): `200ms` – `400ms`
- Decorative motion: `400ms` – `600ms`

### `transition-timing-function`

Controls the speed curve:

|Value|Behaviour|
|---|---|
|`ease`|Slow start, fast middle, slow end (default)|
|`linear`|Constant speed throughout|
|`ease-in`|Slow start, fast finish|
|`ease-out`|Fast start, slow finish — feels natural for things leaving the screen|
|`ease-in-out`|Slow at both ends|
|`cubic-bezier(x1,y1,x2,y2)`|Custom curve|
|`steps(n)`|Discrete jumps — useful for sprite animations|

```css
/* Custom springy curve */
transition-timing-function: cubic-bezier(0.34, 1.56, 0.64, 1);
```

### `transition-delay`

Wait before the transition begins:

```css
transition: color 0.3s ease 0.1s;   /* wait 0.1s, then animate over 0.3s */
```

Useful for staggering multiple elements:

```css
.menu li:nth-child(1) { transition-delay: 0ms; }
.menu li:nth-child(2) { transition-delay: 50ms; }
.menu li:nth-child(3) { transition-delay: 100ms; }
.menu li:nth-child(4) { transition-delay: 150ms; }
```

---

## What Can and Cannot Be Transitioned

### Can Transition

```css
color, background-color, border-color
opacity
transform
width, height, max-width, max-height
padding, margin
border-width, border-radius
box-shadow, text-shadow
font-size, line-height, letter-spacing
```

### Cannot Transition Smoothly (they snap)

```css
display           /* none/block snaps instantly */
font-family       /* snaps */
background-image  /* no interpolation between two images */
```

### `visibility` — Special Case

`visibility` transitions but snaps at the boundary. Combine it with `opacity` for a fade that also removes the element from accessibility:

```css
.tooltip {
  opacity: 0;
  visibility: hidden;
  transition: opacity 0.2s ease, visibility 0.2s ease;
}

.tooltip.visible {
  opacity: 1;
  visibility: visible;
}
```

---

## Common Patterns

```css
/* Smooth link colour */
a {
  color: royalblue;
  transition: color 0.15s ease;
}
a:hover { color: navy; }


/* Card lift on hover */
.card {
  transition: transform 0.2s ease, box-shadow 0.2s ease;
}
.card:hover {
  transform: translateY(-4px);
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.12);
}


/* Button press */
.btn {
  transition: transform 0.1s ease, background 0.2s ease;
}
.btn:hover  { background: royalblue; }
.btn:active { transform: scale(0.97); }


/* Input focus ring */
input {
  border: 1px solid #ccc;
  transition: border-color 0.15s ease, box-shadow 0.15s ease;
}
input:focus {
  border-color: royalblue;
  box-shadow: 0 0 0 3px rgba(65, 105, 225, 0.25);
  outline: none;
}


/* Hamburger → close icon */
.bar {
  transition: transform 0.3s ease, opacity 0.3s ease;
}
.open .bar-middle  { opacity: 0; }
.open .bar-top     { transform: rotate(45deg)  translate(5px, 5px); }
.open .bar-bottom  { transform: rotate(-45deg) translate(5px, -5px); }
```

---

## `will-change` — Performance Hint

Tells the browser a property is about to animate so it can prepare in advance:

```css
.card {
  will-change: transform;
}
```

Use sparingly — it consumes memory. Only add it when you have a real performance issue, not as a default:

```css
/* Good — only on elements about to animate */
.modal-entering { will-change: opacity, transform; }

/* Bad — applied globally */
* { will-change: transform; }   /* wastes memory */
```

---

## Accessibility

Always respect the user's motion preference:

```css
@media (prefers-reduced-motion: reduce) {
  *,
  *::before,
  *::after {
    transition-duration: 0.01ms !important;
    animation-duration:  0.01ms !important;
  }
}
```

Users who enable "Reduce Motion" in their OS have motion sensitivity, vestibular disorders, or epilepsy. Never skip this rule.

---

## Transitions vs Animations — When to Use Which

||Transitions|Animations|
|---|---|---|
|Trigger|Requires a state change (hover, focus, class toggle)|Run automatically|
|Steps|Two states only: A → B|Unlimited keyframes|
|Direction|Reverses automatically|Requires `animation-direction`|
|Use for|Hover effects, UI state changes|Looping, multi-step motion, entrance/exit sequences|

---

