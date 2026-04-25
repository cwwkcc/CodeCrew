## Assignment Overview

You have just completed your first full block of HTML. Before you move on, there is one thing you need to do: **write your notes**.

Not a project. Not a page. Just your notes — in a Markdown file — the same way you would write in a notebook, except this one lives in your vault forever.

The goal is simple: **write down what you understood about HTML, in your own words.** No copy-pasting from the curriculum. No copying from a friend. Your notes should sound like _you_ — the way you would explain something to yourself at 11pm when you forget how a table works.

This is the first note in what will become your personal developer reference. You will keep adding to it throughout CodeCrew.

---

## What Your Notes Should Cover

You have learned a lot of HTML. Your notes should cover all of it — not necessarily in the order it was taught, but in the order that makes sense _to you_.

Here is a reminder of everything you have covered so far:

- How the web works and what HTML actually is
- Document structure — the boilerplate, `<head>`, `<body>`, metadata
- Text content — headings, paragraphs, inline tags (`<strong>`, `<em>`, `<mark>`, `<small>`, `<br>`, `<hr>`)
- Links and navigation — `<a>`, href, relative vs absolute paths, `target`
- Images and figures — `<img>`, `alt`, `<figure>`, `<figcaption>`
- Lists — `<ul>`, `<ol>`, `<li>`, nested lists
- Tables — `<table>`, `<thead>`, `<tbody>`, `<tr>`, `<th>`, `<td>`, `colspan`, `rowspan`

You do not need a section for each topic. You do not need to cover everything equally. Write more about what confused you. Write less about what was obvious. **These are your notes — own them.**

---

## What Makes a Good Personal Note

A good personal note is not a copy of the lesson. It is proof that you understood it.

Here is the difference:

**❌ A bad note (copied, no understanding shown):**

```markdown
## The `<a>` Tag

The `<a>` tag defines a hyperlink, which is used to link from one page to another.
The most important attribute of the `<a>` element is the `href` attribute,
which indicates the link's destination.
```

**✅ A good note (in your own words, shows real understanding):**

```markdown
## Links

`<a>` is basically a pointer. You give it an `href` and it goes there when clicked.

Absolute path = full URL. Works from anywhere.
Relative path = path from the current file. Breaks if you move things around, so
you have to be careful with folder structure.

`target="_blank"` opens in a new tab. I use this when linking outside my own site.

I kept confusing `href="#"` with a real link — it just goes nowhere, it's a placeholder.
```

The second one is shorter, messier, and completely correct. That is the goal.

---

## What You Must Use (Markdown Side)

This is also a Markdown exercise. Your notes must use:

- ✅ Headings — to organize topics (`##`, `###`)
- ✅ Code blocks — for every HTML tag or snippet you reference
- ✅ Bold or italic — for things you want to flag as important or tricky
- ✅ At least one list — unordered or ordered, wherever it fits naturally
- ✅ At least one table — use it somewhere it actually makes sense (a comparison, a cheat sheet row, attributes of a tag)
- ✅ Horizontal rules `---` — to separate major sections

No specific structure is required beyond that. The Markdown syntax itself should feel natural — the same way HTML structure should feel natural now.

---

## Quick Markdown Reference

Since this is your first `.md` file, here is everything you need:

|Element|Syntax|
|---|---|
|Heading 2|`## Text`|
|Heading 3|`### Text`|
|Bold|`**Text**`|
|Italic|`*Text*`|
|Inline code|`` `code` ``|
|Code block|` ```html ` then code then ` ``` `|
|Unordered list|`- Item`|
|Ordered list|`1. Item`|
|Table|`\| Col \| Col \|` then `\|---|
|Horizontal rule|`---`|
|Blockquote|`> Text`|

One important rule: always leave a **blank line** before a heading, list, or code block. If your formatting looks broken, a missing blank line is almost always the reason.

---

## Submission Guidelines

1. **File name**: `yourname.md` — for example `jevindu.md`, `savija.md`.
    
2. **Before submitting**, open the file in Obsidian's preview mode and check:
    
    - Does it look clean and readable?
    - Do your code blocks render properly?
    - Can someone else read your notes and actually learn something from them?

---

## One Last Thing

These notes are for _you_. Nobody is grading your writing style or how many words you wrote. What matters is whether your notes would actually help you if you forgot how something worked six months from now.

If the answer is yes — you did it right.

**Good Luck! 🚀**