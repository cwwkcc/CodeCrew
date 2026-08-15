The `transform` property moves, scales, rotates, or skews an element visually without affecting the document layout. Surrounding elements do not shift — the element's original space is preserved. Transforms are GPU-composited, making them the correct choice for any animated movement.

---

## Transform Functions

### Translate — Move

```css
transform: translateX(50px); /* move right */
transform: translateX(-50px); /* move left */
transform: translateY(20px); /* move down */
transform: translateY(-20px); /* move up */

/* Both axes at once */
transform: translate(50px, -20px); /* right 50px, up 20px */

/* Percentage — relative to the element's own dimensions */
transform: translate(
  -50%,
  -50%
); /* move left by half own width, up by half own height */
transform: translateX(100%); /* slide off screen to the right */
```

### Scale — Resize

```css
transform: scale(1.5); /* 150% — grow */
transform: scale(0.8); /* 80% — shrink */
transform: scaleX(2); /* stretch horizontally only */
transform: scaleY(0.5); /* squish vertically only */
transform: scale(1.2, 0.9); /* different X and Y */
transform: scaleX(-1); /* flip horizontally — mirror */
```

### Rotate

```css
transform: rotate(45deg); /* clockwise */
transform: rotate(-90deg); /* counter-clockwise */
transform: rotate(0.5turn); /* half turn = 180deg */
transform: rotate(1turn); /* full rotation */

/* 3D rotation */
transform: rotateX(30deg); /* tilt top toward viewer */
transform: rotateY(60deg); /* spin around vertical axis */
transform: rotateZ(45deg); /* same as rotate() */
```

### Skew

```css
transform: skewX(10deg); /* slant along X axis */
transform: skewY(5deg); /* slant along Y axis */
transform: skew(10deg, 5deg); /* both axes */
```

---

## Chaining Multiple Transforms

List them space-separated. **Order matters** — they apply right to left:

```css
/* Rotates first, then translates along the rotated axis */
transform: rotate(45deg) translate(100px, 0);

/* Translates first in original space, then rotates in place */
transform: translate(100px, 0) rotate(45deg);
```

These produce different results. Swap the order to see the difference.

---

## `transform-origin`

The pivot point for rotations and scales. Default is `50% 50%` (the element's centre):

```css
transform-origin: center; /* 50% 50% — default */
transform-origin: top left; /* 0 0 — top-left corner */
transform-origin: bottom center; /* 50% 100% — bottom edge */
transform-origin: 0 0; /* explicit top-left */
transform-origin: 20px 40px; /* specific offset */
```

```css
/* Accordion chevron rotates around its centre — default, no change needed */
.chevron {
  transition: transform 0.2s ease;
}
.open .chevron {
  transform: rotate(180deg);
}

/* Door hinge — rotates around the left edge */
.door {
  transform-origin: left center;
  transition: transform 0.4s ease;
}
.door.open {
  transform: rotateY(-90deg);
}
```

---

## Classic Centring Trick

Centre an absolutely positioned element of unknown size:

```css
.modal {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  /*
    top/left puts the top-left corner at the centre of the parent.
    translate(-50%, -50%) shifts it back by half its own width and height.
    Result: element is perfectly centred regardless of its size.
  */
}
```

---

## 3D Transforms and Perspective

3D transforms work in three dimensions. Set `perspective` on the **parent** to define the depth of the 3D stage — lower values mean more dramatic depth, higher values mean flatter:

```css
.scene {
  perspective: 800px; /* typical range: 400px – 1200px */
}

.card {
  transform-style: preserve-3d; /* children also exist in 3D space */
  transition: transform 0.6s ease;
}

.card:hover {
  transform: rotateY(180deg);
}
```

### Card Flip

```html
<div class="scene">
  <div class="card">
    <div class="face front">Front</div>
    <div class="face back">Back</div>
  </div>
</div>
```

```css
.scene {
  width: 200px;
  height: 200px;
  perspective: 800px;
}

.card {
  width: 100%;
  height: 100%;
  position: relative;
  transform-style: preserve-3d;
  transition: transform 0.6s ease;
}

.card:hover {
  transform: rotateY(180deg);
}

.face {
  position: absolute;
  inset: 0;
  backface-visibility: hidden; /* hide when rotated past 90deg */
  display: flex;
  align-items: center;
  justify-content: center;
}

.back {
  transform: rotateY(180deg); /* pre-rotate so it faces backward at start */
}
```

### `backface-visibility`

```css
backface-visibility: hidden; /* face disappears when rotated away from viewer */
backface-visibility: visible; /* face stays visible (default) */
```

---

## Why `transform` Instead of `top` / `left`

```css
/* Slow — triggers layout recalculation on every frame */
@keyframes move-bad {
  from {
    left: 0;
  }
  to {
    left: 200px;
  }
}

/* Fast — handled entirely by GPU, no layout recalculation */
@keyframes move-good {
  from {
    transform: translateX(0);
  }
  to {
    transform: translateX(200px);
  }
}
```

Animating `top`, `left`, `width`, or `height` forces the browser to recalculate every surrounding element's position on every frame. Animating `transform` skips Layout and Paint entirely — the GPU handles it. Always use `transform` for movement.

---

## Common Patterns

```css
/* Hover lift */
.card {
  transition: transform 0.2s ease;
}
.card:hover {
  transform: translateY(-4px);
}

/* Icon rotation on hover */
.icon {
  transition: transform 0.3s ease;
}
.icon:hover {
  transform: rotate(15deg);
}

/* Slide drawer in from left */
.drawer {
  transform: translateX(-100%);
  transition: transform 0.3s ease;
}
.drawer.open {
  transform: translateX(0);
}

/* Chevron flip (accordion) */
.chevron {
  transition: transform 0.2s ease;
}
.accordion.open .chevron {
  transform: rotate(180deg);
}

/* Scale button on press */
.btn:active {
  transform: scale(0.97);
}
```

---

---

## Common Mistakes

```css
/* WRONG: forgetting transform-origin changes the pivot point —
   a rotation looks correct in one context and wrong in another
   because the default origin (center) wasn't what was assumed */
.corner-ribbon {
  transform: rotate(-45deg); /* rotates around its own center by default —
                                    for a corner ribbon you usually want it
                                    to pivot from a corner instead */
}

/* CORRECT: set the pivot point explicitly for the effect you actually want */
.corner-ribbon {
  transform-origin: top left;
  transform: rotate(-45deg);
}
```

- **Not realizing `transform` doesn't affect document flow.** Moving an element with `transform: translateX(50px)` shifts it visually but leaves a "hole" where it used to be and doesn't push neighboring elements — this is often exactly what you want (it's why transforms are cheap to animate) but surprising the first time, if you expected it to behave like changing `margin-left`.
- **Chaining multiple transforms in separate declarations instead of one.** `transform: rotate(10deg); transform: scale(1.1);` — the second line **replaces** the first rather than adding to it. Multiple transforms must be combined in a single declaration: `transform: rotate(10deg) scale(1.1);`.
