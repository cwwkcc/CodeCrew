# CSS Teaching Guide: Build the StudyFlow Dashboard

## The plan

| Phase | What students do | Tools |
|---|---|---|
| 1. Guided build | Build the **StudyFlow dashboard** over 11 stages | `css-dashboard-project.zip`, CSS Lab |
| 2. Solo remix | Build **their own dashboard** (new topic) with no part files | their notes, the Lab |
| 3. Bridge to JS | Add behaviour to the dashboard they already built | see the last section |

Phase 1 teaches the concepts. Phase 2 proves they can do it alone, and that is when they get brave. Keep `parts/` and `checkpoints/` for yourself as the answer key.

**Day 0:** open `dashboard-preview.html` and say "this is what you will build". Then open the CSS Lab and say "this is how we will understand it". Flip the dark mode switch and tick a task. Students will want to know how that works with no JavaScript, which gives you the hook for the whole course.

**CSS Lab:** https://claude.ai/artifact/5YCp9AuCLfUAdis3EFwKWG (also saved as `css-lab.html`). It covers box model, position, flexbox and grid. The other stages use the browser DevTools.

**Browsers:** the dashboard uses `:has()`, `@container` and `@property`, so test on the school computers once.

## The session loop

1. **Goal (5%)**: show the current page and what it should look like after this stage.
2. **Understand (25%)**: use the lab tab or DevTools. Ask the *predict* question before you move any control.
3. **Build (45%)**: students type the stage's part file into their own `style.css`, one rule at a time.
4. **Break it (20%)**: run the challenge at the top of the part file.
5. **Reference (5%)**: point to the matching curriculum module. Notes are for looking things up, not for teaching from.

A student who falls behind starts from the previous `checkpoints/stage-N` folder. Each part file has a `+ FEATURES` section at the bottom. If time is short, teach the first section and leave `+ FEATURES` for fast finishers.

## Stage guides

### Stage 1: Box model (module 02) · Lab: Box model
- **The one idea:** every element is a box with four layers: content, padding, border, margin.
- **Predict:** width 180, padding 20, border 6 with `content-box`. How wide is the visible box? (180 + 40 + 12 = 232.) Switch to `border-box`. (180.)
- **Rule of thumb:** space *inside* the border is padding. Space *between* boxes is margin.
- **Common mistake:** expecting margin to have a background colour.
- **Check:** panels and cards have breathing room and rounded corners.

### Stage 2: Colors and backgrounds (module 03) · DevTools
- **The one idea:** colours live in one palette in `:root`. Treat `var()` as a sneak peek; stage 11 explains it.
- **Show:** the donut is a `conic-gradient`, and the heatmap's five shades come from *one* colour using `color-mix()` and the `--l` level in the HTML.
- **Predict:** what is the difference between `rgb(0 0 0 / 0.5)` and `opacity: 0.5`? (Opacity fades the children too.)
- **Break it:** change `--primary` and watch the sidebar, bars, donut and heatmap follow.
- **Common mistake:** low-contrast text. Check it with the DevTools colour picker.

### Stage 3: Typography and units (modules 04 + 05) · DevTools
- **The one idea:** `rem` scales with the root font size, `px` never does.
- **Predict:** set `html { font-size: 125% }`. What changes? (Everything in rem, including padding.)
- **Show:** `font-variant-numeric: tabular-nums` makes digits the same width, so the stat numbers and times line up.
- **Common mistake:** a line-height with a unit instead of a plain number such as `1.5`.

### Stage 4: Display and positioning (module 06) · Lab: Position
- **The one idea:** `absolute` measures from the nearest *positioned* ancestor, not simply the parent.
- **Predict:** in the lab, B is `absolute` and the parent is not positioned. Where does it go? Tick the checkbox to find out.
- **Show in the project:** the day labels under each bar (`absolute` inside a `relative` bar), the sticky sidebar and top bar, the sticky table header, and the dark mode switch (a hidden checkbox stretched over a track with `inset: 0`).
- **Common mistakes:** forgetting `position: relative` on the parent; `z-index` on a `static` element does nothing.

### Stage 5: Flexbox (module 07) · Lab: Flexbox
- **The one idea:** flex lays items along one axis. `justify-content` follows the main axis, `align-items` the cross axis.
- **Predict:** set `flex-direction: column`. Which property now centres items horizontally? (`align-items`.)
- **Show in the project:** the top bar, the bar chart (`align-items: flex-end` makes bars grow upward), task rows (`space-between`), and `margin-left: auto` pushing the switch to the right.
- **Common mistake:** putting `display: flex` on the children instead of the container.

### Stage 6: Grid (module 08) · Lab: Grid
- **The one idea:** flex is one dimension, grid is two.
- **Predict:** `1fr 2fr 1fr`. How much wider is the middle column? (Double.)
- **Show in the project, in this order:** the whole app shell with `grid-template-areas`; stat cards with `auto-fit`; the 12-column panels with `span`; the timetable, where each block sits on a row set by `--start` and `--span` from the HTML; the heatmap, where `grid-auto-flow: column` fills down each column first.
- **Predict (heatmap):** remove `grid-auto-flow: column`. What happens? (It fills across rows and becomes a different shape.)
- **Common mistake:** using grid for a simple single row. Ask "would flex do this?"

### Stage 7: Responsive design (module 09) · DevTools device mode
- **The one idea:** media queries *override* base styles, so they must come **after** them in the file.
- **Show in the project:** the sidebar becomes a bottom bar, and the app re-declares its `grid-template-areas`.
- **Predict (container query):** the task list restacks at 380px *panel* width. Why not use a media query? (A media query only knows the screen. The same panel can be wide in one layout and narrow in another.)
- **Common mistakes:** forgetting `<meta name="viewport">`; forgetting `container-type` on the parent, so the container query never fires.

### Stage 8: Pseudo-classes and pseudo-elements (module 14) · DevTools
- **The one idea:** a pseudo-class is a *state or position* (`:hover`, `:checked`, `:nth-child`). A pseudo-element is a *fake extra element* (`::before`, `::after`).
- **Show:** the donut hole is a `::before`, the bar tooltip is an `::after` using `content: attr(data-value)`, and the task checkbox is `appearance: none` plus an `::after` tick that appears on `:checked`.
- **Show:** `.tasks li:has(input:checked)` styles a parent based on its child. CSS could not do that until recently.
- **Tip:** use DevTools "force element state" to freeze `:hover` while you edit it.
- **Common mistake:** a `::before` with no `content` property. It will not render at all.

### Stage 9: Transitions and transforms (modules 10 + 12) · DevTools
- **The one idea:** put `transition` on the *normal* state so it works both in and out.
- **Predict:** what if the transition is only on `:hover`? (It animates in, then snaps back.)
- **Show:** the switch knob is a `translateX` on `:checked`; the stat card lift; the tooltip nudge; heatmap cells scaling on hover.
- **Common mistake:** `transition: all`.

### Stage 10: Animations (module 11) · DevTools
- **The one idea:** a transition goes A to B when something triggers it. An animation runs `@keyframes` on its own.
- **Show:** the bars grow in sequence with `animation-delay: calc(var(--i) * 70ms)`. The `--i` comes straight from the HTML.
- **Predict (the donut):** remove `@property --p` and the donut jumps instead of sweeping. Why? (An unregistered custom property is just text, so the browser cannot interpolate it. `@property` says "this is a number".)
- **Rule:** one entrance moment per page. Always respect `prefers-reduced-motion`.
- **Common mistake:** a missing `both` fill-mode makes elements flash before the delay ends.

### Stage 11: Custom properties and dark mode (module 13) · DevTools
- **The one idea:** variables cascade like any property. Change one value in one place and everything that uses it follows.
- **Show:** the whole chart is driven by custom properties set in the HTML (`--h`, `--p`, `--w`, `--i`, `--l`, `--c`). Change one number and the picture changes.
- **Predict:** the last bar has `--bar-color` set on only itself. Why does only that bar change? (Scope: the variable is defined on that element.)
- **Show:** dark mode is `html:has(#theme:checked)` plus `data-theme="dark"`. The dark theme only redefines the variables and touches no other rule.

## Phase 2: solo remix

Students build a dashboard on their own topic. Examples: football league stats, gym tracker, music listening, budget tracker, game inventory. They write the HTML and CSS themselves. Their notes and the Lab are allowed. The part files are not.

Mark it with this checklist (one point each):

| Skill | Evidence |
|---|---|
| Reset | `box-sizing: border-box` applied to everything |
| Layout shell | sidebar, top bar and content built with grid |
| Alignment | flex for the top bar and list rows |
| Positioning | at least one sticky or absolute element |
| Responsive | the layout changes at a phone width |
| Polish | at least one hover transition |
| System | palette in `:root` variables |
| Stretch | one advanced feature of their choice (`:has()`, container query, `@property`, `color-mix()`) |

## Phase 3: bridge to JavaScript

Every item below builds on the dashboard students already made:

1. Save which tasks are ticked (`localStorage`).
2. Remember the theme and add a proper toggle button.
3. Add a "Week / Month" toggle that changes the bars.
4. Generate the heatmap from an array instead of writing 56 `<i>` tags.
5. Make the search box filter the table and tasks.

## If you have fewer sessions

Combine stages: **1** · **2+3** · **4** · **5+6** · **7+8** · **9+10** · **11 and start the remix** · **remix finish**. Keep the Lab for stages 1, 4, 5 and 6 even when you compress; those are the concepts students most often misunderstand.
