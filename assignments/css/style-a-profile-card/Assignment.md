## Assignment Overview

Build a single **profile card** component — the kind of thing you'd see on a team page or an "About the Instructor" section. No layout frameworks, no Flexbox or Grid yet (that's the next assignment) — this one is entirely about the box model, selectors, colour, typography, and units.

You're given the HTML already. Your job is to write **only** the CSS.

---

## Covers

`css/00 - How CSS Works`, `css/01 - Selectors`, `css/02 - The Box Model`, `css/03 - Colors and Backgrounds`, `css/04 - Typography`, `css/05 - Units and Values`

---

## The Starting HTML

Save this as `index.html`. Do not change the HTML structure — everything you need to do is achievable with CSS alone.

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Profile Card</title>
    <link rel="stylesheet" href="style.css" />
  </head>
  <body>
    <div class="card">
      <img class="card__avatar" src="https://i.pravatar.cc/150?img=12" alt="" />
      <h2 class="card__name">Nadeesha Silva</h2>
      <p class="card__role">ICT Society — Lead Developer</p>
      <p class="card__bio">
        Builds things, breaks things, then reads the error message properly the
        second time. Currently deep in a CSS rabbit hole.
      </p>
      <div class="card__tags">
        <span class="card__tag">HTML</span>
        <span class="card__tag">CSS</span>
        <span class="card__tag">JavaScript</span>
      </div>
    </div>
  </body>
</html>
```

---

## Your Task

Write `style.css` so the final result meets every requirement below.

### 1. The Card Container (`.card`)

- A fixed `max-width` (choose something sensible for a card — think in the 300–380px range) and centred horizontally on the page
- Padding on the inside so content doesn't touch the edges
- A background colour that isn't plain white
- A border-radius so the corners are visibly rounded
- A subtle `box-shadow` so it looks lifted off the page
- **Must use `box-sizing: border-box`** — apply it properly (Module 02's Common Mistakes section explains why, and where to put this so it applies everywhere, not just on `.card`)

### 2. The Avatar (`.card__avatar`)

- A perfect circle (this requires two properties working together — `border-radius` alone on a square image won't do it without the right `width`/`height`)
- Centred within the card
- A fixed size in `px` or `rem`

### 3. Typography (`.card__name`, `.card__role`, `.card__bio`)

- `.card__name`: distinctly larger and bolder than the rest of the card
- `.card__role`: a different colour from the body text — used to visually separate "who they are" from "what they do"
- `.card__bio`: a comfortable `line-height` for reading — not the browser default
- **At least one font size set in `rem`, not `px`** (Module 05's Common Mistakes explains why this matters)

### 4. The Tags (`.card__tag`)

- Each tag should look like a small pill/badge — background colour, padding, and enough `border-radius` to round the ends fully
- Tags should sit next to each other with a small gap between them (you can use `margin` for this — Flexbox's `gap` is next assignment)

### 5. A Hover Effect on the Card

- When the mouse hovers over `.card`, the `box-shadow` should become more pronounced (a simple `:hover` selector — no transition/animation timing required yet, that's Assignment 4)

---

## Constraints

- **No Flexbox, no Grid.** If you find yourself wanting `display: flex`, that's a sign you're anticipating the next assignment — for this one, plain block flow, margins, and `text-align: center` are enough.
- **No inline `style=""` attributes.** Everything goes in `style.css`.
- **At least one CSS custom property** (Module 13) — define your main accent colour as `--accent-color` on `:root` and reference it with `var()` at least twice (e.g. for `.card__role` and `.card__tag` background).

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — A perfectly circular avatar needs `border-radius: 50%` **and** equal `width`/`height` **and** `object-fit: cover` (so the image doesn't distort if it isn't already square).

> 💡 **Hint 2** — Centring a block element horizontally with a fixed `max-width` uses `margin: 0 auto`, not `text-align: center` (that only centres inline content _inside_ the element).

> 💡 **Hint 3** — `.card__tags` needs `text-align: center` (or a display change) if you want the pills centred as a group, since `<span>` is inline by default.

---

## Submission Guidelines

1. `index.html` (unmodified from the version given above)
2. `style.css` — your complete stylesheet
3. A one-paragraph note at the top of `style.css` (as a comment) explaining your accent colour choice and why you sized the avatar the way you did

---

## Bonus Challenge 🏆

- 🎨 Add a `.card--dark` variant class with a dark background and light text, and show both versions side by side.
- 🖋️ Load a webfont (Google Fonts) for `.card__name` and set up a proper fallback stack (Module 04's Common Mistakes) in case it fails to load.
- 📏 Make the tags equal-width using only what you know so far (no Flexbox) — trickier than it sounds with inline elements.

---

## Learning Objectives

- ✓ Apply the box model correctly, including `box-sizing: border-box`.
- ✓ Use class selectors deliberately, avoiding IDs and `!important`.
- ✓ Choose and apply a colour palette via custom properties instead of hardcoded values.
- ✓ Use `rem` appropriately for accessible, scalable typography.

**Good Luck! 🚀**
