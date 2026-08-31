# CSS Assignment 3 — Solution Guide

## How to Use This File

Read this **after** you've attempted the assignment on your own. Each problem is explained with what's wrong, why it breaks specifically on mobile, and the fix.

There are **6 problems** in the broken stylesheet.

---

## Problem 1 — Fixed `width: 980px` on `.hero` and `.content`

**Wrong code**:

```css
.hero {
  width: 980px;
}
.content {
  width: 980px;
}
```

**What is wrong**: Both elements have a hardcoded pixel width wider than most phone screens (a typical phone viewport is 360–430px wide).

**Why it matters on mobile**: A fixed-width box wider than the viewport forces horizontal scrolling — the single most common way a "desktop-first" page breaks on a phone. The user has to scroll sideways just to read a sentence.

**Fixed code**:

```css
.hero {
  width: 100%;
}
.content {
  width: 100%;
  max-width: 980px;
  margin: 0 auto;
}
```

`width: 100%` lets the element fill whatever space is actually available; `max-width` on `.content` keeps it from stretching uncomfortably wide on a large desktop screen once you're above 980px.

---

## Problem 2 — `height: 100vh` on `.hero`

**Wrong code**:

```css
.hero {
  height: 100vh;
  padding: 60px;
}
```

**What is wrong**: The height is locked to exactly the viewport's height, regardless of how much text is actually inside.

**Why it matters on mobile**: On a narrow screen, "ICT Society Open Day" may wrap onto two lines instead of one. A fixed-height box doesn't grow to fit that — the text can overflow the box or get visually cut off, and 60px of padding on all sides eats even more of that fixed vertical space on a screen that's already short.

**Fixed code**:

```css
.hero {
  min-height: 40vh;
  padding: 2rem 1.5rem;
}
```

`min-height` sets a floor without preventing the box from growing if its content needs more room — the box adapts to its content instead of clipping it.

---

## Problem 3 — `display: flex` with No Wrapping and Fixed-Width Children

**Wrong code**:

```css
.content {
  display: flex;
}
.info-box {
  width: 320px;
  margin-right: 16px;
}
```

**What is wrong**: Three 320px-wide boxes plus margins add up to roughly 1000px combined. With no `flex-wrap`, Flexbox's default behaviour is to shrink all three items to fit the container rather than wrapping them onto new lines — which, combined with each box's fixed `width`, produces a layout fighting itself.

**Why it matters on mobile**: On a 375px screen, three items squeezed into one row (however awkwardly) are each far too narrow to hold their heading and paragraph text legibly.

**Fixed code**:

```css
.content {
  display: flex;
  flex-direction: column; /* mobile-first: stacked by default */
  gap: 1rem;
}

.info-box {
  width: 100%; /* fills the stacked column */
}

@media (min-width: 768px) {
  .content {
    flex-direction: row; /* becomes a row once there's room */
  }
  .info-box {
    width: auto;
    flex: 1; /* three equal-width columns */
  }
}
```

---

## Problem 4 — Fixed `font-size: 48px` on the Heading

**Wrong code**:

```css
.hero h1 {
  font-size: 48px;
}
```

**What is wrong**: A single, large, fixed pixel size with no adjustment for smaller screens.

**Why it matters on mobile**: 48px takes up a very large proportion of a 375px-wide screen — the heading can look oversized, wrap awkwardly, or in extreme cases contribute to horizontal overflow on its own.

**Fixed code**:

```css
.hero h1 {
  font-size: 1.75rem; /* smaller base size, in rem (Module 05) */
}

@media (min-width: 768px) {
  .hero h1 {
    font-size: 3rem;
  } /* scales up once there's room */
}
```

---

## Problem 5 — `margin-right` Instead of `gap` for Spacing Between Flex Items

**Wrong code**:

```css
.info-box {
  margin-right: 16px;
}
```

**What is wrong**: This isn't a bug on its own, but it's a mobile-first anti-pattern: it adds unwanted right-margin even in the stacked (mobile) layout where the boxes are no longer side-by-side, and it leaves an extra, asymmetric gap after the very last box that a symmetric `gap` on the parent avoids entirely.

**Fixed code**: covered already in the Problem 3 fix above — `gap: 1rem` on `.content` handles spacing for both the stacked and row layouts consistently, with no leftover margin on the last item.

---

## Problem 6 — No Media Queries At All

**Wrong code**: (the entire original stylesheet)

**What is wrong**: Every rule applies identically regardless of viewport width — there's no adaptation mechanism at all.

**Why it matters**: This is really the root cause behind Problems 1–5 all showing up at once — without a `min-width` media query anywhere, there's no way for the desktop-oriented values to ever "back off" on a smaller screen.

**Fixed code**: see the complete rewritten stylesheet in `solution.html`, which combines all five fixes above into one cohesive, mobile-first file.

---

## Bonus — Adding a Third Breakpoint

A tablet-width breakpoint between the two above (e.g. `min-width: 600px`) could switch `.content` to a 2-column `grid` before the full 768px row layout kicks in — a good exercise in recognizing that "mobile vs desktop" is often really "mobile, tablet, desktop," three states rather than two.
