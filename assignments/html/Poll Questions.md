## CodeCrew — Lessons 00 to 03

---

## Lesson 02 — Text Content: Headings, Paragraphs, and Inline Formatting

---

### Question 1

**A developer wants the word "CodeCrew" to stand out on the page. They write this:**

```html
<h1 style="font-size: 16px; font-weight: normal;">CodeCrew</h1>
```

**What is true about this line?**

- A) It's wrong — you can't put a style attribute on h1
- B) It's correct — the style makes it look right, so it is right
- C) It's visually fine but semantically broken — h1 is still an h1 in the DOM
- D) It will not render because font-weight: normal overrides the tag

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                                |
| ------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | You absolutely can put style attributes on any HTML element. That's not the problem here.                                                                                                                                                          |
| B      | ✗       | This is the exact mindset HTML is fighting against. "Looks right" and "is right" are two different things. Screen readers, search engines, and the document outline still see an h1 — not a normal paragraph.                                      |
| C      | ✓       | The style changes the visual output only. The DOM still holds an h1. Screen readers will announce it as the page's main heading. Search engines will weight it as the top-level topic. CSS is a costume — it doesn't change what the element _is_. |
| D      | ✗       | font-weight: normal is a completely valid CSS property. The browser renders it fine — that's actually the whole point of why this mistake is dangerous. It _looks_ fine.                                                                           |
|        |         |                                                                                                                                                                                                                                                    |

---

### Question 2

**A student is building a navigation bar. They write:**

```html
<p><a href="index.html">Home</a></p>
<p><a href="about.html">About</a></p>
<p><a href="contact.html">Contact</a></p>
```

**What is wrong with this structure?**

- A) Links cannot be placed inside p tags
- B) Navigation links should use an anchor tag directly, with no wrapper
- C) This is a list of items — it should be a ul with li elements, wrapped in a nav
- D) Nothing — this is a valid and correct navigation structure

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                                  |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | You can technically nest an anchor inside a p tag. The browser won't break. But that doesn't make it correct.                                                                                                                                                                                        |
| B      | ✗       | Bare anchor tags with no structure are even worse — no grouping, no meaning, just floating links.                                                                                                                                                                                                    |
| C      | ✓       | Navigation is a list of links. Semantically, `<nav><ul><li><a>` is the correct structure. `<nav>` tells the browser this is a navigation landmark. `<ul>` says it's a list of items. Screen readers announce it as navigation and let users skip past it. Three `<p>` tags communicate none of that. |
| D      | ✗       | It renders. It is not correct. The browser is forgiving — it will display almost anything. That forgiveness is not permission.                                                                                                                                                                       |

---

### Question 3

**A student wants space between two ideas. What is the correct fix?**

The student wrote:

```html
<p>First idea.<br /><br />Second idea.</p>
```

- A) It's correct — `<br>` creates paragraph spacing
- B) Replace `<br><br>` with `<hr>`
- C) Split into two separate `<p>` tags
- D) Add a style attribute for margin

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                      |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| A      | ✗       | Wrong. `<br>` is a line break within a single block of content. Using it to fake paragraph spacing is a semantic mistake — you're hiding two ideas inside one paragraph. |
| B      | ✗       | Wrong. `<hr>` is a thematic divider between sections — not a spacing tool. Using it here changes the meaning entirely.                                                   |
| C      | ✓       | Correct. Two separate ideas are two separate paragraphs. CSS handles the visual space between them. HTML handles the meaning.                                            |
| D      | ✗       | Wrong approach. The problem is semantic, not visual. You first fix the HTML structure, then CSS handles appearance.                                                      |

---

### Question 4

**What is the difference between `<strong>` and `<b>`?**

- A) No difference — both make text bold
- B) `<strong>` means important, `<b>` is just visual bold
- C) `<b>` is the correct tag, `<strong>` is outdated
- D) `<strong>` makes text bigger, `<b>` only changes color

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                          |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | They look the same in a browser but mean different things. HTML is about meaning, not appearance.                                                                            |
| B      | ✓       | `<strong>` tells screen readers, search engines, and other tools that this content is semantically important. `<b>` carries zero meaning — it's a purely visual instruction. |
| C      | ✗       | It's the opposite. `<b>` is a legacy tag from when HTML was used for styling. Modern HTML separates meaning from appearance. Use `<strong>`.                                 |
| D      | ✗       | Neither tag changes size or color by default. Both make text bold visually. The difference is entirely in meaning, not appearance.                                           |

---

### Question 5

**A page has this heading structure. What is the problem?**

```html
<h1>CodeCrew</h1>
<h4>What We Teach</h4>
<h2>Join Us</h2>
```

- A) h4 cannot come before h2
- B) Jumping h1 → h4 breaks the document outline
- C) You can only use each heading level once
- D) Nothing — the browser renders it fine

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                            |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | Technically not a browser error — the page will render. But it is semantically wrong. The heading order creates a hierarchy, and skipping levels breaks that structure.                        |
| B      | ✓       | Headings define a document outline. Jumping from h1 to h4 creates a gap in the hierarchy that screen readers and search engines depend on. Always step down one level at a time: h1 → h2 → h3. |
| C      | ✗       | Only h1 is recommended once per page. h2, h3, h4 and below can repeat as many times as you need.                                                                                               |
| D      | ✗       | It will render visually, yes. But "renders fine" is not the same as "correct HTML". Screen readers, search engines, and other tools read meaning — not visual output.                          |

---

### Question 6

**When is `<br>` the correct tag to use?**

- A) To add space between two sections
- B) To separate two different ideas
- C) To break a line inside a single block of content
- D) Whenever you want visual spacing on the page

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                    |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | Section breaks are structural — use separate block tags like `<p>`, `<section>`, or `<hr>` for a thematic divide. `<br>` is not a layout tool.                                                                         |
| B      | ✗       | Two different ideas = two separate `<p>` tags. Don't hide separate ideas inside one paragraph just because `<br>` forces a new line visually.                                                                          |
| C      | ✓       | `<br>` belongs inside a single paragraph when the content itself requires a line break — like a poem, song lyrics, or a postal address. The content is one idea; the line break is part of how that idea is expressed. |
| D      | ✗       | Visual spacing is CSS's job — margin and padding. Using `<br>` tags for layout is one of the oldest bad habits in HTML.                                                                                                |

---

### Question 7

**A student writes this to show the title of a book:**

```html
<p>My favourite book is <em>Dune</em>.</p>
```

**Their senior says to use `<i>` instead. Are they right?**

- A) No — `<i>` is deprecated, always use `<em>`
- B) No — `<em>` and `<i>` are identical, it doesn't matter
- C) Yes — book titles are not spoken emphasis, they are a typographic convention; `<i>` is correct here
- D) Yes — `<i>` is for italic text and this text should be italic

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                                                       |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | `<i>` is not deprecated. It was redefined in HTML5 with a specific, valid meaning: technical terms, foreign phrases, fictional character thoughts, and typographic conventions like titles. It's a legitimate element when used correctly.                                                                                |
| B      | ✗       | They look the same visually. They mean different things. `<em>` means spoken stress — the kind that changes the meaning of a sentence. `<i>` means "offset from the surrounding prose in a conventional way." Titles fall under that second definition.                                                                   |
| C      | ✓       | Book titles, film titles, and album titles are a typographic convention — you're not emphasising _Dune_ the way you'd stress a word in speech. `<em>` is for: _"I never said he did it"_ — where which word you stress changes the meaning. `<i>` is for the technical, conventional use of italics. The senior is right. |
| D      | ✗       | The tag's job is not to produce italics — that's CSS's job. The tag's job is to communicate meaning. "Should be italic" is never a reason to pick a tag.                                                                                                                                                                  |

---

### Question 8

**Which of these four lines is semantically correct for its described purpose?**

- A) A page title: `<title>My Portfolio</title>` inside the `<body>`
- B) A copyright notice: `<small>© 2025 CodeCrew</small>`
- C) A deleted price: `<s>Rs. 4500</s>`
- D) Keyboard shortcut instruction: `<em>Press Ctrl+S to save</em>`

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                                                          |
| ------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | `<title>` belongs exclusively inside `<head>`. Placed in `<body>`, browsers may ignore it or display it as raw text. It sets the browser tab name — it is never a visible page heading.                                                      |
| B      | ✓       | `<small>` originally meant fine print — legal text, copyright notices, disclaimers. Its semantic meaning in HTML5 is specifically "side comments and small print." This is an exact fit.                                                     |
| C      | ✗       | `<s>` marks content that is no longer accurate or relevant — like a cancelled appointment. A crossed-out price on a sale is actually the right case for `<del>`, which carries the meaning "this was removed." `<s>` is close but imprecise. |
| D      | ✗       | `<em>` means spoken emphasis — a word you'd stress out loud. Keyboard instructions are code-like content. The correct tag is `<kbd>`, which exists specifically for keyboard input. `<em>` here is using the wrong meaning entirely.         |

---

## Lesson 03 — Links and Navigation

---

### Question 9

**What does `target="_blank"` do — and what should always come with it?**

- A) Opens link in same tab — nothing extra needed
- B) Opens link in new tab — pair with `rel="noopener"`
- C) Opens link in a popup — pair with `rel="popup"`
- D) Disables the link — used for placeholder anchors

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                      |
| ------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | `target="_blank"` specifically opens the link in a new tab. Opening in the same tab is the default behaviour — no attribute needed.                                                                      |
| B      | ✓       | `target="_blank"` opens the link in a new tab. Always add `rel="noopener noreferrer"` — without it, the new page can access and manipulate your page through `window.opener`. It's a real security risk. |
| C      | ✗       | `target="_blank"` has nothing to do with popups. `rel="popup"` doesn't exist in HTML.                                                                                                                    |
| D      | ✗       | A disabled placeholder link uses `href="#"` or `javascript:void(0)`, not `target="_blank"`. `_blank` always opens something.                                                                             |

---

### Question 10

**You want a link that opens the user's email app. Which href is correct?**

- A) `href="email:hi@codecrew.lk"`
- B) `href="mailto:hi@codecrew.lk"`
- C) `href="send:hi@codecrew.lk"`
- D) `href="contact:hi@codecrew.lk"`

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                      |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| A      | ✗       | `email:` is not a valid URI scheme. The browser will not know what to do with it.                                                                                        |
| B      | ✓       | `mailto:` is the official URI scheme for email links. The browser passes it to the system's default email client, which opens a new message with the address pre-filled. |
| C      | ✗       | `send:` doesn't exist as a URI scheme in HTML.                                                                                                                           |
| D      | ✗       | `contact:` is not a valid URI scheme. The valid schemes you'll use in web development are `https:`, `mailto:`, `tel:`, and fragment `#` — "contact:" is not one of them. |

---

### Question 11

**What is the difference between these two links?**

```html
<a href="about.html">About</a> <a href="/about.html">About</a>
```

- A) No difference — both go to the same page
- B) The first is a relative path, the second is an absolute path from the site root
- C) The second one is broken — the slash makes it an external URL
- D) The first only works on the homepage; the second works everywhere

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                              |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| A      | ✗       | They look identical but behave differently depending on where you are on the site. Same visual output, different meaning — you've seen this before.                                                                                                                                              |
| B      | ✓       | `about.html` is relative — it looks for about.html in the same folder as the current page. If you're inside `/blog/post.html`, it resolves to `/blog/about.html`. `/about.html` is root-relative — it always resolves to `/about.html` from the site's root, regardless of which page you're on. |
| C      | ✗       | A leading slash does not make something external. External URLs require a full scheme — `https://`. A slash means root of the current site.                                                                                                                                                      |
| D      | ✗       | Close to the truth but backwards. `about.html` (no slash) is the one that breaks when used from nested pages. `/about.html` works consistently from anywhere on the site.                                                                                                                        |

---

### Question 12

**A student adds this to their page:**

```html
<a href="#" target="_blank">Learn more</a>
```

**What actually happens when a user clicks this?**

- A) Opens a new blank tab and stays there with nothing loaded
- B) Opens a new tab that loads the homepage
- C) Opens a new tab that loads the current page
- D) Does nothing — href="#" disables the link

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                      |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | The tab does not load nothing. `#` is a fragment pointing to the top of the current page — that location is still a real URL.                                                                                                            |
| B      | ✗       | `#` has no relation to the homepage. It refers to the current document.                                                                                                                                                                  |
| C      | ✓       | `href="#"` means "the top of the current page." `target="_blank"` opens that in a new tab. So clicking this opens a new tab that loads an exact copy of the page the user is already on. A common mistake — and a common interview trap. |
| D      | ✗       | `href="#"` is one of the most active links you can write. It always resolves. It never disables.                                                                                                                                         |

---

### Question 13

**A student adds a link to call a phone number on mobile. Which is correct?**

- A) `href="call:+94712345678"`
- B) `href="phone:+94712345678"`
- C) `href="tel:+94712345678"`
- D) `href="dial:+94712345678"`

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                    |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| A      | ✗       | `call:` is not a valid URI scheme. The browser has no handler for it and will do nothing.                                                                                                              |
| B      | ✗       | `phone:` is also not a valid URI scheme. Only a defined set of schemes have browser handlers.                                                                                                          |
| C      | ✓       | `tel:` is the correct scheme for telephone links. On mobile, tapping it opens the phone dialler with the number pre-filled. The number should always include the country code in international format. |
| D      | ✗       | `dial:` doesn't exist as a URI scheme. There are very few valid schemes — guessing variations of the right word will always fail.                                                                      |

---

### Question 14

**A student adds a security issue to their code without knowing it. What is missing?**

```html
<a href="https://github.com" target="_blank">GitHub</a>
```

- A) The link needs `type="external"` to warn the browser
- B) `target="_blank"` without `rel="noopener noreferrer"` lets the new page access and control the original page
- C) External links must use `href="external:https://..."` as the scheme
- D) There is no security issue — this is completely correct

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                                                                                |
| ------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | `type="external"` is not a valid attribute on anchor tags. It does nothing.                                                                                                                                                                                                                                                                        |
| B      | ✓       | Without `rel="noopener noreferrer"`, the page opened in the new tab gets access to the `window.opener` object of your page. A malicious site can use this to redirect your original page to a phishing URL while the user is distracted. `rel="noopener"` cuts that connection. `noreferrer` also hides where the user came from. Always use both. |
| C      | ✗       | `external:` is not a URI scheme. The browser would not know what to do with it.                                                                                                                                                                                                                                                                    |
| D      | ✗       | It works and renders correctly — but correct-looking code can still be insecure. This is exactly the kind of thing code review exists to catch.                                                                                                                                                                                                    |

---

### Question 15

**A student writes two links on the same page:**

```html
<h2 id="contact section">Contact</h2>
<a href="#contact section">Jump to Contact</a>
```

**Will this anchor link work?**

- A) Yes — the id and href match exactly, so it works
- B) No — `id` values cannot contain spaces; this is invalid
- C) Yes — browsers automatically handle spaces in fragment identifiers
- D) No — `<h2>` elements cannot have an `id` attribute

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                     |
| ------ | ------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | Matching text alone is not enough. The `id` value itself is invalid. An invalid `id` produces an invalid anchor target, regardless of whether the `href` matches.       |
| B      | ✓       | The HTML spec requires that `id` values contain no spaces. A space in an `id` is invalid markup. The fix is `id="contact-section"` and `href="#contact-section"`.       |
| C      | ✗       | Some browsers may attempt to encode the space and resolve the fragment anyway, but this is undefined error-recovery behaviour, not specified behaviour you can rely on. |
| D      | ✗       | `id` is a global attribute — it can go on any HTML element. `<h2>`, `<p>`, `<div>`, `<section>` — any of them can be an anchor target.                                  |

---

## Lesson 01 — Document Structure and the HTML Boilerplate

---

### Question 16

**A student forgets to add `<!DOCTYPE html>` to their page. What actually happens?**

- A) The page throws an error and won't render
- B) The browser renders it in quirks mode, which emulates old broken behaviour
- C) The browser ignores the missing declaration and renders normally
- D) The page renders correctly on modern browsers but breaks on old ones

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                                            |
| ------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | The browser never refuses to render HTML. It will always try to display something. Crashing on missing declarations would break half the internet.                                                                                                                                                             |
| B      | ✓       | Without `<!DOCTYPE html>`, the browser switches to quirks mode — a compatibility mode that deliberately emulates the incorrect, inconsistent behaviour of 1990s browsers. Box model calculations, layout, and rendering all change. Things that look fine in standards mode can silently break in quirks mode. |
| C      | ✗       | The page _looks_ fine on screen, so students assume it's fine. It isn't. The rendering engine is running in the wrong mode — you just can't see it until something subtle breaks.                                                                                                                              |
| D      | ✗       | It's the opposite. Quirks mode matters most on modern browsers because they still maintain it for backward compatibility. Modern browsers are the ones that will silently switch modes on you.                                                                                                                 |

---

### Question 17

**Why does the `lang` attribute on the `<html>` tag matter? Pick the most complete answer.**

```html
<html lang="en"></html>
```

- A) It has no real effect — it's just a label for developers
- B) It only affects spell-checking in the browser
- C) It controls pronunciation in screen readers, language targeting in search engines, and CSS hyphenation
- D) It is only needed when the page contains multiple languages

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                  |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| A      | ✗       | It is consumed by multiple systems that are invisible to a developer looking at the screen. "No visual effect" is not the same as "no effect."                                                                                                                                       |
| B      | ✗       | Spell-check is one small side effect. The real consumers are screen readers, search engines, and CSS — all three depend on `lang` for fundamentally different reasons.                                                                                                               |
| C      | ✓       | Screen readers use it to select the correct pronunciation engine — `lang="si"` switches to Sinhala pronunciation. Search engines use it for language targeting. `hyphens: auto` in CSS uses it to know which hyphenation dictionary to load. Omitting it is an accessibility defect. |
| D      | ✗       | `lang` is required on every page, regardless of whether it mixes languages. Multi-language content uses `lang` on individual elements to override the root — but the root declaration is always mandatory.                                                                           |

---

### Question 18

**Where must `<meta charset="UTF-8">` appear, and why?**

- A) Anywhere inside `<head>` — position doesn't matter
- B) As the first child of `<head>`, before `<title>`
- C) Inside `<body>`, just before the first text content
- D) At the very top of the file, before `<!DOCTYPE html>`

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                            |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | Position absolutely matters here. The browser reads the file top to bottom. If it encounters the `<title>` before it knows the encoding, it may misread any non-ASCII characters in the title. |
| B      | ✓       | The browser needs to know the encoding before it reads anything else in the document — including the title, which might contain characters outside ASCII. First child of `<head>` is the rule. |
| C      | ✗       | Metadata belongs in `<head>`. A `<meta>` tag placed in `<body>` is invalid HTML. The browser may move it, ignore it, or handle it unpredictably.                                               |
| D      | ✗       | `<!DOCTYPE html>` must be the absolute first line — nothing before it, not even whitespace. The charset declaration goes inside `<head>`, not before the doctype.                              |

---

### Question 19

**What is wrong with this HTML?**

```html
<span>
  <p>This is some text.</p>
</span>
```

- A) `<span>` needs a class attribute to be valid
- B) A block element (`<p>`) cannot be nested inside an inline element (`<span>`)
- C) Nothing — this renders correctly so it is correct
- D) `<span>` is deprecated and should be replaced with `<div>`

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                      |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | `<span>` works perfectly fine without a class. A class is optional — it's used as a hook for CSS and JavaScript, not for validity.                                                                                                                                                       |
| B      | ✓       | `<span>` is inline — it can only hold other inline elements and text. `<p>` is a block-level element. Nesting a block inside an inline violates the content model. The browser will attempt to fix it using error recovery, but you're relying on the browser's guess, not correct HTML. |
| C      | ✗       | The browser's error recovery algorithm is forgiving — it will rearrange invalid nesting to make something displayable. That forgiveness is not permission.                                                                                                                               |
| D      | ✗       | `<span>` is not deprecated. It's the standard generic inline container, actively used in modern HTML.                                                                                                                                                                                    |

---

### Question 20

**How many spaces does this render as in the browser?**

```html
<p>Hello world</p>
```

- A) 10 spaces — exactly as written
- B) 0 spaces — the browser strips whitespace between words
- C) 1 space — the browser collapses all consecutive whitespace to a single space
- D) It depends on the browser

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                         |
| ------ | ------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | The browser does not preserve whitespace in HTML content by default. This is why you can freely indent your code without it affecting the visual output.                                                                                                                                    |
| B      | ✗       | It doesn't strip whitespace entirely — words would run together. It collapses it down to one space, not zero.                                                                                                                                                                               |
| C      | ✓       | The HTML spec says consecutive whitespace — spaces, tabs, newlines — in text content is collapsed to a single space. This is what makes clean indentation possible and why `<br>` exists for intentional line breaks. If you need to preserve exact whitespace, that's what `<pre>` is for. |
| D      | ✗       | This behaviour is defined in the HTML spec — every standards-mode browser does it the same way. It is not browser-dependent.                                                                                                                                                                |

---

### Question 21

**What does an `<a>` tag without an `href` attribute do?**

```html
<a>Click here</a>
```

- A) It throws an HTML error and won't render
- B) It behaves as a normal link but goes to a blank page
- C) It renders as text with no link behaviour — it's just a placeholder anchor
- D) It links to the current page, equivalent to `href="#"`

**Answer: C**

| Option | Verdict | Why                                                                                                                                                                                                                                                      |
| ------ | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | There's no such thing as an HTML rendering error. The browser will display it. Missing `href` is not a fatal problem — it's a defined state.                                                                                                             |
| B      | ✗       | Without `href`, there is no navigation behaviour at all. No clicking, no cursor change, no underline by default. It doesn't go anywhere because it isn't a link.                                                                                         |
| C      | ✓       | An `<a>` without `href` is a placeholder anchor — valid in HTML, and sometimes used intentionally for links that will be activated dynamically by JavaScript later. It renders as plain text with no focus, no pointer cursor, and no default underline. |
| D      | ✗       | `href="#"` is an active link — it gives the element link behaviour and navigates to the top of the page on click. A missing `href` does none of that.                                                                                                    |

---

### Question 22

**What is the difference between these two lines?**

```html
<link rel="stylesheet" href="/style.css" />
<a href="/style.css">Download stylesheet</a>
```

- A) Nothing — both are just links to the same file
- B) `<link>` is for machine-readable resource relationships; `<a>` is for user-navigable links
- C) `<link>` belongs in `<body>`; `<a>` belongs in `<head>`
- D) `<link>` is deprecated — `<a>` should be used for stylesheets too

**Answer: B**

| Option | Verdict | Why                                                                                                                                                                                                                                                                                                                                                                                                  |
| ------ | ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| A      | ✗       | They point to the same file, but the meaning and the audience are completely different. One is an instruction to the browser. One is an offer to the user. Same URL, entirely different semantics.                                                                                                                                                                                                   |
| B      | ✓       | `<link>` is a machine-readable declaration in `<head>`. It tells the browser: "this document has a relationship with this resource" — load it as a stylesheet, use it as a favicon, treat it as the canonical URL. The user never sees it. `<a>` is a clickable link in `<body>` — it exists for humans to follow. Two different elements, two different jobs, two different places in the document. |
| C      | ✗       | It's the opposite. `<link>` belongs in `<head>`. `<a>` belongs in `<body>`.                                                                                                                                                                                                                                                                                                                          |
| D      | ✗       | `<link>` is not deprecated — it's the correct and only way to attach stylesheets declaratively in HTML. Using `<a>` for a stylesheet would do nothing except offer the user a link to download the raw CSS file.                                                                                                                                                                                     |
