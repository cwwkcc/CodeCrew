## What is Markdown?

**Markdown** is a lightweight text formatting language that lets you write structured content using plain text symbols. It was created by John Gruber in 2004 with one goal: make writing for the web as readable and writable as possible.

A Markdown file ends with `.md` and looks like normal text when you write it — but when rendered by a tool like Obsidian, GitHub, or VS Code, those symbols become real formatting: headings, bold text, lists, and links.

```
You write this:        You see this:
**hello**          →   hello (bold)
# Title            →   A large heading
- item             →   A bullet point
```

---

## Why Markdown Matters to a Developer

Markdown is everywhere in the developer world. Understanding it is not optional — it is a basic professional skill.

**Where you will encounter Markdown**:

- `README.md` files in every GitHub repository
- Documentation sites (Docusaurus, GitBook, MkDocs)
- This Obsidian vault — every file here is a `.md` file
- Notion, Confluence, Linear, Jira (all support Markdown)
- Discord, Slack, Telegram (partial Markdown in messages)
- Blog platforms (Dev.to, Hashnode)
- API documentation

**The key insight**: Markdown separates content from presentation — exactly like HTML separates content from CSS. You focus on what you are writing, not how it will look.

---

## Markdown vs HTML

You already know HTML. Markdown covers the same basic ground — but is designed for humans to write quickly, not for machines to parse precisely.

| Purpose         | HTML                            | Markdown      |
| --------------- | ------------------------------- | ------------- |
| Heading         | `<h1>Title</h1>`                | `# Title`     |
| Bold            | `<strong>text</strong>`         | `**text**`    |
| Italic          | `<em>text</em>`                 | `*text*`      |
| Link            | `<a href="url">text</a>`        | `[text](url)` |
| List item       | `<li>item</li>`                 | `- item`      |
| Image           | `<img src="..." alt="...">`     | `![alt](url)` |
| Code            | `<code>text</code>`             | `` `text` ``  |
| Blockquote      | `<blockquote>text</blockquote>` | `> text`      |
| Horizontal rule | `<hr>`                          | `---`         |

**Markdown is not a replacement for HTML.** Markdown gets converted _into_ HTML behind the scenes. It is a shorthand for writing the most common HTML elements quickly.

---

## Core Syntax

### Headings

Use `#` symbols. The number of `#` symbols matches the heading level.

```markdown
# Heading 1

## Heading 2

### Heading 3

#### Heading 4

##### Heading 5

###### Heading 6
```

**Rules**:

- Always put a space between the `#` and the text: `# Title` ✓ not `#Title` ✗
- Never skip levels — go `#` → `##` → `###`, not `#` → `###`.
- There should only be one `#` heading per document (just like `<h1>`).

---

### Bold and Italic

```markdown
**Bold text**
_Italic text_
_**Bold and italic**_
~~Strikethrough~~
```

**When to use each**:

- **Bold** — for key terms, important facts, warnings
- _Italic_ — for titles, technical terms, emphasis, tone
- _**Bold italic**_ — for something critical that also needs emphasis
- ~~Strikethrough~~ — for things that no longer apply or intentional corrections

---

### Lists

**Unordered list** (use `-`, `*`, or `+` — be consistent):

```markdown
- Item one
- Item two
  - Nested item (indent with 2 spaces)
  - Another nested item
- Item three
```

**Ordered list**:

```markdown
1. First step
2. Second step
3. Third step
```

> 💡 In an ordered list, the actual numbers don't matter to most renderers. You can write `1. 1. 1.` and Markdown will still number them correctly. But writing real numbers is better practice for readability.

**Task list** (Obsidian and GitHub support this):

```markdown
- [x] Completed task
- [ ] Incomplete task
- [ ] Another task
```

---

### Links

**External link**:

```markdown
[Display text](https://example.com)
[GitHub](https://github.com)
[CodeCrew](https://github.com/codecrew)
```

**Link with a title** (shows on hover):

```markdown
[GitHub](https://github.com "Visit GitHub")
```

**Bare URL** (renders as a clickable link in most renderers):

```markdown
https://example.com
```

**Obsidian internal link** (links to another note in your vault):

```markdown
[[Note Name]]
[[Note Name|Custom display text]]
[[Note Name#Heading]]
```

---

### Images

```markdown
![Alt text](path/to/image.png)
![CodeCrew Logo](resources/assets/Images/codecrew.jpg)
![External image](https://example.com/image.png)
```

The alt text in `[]` works exactly like the `alt` attribute in HTML `<img>` — it describes the image for accessibility and for when the image fails to load.

---

### Blockquotes

```markdown
> This is a blockquote.

> You can write multiple lines.
> They will be grouped together.

> You can also nest blockquotes.
>
> > This is a nested blockquote.
```

Use blockquotes for:

- Quotes from people
- Important callouts or notes
- Passages you are referencing from another source

---

### Code

**Inline code** — for short snippets, file names, commands, or HTML tags mentioned in text:

```markdown
Use the `console.log()` function to debug.
Save the file as `index.html`.
The `<strong>` tag means importance.
```

**Code block** — for multi-line code. Use three backticks and optionally specify the language:

````markdown
```javascript
function greet(name) {
  return `Hello, ${name}!`;
}
```

```html
<h1>Hello World</h1>
```

```css
body {
  font-family: sans-serif;
}
```
````

Specifying the language after the opening triple backtick enables **syntax highlighting** in Obsidian, GitHub, and most renderers.

---

### Horizontal Rule

A horizontal rule creates a visual separator — identical in purpose to `<hr>` in HTML.

```markdown
---
```

Use it to separate major sections of a document. Three or more hyphens, asterisks, or underscores all work (`---`, `***`, `___`). Stick to `---` for consistency.

---

### Tables

```markdown
| Column 1 | Column 2 | Column 3 |
| -------- | -------- | -------- |
| Cell     | Cell     | Cell     |
| Cell     | Cell     | Cell     |
```

**Column alignment**:

```markdown
| Left | Center | Right |
| :--- | :----: | ----: |
| text |  text  |  text |
```

- `:---` — left align (default)
- `:---:` — center align
- `---:` — right align

---

## Frontmatter (YAML Metadata)

Markdown files can include a metadata block at the very top called **frontmatter**. It is written in YAML and enclosed in triple dashes `---`.

```markdown
---
title: My Note
date: 2026-04-24
tags: [html, codecrew, notes]
author: Jevindu
---

# My Note

Content starts here...
```

Obsidian reads this frontmatter and uses it for search, filtering, and organization. You do not need to use it in every file, but it becomes very useful as your vault grows.

---

## Obsidian-Specific Features

Obsidian extends standard Markdown with features unique to the app.

### Internal Links and Embeds

```markdown
Link to another note:       [[Note Name]]
Embed another note inline:  ![[Note Name]]
Embed a specific heading:   ![[Note Name#Heading]]
Embed an image from vault:  ![[image.png]]
```

### Callouts

```markdown
> [!NOTE]
> This is a note callout.

> [!WARNING]
> This is a warning.

> [!TIP]
> This is a helpful tip.

> [!IMPORTANT]
> Pay attention to this.
```

Callout types: `NOTE`, `TIP`, `WARNING`, `DANGER`, `INFO`, `SUCCESS`, `QUESTION`, `QUOTE`.

### Tags

```markdown
#topic
#html/attributes
#status/in-progress
```

Tags can be nested with `/` to create a hierarchy.

---

## Markdown Rendering

Markdown is just text. It only becomes formatted when passed through a **Markdown renderer** — a piece of software that reads the symbols and converts them to HTML or a visual output.

```
.md file (plain text)
    ↓
Markdown renderer (Obsidian / GitHub / VS Code)
    ↓
Formatted output (HTML / visual)
```

Different renderers support different features. **CommonMark** is the standard specification. **GitHub Flavored Markdown (GFM)** extends it with tables, task lists, and strikethrough. Obsidian extends it further with callouts, internal links, and embeds.

When writing Markdown for a specific platform, check what that platform supports.

---

## Common Mistakes

❌ **Missing space after `#`**

```markdown
#Heading ✗

# Heading ✓
```

---

❌ **No blank line before a list or heading**

```markdown
Some paragraph text.

- Item one ✗

Some paragraph text.

- Item one ✓
```

---

❌ **Skipping heading levels**

```markdown
# Title

### Section ✗

# Title

## Section ✓
```

---

❌ **Mixing list markers inconsistently**

```markdown
- Item one

* Item two ✗ (inconsistent)

- Item one
- Item two ✓
```

---

## Quick Reference Card

| Element         | Syntax             |
| --------------- | ------------------ |
| Heading 1       | `# Text`           |
| Heading 2       | `## Text`          |
| Heading 3       | `### Text`         |
| Bold            | `**Text**`         |
| Italic          | `*Text*`           |
| Bold + Italic   | `***Text***`       |
| Strikethrough   | `~~Text~~`         |
| Unordered list  | `- Item`           |
| Ordered list    | `1. Item`          |
| Task list       | `- [ ] Item`       |
| Link            | `[Text](url)`      |
| Image           | `![Alt](url)`      |
| Inline code     | `` `code` ``       |
| Code block      | ` ```lang `        |
| Blockquote      | `> Text`           |
| Horizontal rule | `---`              |
| Table           | `\| Col \| Col \|` |
| Internal link   | `[[Note]]`         |
| Embed           | `![[Note]]`        |

---

## Further Reading

- [CommonMark Specification](https://commonmark.org/) — the Markdown standard
- [GitHub Flavored Markdown](https://github.github.com/gfm/) — GFM extensions
- [Obsidian Help: Basic Formatting](https://help.obsidian.md/Editing+and+formatting/Basic+formatting+syntax)
- [Markdown Guide](https://www.markdownguide.org/) — comprehensive reference with examples
