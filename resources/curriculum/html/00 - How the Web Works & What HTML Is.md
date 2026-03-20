## What is the Web?

The Web (World Wide Web) is a system of documents and resources linked together and accessible over the internet. The three technologies that make it work are:

- **HTML** — defines the _structure and meaning_ of content
- **CSS** — defines the _visual presentation_ of that content
- **JavaScript** — defines _behaviour_ (interactivity, dynamic updates)

This curriculum covers HTML exclusively. CSS and JS are separate concerns; a well-written HTML document should be fully readable and navigable even with neither of them loaded.

---

## The Request–Response Cycle

When you visit a URL in a browser, this is what happens:

1. **DNS resolution** — the browser converts the domain name (e.g. `example.com`) into an IP address by querying a Domain Name System server.
2. **TCP connection** — the browser opens a connection to the server at that IP.
3. **TLS handshake** (HTTPS only) — the browser and server negotiate encryption.
4. **HTTP request** — the browser sends a `GET` request for the resource at the given path.
5. **HTTP response** — the server replies with a status code and the resource body. For a typical page, the body is an HTML document.
6. **Parsing** — the browser parses the HTML, builds a DOM tree, discovers linked resources (CSS, images, scripts), and fires additional requests for each.
7. **Rendering** — the browser paints pixels to the screen.

HTML is the starting point of this entire chain. Everything else depends on it being well-formed.

---

## What HTML Is (and Is Not)

**HTML is a markup language**, not a programming language. It annotates plain text with _tags_ to give it meaning:

```html
<p>This is a paragraph.</p>
```

The browser reads those tags and knows: "this text is a paragraph." It does not run logic, make decisions, or store state. That is what JavaScript is for.

**HTML describes meaning, not appearance.** `<strong>` means "this content is important", not "make this bold". The browser happens to render it bold by default, but that is a browser stylesheet choice, not HTML's job. This distinction — _semantics_ vs _presentation_ — is the most important concept in the entire curriculum.

---

## A Brief History

|Year|Event|
|---|---|
|1991|Tim Berners-Lee proposes HTML at CERN. First version has ~18 tags.|
|1995|HTML 2.0 — first formal specification.|
|1997|HTML 4.0 — tables, forms, frames, internationalisation.|
|1998|XHTML 1.0 — HTML rewritten as strict XML.|
|2008|HTML5 draft published by WHATWG.|
|2014|HTML5 becomes a W3C Recommendation.|
|2019|WHATWG's HTML Living Standard becomes the single authoritative spec.|

There is no "HTML6." The spec is now a _living standard_ — it evolves continuously. Always reference [html.spec.whatwg.org](https://html.spec.whatwg.org/) for authoritative answers.

---

## Browsers and the DOM

When a browser parses an HTML document it constructs a **Document Object Model (DOM)** — a tree of objects in memory that represents the document's structure. Each HTML element becomes a _node_ in this tree. JavaScript later reads and mutates this tree; CSS selectors target nodes in it.

Understanding that HTML produces a _tree_ (not a flat list of tags) is essential before you write a single line of markup.

---

## Key Terms

|Term|Meaning|
|---|---|
|**Element**|A complete unit: opening tag + content + closing tag. e.g. `<p>Hello</p>`|
|**Tag**|The angle-bracket notation that delimits an element. e.g. `<p>` or `</p>`|
|**Attribute**|Extra information inside an opening tag. e.g. `href="..."` in `<a href="...">`|
|**Void element**|An element with no closing tag and no content. e.g. `<br>`, `<img>`|
|**DOM**|The in-memory tree the browser builds from parsed HTML|
|**Semantic HTML**|Using elements for their meaning, not their default appearance|

---

## What You Will Build by the End of This Curriculum

A fully valid, accessible, semantic HTML document that a browser, a screen reader, a search engine crawler, and a fellow developer can all understand perfectly — with zero CSS and zero JavaScript.