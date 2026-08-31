## Assignment Overview

Below is a complete HTML + CSS page for a simple event announcement. It was written by someone who tested it once on a laptop, decided it looked fine, and never opened it on a phone.

It looks completely broken on a small screen. Your job is to **find every responsive-design mistake, understand why it breaks on mobile, and rewrite the CSS correctly** — mobile-first, using proper media queries.

There is no need to change the HTML at all. Focus entirely on the CSS.

---

## Covers

`css/09 - Responsive Design and Media Queries` (with real dependencies on `css/05 - Units and Values` and `css/06 - Display and Positioning`)

---

## The HTML (unchanged — do not edit)

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>ICT Society — Open Day</title>
    <link rel="stylesheet" href="style.css" />
  </head>
  <body>
    <header class="hero">
      <h1>ICT Society Open Day</h1>
      <p>Saturday, 14 March — Main Auditorium</p>
    </header>

    <main class="content">
      <section class="info-box">
        <h2>What to Expect</h2>
        <p>
          Live demos, project showcases, and a Q&A with the current committee.
        </p>
      </section>
      <section class="info-box">
        <h2>Who Should Come</h2>
        <p>
          Any student curious about web development, robotics, or competitive
          programming.
        </p>
      </section>
      <section class="info-box">
        <h2>How to Register</h2>
        <p>
          Scan the QR code at the school notice board, or speak to any committee
          member.
        </p>
      </section>
    </main>
  </body>
</html>
```

## The Broken CSS

```css
.hero {
  width: 980px;
  height: 100vh;
  padding: 60px;
  background: #1b4332;
  color: white;
}

.hero h1 {
  font-size: 48px;
}

.content {
  width: 980px;
  display: flex;
}

.info-box {
  width: 320px;
  padding: 24px;
  margin-right: 16px;
  border: 1px solid #ccc;
}

.info-box h2 {
  font-size: 14px;
}
```

---

## Your Task

### Step 1 — Diagnose the Problems

Open the page and resize your browser down to a phone width (or use DevTools' device emulation). Write down every problem you can find. Use this format:

```
Problem 1
Property : ...
Why it breaks on mobile : ...
Fix : ...
```

There are **6 distinct responsive-design problems** in this stylesheet.

### Step 2 — Rewrite the CSS, Mobile-First

Rewrite `style.css` from scratch following the **mobile-first** approach (Module 09's Common Mistakes section covers exactly this pattern): write the simple, single-column, small-screen version as your base styles, then use `min-width` media queries to enhance the layout for larger screens.

Your final result should:

- Look correct and readable at a 375px-wide viewport (no horizontal scrolling, no cut-off text, no oversized fixed-width boxes forcing overflow)
- Rearrange the three `.info-box` sections from a horizontal row (on desktop) into a stacked column (on mobile)
- Scale the heading text down sensibly on small screens instead of staying at a fixed 48px
- Use at least one breakpoint chosen based on where _your own layout_ actually starts looking cramped — not a hardcoded device width

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — `width: 980px` on `.hero` and `.content` is a fixed width. What happens to a fixed-width box wider than the actual screen? Compare against a width expressed as a percentage or left to fill naturally.

> 💡 **Hint 2** — `height: 100vh` on an element containing text that might wrap to more lines than expected is risky — what happens on a narrow screen where the heading wraps to two lines, but the box height doesn't grow to fit?

> 💡 **Hint 3** — `.content { display: flex; }` with no `flex-wrap` and fixed-width children — what does Flexbox do by default when its children's widths add up to more than the container?

> 💡 **Hint 4** — A 48px heading on a 375px-wide screen takes up a very large proportion of the available width. Should this be a fixed `px` value at every screen size?

---

## Submission Guidelines

1. `index.html` (unmodified)
2. `style.css` — your complete, mobile-first rewrite
3. `problems.md` — your list of the 6 problems found in Step 1, in the format shown above

---

## Bonus Challenge 🏆

- 📱 Add a second breakpoint so the layout has genuinely three distinct states (mobile stacked, tablet 2-column, desktop 3-column row) rather than just two.
- 🖨️ Add a `@media print` block that removes the coloured `.hero` background before printing (a real, common request for pages like this).

---

## Learning Objectives

- ✓ Recognize fixed-width, fixed-height, and fixed-font-size patterns that break responsiveness before they cause visible problems.
- ✓ Rewrite a layout mobile-first instead of retrofitting media queries onto a desktop-first stylesheet.
- ✓ Choose breakpoints based on actual content behaviour, not memorized device widths.

**Good Luck! 🚀**
