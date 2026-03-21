CSS animations let you define multi-step motion with `@keyframes`. Unlike transitions (which only go from A to B), animations can have as many steps as you like and run automatically without user interaction.

## `@keyframes`

Defines the steps of an animation:

```css
@keyframes fade-in {
  from { opacity: 0; }
  to   { opacity: 1; }
}

@keyframes slide-up {
  from {
    opacity: 0;
    transform: translateY(20px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

/* Multiple steps with percentages */
@keyframes pulse {
  0%   { transform: scale(1); }
  50%  { transform: scale(1.05); }
  100% { transform: scale(1); }
}
```

---

## Applying an Animation

```css
.element {
  animation: name duration timing-function delay iteration-count direction fill-mode;
}
```

```css
.hero-text {
  animation: fade-in 0.6s ease-out;
}

/* Full syntax */
.loader {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
```

### Individual Properties

```css
animation-name: fade-in;
animation-duration: 0.6s;
animation-timing-function: ease-out;
animation-delay: 0.2s;
animation-iteration-count: infinite;  /* or a number: 3 */
animation-direction: normal;          /* normal, reverse, alternate, alternate-reverse */
animation-fill-mode: forwards;        /* keep final state after animation ends */
animation-play-state: paused;         /* pause/resume with JS */
```

### `animation-fill-mode`

|Value|Behaviour|
|---|---|
|`none`|Element returns to original state after animation (default)|
|`forwards`|Element keeps the final keyframe state|
|`backwards`|Element applies the first keyframe during delay|
|`both`|Combines forwards and backwards|

---

## Multiple Animations

```css
.element {
  animation:
    fade-in 0.5s ease,
    slide-up 0.5s ease;
}
```

---

## Accessibility

Always respect the user's motion preference:

```css
@media (prefers-reduced-motion: reduce) {
  .animated {
    animation: none;
    transition: none;
  }
}
```

---