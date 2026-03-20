## The `<form>` Element

```html
<form action="/submit" method="post" novalidate>
  ...
</form>
```

| Attribute      | Values                | Purpose                                                                      |
| -------------- | --------------------- | ---------------------------------------------------------------------------- |
| `action`       | URL                   | Where to send form data on submission                                        |
| `method`       | `get` / `post`        | HTTP method. Use `post` for data that changes state or contains private info |
| `enctype`      | `multipart/form-data` | Required for file uploads                                                    |
| `novalidate`   | Boolean               | Disables browser built-in validation (for custom JS validation)              |
| `autocomplete` | `on` / `off`          | Allow/suppress browser autofill                                              |

---

## Labels and Inputs

**Every input must have a label.** No exceptions.

```html
<!-- Explicit association via for/id -->
<label for="username">Username</label>
<input type="text" id="username" name="username">

<!-- Implicit association (label wraps input) -->
<label>
  Username
  <input type="text" name="username">
</label>
```

The `for` attribute of `<label>` must match the `id` of the input. This creates the accessible name for the field and also makes the label clickable to focus the input.

---

## All `<input>` Types

```html
<!-- Text inputs -->
<input type="text">          <!-- Single-line text -->
<input type="password">      <!-- Masked text -->
<input type="email">         <!-- Email — validates format on mobile keyboards -->
<input type="url">           <!-- URL -->
<input type="search">        <!-- Search field — may show clear button -->
<input type="tel">           <!-- Phone number — triggers numeric keyboard on mobile -->

<!-- Numeric -->
<input type="number" min="0" max="100" step="5">
<input type="range"  min="0" max="100" step="1" value="50">

<!-- Date and time -->
<input type="date">
<input type="time">
<input type="datetime-local">
<input type="month">
<input type="week">

<!-- Choice -->
<input type="checkbox" name="agree" value="yes">
<input type="radio"    name="plan"  value="basic">

<!-- File -->
<input type="file" accept=".pdf,.docx" multiple>

<!-- Colour -->
<input type="color" value="#3a86ff">

<!-- Hidden (not shown, submitted with form) -->
<input type="hidden" name="csrf_token" value="abc123">

<!-- Buttons (prefer <button> over these) -->
<input type="submit"  value="Send">
<input type="reset"   value="Clear">
<input type="button"  value="Click me">
<input type="image"   src="submit.png" alt="Submit">
```

---

## The `<button>` Element

Prefer `<button>` over `<input type="submit">` — it can contain HTML:

```html
<button type="submit">Send Message</button>
<button type="reset">Clear Form</button>
<button type="button">Trigger Action</button> <!-- does nothing by default -->
```

`type="button"` prevents accidental form submission when nested inside a `<form>`.

---

## Common Input Attributes

|Attribute|Purpose|
|---|---|
|`name`|Key in the submitted form data — required for submission|
|`id`|For label association|
|`value`|Default or submitted value|
|`placeholder`|Hint text inside the field (not a label replacement)|
|`required`|Field must not be empty|
|`disabled`|Field is non-interactive and not submitted|
|`readonly`|Value shown but not editable; still submitted|
|`autofocus`|This field gets focus on page load|
|`autocomplete`|Browser autofill hint (e.g. `"email"`, `"given-name"`, `"new-password"`)|
|`pattern`|Regex validation pattern|
|`minlength` / `maxlength`|Character count limits|
|`min` / `max` / `step`|Numeric range constraints|
|`multiple`|Accept multiple values (file inputs, email)|

---

## `<textarea>`

Multi-line text input:

```html
<label for="bio">Biography</label>
<textarea id="bio" name="bio" rows="5" cols="40" maxlength="500">
  Default text here
</textarea>
```

Note: `<textarea>` is not a void element — it has a closing tag and its content is the default value.

---

## `<select>` and `<option>`

```html
<label for="country">Country</label>
<select id="country" name="country" required>
  <option value="">-- Select a country --</option>
  <option value="lk">Sri Lanka</option>
  <option value="in">India</option>
  <option value="sg" selected>Singapore</option>
</select>
```

`<optgroup>` groups options:

```html
<select name="car">
  <optgroup label="European">
    <option value="bmw">BMW</option>
    <option value="vw">Volkswagen</option>
  </optgroup>
  <optgroup label="Japanese">
    <option value="toyota">Toyota</option>
  </optgroup>
</select>
```

---

## `<datalist>` — Autocomplete Suggestions

Provides suggestions without restricting input:

```html
<label for="browser">Browser</label>
<input type="text" id="browser" name="browser" list="browser-list">
<datalist id="browser-list">
  <option value="Chrome">
  <option value="Firefox">
  <option value="Safari">
  <option value="Edge">
</datalist>
```

The `list` attribute on `<input>` links to the `<datalist>` by `id`.

---

## `<fieldset>` and `<legend>`

Groups related inputs, especially radio buttons and checkboxes:

```html
<fieldset>
  <legend>Preferred contact method</legend>
  <label><input type="radio" name="contact" value="email"> Email</label>
  <label><input type="radio" name="contact" value="phone"> Phone</label>
  <label><input type="radio" name="contact" value="post"> Post</label>
</fieldset>
```

`<legend>` is the accessible label for the group. Without it, screen readers announce each radio button without context.

---

## `<output>`

Displays the result of a calculation:

```html
<form oninput="result.value = parseInt(a.value) + parseInt(b.value)">
  <input type="number" id="a" name="a" value="0"> +
  <input type="number" id="b" name="b" value="0"> =
  <output name="result" for="a b">0</output>
</form>
```

---

## `<progress>` and `<meter>`

```html
<!-- Task progress (0–100%) -->
<label for="upload">Upload progress:</label>
<progress id="upload" value="72" max="100">72%</progress>

<!-- Scalar measurement within a range -->
<label for="disk">Disk usage:</label>
<meter id="disk" value="6" min="0" max="10" low="3" high="8" optimum="2">
  6 GB of 10 GB used
</meter>
```

`<progress>` is for completion; `<meter>` is for a measurement with a known range.