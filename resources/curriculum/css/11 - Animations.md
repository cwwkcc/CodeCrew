CSS animations define multi-step motion with `@keyframes`. Unlike transitions — which animate between two states when triggered — animations can have any number of steps, run automatically on page load, and loop indefinitely without any user interaction.

---

## `@keyframes` — Defining the Motion

```css
/* Two states — from/to is the same as 0%/100% */
@keyframes fade-in {
  from { opacity: 0; }
  to   { opacity: 1; }
}

/* Multi-step with percentages */
@keyframes pulse {
  0%   { transform: scale(1); }
  50%  { transform: scale(1.08); }
  100% { transform: scale(1); }
}

/* Multiple properties per keyframe */
@keyframes slide-in-up {
  from {
    opacity: 0;
    transform: translateY(24px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

/* Many stops */
@keyframes progress-bar {
  0%   { width: 0%; }
  20%  { width: 35%; }
  60%  { width: 70%; }
  80%  { width: 85%; }
  100% { width: 100%; }
}
```

The `@keyframes` name is case-sensitive. You can define them anywhere in the stylesheet.

---

## Applying an Animation

```css
.element {
  animation: name duration timing-function delay iteration-count direction fill-mode;
}
```

```css
/* Basic entrance */
.hero-text {
  animation: fade-in 0.6s ease-out;
}

/* Infinite loop */
.spinner {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}

/* Delayed entrance, hold final state */
.card:nth-child(2) {
  animation: slide-in-up 0.4s ease-out 0.1s both;
}
```

---

## Individual Animation Properties

```css
animation-name:             slide-in-up;
animation-duration:         0.4s;
animation-timing-function:  ease-out;
animation-delay:            0.1s;
animation-iteration-count:  1;           /* or: infinite, or a number: 3 */
animation-direction:        normal;
animation-fill-mode:        both;
animation-play-state:       running;     /* or: paused */
```

### `animation-iteration-count`

```css
animation-iteration-count: 1;         /* play once (default) */
animation-iteration-count: 3;         /* play exactly 3 times */
animation-iteration-count: infinite;  /* loop forever */
```

### `animation-direction`

|Value|Behaviour|
|---|---|
|`normal`|Forward each time (default)|
|`reverse`|Backward each time|
|`alternate`|Forward, then backward, then forward...|
|`alternate-reverse`|Backward, then forward, then backward...|

### `animation-fill-mode`

Controls the element's state **before** the animation starts (during delay) and **after** it ends:

|Value|Behaviour|
|---|---|
|`none`|Default state during delay; snaps back to default after (default)|
|`forwards`|Holds the final keyframe after the animation ends|
|`backwards`|Applies the first keyframe during the delay period|
|`both`|Combines `forwards` and `backwards`|

```css
/* Without forwards — element snaps back to original style when done */
.box { animation: slide-in 0.5s ease; }

/* With forwards — element stays in its final animated position */
.box { animation: slide-in 0.5s ease forwards; }

/* With both — first keyframe applied during delay, stays in final state after */
.box { animation: slide-in 0.5s ease 0.2s both; }
```

`both` is the most commonly useful value for entrance animations.

### `animation-play-state`

Pause and resume without removing the animation:

```css
.spinner {
  animation: spin 1s linear infinite;
}

.spinner:hover {
  animation-play-state: paused;
}
```

---

## Multiple Animations

Apply more than one animation simultaneously, comma-separated:

```css
.element {
  animation:
    fade-in    0.5s ease forwards,
    slide-in-up 0.5s ease forwards;
}
```

---

## Staggered Entrance

Animate a list of items with increasing delays so they appear one by one:

```css
.card-list li {
  opacity: 0;
  animation: slide-in-up 0.4s ease forwards;
}

.card-list li:nth-child(1) { animation-delay: 0ms; }
.card-list li:nth-child(2) { animation-delay: 80ms; }
.card-list li:nth-child(3) { animation-delay: 160ms; }
.card-list li:nth-child(4) { animation-delay: 240ms; }
```

---

## Common Animation Recipes

```css
/* Fade in */
@keyframes fade-in {
  from { opacity: 0; }
  to   { opacity: 1; }
}

/* Slide in from bottom */
@keyframes slide-up {
  from { opacity: 0; transform: translateY(20px); }
  to   { opacity: 1; transform: translateY(0); }
}

/* Slide in from left */
@keyframes slide-right {
  from { opacity: 0; transform: translateX(-20px); }
  to   { opacity: 1; transform: translateX(0); }
}

/* Loading spinner */
@keyframes spin {
  to { transform: rotate(360deg); }
}
.spinner {
  width: 24px;
  height: 24px;
  border: 3px solid rgba(0, 0, 0, 0.1);
  border-top-color: royalblue;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

/* Skeleton loading shimmer */
@keyframes shimmer {
  from { background-position: -200% 0; }
  to   { background-position:  200% 0; }
}
.skeleton {
  background: linear-gradient(90deg, #e8e8e8 25%, #f5f5f5 50%, #e8e8e8 75%);
  background-size: 200% 100%;
  animation: shimmer 1.5s infinite;
  border-radius: 4px;
}

/* Bounce in */
@keyframes bounce-in {
  0%   { transform: scale(0.3); opacity: 0; }
  50%  { transform: scale(1.1); }
  70%  { transform: scale(0.95); }
  100% { transform: scale(1);   opacity: 1; }
}

/* Ping — notification badge ripple */
@keyframes ping {
  75%, 100% {
    transform: scale(2);
    opacity: 0;
  }
}
.badge-ping {
  animation: ping 1s cubic-bezier(0, 0, 0.2, 1) infinite;
}
```

---

## Performance

Only animate `transform` and `opacity` for smooth 60fps motion. These properties skip Layout and Paint and go directly to the GPU compositor:

```css
/* Fast — GPU only, no layout recalculation */
animation: fade-in 0.3s ease;   /* opacity */
animation: slide-up 0.3s ease;  /* transform */

/* Slow — triggers layout recalculation on every frame */
@keyframes bad {
  from { width: 100px; }
  to   { width: 200px; }
}

@keyframes also-bad {
  from { top: 0; }
  to   { top: 100px; }
}
```

Rendering pipeline:

- **Layout** (slow) — triggered by: `width`, `height`, `padding`, `margin`, `top`, `left`
- **Paint** — triggered by: `background-color`, `color`, `box-shadow`
- **Composite** (fast, GPU) — triggered by: `transform`, `opacity`

---

## Accessibility

```css
@media (prefers-reduced-motion: reduce) {
  *,
  *::before,
  *::after {
    animation-duration:        0.01ms !important;
    animation-iteration-count: 1      !important;
    transition-duration:       0.01ms !important;
  }
}
```

---

