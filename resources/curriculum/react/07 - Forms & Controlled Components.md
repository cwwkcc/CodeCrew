> Forms are where React's model of "UI as a function of state" meets the real world. HTML forms have their own internal state. React has its own state. Controlled components are the pattern for making React own the form state — making forms predictable, validatable, and easy to work with.

---

## Table of Contents

1. [Uncontrolled vs Controlled Components](#1-uncontrolled-vs-controlled-components)
2. [Controlled Input Patterns](#2-controlled-input-patterns)
3. [All Input Types](#3-all-input-types)
4. [Form Submission](#4-form-submission)
5. [Validation](#5-validation)
6. [Multi-Field Forms](#6-multi-field-forms)
7. [Dynamic Forms](#7-dynamic-forms)
8. [File Inputs](#8-file-inputs)
9. [When to Use Libraries](#9-when-to-use-libraries)

---

## 1. Uncontrolled vs Controlled Components

**Uncontrolled**: The DOM manages the form state. You read the value when you need it (via `ref`).

**Controlled**: React state manages the form value. Every keystroke updates state, state drives the displayed value.

```jsx
// Uncontrolled — DOM owns the value
function UncontrolledInput() {
  const inputRef = useRef(null);

  function handleSubmit() {
    console.log(inputRef.current.value); // read value on demand
  }

  return (
    <>
      <input ref={inputRef} defaultValue="initial" />
      <button onClick={handleSubmit}>Submit</button>
    </>
  );
}

// Controlled — React owns the value
function ControlledInput() {
  const [value, setValue] = useState("initial");

  return (
    <>
      <input
        value={value} // React sets the displayed value
        onChange={(e) => setValue(e.target.value)} // React updates state on every keystroke
      />
      <p>Current: {value}</p> // always in sync — no need to read from DOM
    </>
  );
}
```

**Use controlled components** in almost all cases. They let you:

- Validate input as the user types
- Conditionally disable submit based on current values
- Programmatically clear or pre-fill the form
- Implement dependent fields (field B changes based on field A's value)

**Use uncontrolled** when you need: file inputs (always), or integrating with non-React code that needs to own the DOM.

---

## 2. Controlled Input Patterns

The controlled pattern: `value={state}` + `onChange={updateState}`.

```jsx
// Text input
const [name, setName] = useState("");
<input
  type="text"
  value={name}
  onChange={e => setName(e.target.value)}
/>

// Without onChange handler — READ-ONLY (intentional)
<input value={name} readOnly />

// Without value prop — UNCONTROLLED (not what you usually want)
<input onChange={e => setName(e.target.value)} />
// React warning: changing from uncontrolled to controlled
```

### The undefined/null gotcha

```jsx
// ✗ — if value is undefined or null, React treats it as uncontrolled
const [name, setName] = useState();  // undefined by default!
<input value={name} onChange={...} />
// Warning: A component is changing an uncontrolled input to be controlled

// ✓ — always initialise to a string
const [name, setName] = useState("");  // empty string, not undefined
```

---

## 3. All Input Types

### Text, email, password, number, search

```jsx
// Standard pattern — same for all text-like inputs
const [email, setEmail] = useState("");
<input
  type="email"
  value={email}
  onChange={(e) => setEmail(e.target.value)}
  placeholder="Enter your email"
  autoComplete="email"
/>;

// Number input — value is always a string from e.target.value
const [age, setAge] = useState("");
<input
  type="number"
  value={age}
  onChange={(e) => setAge(e.target.value)} // "17" as string
  min={1}
  max={100}
/>;
// Parse when you use it: Number(age) or parseInt(age, 10)
```

### Textarea

```jsx
// Textarea in React — self-closing, value not as content
const [bio, setBio] = useState("");
<textarea
  value={bio}
  onChange={(e) => setBio(e.target.value)}
  rows={5}
  placeholder="Tell us about yourself..."
/>;
// Note: <textarea>content here</textarea> works in HTML but not in React
// In React, use value prop
```

### Select (dropdown)

```jsx
const [grade, setGrade] = useState(""); // or "11" for a default

<select value={grade} onChange={(e) => setGrade(e.target.value)}>
  <option value="">Select a grade</option>
  {[6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
    <option key={g} value={String(g)}>
      Grade {g}
    </option>
  ))}
</select>;

// Multi-select — value is an array
const [selectedSubjects, setSelectedSubjects] = useState([]);
<select
  multiple
  value={selectedSubjects}
  onChange={(e) => {
    const selected = Array.from(e.target.selectedOptions, (opt) => opt.value);
    setSelectedSubjects(selected);
  }}
>
  <option value="math">Mathematics</option>
  <option value="science">Science</option>
  <option value="english">English</option>
</select>;
```

### Checkbox

```jsx
// Single checkbox — use checked, not value
const [agreed, setAgreed] = useState(false);
<input
  type="checkbox"
  checked={agreed}
  onChange={e => setAgreed(e.target.checked)}  // e.target.checked, not value
  id="terms"
/>
<label htmlFor="terms">I agree to the terms</label>

// Multiple checkboxes — maintain a Set or array of selected values
const [selectedDays, setSelectedDays] = useState(new Set());

function toggleDay(day) {
  setSelectedDays(prev => {
    const next = new Set(prev);
    if (next.has(day)) next.delete(day);
    else next.add(day);
    return next;
  });
}

{["Mon", "Tue", "Wed", "Thu", "Fri"].map(day => (
  <label key={day}>
    <input
      type="checkbox"
      checked={selectedDays.has(day)}
      onChange={() => toggleDay(day)}
    />
    {day}
  </label>
))}
```

### Radio buttons

```jsx
const [role, setRole] = useState("student");

{
  ["student", "teacher", "parent"].map((r) => (
    <label key={r}>
      <input
        type="radio"
        name="role" // same name groups them
        value={r}
        checked={role === r} // controlled: compare to state
        onChange={(e) => setRole(e.target.value)}
      />
      {r.charAt(0).toUpperCase() + r.slice(1)}
    </label>
  ));
}
```

### Range slider

```jsx
const [volume, setVolume] = useState(50);
<input
  type="range"
  min={0}
  max={100}
  step={1}
  value={volume}
  onChange={e => setVolume(Number(e.target.value))}
/>
<span>{volume}%</span>
```

---

## 4. Form Submission

```jsx
function ContactForm() {
  const [formData, setFormData] = useState({
    name: "",
    email: "",
    message: "",
  });
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [submitted, setSubmitted] = useState(false);

  async function handleSubmit(e) {
    e.preventDefault(); // ALWAYS — prevents page reload

    setIsSubmitting(true);
    try {
      await submitContactForm(formData);
      setSubmitted(true);
    } catch (err) {
      alert("Submission failed: " + err.message);
    } finally {
      setIsSubmitting(false);
    }
  }

  if (submitted) {
    return <SuccessMessage />;
  }

  return (
    <form onSubmit={handleSubmit}>
      <input
        name="name"
        value={formData.name}
        onChange={(e) =>
          setFormData((prev) => ({ ...prev, name: e.target.value }))
        }
        required
      />
      <input
        name="email"
        type="email"
        value={formData.email}
        onChange={(e) =>
          setFormData((prev) => ({ ...prev, email: e.target.value }))
        }
        required
      />
      <textarea
        name="message"
        value={formData.message}
        onChange={(e) =>
          setFormData((prev) => ({ ...prev, message: e.target.value }))
        }
        required
      />
      <button type="submit" disabled={isSubmitting}>
        {isSubmitting ? "Sending..." : "Send"}
      </button>
    </form>
  );
}
```

---

## 5. Validation

### Validation on submit

```jsx
function RegistrationForm() {
  const [form, setForm] = useState({ name: "", email: "", password: "" });
  const [errors, setErrors] = useState({});

  function validate(data) {
    const errs = {};
    if (!data.name.trim()) errs.name = "Name is required";
    else if (data.name.length < 2)
      errs.name = "Name must be at least 2 characters";

    if (!data.email.trim()) errs.email = "Email is required";
    else if (!data.email.includes("@")) errs.email = "Email is invalid";

    if (!data.password) errs.password = "Password is required";
    else if (data.password.length < 8)
      errs.password = "Password must be at least 8 characters";

    return errs;
  }

  function handleSubmit(e) {
    e.preventDefault();
    const errs = validate(form);
    setErrors(errs);
    if (Object.keys(errs).length > 0) return; // stop if errors
    submitForm(form);
  }

  return (
    <form onSubmit={handleSubmit}>
      <div className="field">
        <input
          value={form.name}
          onChange={(e) => setForm((p) => ({ ...p, name: e.target.value }))}
          aria-invalid={!!errors.name}
          aria-describedby={errors.name ? "name-error" : undefined}
        />
        {errors.name && (
          <p id="name-error" className="error" role="alert">
            {errors.name}
          </p>
        )}
      </div>
      {/* ... other fields */}
    </form>
  );
}
```

### Validation on blur (when field loses focus)

```jsx
function ValidatedInput({ name, label, type = "text", validate }) {
  const [value, setValue] = useState("");
  const [error, setError] = useState("");
  const [touched, setTouched] = useState(false);

  function handleBlur() {
    setTouched(true);
    const err = validate(value);
    setError(err ?? "");
  }

  function handleChange(e) {
    setValue(e.target.value);
    if (touched) {
      // Validate live once the field has been touched
      const err = validate(e.target.value);
      setError(err ?? "");
    }
  }

  return (
    <div className="field">
      <label htmlFor={name}>{label}</label>
      <input
        id={name}
        name={name}
        type={type}
        value={value}
        onChange={handleChange}
        onBlur={handleBlur}
        className={error ? "input-error" : ""}
      />
      {error && <p className="error">{error}</p>}
    </div>
  );
}
```

---

## 6. Multi-Field Forms

### Generic onChange handler

```jsx
function StudentForm() {
  const [form, setForm] = useState({
    firstName: "",
    lastName: "",
    grade: "",
    email: "",
    phone: "",
  });

  // One handler for all text fields — uses input's name attribute
  function handleChange(e) {
    const { name, value, type, checked } = e.target;
    setForm((prev) => ({
      ...prev,
      [name]: type === "checkbox" ? checked : value,
    }));
  }

  return (
    <form>
      <input name="firstName" value={form.firstName} onChange={handleChange} />
      <input name="lastName" value={form.lastName} onChange={handleChange} />
      <input
        name="email"
        value={form.email}
        onChange={handleChange}
        type="email"
      />
      <input
        name="phone"
        value={form.phone}
        onChange={handleChange}
        type="tel"
      />
      <select name="grade" value={form.grade} onChange={handleChange}>
        {[6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
          <option key={g} value={g}>
            Grade {g}
          </option>
        ))}
      </select>
    </form>
  );
}
```

---

## 7. Dynamic Forms

Forms where fields can be added or removed.

```jsx
function QualificationsForm() {
  const [qualifications, setQualifications] = useState([
    { id: crypto.randomUUID(), subject: "", grade: "" },
  ]);

  function addQualification() {
    setQualifications((prev) => [
      ...prev,
      { id: crypto.randomUUID(), subject: "", grade: "" },
    ]);
  }

  function removeQualification(id) {
    setQualifications((prev) => prev.filter((q) => q.id !== id));
  }

  function updateQualification(id, field, value) {
    setQualifications((prev) =>
      prev.map((q) => (q.id === id ? { ...q, [field]: value } : q)),
    );
  }

  return (
    <div>
      {qualifications.map((qual) => (
        <div key={qual.id} className="qualification-row">
          <input
            placeholder="Subject"
            value={qual.subject}
            onChange={(e) =>
              updateQualification(qual.id, "subject", e.target.value)
            }
          />
          <select
            value={qual.grade}
            onChange={(e) =>
              updateQualification(qual.id, "grade", e.target.value)
            }
          >
            <option value="">Grade</option>
            {["A", "B", "C", "S", "F"].map((g) => (
              <option key={g} value={g}>
                {g}
              </option>
            ))}
          </select>
          <button
            type="button"
            onClick={() => removeQualification(qual.id)}
            disabled={qualifications.length === 1}
          >
            Remove
          </button>
        </div>
      ))}
      <button type="button" onClick={addQualification}>
        + Add Subject
      </button>
    </div>
  );
}
```

---

## 8. File Inputs

File inputs are always **uncontrolled** — you can't set `value` on them (security restriction). Use a `ref` or read from `e.target.files`.

```jsx
function FileUpload({ onUpload }) {
  const [preview, setPreview] = useState(null);
  const [isDragging, setIsDragging] = useState(false);

  function handleFileChange(e) {
    const file = e.target.files[0];
    if (!file) return;

    // Show preview for images
    if (file.type.startsWith("image/")) {
      const reader = new FileReader();
      reader.onload = (e) => setPreview(e.target.result);
      reader.readAsDataURL(file);
    }

    onUpload(file);
  }

  function handleDrop(e) {
    e.preventDefault();
    setIsDragging(false);
    const file = e.dataTransfer.files[0];
    if (file) handleFileChange({ target: { files: [file] } });
  }

  return (
    <div
      className={`upload-zone ${isDragging ? "dragging" : ""}`}
      onDragOver={(e) => {
        e.preventDefault();
        setIsDragging(true);
      }}
      onDragLeave={() => setIsDragging(false)}
      onDrop={handleDrop}
    >
      {preview ? (
        <img src={preview} alt="Preview" className="preview" />
      ) : (
        <p>Drag & drop or click to upload</p>
      )}
      <input
        type="file"
        accept="image/*,.pdf"
        onChange={handleFileChange}
        className="file-input-hidden"
      />
    </div>
  );
}
```

---

## 9. When to Use Libraries

For simple forms, vanilla React state is fine. For complex forms with many fields, cross-field validation, and async submission, use a library.

**React Hook Form** (covered in File 17) is the industry standard:

- Minimal re-renders — inputs are uncontrolled internally, controlled externally via `register`
- Built-in validation with schema libraries (Zod, Yup)
- Handles arrays of fields, nested objects, async validation

```jsx
// Taste of React Hook Form — full coverage in File 17
import { useForm } from "react-hook-form";
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";

const schema = z.object({
  name: z.string().min(2, "Name must be at least 2 characters"),
  email: z.string().email("Invalid email"),
});

function Form() {
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm({
    resolver: zodResolver(schema),
  });

  return (
    <form onSubmit={handleSubmit((data) => console.log(data))}>
      <input {...register("name")} />
      {errors.name && <p>{errors.name.message}</p>}

      <input {...register("email")} />
      {errors.email && <p>{errors.email.message}</p>}

      <button type="submit">Submit</button>
    </form>
  );
}
```

---

## Summary

```
Controlled vs Uncontrolled:
  Controlled: value={state} + onChange={setState} — React owns the value
  Uncontrolled: ref or defaultValue — DOM owns the value
  Use controlled in almost all cases

The controlled pattern:
  value={state}                    — what's displayed
  onChange={e => setState(e.target.value)} — how state updates

Never undefined/null as value:
  useState("") not useState()      — otherwise React warns/breaks

Input types:
  text/email/password/search  → value + onChange (e.target.value)
  textarea                    → value + onChange, self-closing
  select                      → value + onChange (e.target.value)
  checkbox                    → checked + onChange (e.target.checked)
  radio                       → checked={state === value} + onChange
  file                        → always uncontrolled, use e.target.files

Form submission:
  Always e.preventDefault() on onSubmit
  Disable submit button while submitting
  Handle loading, success, and error states

Validation:
  On submit: validate entire form, setErrors, stop if errors
  On blur: validate field when it loses focus (better UX)
  On change after touched: live validation once field was touched

Multi-field:
  Generic handler: use input name attribute → setForm(p => ({...p, [name]: value}))
  Dynamic fields: use array of objects with stable IDs (not index)

Libraries:
  React Hook Form (File 17) — for complex forms
  Minimal re-renders, schema validation (Zod), field arrays
```

---

_Next: [08 — useEffect & Side Effects](./08%20-%20useEffect%20%26%20Side%20Effects.md)_
