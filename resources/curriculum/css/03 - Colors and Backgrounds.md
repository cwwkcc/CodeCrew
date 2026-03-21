## Color Formats

CSS accepts colour in multiple formats. All produce the same result — pick the format that is most readable for your use case:

```css
/* Named colours */
color: red;
color: navy;
color: transparent;

/* Hexadecimal — #RRGGBB */
color: #ff0000;    /* red */
color: #1a2b3c;    /* dark blue-grey */
color: #fff;       /* shorthand for #ffffff — white */
color: #f006;      /* shorthand with alpha: #RGBA */

/* RGB */
color: rgb(255, 0, 0);           /* red */
color: rgb(26, 43, 60);          /* dark blue-grey */

/* RGBA — adds opacity channel (0 = transparent, 1 = fully opaque) */
color: rgba(0, 0, 0, 0.5);       /* 50% transparent black */

/* HSL — Hue (0–360°), Saturation (%), Lightness (%) */
color: hsl(200, 80%, 40%);

/* HSLA */
color: hsla(200, 80%, 40%, 0.7);

/* Modern syntax (spaces, optional slash for alpha) */
color: rgb(26 43 60);
color: rgb(26 43 60 / 50%);
color: hsl(200 80% 40% / 0.7);
```

HSL is the most human-readable format because it maps to how we think about colour: hue (what colour), saturation (how vivid), lightness (how light or dark).

---

## The `color` Property

Sets the foreground colour — text and borders that use `currentColor`:

```css
p { color: #333; }
a { color: hsl(220, 90%, 55%); }
```

---

## Backgrounds

### `background-color`

```css
div { background-color: #f0f4f8; }
div { background-color: transparent; }  /* default */
```

### `background-image`

```css
/* URL */
div { background-image: url('/images/hero.jpg'); }

/* Linear gradient */
div { background-image: linear-gradient(to right, #3a86ff, #8338ec); }
div { background-image: linear-gradient(135deg, #3a86ff 0%, #8338ec 100%); }

/* Radial gradient */
div { background-image: radial-gradient(circle, #3a86ff, #8338ec); }

/* Multiple backgrounds (first listed is on top) */
div {
  background-image:
    url('/images/overlay.png'),
    linear-gradient(to bottom, #000, #333);
}
```

### Background Control Properties

```css
div {
  background-repeat: no-repeat;    /* repeat, repeat-x, repeat-y, no-repeat */
  background-size: cover;          /* cover, contain, or explicit px/% */
  background-position: center;     /* center, top left, 50% 50% */
  background-attachment: scroll;   /* scroll, fixed (parallax), local */
  background-clip: border-box;     /* how far bg extends: border-box, padding-box, content-box */
  background-origin: padding-box;  /* where bg-position 0,0 starts */
}
```

### Shorthand

```css
/* color  image  position / size  repeat  attachment  origin  clip */
background: #f0f4f8 url('/img/bg.png') center / cover no-repeat;
```

---

## `opacity`

Makes the entire element (including children) transparent:

```css
.overlay { opacity: 0.5; }  /* 0 = invisible, 1 = fully visible */
```

Different from RGBA/HSLA alpha — `opacity` affects the entire element including its children. RGBA/HSLA alpha only affects that one property value.

---

## CSS Gradients as Backgrounds

Gradients are treated as images, not colours:

```css
.hero {
  background: linear-gradient(
    to bottom right,
    hsl(220, 90%, 55%),
    hsl(280, 80%, 50%)
  );
}

/* Transparent overlay on top of an image */
.card {
  background-image:
    linear-gradient(rgba(0,0,0,0.5), rgba(0,0,0,0.5)),
    url('/images/photo.jpg');
  background-size: cover;
  color: white;
}
```