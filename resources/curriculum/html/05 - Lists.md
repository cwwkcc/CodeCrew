HTML has three list types. `<ul>` for unordered items, `<ol>` for ordered sequences, and `<dl>` for term–definition pairs. Lists are also the correct semantic structure for navigation menus — a nav is literally a list of links.

---

## Unordered List: `<ul>`

For items where order does not matter:

```html
<ul>
  <li>Apples</li>
  <li>Bananas</li>
  <li>Mangoes</li>
</ul>
```

The browser renders a bullet point (`•`) before each item by default.

---

## Ordered List: `<ol>`

For items where sequence matters — steps, rankings, instructions:

```html
<ol>
  <li>Boil water</li>
  <li>Add pasta</li>
  <li>Cook for 10 minutes</li>
  <li>Drain and serve</li>
</ol>
```

The browser numbers items automatically starting from 1.

### `<ol>` Attributes

```html
<!-- Start numbering from 5 instead of 1 -->
<ol start="5">
  <li>Step five</li>
  <li>Step six</li>
</ol>

<!-- Count downward -->
<ol reversed>
  <li>Third place</li>
  <li>Second place</li>
  <li>First place</li>
</ol>

<!-- Use letters instead of numbers -->
<ol type="a">
  <li>Option a</li>
  <li>Option b</li>
</ol>
```

|`type` value|Marker style|
|---|---|
|`1`|Numbers — 1, 2, 3 (default)|
|`a`|Lowercase letters — a, b, c|
|`A`|Uppercase letters — A, B, C|
|`i`|Lowercase roman — i, ii, iii|
|`I`|Uppercase roman — I, II, III|

### Overriding a Specific Item's Number

```html
<ol>
  <li>Item one</li>
  <li value="10">Jump to ten</li>   <!-- continues: 10, 11, 12... -->
  <li>Item eleven</li>
</ol>
```

---

## Description List: `<dl>`

For term–definition pairs — glossaries, FAQs, key–value displays:

```html
<dl>
  <dt>CPU</dt>
  <dd>Central Processing Unit — executes program instructions.</dd>

  <dt>RAM</dt>
  <dd>Random Access Memory — volatile short-term storage for running programs.</dd>

  <!-- One term, multiple definitions -->
  <dt>HTTP</dt>
  <dd>HyperText Transfer Protocol — transfers web documents.</dd>
  <dd>Stateless application-layer protocol built on top of TCP.</dd>

  <!-- Multiple terms, one definition -->
  <dt>JS</dt>
  <dt>JavaScript</dt>
  <dd>The programming language of the web.</dd>
</dl>
```

`<dt>` is the term. `<dd>` is the definition. One `<dt>` can have multiple `<dd>` elements, and multiple `<dt>` elements can share one `<dd>`.

---

## Nesting

Lists nest by placing the child list **inside** a `<li>` element, not after it:

```html
<ul>
  <li>Frontend
    <ul>
      <li>HTML</li>
      <li>CSS</li>
      <li>JavaScript</li>
    </ul>
  </li>
  <li>Backend
    <ul>
      <li>Node.js</li>
      <li>Databases</li>
    </ul>
  </li>
</ul>
```

You can mix list types when nesting — an `<ol>` inside a `<ul>` is valid.

---

## Lists as Navigation Structure

Navigation menus are semantically a list of links. The correct HTML structure is `<nav>` + `<ul>` + `<li>` + `<a>`:

```html
<nav>
  <ul>
    <li><a href="/">Home</a></li>
    <li><a href="/courses">Courses</a></li>
    <li><a href="/about">About</a></li>
    <li><a href="/contact">Contact</a></li>
  </ul>
</nav>
```

This is semantically correct — a navigation menu _is_ a list of links. Screen readers announce it as such. How it looks (removing bullets, making it horizontal) is handled later with CSS.

---

## Common Mistakes

```html
<!-- WRONG — <li> directly inside a <div>, not a list element -->
<div>
  <li>Item</li>
</div>

<!-- RIGHT -->
<ul>
  <li>Item</li>
</ul>


<!-- WRONG — nested list placed after </li>, outside the parent item -->
<ul>
  <li>Parent</li>
  <ul>              <!-- this <ul> is a sibling of <li>, not a child -->
    <li>Child</li>
  </ul>
</ul>

<!-- RIGHT — nested list goes inside the <li> -->
<ul>
  <li>Parent
    <ul>
      <li>Child</li>
    </ul>
  </li>
</ul>
```

---
