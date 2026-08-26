> **Series overview:** This is file 7 of 7. It covers the full tailwind.config.js API — extending and replacing the theme, custom utilities, plugins, CSS variables integration, the @apply directive, and the component extraction patterns that keep large Tailwind codebases maintainable.
>
> **A note on versions:** Sections 1–10 below document `tailwind.config.js`, which is the **v3** configuration model. It's still essential to know — most existing Tailwind codebases run v3, and you'll be reading (or migrating) this exact structure regularly. Section 13 covers the **v4** `@theme` equivalent in full, including the concrete breaking changes to expect when migrating. Sections 11–12 (component extraction, `cn()`) are version-agnostic and apply identically either way.

---

## Table of Contents

1. [tailwind.config.js — The Full Shape](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#1-tailwindconfigjs--the-full-shape)
2. [extend vs Replace](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#2-extend-vs-replace)
3. [Customising the Colour Palette](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#3-customising-the-colour-palette)
4. [Customising Spacing, Sizing & Breakpoints](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#4-customising-spacing-sizing--breakpoints)
5. [Customising Typography](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#5-customising-typography)
6. [CSS Variables as Design Tokens](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#6-css-variables-as-design-tokens)
7. [The @apply Directive — When and When Not To Use It](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#7-the-apply-directive--when-and-when-not-to-use-it)
8. [Adding Custom Utilities with @layer](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#8-adding-custom-utilities-with-layer)
9. [Plugins — Extending Tailwind Programmatically](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#9-plugins--extending-tailwind-programmatically)
10. [Safelist — Ensuring Dynamic Classes Are Generated](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#10-safelist--ensuring-dynamic-classes-are-generated)
11. [Component Extraction Patterns](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#11-component-extraction-patterns)
12. [The cn() Utility Pattern](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#12-the-cn-utility-pattern)
13. [Tailwind v4 — What's Changing](https://claude.ai/chat/9e9fb9e9-f308-4e3f-916e-a39041ab68d5#13-tailwind-v4--whats-changing)

---

## 1. tailwind.config.js — The Full Shape

```js
/** @type {import('tailwindcss').Config} */
module.exports = {
  // ── CONTENT ──────────────────────────────────────────────────────────────
  // Files to scan for class names. JIT only generates CSS for classes found here.
  content: [
    "./src/**/*.{html,js,ts,jsx,tsx,mdx}",
    "./components/**/*.{js,ts,jsx,tsx}",
    "./app/**/*.{js,ts,jsx,tsx}",
    // You can also use arrays of file paths:
    // { raw: '<div class="bg-red-500">', extension: 'html' },
  ],

  // ── DARK MODE ─────────────────────────────────────────────────────────────
  // 'media'  → prefers-color-scheme  (automatic, follows OS)
  // 'class'  → requires <html class="dark">  (manual/toggle)
  // ['class', '[data-theme="dark"]'] → custom selector
  darkMode: "class",

  // ── THEME ─────────────────────────────────────────────────────────────────
  theme: {
    // Keys here REPLACE the default values for that category.
    // Use this to enforce strict design tokens.

    // theme.extend ADDS to the defaults without replacing them.
    extend: {},
  },

  // ── PLUGINS ──────────────────────────────────────────────────────────────
  plugins: [],

  // ── PREFIX ───────────────────────────────────────────────────────────────
  // Adds a prefix to all Tailwind classes to avoid conflicts with other CSS.
  // prefix: 'tw-',  // bg-blue-500 becomes tw-bg-blue-500

  // ── IMPORTANT ─────────────────────────────────────────────────────────────
  // Adds !important to all utility declarations (use only as a last resort).
  // important: true,
  // Or scope to a selector:
  // important: '#app',

  // ── SEPARATOR ────────────────────────────────────────────────────────────
  // Character used to separate variant prefix from utility.
  // separator: ':',  // default — produces hover:bg-blue-500
};
```

---

## 2. extend vs Replace

```js
module.exports = {
  theme: {
    // ── REPLACING ──────────────────────────────────────────────────────────
    // Setting a key directly in theme REPLACES Tailwind's defaults entirely.
    colors: {
      // ← ALL default colours are gone. Only these exist.
      white: "#ffffff",
      black: "#000000",
      brand: {
        50: "#eff6ff",
        500: "#3b82f6",
        900: "#1e3a8a",
      },
    },

    // ── EXTENDING ──────────────────────────────────────────────────────────
    // Keys in theme.extend MERGE with Tailwind's defaults.
    extend: {
      colors: {
        // ← All default colours still exist PLUS these new ones.
        brand: {
          50: "#eff6ff",
          500: "#3b82f6",
          900: "#1e3a8a",
        },
      },
      spacing: {
        13: "3.25rem", // adds p-13, m-13, w-13, h-13, gap-13, etc.
        18: "4.5rem",
        128: "32rem",
      },
      borderRadius: {
        "4xl": "2rem",
      },
      fontFamily: {
        display: ["Playfair Display", "serif"],
      },
    },
  },
};
```

---

## 3. Customising the Colour Palette

### Option A — Extend with custom colours

```js
const colors = require("tailwindcss/colors");

module.exports = {
  theme: {
    extend: {
      colors: {
        // Add a completely new colour with all shades
        brand: {
          50: "#fdf4ff",
          100: "#fae8ff",
          200: "#f5d0fe",
          300: "#f0abfc",
          400: "#e879f9",
          500: "#d946ef",
          600: "#c026d3",
          700: "#a21caf",
          800: "#86198f",
          900: "#701a75",
          950: "#4a044e",
        },
        // Alias an existing colour under a new name
        primary: colors.blue,
        danger: colors.red,
        // Single value (no shades)
        github: "#1B1F24",
      },
    },
  },
};
```

### Option B — Replace palette entirely (strict design tokens)

```js
module.exports = {
  theme: {
    colors: {
      transparent: "transparent",
      current: "currentColor",
      black: "#000",
      white: "#fff",
      // Only the colours your design system uses
      gray: {
        50: "#f9fafb",
        100: "#f3f4f6",
        200: "#e5e7eb",
        300: "#d1d5db",
        400: "#9ca3af",
        500: "#6b7280",
        600: "#4b5563",
        700: "#374151",
        800: "#1f2937",
        900: "#111827",
      },
      primary: {
        50: "#eff6ff",
        100: "#dbeafe",
        500: "#3b82f6",
        600: "#2563eb",
        700: "#1d4ed8",
      },
    },
  },
};
```

---

## 4. Customising Spacing, Sizing & Breakpoints

### Spacing

```js
// The spacing key controls padding, margin, width, height, gap, inset,
// and other sizing utilities simultaneously.
module.exports = {
  theme: {
    extend: {
      spacing: {
        // Add values beyond the default scale
        13: "3.25rem", // 52px
        15: "3.75rem", // 60px
        18: "4.5rem", // 72px
        88: "22rem", // 352px
        104: "26rem", // 416px
        112: "28rem", // 448px
        128: "32rem", // 512px
        // Pixel values
        "0.5px": "0.5px",
        "1px": "1px",
      },
    },
  },
};
```

### Max-Width

```js
extend: {
  maxWidth: {
    '8xl': '88rem',   // 1408px
    '9xl': '96rem',   // 1536px
    // Matches a custom container width
    'prose-wide': '80ch',
  },
}
```

### Breakpoints

```js
// Replace breakpoints entirely
module.exports = {
  theme: {
    screens: {
      'xs': '475px',
      'sm': '640px',
      'md': '768px',
      'lg': '1024px',
      'xl': '1280px',
      '2xl': '1536px',
    },
  },
}

// Add a max-width breakpoint (non-mobile-first — use sparingly)
extend: {
  screens: {
    'xs': '475px',
    // max-width breakpoints (apply BELOW the specified size)
    'max-sm': { max: '639px' },  // → @media (max-width: 639px)
    // raw media query
    'print': { raw: 'print' },
  },
}
```

---

## 5. Customising Typography

```js
module.exports = {
  theme: {
    extend: {
      // Font families
      fontFamily: {
        sans: ["Inter var", "ui-sans-serif", "system-ui"],
        serif: ["Playfair Display", "ui-serif", "Georgia"],
        mono: ["JetBrains Mono", "ui-monospace", "SFMono-Regular"],
        display: ['"Cal Sans"', "sans-serif"],
      },

      // Font sizes — each entry: [font-size, { lineHeight, letterSpacing, fontWeight }]
      fontSize: {
        "2xs": ["0.625rem", { lineHeight: "0.75rem" }], // 10px
        // Override default line-heights
        sm: ["0.875rem", { lineHeight: "1.375rem" }], // custom leading for sm
        // Fluid type
        "fluid-lg": ["clamp(1.125rem, 2.5vw, 1.5rem)", { lineHeight: "1.4" }],
      },

      // Line heights
      lineHeight: {
        "extra-loose": "2.5",
        12: "3rem",
      },

      // Letter spacing
      letterSpacing: {
        "ultra-tight": "-0.075em",
        "ultra-wide": "0.2em",
      },
    },
  },
};
```

---

## 6. CSS Variables as Design Tokens

Using CSS custom properties (variables) as Tailwind values enables runtime theming — you can change the variable value with JavaScript and all Tailwind utilities using it update instantly.

```css
/* globals.css */
@tailwind base;
@tailwind components;
@tailwind utilities;

@layer base {
  :root {
    /* Light theme */
    --color-background: 255 255 255;
    --color-foreground: 17 24 39;
    --color-primary: 37 99 235;
    --color-primary-foreground: 255 255 255;
    --color-muted: 243 244 246;
    --color-muted-foreground: 107 114 128;
    --color-border: 229 231 235;
    --color-ring: 37 99 235;

    --radius: 0.5rem;
  }

  .dark {
    /* Dark theme */
    --color-background: 9 9 11;
    --color-foreground: 250 250 250;
    --color-primary: 59 130 246;
    --color-primary-foreground: 9 9 11;
    --color-muted: 39 39 42;
    --color-muted-foreground: 161 161 170;
    --color-border: 39 39 42;
    --color-ring: 59 130 246;
  }
}
```

```js
// tailwind.config.js — wire variables to utilities
module.exports = {
  theme: {
    extend: {
      colors: {
        background: "rgb(var(--color-background) / <alpha-value>)",
        foreground: "rgb(var(--color-foreground) / <alpha-value>)",
        primary: {
          DEFAULT: "rgb(var(--color-primary) / <alpha-value>)",
          foreground: "rgb(var(--color-primary-foreground) / <alpha-value>)",
        },
        muted: {
          DEFAULT: "rgb(var(--color-muted) / <alpha-value>)",
          foreground: "rgb(var(--color-muted-foreground) / <alpha-value>)",
        },
        border: "rgb(var(--color-border) / <alpha-value>)",
        ring: "rgb(var(--color-ring) / <alpha-value>)",
      },
      borderRadius: {
        lg: "var(--radius)",
        md: "calc(var(--radius) - 2px)",
        sm: "calc(var(--radius) - 4px)",
      },
    },
  },
};
```

```html
<!-- Now dark mode works by toggling .dark, no per-element dark: classes needed -->
<div class="bg-background text-foreground">
  <button class="bg-primary text-primary-foreground rounded-lg px-4 py-2">
    Button
  </button>
  <p class="text-muted-foreground text-sm">Helper text</p>
</div>

<!-- With opacity modifier — the <alpha-value> token enables this -->
<div class="bg-primary/20">
  <!-- 20% opacity primary background -->
  <div class="border border-border/50"></div>
</div>
```

```
The <alpha-value> placeholder:
  When Tailwind processes bg-primary/20, it looks at the colour definition.
  If the definition uses <alpha-value>, Tailwind replaces it with 0.2.
  Result: rgb(var(--color-primary) / 0.2)

  Without <alpha-value>, the / opacity modifier doesn't work.
  This is why the CSS variable pattern must use this specific syntax.
```

---

## 7. The @apply Directive — When and When Not To Use It

`@apply` lets you use Tailwind utilities inside a CSS rule.

```css
/* What @apply does */
.btn {
  @apply px-4 py-2 rounded-md font-medium transition-colors;
}

/* Compiles to: */
.btn {
  padding-left: 1rem;
  padding-right: 1rem;
  padding-top: 0.5rem;
  padding-bottom: 0.5rem;
  border-radius: 0.375rem;
  font-weight: 500;
  transition-property:
    color, background-color, border-color, text-decoration-color, fill, stroke;
  transition-timing-function: cubic-bezier(0.4, 0, 0.2, 1);
  transition-duration: 150ms;
}
```

### When @apply Is Appropriate

```css
/* 1. Styling third-party HTML you can't add classes to */
.markdown-body h1 {
  @apply text-3xl font-bold text-gray-900 mt-8 mb-4;
}

.markdown-body a {
  @apply text-blue-600 underline hover:text-blue-800;
}

/* 2. Base element resets in @layer base */
@layer base {
  h1 {
    @apply text-2xl font-bold tracking-tight;
  }
  h2 {
    @apply text-xl font-semibold;
  }
  a {
    @apply text-blue-600 hover:underline;
  }
}

/* 3. Complex pseudo-element styles that are painful inline */
.fancy-underline::after {
  @apply absolute bottom-0 left-0 w-full h-0.5 bg-blue-500 scale-x-0 transition-transform;
  content: "";
}
.fancy-underline:hover::after {
  @apply scale-x-100;
}
```

### When NOT To Use @apply

```css
/* BAD: creates a component class for something you should extract as a React component */
.card {
  @apply bg-white rounded-xl shadow-md overflow-hidden p-6;
}

/* Why bad:
   1. You've invented a name (.card) for something with no agreed meaning.
   2. Anyone using .card must read this file to know what it does.
   3. The class will accumulate more and more rules over time.
   4. You can't conditionally apply individual utilities (e.g., different padding at breakpoints).
   
   Instead: create a <Card> React/Vue component with the classes inline.
*/
```

---

## 8. Adding Custom Utilities with @layer

`@layer` adds styles to a Tailwind layer. Styles in `utilities` get the same specificity as Tailwind's utilities.

```css
@layer utilities {
  /* Custom utility that acts like any built-in utility */
  .scrollbar-hide {
    -ms-overflow-style: none;
    scrollbar-width: none;
  }
  .scrollbar-hide::-webkit-scrollbar {
    display: none;
  }

  /* Fluid typography utility */
  .text-fluid-xl {
    font-size: clamp(1.25rem, 3vw, 2rem);
  }

  /* Text balance (newer CSS) */
  .text-balance {
    text-wrap: balance;
  }

  .text-pretty {
    text-wrap: pretty;
  }
}
```

```css
@layer components {
  /* Named component patterns — use sparingly */
  /* Good for design system primitives that appear 50+ times */
  .input-base {
    @apply block w-full rounded-md border border-gray-300 px-3 py-2 text-sm
           focus:border-blue-500 focus:outline-none focus:ring-1 focus:ring-blue-500;
  }

  .label-base {
    @apply block text-sm font-medium text-gray-700 mb-1;
  }
}
```

---

## 9. Plugins — Extending Tailwind Programmatically

Plugins can add utilities, components, variants, or base styles.

```js
const plugin = require("tailwindcss/plugin");

module.exports = {
  plugins: [
    // Official plugins
    require("@tailwindcss/forms"), // Better form element styling
    require("@tailwindcss/typography"), // Prose content styling
    require("@tailwindcss/aspect-ratio"), // Pre-v3 aspect ratio support
    require("@tailwindcss/container-queries"), // Container queries

    // Custom plugin
    plugin(function ({
      addUtilities,
      addComponents,
      addBase,
      theme,
      matchUtilities,
    }) {
      // Add static utilities
      addUtilities({
        ".scrollbar-hide": {
          "-ms-overflow-style": "none",
          "scrollbar-width": "none",
          "&::-webkit-scrollbar": { display: "none" },
        },
        ".no-tap-highlight": {
          "-webkit-tap-highlight-color": "transparent",
        },
      });

      // Add dynamic utilities (with value scale)
      matchUtilities(
        {
          "text-shadow": (value) => ({
            textShadow: value,
          }),
        },
        {
          values: theme("textShadow"),
          // Requires theme.extend.textShadow in config
        },
      );
    }),
  ],
};
```

---

## 10. Safelist — Ensuring Dynamic Classes Are Generated

When class names are built dynamically at runtime (e.g., from a database value), Tailwind can't scan them. The safelist ensures those classes are always generated.

```js
module.exports = {
  content: ["./src/**/*.{js,ts,jsx,tsx}"],

  safelist: [
    // String — always generate these exact classes
    "bg-red-500",
    "bg-green-500",
    "text-center",

    // Pattern — generate all matching classes
    {
      pattern: /bg-(red|green|blue|yellow)-(100|200|500|700)/,
    },
    {
      pattern: /text-(sm|base|lg|xl)/,
      variants: ["hover", "md", "lg"], // also generate hover:text-sm, md:text-sm, etc.
    },
  ],
};
```

```tsx
// Use case: status badge colours from an API
const statusColors = {
  pending:  'bg-yellow-100 text-yellow-800',
  active:   'bg-green-100 text-green-800',
  inactive: 'bg-gray-100 text-gray-800',
  error:    'bg-red-100 text-red-800',
} as const;

// These full strings are in source code → Tailwind finds them → no safelist needed
<span className={statusColors[item.status]}>

// But if the object comes from a database, safelist is required.
```

---

## 11. Component Extraction Patterns

### Pattern 1 — React Component (preferred)

```tsx
// Button.tsx — the utility classes live in one place
interface ButtonProps {
  variant?: "primary" | "secondary" | "ghost";
  size?: "sm" | "md" | "lg";
  children: React.ReactNode;
  className?: string;
}

const variantClasses = {
  primary: "bg-blue-600 text-white hover:bg-blue-700",
  secondary: "bg-white text-gray-900 border border-gray-300 hover:bg-gray-50",
  ghost: "text-gray-700 hover:bg-gray-100 hover:text-gray-900",
};

const sizeClasses = {
  sm: "px-3 py-1.5 text-sm",
  md: "px-4 py-2 text-sm",
  lg: "px-6 py-3 text-base",
};

export function Button({
  variant = "primary",
  size = "md",
  className,
  ...props
}: ButtonProps) {
  return (
    <button
      className={cn(
        // Base styles always applied
        "inline-flex items-center justify-center rounded-md font-medium",
        "transition-colors focus-visible:outline-none",
        "focus-visible:ring-2 focus-visible:ring-blue-500 focus-visible:ring-offset-2",
        "disabled:pointer-events-none disabled:opacity-50",
        // Variant styles
        variantClasses[variant],
        // Size styles
        sizeClasses[size],
        // Allow caller to override
        className,
      )}
      {...props}
    />
  );
}
```

### Pattern 2 — Class Variance Authority (cva)

```bash
npm install class-variance-authority
```

```tsx
import { cva, type VariantProps } from 'class-variance-authority';

const button = cva(
  // Base classes (always applied)
  'inline-flex items-center justify-center rounded-md font-medium transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-blue-500 disabled:opacity-50',
  {
    variants: {
      variant: {
        primary:   'bg-blue-600 text-white hover:bg-blue-700',
        secondary: 'border border-gray-300 bg-white hover:bg-gray-50',
        ghost:     'hover:bg-gray-100 hover:text-gray-900',
        destructive: 'bg-red-600 text-white hover:bg-red-700',
      },
      size: {
        sm:  'h-8 px-3 text-xs',
        md:  'h-9 px-4 text-sm',
        lg:  'h-11 px-8 text-base',
        icon:'h-9 w-9',
      },
    },
    defaultVariants: {
      variant: 'primary',
      size: 'md',
    },
  }
);

type ButtonProps = React.ButtonHTMLAttributes<HTMLButtonElement>
  & VariantProps<typeof button>
  & { className?: string };

export function Button({ className, variant, size, ...props }: ButtonProps) {
  return (
    <button className={cn(button({ variant, size }), className)} {...props} />
  );
}

// Usage
<Button variant="secondary" size="lg">Click</Button>
<Button variant="destructive">Delete</Button>
```

---

## 12. The cn() Utility Pattern

```bash
npm install clsx tailwind-merge
```

```ts
// lib/utils.ts — add this once, import everywhere
import { clsx, type ClassValue } from "clsx";
import { twMerge } from "tailwind-merge";

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}
```

```tsx
// What cn() does:
// clsx: combines class strings, handles conditional arrays/objects
// twMerge: resolves Tailwind conflicts (last one wins, deduplication)

cn('px-4 py-2', 'px-6')                  // → 'py-2 px-6'  (conflict resolved)
cn('text-sm', isLarge && 'text-lg')       // → 'text-sm' or 'text-lg'
cn({ 'bg-red-500': isError, 'bg-green-500': isSuccess })

// Component that accepts className overrides
function Card({ className, ...props }) {
  return (
    <div
      className={cn('bg-white rounded-xl shadow p-6', className)}
      {...props}
    />
  );
}

// Caller can override individual utilities
<Card className="p-0 shadow-none">   // p-6 and shadow are replaced cleanly
```

---

## 13. Tailwind v4 — CSS-First Config (the Current Default)

Tailwind v4 is not an incremental update — it's a rewrite of the engine (from a JavaScript/PostCSS pipeline to **Oxide**, written in Rust) and a full move of configuration from JS into CSS. As of 2026, **v4 is the default for any new project**; there's no real reason to start on v3 unless you need to support very old browsers (v4 targets Safari 16.4+, Chrome 111+, Firefox 128+).

### The `@theme` Directive — Full Shape

```css
/* app.css — this file replaces tailwind.config.js entirely */
@import "tailwindcss";

@theme {
  /* Colours — every --color-* becomes both a utility AND a real CSS variable */
  --color-brand-50: #eff6ff;
  --color-brand-500: #3b82f6;
  --color-brand-900: #1e3a8a;

  /* Fonts */
  --font-sans: "Inter", sans-serif;
  --font-display: "Playfair Display", serif;

  /* Spacing — extends the default scale, same idea as theme.extend.spacing */
  --spacing-13: 3.25rem;
  --spacing-128: 32rem;

  /* Breakpoints — replaces theme.screens */
  --breakpoint-xs: 30rem;
  --breakpoint-3xl: 120rem;

  /* Border radius, shadows, etc. all follow the same --category-name pattern */
  --radius-4xl: 2rem;
}
```

```css
/* Because every @theme value is a real CSS variable, you can use it
   directly in plain CSS too — not just inside Tailwind class names */
.custom-element {
  color: var(--color-brand-500);
  font-family: var(--font-display);
}
```

```
Reading the pattern:
  --color-{name}       → bg-{name}, text-{name}, border-{name}, etc.
  --spacing-{name}     → p-{name}, m-{name}, w-{name}, gap-{name}, etc.
  --breakpoint-{name}  → {name}: responsive prefix (e.g. xs:flex)
  --font-{name}        → font-{name}
  --radius-{name}      → rounded-{name}
```

### Replacing vs Extending in v4

v3's extend-vs-replace distinction (Section 2) still exists, but it's controlled by _namespace_, not by a separate `extend` key:

```css
@theme {
  /* This ADDS a new colour — the defaults (red, blue, gray, etc.) still exist */
  --color-brand-500: #3b82f6;
}

@theme {
  /* To REPLACE the entire default palette, clear the namespace first */
  --color-*: initial;
  --color-brand-500: #3b82f6;
  --color-white: #ffffff;
  --color-black: #000000;
  /* Now ONLY these colours exist — same effect as v3's theme.colors (no extend) */
}
```

### Content Detection — Automatic

```css
/* No content: [...] array needed for standard project layouts.
   Tailwind scans your project automatically, respecting .gitignore. */

/* For files outside the normal scan path (e.g. a shared package
   in a monorepo), add them explicitly: */
@source "../../packages/ui/src/**/*.tsx";

/* To exclude a path Tailwind would otherwise scan: */
@source not "./src/legacy/**";
```

### Plugins and `@apply` — Mostly Unchanged

```css
@import "tailwindcss";
@plugin "@tailwindcss/forms";
@plugin "@tailwindcss/typography";

/* @apply still works exactly as in v3 */
.prose h2 {
  @apply text-2xl font-bold text-gray-900 mt-8 mb-4;
}
```

### Breaking Changes to Expect When Migrating

These are the ones that actually bite in a real migration — not a exhaustive changelog, just the ones worth knowing before you start:

| What changed                    | v3                            | v4                                                                                                                              | Fix                                                                         |
| ------------------------------- | ----------------------------- | ------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------- |
| Gradient utilities renamed      | `bg-gradient-to-r`            | `bg-linear-to-r`                                                                                                                | Find-and-replace `gradient-to-` → `linear-to-`                              |
| Default border colour           | `gray-200`                    | `currentColor`                                                                                                                  | Add explicit colours where borders now look wrong: `border border-gray-200` |
| `flex-shrink-0` / `flex-grow-0` | supported                     | renamed                                                                                                                         | Use `shrink-0` / `grow-0`                                                   |
| Default ring width/colour       | `blue-500`, 3px               | `currentColor`, 1px                                                                                                             | Set `ring-2 ring-blue-500` explicitly if you relied on the old default      |
| Config file                     | `tailwind.config.js` required | Optional — CSS-first via `@theme`                                                                                               | Use the official upgrade tool (below) rather than hand-migrating            |
| Default dark mode strategy      | `media` unless configured     | Still `media` by default, but the `class` strategy now needs an explicit `@variant`: `@variant dark (&:where(.dark, .dark *));` | Add that one line if you use class-based dark mode toggling                 |

### Migration Path

```bash
# The official codemod handles most of the mechanical work —
# dependency updates, config-to-CSS conversion, class renames
npx @tailwindcss/upgrade

# Manual setup, if you'd rather migrate by hand
npm uninstall tailwindcss postcss autoprefixer
npm install tailwindcss @tailwindcss/vite   # or @tailwindcss/postcss
```

```
1. Run the upgrade tool, or start replacing @tailwind directives with @import "tailwindcss"
2. Move theme.extend values into an @theme block, one namespace at a time
3. Fix the breaking changes above (gradient names, border colour, ring defaults)
4. Delete tailwind.config.js once nothing references it (or keep select
   plugins alive during transition via @config "./tailwind.config.js";)
5. Re-check any custom plugin code — the plugin API is mostly compatible,
   but matchUtilities-heavy plugins are worth testing carefully
```

If you inherit or maintain a v3 codebase, none of this is urgent — v3 is still maintained and plenty of production apps run it fine. Migrate opportunistically (during an unrelated refactor) rather than as its own project, unless the build-speed gains from Oxide are actually a pain point for you day to day.
