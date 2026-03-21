Custom properties (also called CSS variables) let you store a value once and reuse it throughout your stylesheet. When you update the variable, every place that uses it updates automatically.

## Declaring a Custom Property

Custom properties start with `--` and are declared inside a selector:

```css
:root {
  --color-primary: hsl(220, 90%, 55%);
  --color-text: #1a1a2e;
  --color-bg: #f8f9fa;
  --spacing-sm: 0.5rem;
  --spacing-md: 1rem;
  --spacing-lg: 2rem;
  --font-size-base: 1rem;
  --border-radius: 8px;
}
```

`:root` is the `<html>` element — declaring variables here makes them available everywhere. Variables cascade like regular properties and can be overridden in narrower scopes.

## Using a Custom Property

Use `var()` to read a custom property:

```css
.button {
  background: var(--color-primary);
  padding: var(--spacing-sm) var(--spacing-md);
  border-radius: var(--border-radius);
}

p {
  color: var(--color-text);
  font-size: var(--font-size-base);
}
```

## Fallback Values

Provide a fallback in case the variable is not defined:

```css
color: var(--color-accent, navy);  /* use navy if --color-accent is not set */
```

## Scoped Variables and Overriding

Variables cascade — a more specific selector overrides a broader one:

```css
:root {
  --color-primary: blue;
}

.danger {
  --color-primary: red;  /* overrides only inside .danger and its descendants */
}

.button {
  background: var(--color-primary);
  /* blue everywhere except inside .danger where it is red */
}
```

## Dynamic Updates (Dark Mode)

```css
:root {
  --bg: #ffffff;
  --text: #1a1a2e;
}

@media (prefers-color-scheme: dark) {
  :root {
    --bg: #111827;
    --text: #f9fafb;
  }
}

body {
  background: var(--bg);
  color: var(--text);
}
```

Custom properties can also be updated with JavaScript: `document.documentElement.style.setProperty('--color-primary', 'red')`

---