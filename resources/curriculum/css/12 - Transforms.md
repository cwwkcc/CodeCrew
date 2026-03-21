The `transform` property moves, scales, rotates, or skews an element visually without affecting the layout of surrounding elements. Transforms are hardware-accelerated and do not trigger reflow, making them the preferred tool for smooth animations.

## Transform Functions

```css
/* Translation — move without affecting layout */
transform: translateX(50px);
transform: translateY(-20px);
transform: translate(50px, -20px);  /* X and Y together */
transform: translateZ(100px);       /* depth (3D) */
transform: translate3d(50px, -20px, 100px);

/* Scale */
transform: scale(1.5);        /* 150% — uniform */
transform: scaleX(2);         /* stretch horizontally */
transform: scaleY(0.5);       /* squish vertically */
transform: scale(1.2, 0.8);   /* different X and Y */

/* Rotation */
transform: rotate(45deg);
transform: rotateX(30deg);    /* 3D rotation around X axis */
transform: rotateY(60deg);    /* 3D rotation around Y axis — "flip" effect */

/* Skew */
transform: skewX(10deg);
transform: skewY(5deg);
transform: skew(10deg, 5deg);
```

## Chaining Multiple Transforms

List them space-separated. Order matters — transforms are applied right to left:

```css
transform: rotate(45deg) scale(1.2) translate(10px, 0);
```

---

## `transform-origin`

Sets the point around which the transform is applied. Default is `50% 50%` (centre):

```css
transform-origin: top left;
transform-origin: 0 0;
transform-origin: center bottom;
transform-origin: 50% 100%;
```

---

## 3D Transforms and Perspective

For 3D effects, the parent needs `perspective` set:

```css
.card-container {
  perspective: 1000px;
}

.card {
  transform-style: preserve-3d;
  transition: transform 0.6s ease;
}

.card:hover {
  transform: rotateY(180deg);
}
```

---

## Why Transforms Are Better Than Moving with `top/left`

```css
/* BAD — triggers layout recalculation on every frame */
.bad { top: 100px; transition: top 0.3s; }

/* GOOD — only triggers compositing, GPU-accelerated */
.good { transform: translateY(100px); transition: transform 0.3s; }
```

For animations, always prefer `transform` and `opacity` over animating `width`, `height`, `top`, `left`, `margin`, or `padding`. The latter trigger layout recalculation (reflow) which is expensive.

---