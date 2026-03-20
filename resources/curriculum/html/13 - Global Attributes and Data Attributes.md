## Global Attributes (available on every HTML element)

| Attribute         | Purpose                                                                 |
| ----------------- | ----------------------------------------------------------------------- |
| `id`              | Unique identifier within the document                                   |
| `class`           | Space-separated list of CSS class names                                 |
| `lang`            | Language override for this element                                      |
| `dir`             | Text direction: `ltr`, `rtl`, `auto`                                    |
| `title`           | Advisory tooltip text (not a substitute for `alt` or `<label>`)         |
| `hidden`          | Hides the element from rendering and assistive technology               |
| `tabindex`        | Controls keyboard tab order                                             |
| `contenteditable` | Makes element editable by the user                                      |
| `draggable`       | Makes element draggable (Drag and Drop API)                             |
| `spellcheck`      | Enable/disable spellcheck                                               |
| `translate`       | Whether to translate content: `yes` / `no`                              |
| `accesskey`       | Keyboard shortcut (use carefully — conflicts with OS/browser shortcuts) |
| `style`           | Inline CSS (avoid; use external stylesheets)                            |

## ARIA global attributes

`role`, `aria-*` — covered in Module 12.

## Event handler attributes

`onclick`, `onkeydown`, etc. — avoid inline handlers; use JS `addEventListener` instead.

---

## Data Attributes: `data-*`

Store custom, application-specific data directly on HTML elements:

```html
<li data-user-id="42" data-role="admin" data-last-login="2025-08-01">
  Cinderax
</li>
```

Rules:

- Must start with `data-`
- Followed by at least one character
- No uppercase letters
- No semicolons

Access in JavaScript:

```javascript
const item = document.querySelector('li');
console.log(item.dataset.userId);    // "42"  (kebab-case → camelCase)
console.log(item.dataset.role);      // "admin"
```

Access in CSS:

```css
li[data-role="admin"] {
  font-weight: bold;
}
```

Data attributes are ideal for passing configuration from server-rendered HTML to JavaScript without inventing your own `id` schemes or hidden inputs.

---