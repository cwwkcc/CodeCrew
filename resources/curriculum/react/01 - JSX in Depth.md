> JSX looks like HTML inside JavaScript. It isn't. Understanding what JSX actually is — and what it compiles to — removes all the confusion about why certain things work the way they do.

---

## Table of Contents

1. [What JSX Actually Is](#1-what-jsx-actually-is)
2. [JSX Compiles to Function Calls](#2-jsx-compiles-to-function-calls)
3. [JSX Expressions](#3-jsx-expressions)
4. [JSX vs HTML — The Differences](#4-jsx-vs-html--the-differences)
5. [Conditional Rendering in JSX](#5-conditional-rendering-in-jsx)
6. [Rendering Lists](#6-rendering-lists)
7. [JSX Fragments](#7-jsx-fragments)
8. [JSX Spread](#8-jsx-spread)
9. [Children](#9-children)
10. [JSX Gotchas](#10-jsx-gotchas)

---

## 1. What JSX Actually Is

JSX is **not** HTML. It's a syntax extension for JavaScript that looks like HTML. Your browser cannot understand JSX — a compiler (Babel or the TypeScript compiler) transforms it into plain JavaScript before it runs.

```jsx
// What you write (JSX):
const element = <h1 className="title">Hello, {name}</h1>;

// What the compiler produces (plain JS):
const element = React.createElement(
  "h1",
  { className: "title" },
  "Hello, ",
  name,
);
```

JSX is JavaScript. It follows JavaScript rules, not HTML rules. It can appear anywhere a JavaScript expression can appear — inside variables, function returns, ternaries, arrays.

---

## 2. JSX Compiles to Function Calls

`React.createElement(type, props, ...children)` creates a plain JavaScript object describing what to render. React calls these objects **React elements** (not DOM elements).

```jsx
// JSX
<button className="btn" onClick={handleClick} disabled={isLoading}>
  Submit
</button>

// Compiled to:
React.createElement(
  "button",
  {
    className: "btn",
    onClick: handleClick,
    disabled: isLoading,
  },
  "Submit"
)

// Which produces a plain JS object:
{
  type: "button",
  props: {
    className: "btn",
    onClick: handleClick,
    disabled: false,
    children: "Submit",
  },
  key: null,
  ref: null,
}
```

This object is not a DOM element. It's a lightweight description. React uses it to figure out what DOM elements to create or update. This is the virtual DOM.

With modern React (17+) and the new JSX transform, you don't need to `import React` just to use JSX — the compiler handles it automatically.

---

## 3. JSX Expressions

Anything inside `{}` in JSX is a JavaScript expression. It's evaluated and the result is rendered.

```jsx
const name = "Ashan";
const score = 91;
const student = { name: "Dineth", grade: 11 };

// Variables
<h1>{name}</h1>                      // "Ashan"

// Arithmetic
<p>Score: {score}/100</p>            // "Score: 91/100"
<p>Next year: {student.grade + 1}</p> // "Next year: 12"

// Method calls
<p>{name.toUpperCase()}</p>          // "ASHAN"
<p>{new Date().getFullYear()}</p>    // "2026"

// Ternary
<p>{score >= 75 ? "Distinction" : "Pass"}</p>

// Function calls
<p>{formatDate(new Date())}</p>

// Template literals (though usually unnecessary)
<p>{`${name} — Grade ${student.grade}`}</p>
```

**What can go inside `{}`:**

```jsx
// ✓ Valid — these render
{
  42;
} // number → "42"
{
  ("hello");
} // string → "hello"
{
  true;
} // boolean → nothing (booleans are not rendered)
{
  null;
} // null → nothing
{
  undefined;
} // undefined → nothing
{
  <span />;
} // JSX → rendered
{
  [1, 2, 3];
} // array → each item rendered

// ✗ Invalid — these cause errors or unexpected output
{
  objects;
} // Error: Objects are not valid as a React child
{
  {
    key: "val";
  }
} // Error: same reason
```

---

## 4. JSX vs HTML — The Differences

JSX looks like HTML but has important differences. These catch everyone at first.

### `className` not `class`

```jsx
// HTML:
<div class="container">

// JSX:
<div className="container">
```

`class` is a reserved keyword in JavaScript. JSX uses `className`.

### `htmlFor` not `for`

```jsx
// HTML:
<label for="email">Email</label>

// JSX:
<label htmlFor="email">Email</label>
```

### camelCase event handlers

```jsx
// HTML:
<button onclick="handleClick()">

// JSX:
<button onClick={handleClick}>
```

All event handlers are camelCase: `onClick`, `onChange`, `onSubmit`, `onKeyDown`, `onMouseEnter`, etc.

### camelCase style properties

```jsx
// HTML:
<div style="background-color: red; font-size: 16px;">

// JSX: style takes an OBJECT, properties are camelCase
<div style={{ backgroundColor: "red", fontSize: "16px" }}>
// Note the double {{ — outer {} is JSX expression, inner {} is JS object
```

### Self-closing tags — required for void elements

```jsx
// HTML (void elements don't need closing slash):
<input type="text">
<br>
<img src="...">

// JSX — ALL elements must be closed:
<input type="text" />
<br />
<img src="..." />
<MyComponent />   // custom components too
```

### One root element

```jsx
// ✗ Error — JSX must have one root element
return (
  <h1>Title</h1>
  <p>Content</p>
);

// ✓ Wrap in a container
return (
  <div>
    <h1>Title</h1>
    <p>Content</p>
  </div>
);

// ✓ Or use a Fragment (preferred — no extra DOM element)
return (
  <>
    <h1>Title</h1>
    <p>Content</p>
  </>
);
```

### Boolean attributes

```jsx
// HTML — presence of attribute = true
<input disabled>
<input checked>

// JSX — must be explicit, or use shorthand for true
<input disabled={true} />
<input disabled />          // shorthand for disabled={true}
<input disabled={false} />  // explicitly not disabled
<input disabled={isLoading} />  // conditional
```

### Comments in JSX

```jsx
// Inside JSX, JavaScript comments must be inside {}
return (
  <div>
    {/* This is a JSX comment */}
    <p>Content</p>
    {/* Multi-line
        comment */}
  </div>
);

// Outside JSX tags: normal JS comments work
// This is fine
const element = <div />;
```

---

## 5. Conditional Rendering in JSX

JSX is JavaScript, so you use JavaScript for conditionals — not special template syntax.

### Ternary operator

```jsx
// Short: ternary for two-way conditions
<div>
  {isLoggedIn ? <UserMenu /> : <LoginButton />}
</div>

// Multi-line ternary
<div>
  {isLoading
    ? <Spinner />
    : <Content data={data} />
  }
</div>
```

### `&&` operator — render or nothing

```jsx
// Render only when condition is true
<div>
  {isAdmin && <AdminPanel />}
  {errors.length > 0 && <ErrorList errors={errors} />}
  {user.isPremium && <PremiumBadge />}
</div>

// ⚠️ GOTCHA: if the left side is 0 (falsy number), 0 is RENDERED
const count = 0;
<div>{count && <Badge />}</div>
// Renders: <div>0</div>  ← the "0" appears in the UI!

// Fix: convert to boolean explicitly
<div>{count > 0 && <Badge />}</div>
<div>{!!count && <Badge />}</div>
<div>{Boolean(count) && <Badge />}</div>
```

### `if` statements — outside JSX

```jsx
// You can't use if/else directly inside JSX
// But you can compute the value before returning
function StatusBadge({ status }) {
  let content;
  let className;

  if (status === "active") {
    content = "Active";
    className = "badge-green";
  } else if (status === "pending") {
    content = "Pending";
    className = "badge-yellow";
  } else {
    content = "Inactive";
    className = "badge-grey";
  }

  return <span className={className}>{content}</span>;
}

// Or extract to a variable
function Alert({ type, message }) {
  const icon = type === "error" ? "❌" : type === "warning" ? "⚠️" : "ℹ️";

  return (
    <div className={`alert alert-${type}`}>
      <span>{icon}</span>
      <p>{message}</p>
    </div>
  );
}
```

---

## 6. Rendering Lists

Use `.map()` to transform an array of data into an array of JSX elements.

```jsx
function StudentList({ students }) {
  return (
    <ul>
      {students.map((student) => (
        <li key={student.id}>
          {student.name} — Grade {student.grade}
        </li>
      ))}
    </ul>
  );
}
```

### The `key` prop — required and important

Every element in a mapped list needs a **unique, stable `key` prop**. React uses keys to identify which items changed, were added, or were removed during reconciliation.

```jsx
// ✓ Use a unique, stable identifier (database ID is perfect)
students.map((s) => <StudentCard key={s.id} student={s} />);

// ✓ If no ID, use a naturally unique value
categories.map((c) => <CategoryTab key={c.slug} category={c} />);

// ✗ Never use array index as key (when list can reorder or change)
students.map((s, index) => <StudentCard key={index} student={s} />);
// Why: if you insert at the start, all keys shift → React remounts everything

// ✗ Never use random values
students.map((s) => <StudentCard key={Math.random()} student={s} />);
// Why: new key on every render → remount on every render → kills performance

// Index is acceptable ONLY when:
// - The list is static (never reordered, never items inserted/removed)
// - Items have no stable ID
const staticLinks = ["Home", "About", "Contact"];
staticLinks.map((link, i) => <NavLink key={i}>{link}</NavLink>);
```

### Keys must be unique among siblings, not globally

```jsx
// These two lists have overlapping key values — that's fine
// Keys only need to be unique within their own list
<ul>
  {posts.map(post => <li key={post.id}>{post.title}</li>)}
</ul>
<ul>
  {comments.map(comment => <li key={comment.id}>{comment.text}</li>)}
</ul>
```

---

## 7. JSX Fragments

Fragments let you return multiple elements without adding an extra DOM node.

```jsx
// Long syntax — use when you need to pass a key
import { Fragment } from "react";

function StudentRow({ student }) {
  return (
    <Fragment>
      <td>{student.name}</td>
      <td>{student.grade}</td>
      <td>{student.score}</td>
    </Fragment>
  );
}

// Short syntax — most common (no key support)
function StudentRow({ student }) {
  return (
    <>
      <td>{student.name}</td>
      <td>{student.grade}</td>
      <td>{student.score}</td>
    </>
  );
}

// Keyed fragments — when rendering a list of fragments
{
  students.map((student) => (
    <Fragment key={student.id}>
      <dt>{student.name}</dt>
      <dd>{student.score}</dd>
    </Fragment>
  ));
}
```

---

## 8. JSX Spread

Spread props from an object onto a component — useful for passing through props.

```jsx
const buttonProps = {
  type: "submit",
  disabled: isLoading,
  className: "btn btn-primary",
};

// Without spread
<button type={buttonProps.type} disabled={buttonProps.disabled} className={buttonProps.className}>
  Submit
</button>

// With spread
<button {...buttonProps}>Submit</button>

// Spread + override — later props win
<button {...buttonProps} className="btn btn-danger">
  Delete
</button>
// className is "btn btn-danger" (override wins)

// Common pattern: pass-through props
function Input({ label, error, ...inputProps }) {
  // inputProps contains everything except label and error
  return (
    <div>
      <label>{label}</label>
      <input {...inputProps} />  {/* spreads type, placeholder, value, onChange, etc. */}
      {error && <p className="error">{error}</p>}
    </div>
  );
}

<Input
  label="Email"
  error={errors.email}
  type="email"
  placeholder="your@email.com"
  value={email}
  onChange={e => setEmail(e.target.value)}
/>
```

---

## 9. Children

`children` is a special prop — it's whatever is between the opening and closing tags.

```jsx
// Using children
function Card({ children, title }) {
  return (
    <div className="card">
      <h2 className="card-title">{title}</h2>
      <div className="card-body">
        {children}
      </div>
    </div>
  );
}

// Passing children between tags
<Card title="Student Profile">
  <Avatar src={user.avatarUrl} />
  <p>{user.bio}</p>
  <Button>Edit Profile</Button>
</Card>

// Children can be anything
<Card title="Stats">Just text</Card>
<Card title="Empty" />           // children is undefined
<Card title="Multiple">
  <p>First</p>
  <p>Second</p>
  <p>Third</p>
</Card>

// Inspect children type
import { Children, isValidElement } from "react";

function List({ children }) {
  const count = Children.count(children);
  return (
    <ul>
      <p>{count} items</p>
      {Children.map(children, child => (
        <li>{child}</li>
      ))}
    </ul>
  );
}
```

---

## 10. JSX Gotchas

### Strings must be in quotes or expressions

```jsx
// ✓
<p>Hello world</p>          // plain text — fine
<p>{"Hello world"}</p>      // string expression — fine
<p>{`Hello ${name}`}</p>    // template literal — fine

// ✗ — JSX string escaping issues
<p>{"It's a string"}</p>    // apostrophes fine in JS strings
<p>It&apos;s a string</p>   // HTML entity (old-style)
```

### `null`, `undefined`, `false` render nothing

```jsx
// These render nothing — useful for conditional rendering
{
  null;
}
{
  undefined;
}
{
  false;
}
{
  true;
}

// But 0 renders as "0" — this is the common bug
{
  0;
} // renders "0" in the DOM
{
  0 && <Component />;
} // renders "0" not nothing!
```

### Objects cannot be rendered directly

```jsx
const user = { name: "Ashan", grade: 11 };

// ✗ Error: Objects are not valid as a React child
<p>{user}</p>

// ✓ Access properties
<p>{user.name}</p>
<p>{JSON.stringify(user)}</p>  // for debugging
```

### JSX is an expression — use it as one

```jsx
// JSX can be stored, passed, returned — it's just a value
const heading = <h1>Title</h1>;
const items = students.map((s) => <li key={s.id}>{s.name}</li>);

function renderStatus(status) {
  if (status === "loading") return <Spinner />;
  if (status === "error") return <ErrorMessage />;
  return <Content />;
}
```

---

## Summary

```
JSX compiles to:
  React.createElement(type, props, ...children)
  Produces a plain JS object — the "virtual DOM element"

JSX vs HTML:
  className not class
  htmlFor not for
  camelCase events: onClick, onChange, onSubmit
  camelCase style properties: backgroundColor, fontSize
  All tags must be closed: <br />, <img />, <MyComponent />
  One root element (or Fragment)
  Boolean shorthand: disabled={true} → disabled

Expressions in JSX:
  {} accepts any JS expression
  null, undefined, false → render nothing
  0 → renders "0" (common bug with &&)
  Objects → Error (access properties instead)

Lists:
  .map() to transform data → JSX
  key prop required: must be unique, stable — use database IDs
  Never use Math.random() or index (when list can change) as key

Fragments:
  <> </> — no extra DOM element
  <Fragment key={...}> — when you need a key

Spread:
  {...props} — spread an object as props
  Later props override earlier ones
  Useful for pass-through wrappers

Children:
  Whatever is between opening and closing tags
  Received as props.children
  Can be text, JSX, arrays, or nothing
```

---

_Next: [02 — Components & Props](./02%20-%20Components%20%26%20Props.md)_
