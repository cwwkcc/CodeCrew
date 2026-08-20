> For complex forms, vanilla React state doesn't scale — re-renders on every keystroke, manual validation logic, repetitive field handling. React Hook Form solves the re-render problem. Zod solves the validation logic problem. Together they're the industry standard for production forms.

---

## Table of Contents

1. [Why React Hook Form](#1-why-react-hook-form)
2. [Setup](#2-setup)
3. [Basic Form](#3-basic-form)
4. [Zod Schema Validation](#4-zod-schema-validation)
5. [All Input Types with RHF](#5-all-input-types-with-rhf)
6. [Watching Fields and Dependencies](#6-watching-fields-and-dependencies)
7. [Field Arrays — Dynamic Fields](#7-field-arrays--dynamic-fields)
8. [Async Validation](#8-async-validation)
9. [Integration with UI Libraries](#9-integration-with-ui-libraries)
10. [Form Submission with TanStack Query](#10-form-submission-with-tanstack-query)

---

## 1. Why React Hook Form

**The re-render problem:** In a controlled form with `useState`, every keystroke re-renders the entire form. For a form with 20 fields, that's 20 components re-rendering on every keystroke.

RHF is **uncontrolled by default** — form inputs are managed by the DOM, not React state. React only gets involved on blur (validation) and submit. Result: near-zero re-renders while typing.

```
Controlled form (useState):      React Hook Form:
  Keystroke → setState            Keystroke → (nothing — DOM handles it)
  → re-render entire form         Blur → validate that field
  → reconciliation                Submit → validate all, get values

  20 fields × 10 chars = 200 renders   20 fields × 10 chars = 0 renders
```

---

## 2. Setup

```bash
npm install react-hook-form @hookform/resolvers zod
```

---

## 3. Basic Form

```jsx
import { useForm } from "react-hook-form";

function SimpleForm() {
  const {
    register, // connect inputs to RHF
    handleSubmit, // wrap your submit handler
    formState: {
      errors, // validation errors
      isSubmitting, // true while onSubmit is running
      isDirty, // true if any field changed from default
      isValid, // true if no errors
    },
    reset, // reset to default values
    getValues, // read current values without re-render
    setValue, // programmatically set a field
    watch, // subscribe to field value changes
  } = useForm({
    defaultValues: {
      name: "",
      email: "",
      message: "",
    },
    mode: "onBlur", // when to validate: onChange | onBlur | onSubmit | all
  });

  async function onSubmit(data) {
    // data is typed and validated — only called if valid
    await submitForm(data);
    reset(); // clear the form on success
  }

  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <div>
        <label htmlFor="name">Name</label>
        <input
          id="name"
          {...register("name", {
            required: "Name is required",
            minLength: { value: 2, message: "At least 2 characters" },
          })}
        />
        {errors.name && <p className="error">{errors.name.message}</p>}
      </div>

      <div>
        <label htmlFor="email">Email</label>
        <input
          id="email"
          type="email"
          {...register("email", {
            required: "Email is required",
            pattern: {
              value: /^[^\s@]+@[^\s@]+\.[^\s@]+$/,
              message: "Invalid email address",
            },
          })}
        />
        {errors.email && <p className="error">{errors.email.message}</p>}
      </div>

      <button type="submit" disabled={isSubmitting}>
        {isSubmitting ? "Sending..." : "Send"}
      </button>
    </form>
  );
}
```

`{...register("fieldName", rules)}` spreads `name`, `ref`, `onChange`, and `onBlur` onto the input — connecting it to RHF.

---

## 4. Zod Schema Validation

Instead of writing validation rules inline (which gets messy fast), define them once in a Zod schema. Zod gives you:

- Full TypeScript types inferred from the schema
- Complex cross-field validation
- Reusable schemas across frontend and backend

```tsx
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { useForm } from "react-hook-form";

// Define the schema
const admissionsSchema = z.object({
  studentName: z.string().min(2, "Name must be at least 2 characters"),
  dateOfBirth: z.string().min(1, "Date of birth is required"),
  grade: z.number({ required_error: "Grade is required" }).min(1).max(13),
  parentName: z.string().min(2, "Parent name required"),
  parentEmail: z.string().email("Invalid email"),
  parentPhone: z.string().min(10, "Phone number must be at least 10 digits"),
  address: z.object({
    street: z.string().min(1, "Street is required"),
    city: z.string().min(1, "City is required"),
  }),
  previousSchool: z.string().optional(),
  documents: z.array(z.string()).min(1, "At least one document required"),
});

// TypeScript type inferred automatically
type AdmissionsForm = z.infer<typeof admissionsSchema>;
// { studentName: string, grade: number, parentEmail: string, ... }

function AdmissionsForm() {
  const {
    register,
    handleSubmit,
    formState: { errors, isSubmitting },
    control, // for Controller — custom/external inputs
  } = useForm<AdmissionsForm>({
    resolver: zodResolver(admissionsSchema), // connect Zod to RHF
    defaultValues: {
      grade: 6,
      documents: [],
    },
  });

  async function onSubmit(data: AdmissionsForm) {
    // data is fully typed and validated
    await submitAdmission(data);
  }

  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      <input {...register("studentName")} placeholder="Student's full name" />
      {errors.studentName && <p>{errors.studentName.message}</p>}

      <input
        {...register("parentEmail")}
        type="email"
        placeholder="Parent's email"
      />
      {errors.parentEmail && <p>{errors.parentEmail.message}</p>}

      {/* Nested field */}
      <input {...register("address.city")} placeholder="City" />
      {errors.address?.city && <p>{errors.address.city.message}</p>}

      <button type="submit" disabled={isSubmitting}>
        Submit Application
      </button>
    </form>
  );
}
```

### Cross-field validation with Zod

```typescript
const passwordSchema = z
  .object({
    password: z.string().min(8, "At least 8 characters"),
    confirmPassword: z.string(),
  })
  .refine((data) => data.password === data.confirmPassword, {
    message: "Passwords don't match",
    path: ["confirmPassword"], // error appears on confirmPassword field
  });

const dateRangeSchema = z
  .object({
    startDate: z.string(),
    endDate: z.string(),
  })
  .refine((data) => new Date(data.endDate) > new Date(data.startDate), {
    message: "End date must be after start date",
    path: ["endDate"],
  });
```

---

## 5. All Input Types with RHF

### Checkbox

```jsx
// Single checkbox — use register with valueAsBoolean or just use it directly
<input type="checkbox" {...register("agreedToTerms")} />
// value will be: true | false

// Zod: agreedToTerms: z.boolean().refine(v => v, "You must agree to the terms")
```

### Select

```jsx
<select {...register("grade", { valueAsNumber: true })}>
  <option value="">Select grade</option>
  {[6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
    <option key={g} value={g}>
      Grade {g}
    </option>
  ))}
</select>
// valueAsNumber: true — converts string "11" to number 11 automatically
```

### Radio buttons

```jsx
{
  ["Sinhala", "Tamil", "English"].map((lang) => (
    <label key={lang}>
      <input type="radio" value={lang} {...register("mediumOfInstruction")} />
      {lang}
    </label>
  ));
}
```

### Number input

```jsx
<input
  type="number"
  {...register("score", {
    valueAsNumber: true, // parse as number automatically
    min: { value: 0, message: "Score must be ≥ 0" },
    max: { value: 100, message: "Score must be ≤ 100" },
  })}
/>
```

### `Controller` — for custom inputs and UI library components

Some inputs (date pickers, select components, sliders) don't expose a native `ref`. Use `Controller` to integrate them.

```jsx
import { Controller } from "react-hook-form";

// Custom Select component (shadcn, Radix, etc.)
<Controller
  name="grade"
  control={control}
  render={({ field }) => (
    <Select
      value={field.value}
      onValueChange={field.onChange}
      onOpenChange={() => field.onBlur()}
    >
      <SelectTrigger>
        <SelectValue placeholder="Select grade" />
      </SelectTrigger>
      <SelectContent>
        {[6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
          <SelectItem key={g} value={g}>
            Grade {g}
          </SelectItem>
        ))}
      </SelectContent>
    </Select>
  )}
/>;
{
  errors.grade && <p>{errors.grade.message}</p>;
}

// Date picker (react-datepicker, etc.)
<Controller
  name="dateOfBirth"
  control={control}
  render={({ field }) => (
    <DatePicker
      selected={field.value ? new Date(field.value) : null}
      onChange={(date) => field.onChange(date?.toISOString())}
      onBlur={field.onBlur}
      placeholderText="Date of birth"
    />
  )}
/>;
```

---

## 6. Watching Fields and Dependencies

```jsx
const { watch, getValues } = useForm({ ... });

// Watch a single field — re-renders on every change to "grade"
const grade = watch("grade");

// Watch multiple fields
const [startDate, endDate] = watch(["startDate", "endDate"]);

// Watch all fields
const allValues = watch();

// Conditional rendering based on field value
function AdmissionsForm() {
  const { register, watch } = useForm();
  const isTransferStudent = watch("isTransferStudent");

  return (
    <form>
      <label>
        <input type="checkbox" {...register("isTransferStudent")} />
        Transfer student
      </label>

      {isTransferStudent && (
        <input
          {...register("previousSchool", { required: "Previous school is required" })}
          placeholder="Previous school name"
        />
      )}
    </form>
  );
}

// getValues — read values without causing a re-render
function handleSaveDraft() {
  const currentValues = getValues();
  saveDraft(currentValues);
}
```

---

## 7. Field Arrays — Dynamic Fields

```jsx
import { useForm, useFieldArray } from "react-hook-form";

function QualificationsForm() {
  const {
    register,
    control,
    handleSubmit,
    formState: { errors },
  } = useForm({
    defaultValues: {
      qualifications: [{ subject: "", grade: "", year: "" }],
    },
  });

  const { fields, append, remove, move } = useFieldArray({
    control,
    name: "qualifications",
  });

  return (
    <form onSubmit={handleSubmit(onSubmit)}>
      {fields.map((field, index) => (
        <div key={field.id}>
          {" "}
          {/* use field.id, NOT index as key */}
          <input
            {...register(`qualifications.${index}.subject`, {
              required: "Subject is required",
            })}
            placeholder="Subject"
          />
          {errors.qualifications?.[index]?.subject && (
            <p>{errors.qualifications[index].subject.message}</p>
          )}
          <select {...register(`qualifications.${index}.grade`)}>
            {["A", "B", "C", "S", "F"].map((g) => (
              <option key={g} value={g}>
                {g}
              </option>
            ))}
          </select>
          <input
            {...register(`qualifications.${index}.year`, {
              valueAsNumber: true,
            })}
            type="number"
            placeholder="Year"
          />
          <button
            type="button"
            onClick={() => remove(index)}
            disabled={fields.length === 1}
          >
            Remove
          </button>
        </div>
      ))}

      <button
        type="button"
        onClick={() =>
          append({ subject: "", grade: "A", year: new Date().getFullYear() })
        }
      >
        + Add Subject
      </button>

      <button type="submit">Submit</button>
    </form>
  );
}
```

---

## 8. Async Validation

```jsx
register("username", {
  validate: async (value) => {
    if (!value) return "Username is required";

    const isAvailable = await checkUsernameAvailability(value);
    if (!isAvailable) return "Username is already taken";

    return true; // return true (or no return) = valid
  },
});

// With debouncing — don't hit the API on every keystroke
import { useForm } from "react-hook-form";

// Use mode: "onBlur" to only validate when field loses focus
const { register } = useForm({ mode: "onBlur" });
```

---

## 9. Integration with UI Libraries

### shadcn/ui Form components

```tsx
// shadcn/ui has a Form component that integrates with RHF
import {
  Form,
  FormField,
  FormItem,
  FormLabel,
  FormControl,
  FormMessage,
} from "@/components/ui/form";
import { Input } from "@/components/ui/input";

function StudentForm() {
  const form = useForm<FormValues>({
    resolver: zodResolver(schema),
  });

  return (
    <Form {...form}>
      <form onSubmit={form.handleSubmit(onSubmit)}>
        <FormField
          control={form.control}
          name="studentName"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Student Name</FormLabel>
              <FormControl>
                <Input placeholder="Full name" {...field} />
              </FormControl>
              <FormMessage /> {/* automatically shows error */}
            </FormItem>
          )}
        />
        <button type="submit">Submit</button>
      </form>
    </Form>
  );
}
```

---

## 10. Form Submission with TanStack Query

```tsx
function AdmissionsForm() {
  const queryClient = useQueryClient();

  const form = useForm<AdmissionsFormData>({
    resolver: zodResolver(admissionsSchema),
  });

  const submitMutation = useMutation({
    mutationFn: submitAdmissions,
    onSuccess: (data) => {
      queryClient.invalidateQueries({ queryKey: ["applications"] });
      showToast("Application submitted successfully!", "success");
      form.reset();
    },
    onError: (error) => {
      // Map server validation errors back to form fields
      if (error.fieldErrors) {
        Object.entries(error.fieldErrors).forEach(([field, message]) => {
          form.setError(field as keyof AdmissionsFormData, { message });
        });
      } else {
        showToast(error.message, "error");
      }
    },
  });

  return (
    <Form {...form}>
      <form onSubmit={form.handleSubmit((data) => submitMutation.mutate(data))}>
        {/* fields */}
        <button
          type="submit"
          disabled={submitMutation.isPending || !form.formState.isValid}
        >
          {submitMutation.isPending ? "Submitting..." : "Submit Application"}
        </button>
      </form>
    </Form>
  );
}
```

---

## Summary

```
React Hook Form:
  Uncontrolled by default — near-zero re-renders while typing
  register("fieldName", rules) — connect input to RHF
  handleSubmit(onSubmit) — validates then calls onSubmit with typed data
  formState.errors — validation errors per field

Validation modes:
  mode: "onBlur"   — validate on field blur (best UX for most forms)
  mode: "onChange" — validate on every keystroke (for instant feedback)
  mode: "onSubmit" — validate only on submit (simplest)

Zod schema:
  zodResolver(schema) — connect to useForm
  z.object({ field: z.string().min(2, "message") })
  .refine() — cross-field validation
  z.infer<typeof schema> — TypeScript type from schema

Input types:
  Text, email, password  → register() directly
  Number                 → register("f", { valueAsNumber: true })
  Checkbox               → register() — value is boolean
  Select                 → register() with valueAsNumber if needed
  Custom/UI components   → Controller + render prop

Field arrays:
  useFieldArray({ control, name }) → { fields, append, remove, move }
  Use field.id as key (not index)
  Access: register("arr.${index}.field")

Watching:
  watch("field") — re-renders on change
  getValues() — read without re-render

With shadcn:
  <Form>, <FormField>, <FormItem>, <FormLabel>, <FormControl>, <FormMessage>
  FormMessage automatically shows error.message

With TanStack Query:
  mutationFn: submitAdmissions
  onSuccess: reset form, invalidate queries
  onError: form.setError() to map server errors to fields
```

---

_Next: [18 — Global State (Zustand)](<./18%20-%20Global%20State%20(Zustand).md>)_
