A transition smoothly animates a property from one value to another when it changes (usually on hover or focus). It is the simplest form of CSS animation.

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

---

## Properties

```css
/* Single property */
transition: color 0.2s ease;

/* Multiple properties */
transition: color 0.2s ease, transform 0.3s ease-out;

/* All properties (use carefully — can be expensive) */
transition: all 0.3s ease;
```

### Duration

How long the transition takes. Always include a unit:

```css
transition: color 300ms ease;  /* milliseconds */
transition: color 0.3s ease;   /* seconds */
```

### Timing Function

Controls the speed curve of the transition:

|Value|Behaviour|
|---|---|
|`ease`|Slow start, fast middle, slow end (default)|
|`linear`|Constant speed|
|`ease-in`|Slow start, fast end|
|`ease-out`|Fast start, slow end|
|`ease-in-out`|Slow start and end|
|`cubic-bezier(x1,y1,x2,y2)`|Custom curve|

### Delay

Wait before the transition starts:

```css
transition: color 0.3s ease 0.1s;  /* 0.1s delay */
```

---

## What Can Be Transitioned

Properties with numeric or colour values can transition smoothly: `color`, `background-color`, `opacity`, `transform`, `width`, `height`, `padding`, `margin`, `border-radius`, `box-shadow`, `font-size`

Properties that cannot be transitioned smoothly (they snap): `display`, `visibility` (visibility actually does transition — it fades at the end), `font-family`

---

## Common Patterns

```css
/* Smooth hover colour change */
a {
  color: navy;
  transition: color 0.2s ease;
}
a:hover { color: royalblue; }

/* Scale on hover */
.card {
  transition: transform 0.2s ease, box-shadow 0.2s ease;
}
.card:hover {
  transform: translateY(-4px);
  box-shadow: 0 8px 24px rgba(0,0,0,0.12);
}

/* Smooth show/hide with opacity */
.tooltip {
  opacity: 0;
  transition: opacity 0.2s ease;
}
.tooltip.visible { opacity: 1; }
```

---