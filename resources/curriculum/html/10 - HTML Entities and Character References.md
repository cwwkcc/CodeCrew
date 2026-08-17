HTML uses `<`, `>`, and `&` as syntax characters. To display them as literal text, you use character references. With UTF-8 encoding — which every modern page uses — most other characters can be typed directly without any escaping.

---

## The Three Characters You Must Always Escape

```html
<!-- & must become &amp; in text content AND in attribute values -->
<p>Use ?a=1&amp;b=2 in query strings</p>
<a href="/search?a=1&amp;b=2">Search</a>

<!-- < and > must be escaped when not used as tag delimiters -->
<p>If x &lt; 10 and y &gt; 5, then proceed.</p>

<!-- " must be escaped inside double-quoted attribute values -->
<input value='She said "hello"' />

<!-- ' must be escaped inside single-quoted attribute values -->
<input value="It's working" />
```

Any other character — if your file is saved as UTF-8 — can be written directly.

---

## Named Character References

| Display | Entity     | Description              |
| ------- | ---------- | ------------------------ |
| `<`     | `&lt;`     | Less-than sign           |
| `>`     | `&gt;`     | Greater-than sign        |
| `&`     | `&amp;`    | Ampersand                |
| `"`     | `&quot;`   | Double quote             |
| `'`     | `&apos;`   | Apostrophe               |
|         | `&nbsp;`   | Non-breaking space       |
| `©`     | `&copy;`   | Copyright                |
| `®`     | `&reg;`    | Registered trademark     |
| `™`     | `&trade;`  | Trademark                |
| `—`     | `&mdash;`  | Em dash                  |
| `–`     | `&ndash;`  | En dash                  |
| `…`     | `&hellip;` | Ellipsis                 |
| `→`     | `&rarr;`   | Right arrow              |
| `←`     | `&larr;`   | Left arrow               |
| `↑`     | `&uarr;`   | Up arrow                 |
| `↓`     | `&darr;`   | Down arrow               |
| `×`     | `&times;`  | Multiplication sign      |
| `÷`     | `&divide;` | Division sign            |
| `±`     | `&plusmn;` | Plus-minus               |
| `°`     | `&deg;`    | Degree sign              |
| `½`     | `&frac12;` | One half                 |
| `¼`     | `&frac14;` | One quarter              |
| `¾`     | `&frac34;` | Three quarters           |
| `€`     | `&euro;`   | Euro sign                |
| `£`     | `&pound;`  | Pound sign               |
| `¥`     | `&yen;`    | Yen sign                 |
| `•`     | `&bull;`   | Bullet                   |
| `"`     | `&ldquo;`  | Left double quote        |
| `"`     | `&rdquo;`  | Right double quote       |
| `'`     | `&lsquo;`  | Left single quote        |
| `'`     | `&rsquo;`  | Right single quote       |
| `∞`     | `&infin;`  | Infinity                 |
| `≠`     | `&ne;`     | Not equal to             |
| `≤`     | `&le;`     | Less than or equal to    |
| `≥`     | `&ge;`     | Greater than or equal to |
| `π`     | `&pi;`     | Greek pi                 |
| `√`     | `&radic;`  | Square root              |

---

## Numeric Character References

Any Unicode character can be referenced by its code point — decimal or hexadecimal:

```html
&#169;
<!-- © — decimal -->
&#x00A9;
<!-- © — hexadecimal (U+00A9) -->

&#8364;
<!-- € — decimal -->
&#x20AC;
<!-- € — hexadecimal -->

&#128512;
<!-- 😀 — emoji, decimal -->
&#x1F600;
<!-- 😀 — emoji, hexadecimal -->
```

The hex form is more common since Unicode documentation uses hex (U+00A9, U+20AC, etc.).

---

## `&nbsp;` — Non-Breaking Space

A regular space is a line-break opportunity — the browser may wrap the line there. `&nbsp;` prevents the break:

```html
<!-- Keeps "10 km" on one line — never broken across lines -->
<p>The distance is 10&nbsp;km.</p>

<!-- Keeps a title together -->
<p>Dr.&nbsp;Perera will see you now.</p>

<!-- Unit values -->
<p>Temperature: 37.5&nbsp;°C</p>
```

Do not use `&nbsp;` to add spacing or indentation for visual purposes — that is CSS's job.

---

## When You Do and Don't Need to Escape

With `<meta charset="UTF-8">` in `<head>` (which every page should have), most characters can be typed directly:

```html
<!-- Must escape — these conflict with HTML syntax -->
<p>if (a &lt; b &amp;&amp; c &gt; d)</p>

<!-- Can type directly — no syntax conflict -->
<p>Temperature: 32°C</p>
<p>Copyright © 2025 Paideon</p>
<p>Price: Rs. 1,500.00</p>
<p>→ Learn more</p>
```

Inside attribute values, be more careful:

```html
<!-- Double-quoted attribute — escape " inside it -->
<meta name="description" content='She said "hello".' />

<!-- URL attribute — escape & inside it -->
<a href="/search?lang=si&amp;q=hello">Search</a>
```

---

## Showing HTML Code on a Page

When you want to display HTML tags as visible text (like in a tutorial), escape the angle brackets:

```html
<pre><code>
&lt;div class="container"&gt;
  &lt;p&gt;Hello World&lt;/p&gt;
&lt;/div&gt;
</code></pre>
```

The browser renders this as:

```
<div class="container">
  <p>Hello World</p>
</div>
```

---

_Next: [The `<head>` Element and Metadata](https://claude.ai/chat/11%20-%20The%20head%20Element%20and%20Metadata.md)_
---

## Common Mistakes

```html
<!-- WRONG: raw < in text content — the browser tries to parse it as
     the start of a tag and the rest of the sentence can disappear -->
<p>To create a tag, type < followed by the element name.</p>

<!-- CORRECT: escape it as an entity -->
<p>To create a tag, type &lt; followed by the element name.</p>

<!-- WRONG: raw & followed by something that looks like an entity name —
     browsers may try (and fail) to resolve it -->
<p>Terms & Conditions</p>

<!-- CORRECT -->
<p>Terms &amp; Conditions</p>
```

The three characters that actually need escaping in normal text content are `<`, `>`, and `&` — everything else (curly quotes, em dashes, accented letters) can usually just be typed directly since modern files are UTF-8 by default (see Module 01's `<meta charset="UTF-8">`). Reach for a named entity like `&copy;` or `&mdash;` mainly when you can't easily type the character itself, not as a blanket habit for every non-ASCII symbol.
