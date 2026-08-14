Every visible element on the page has a colour — text, backgrounds, borders, shadows. CSS accepts colour in several formats and provides a rich set of properties for controlling backgrounds including solid fills, images, and gradients.

---

## Color Formats

```css
/* Named colours — 140 predefined names */
color: red;
color: navy;
color: tomato;
color: royalblue;
color: transparent; /* fully transparent — same as rgba(0,0,0,0) */

/* Hex — #RRGGBB */
color: #ff0000; /* red */
color: #1a2b3c; /* dark blue-grey */
color: #fff; /* shorthand for #ffffff */
color: #000; /* shorthand for #000000 */

/* Hex with alpha — #RRGGBBAA */
color: #ff000080; /* red, 50% transparent */
color: #fff8; /* white, ~53% transparent (shorthand #RGBA) */

/* RGB */
color: rgb(255, 0, 0); /* red */
color: rgb(26, 43, 60); /* dark blue-grey */

/* RGB with alpha — 0 = transparent, 1 = opaque */
color: rgba(0, 0, 0, 0.5); /* 50% black */
color: rgba(255, 255, 255, 0.1); /* barely-there white */

/* Modern space-separated syntax (no commas) */
color: rgb(26 43 60);
color: rgb(26 43 60 / 50%); /* alpha as percentage */
color: rgb(26 43 60 / 0.5); /* alpha as decimal */
```

---

## HSL — The Most Useful Format

HSL maps to how humans think about colour: **H**ue (which colour), **S**aturation (how vivid), **L**ightness (how light or dark):

```css
color: hsl(0, 100%, 50%); /* red */
color: hsl(120, 100%, 50%); /* green */
color: hsl(240, 100%, 50%); /* blue */
color: hsl(220, 90%, 55%); /* royalblue-ish */

/* With alpha */
color: hsla(220, 90%, 55%, 0.5);
color: hsl(220 90% 55% / 0.5); /* modern syntax */
```

The hue wheel:

| Degree    | Colour  |
| --------- | ------- |
| 0° / 360° | Red     |
| 30°       | Orange  |
| 60°       | Yellow  |
| 120°      | Green   |
| 180°      | Cyan    |
| 240°      | Blue    |
| 270°      | Purple  |
| 300°      | Magenta |

**Why HSL is better for UI work:**

```css
/* Hard to create related colours in hex or rgb */
--primary: hsl(220, 90%, 55%);
--primary-light: hsl(220, 90%, 70%); /* just changed lightness */
--primary-dark: hsl(220, 90%, 35%); /* just changed lightness */
--primary-muted: hsl(220, 40%, 55%); /* just changed saturation */
```

With HSL you can generate a whole palette by adjusting one value.

---

## `currentColor`

A special keyword that equals the element's current `color` value. Useful for borders, SVG fills, and shadows that should always match the text colour:

```css
.icon {
  color: royalblue;
  border: 2px solid currentColor; /* border matches text colour automatically */
  fill: currentColor; /* SVG icon inherits text colour */
}

.icon:hover {
  color: navy;
  /* border and fill update automatically — no need to repeat them */
}
```

---

## `color-mix()`

Mix two colours in a given colour space:

```css
/* 50% mix of royalblue and white */
color: color-mix(in srgb, royalblue 50%, white);

/* Create a tint (lighten) */
background: color-mix(in hsl, hsl(220 90% 55%) 30%, white);

/* Create a shade (darken) */
background: color-mix(in hsl, hsl(220 90% 55%) 70%, black);

/* Mix two brand colours */
border-color: color-mix(in oklch, var(--color-primary), var(--color-secondary));
```

---

## `opacity`

Makes the **entire element** (including all children) transparent:

```css
.overlay {
  opacity: 0.5;
} /* 0 = invisible, 1 = fully visible */
```

Different from RGBA/HSLA alpha — `opacity` affects the whole element and its children. A colour's alpha channel only affects that one colour value.

```css
/* This makes the background semi-transparent but the text stays fully opaque */
background-color: rgba(0, 0, 0, 0.5);

/* This makes EVERYTHING in the element semi-transparent, including text */
opacity: 0.5;
```

---

## `background-color`

```css
div {
  background-color: #f0f4f8;
}
div {
  background-color: hsl(220, 20%, 97%);
}
div {
  background-color: transparent;
} /* default — shows parent through */
```

---

## `background-image`

Backgrounds can be images or gradients. Gradients are treated as images, not colours.

### Image

```css
div {
  background-image: url("/images/hero.jpg");
  background-repeat: no-repeat;
  background-size: cover;
  background-position: center;
}
```

### Linear Gradient

```css
/* Direction keywords */
background-image: linear-gradient(to right, #3a86ff, #8338ec);
background-image: linear-gradient(to bottom, #3a86ff, #8338ec);
background-image: linear-gradient(to bottom right, #3a86ff, #8338ec);

/* Angle */
background-image: linear-gradient(135deg, #3a86ff, #8338ec);
background-image: linear-gradient(45deg, royalblue, crimson);

/* Multiple colour stops */
background-image: linear-gradient(
  to right,
  #3a86ff 0%,
  #8338ec 50%,
  #ff006e 100%
);

/* Hard stop — no blending between colours */
background-image: linear-gradient(to right, royalblue 50%, crimson 50%);
```

### Radial Gradient

```css
background-image: radial-gradient(circle, #3a86ff, #8338ec);
background-image: radial-gradient(ellipse at center, #3a86ff, #8338ec);
background-image: radial-gradient(circle at top left, royalblue, transparent);

/* With stops */
background-image: radial-gradient(
  circle at 30% 40%,
  hsl(220, 90%, 60%) 0%,
  hsl(280, 80%, 50%) 60%,
  transparent 100%
);
```

### Conic Gradient

```css
/* Colour rotates around a centre point */
background-image: conic-gradient(red, yellow, green, blue, red);

/* Pie chart slices with hard stops */
background-image: conic-gradient(
  royalblue 0deg 90deg,
  crimson 90deg 200deg,
  orange 200deg 360deg
);
```

---

## Background Control Properties

```css
div {
  background-repeat: no-repeat; /* repeat | repeat-x | repeat-y | no-repeat | space | round */
  background-size: cover; /* cover | contain | auto | 100px 200px | 50% */
  background-position: center; /* center | top left | 50% 50% | 20px 30px */
  background-attachment: scroll; /* scroll | fixed (parallax) | local */
  background-clip: border-box; /* border-box | padding-box | content-box | text */
  background-origin: padding-box; /* border-box | padding-box | content-box */
}
```

### `background-size` values

| Value         | Behaviour                                            |
| ------------- | ---------------------------------------------------- |
| `cover`       | Scale to fill the box — may crop the image           |
| `contain`     | Scale to fit entirely inside — may leave empty space |
| `auto`        | Original image size                                  |
| `100% 100%`   | Stretch to fill — distorts aspect ratio              |
| `200px 150px` | Explicit width and height                            |

### `background-clip: text`

Clips the background to the text shape — creates gradient text:

```css
.gradient-text {
  background-image: linear-gradient(135deg, royalblue, crimson);
  background-clip: text;
  -webkit-background-clip: text;
  color: transparent; /* hide the actual text colour so the background shows */
}
```

---

## Multiple Backgrounds

Layer multiple backgrounds comma-separated. The first listed is on top:

```css
.card {
  background-image:
    linear-gradient(rgba(0, 0, 0, 0.4), rgba(0, 0, 0, 0.4)),
    /* dark overlay on top */ url("/images/photo.jpg"); /* image underneath */
  background-size: cover;
  color: white;
}

/* Pattern overlay + solid colour */
.hero {
  background-image:
    url("/images/noise.png"), linear-gradient(135deg, royalblue, crimson);
  background-size: 200px, cover;
  background-repeat: repeat, no-repeat;
}
```

---

## Shorthand

```css
/* background: color image position/size repeat attachment origin clip */
background: #f0f4f8 url("/img/bg.png") center / cover no-repeat;

/* Gradient shorthand */
background: linear-gradient(135deg, royalblue, crimson);

/* With position and size */
background: url("/img/logo.png") top right / 100px no-repeat;
```

When using shorthand with multiple backgrounds, separate each layer with a comma. The `background-color` can only appear in the last layer.

---

## Box Shadow and Text Shadow

```css
/* box-shadow: offset-x offset-y blur spread color */
box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
box-shadow: 0 8px 24px rgba(0, 0, 0, 0.15);

/* Inset shadow (inside the element) */
box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.1);

/* Multiple shadows */
box-shadow:
  0 1px 3px rgba(0, 0, 0, 0.12),
  0 4px 12px rgba(0, 0, 0, 0.08);

/* Coloured glow */
box-shadow: 0 0 20px hsl(220, 90%, 55%);

/* text-shadow: offset-x offset-y blur color */
text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);
text-shadow: 1px 1px 0 navy;
```

---

---

## Common Mistakes

```css
/* WRONG: the same color value typed out everywhere — change the brand
   color later and you're doing a project-wide find-and-replace */
.button {
  background: #1b4332;
}
.header {
  background: #1b4332;
}
.badge {
  border-color: #1b4332;
}

/* CORRECT: one source of truth, referenced everywhere */
:root {
  --color-forest: #1b4332;
}
.button {
  background: var(--color-forest);
}
.header {
  background: var(--color-forest);
}
.badge {
  border-color: var(--color-forest);
}
```

- **Hardcoding colors instead of defining a small palette of custom properties up front** (covered properly in Module 13) — this is the single habit that most separates a maintainable stylesheet from one that becomes painful to re-theme.
- **Picking colors without checking contrast.** Light grey text on a white card might look elegant on your monitor and fail WCAG contrast requirements for anyone with low vision, or anyone on a phone screen in direct sunlight. [WebAIM's contrast checker](https://webaim.org/resources/contrastchecker/) takes ten seconds to run against a color pair — cheap insurance against a real accessibility defect.
