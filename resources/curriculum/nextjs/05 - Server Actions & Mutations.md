> Server Actions are async functions that run on the server and can be called directly from Client Components. They replace the pattern of "write a Client Component → call an API route → write a separate API handler" — you write one function, call it directly, and Next.js handles the HTTP round-trip. They're how you handle form submissions, mutations, and server-side side effects in App Router.

---

## Table of Contents

1. [What Server Actions Are](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#1-what-server-actions-are)
2. [Defining Server Actions](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#2-defining-server-actions)
3. [Calling Server Actions from Client Components](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#3-calling-server-actions-from-client-components)
4. [Form Actions](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#4-form-actions)
5. [useActionState — Pending and Error States](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#5-useactionstate--pending-and-error-states)
6. [useOptimistic — Instant UI Updates](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#6-useoptimistic--instant-ui-updates)
7. [Revalidating Data After Mutations](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#7-revalidating-data-after-mutations)
8. [Server Action Patterns](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#8-server-action-patterns)
9. [Validation with Zod](https://claude.ai/chat/5ef97483-862b-4ab3-af0d-39a5aafd519d#9-validation-with-zod)

---

## 1. What Server Actions Are

Before Server Actions, a mutation looked like this:

```
Client Component
  → calls fetch("/api/students", { method: "POST", body: ... })
    → app/api/students/route.ts (POST handler)
      → prisma.student.create(...)
        → returns JSON
    → Client re-fetches or updates local state
```

Three files, three concerns to keep in sync, manual serialisation on both ends.

With Server Actions:

```
Client Component
  → calls createStudent(formData)
    → function runs on server, calls prisma.student.create(...)
      → returns result directly
    → Client receives result
```

One function. The HTTP transport is handled by Next.js automatically.

```tsx
// actions/students.ts
"use server";

import { prisma } from "@/lib/prisma";
import { revalidatePath } from "next/cache";

export async function createStudent(formData: FormData) {
  const name = formData.get("name") as string;
  const grade = parseInt(formData.get("grade") as string);

  await prisma.student.create({ data: { name, grade } });

  revalidatePath("/students"); // tell Next.js to re-render /students
}
```

```tsx
// Client Component
"use client";

import { createStudent } from "@/actions/students";

export default function AddStudentForm() {
  return (
    <form action={createStudent}>
      <input name="name" type="text" placeholder="Student name" />
      <input name="grade" type="number" placeholder="Grade" />
      <button type="submit">Add Student</button>
    </form>
  );
}
```

No API route. No `fetch`. No JSON serialisation. The form's `action` prop calls the server function directly.

---

## 2. Defining Server Actions

### In a dedicated file (recommended)

```tsx
// actions/students.ts
"use server";
// All exports from this file are Server Actions

import { prisma } from "@/lib/prisma";
import { revalidatePath } from "next/cache";
import { redirect } from "next/navigation";

export async function createStudent(formData: FormData) { ... }
export async function updateStudent(id: number, formData: FormData) { ... }
export async function deleteStudent(id: number) { ... }
```

### Inline in a Server Component

```tsx
// app/[locale]/students/page.tsx — Server Component
export default async function StudentsPage() {
  // Inline Server Action — defined and used in the same file
  async function deleteStudent(formData: FormData) {
    "use server"; // directive on the individual function
    const id = parseInt(formData.get("id") as string);
    await prisma.student.delete({ where: { id } });
    revalidatePath("/students");
  }

  const students = await prisma.student.findMany();

  return (
    <ul>
      {students.map((s) => (
        <li key={s.id}>
          {s.name}
          <form action={deleteStudent}>
            <input type="hidden" name="id" value={s.id} />
            <button type="submit">Delete</button>
          </form>
        </li>
      ))}
    </ul>
  );
}
```

Inline actions are fine for simple cases. For anything reused across components, put it in a dedicated `actions/` file.

---

## 3. Calling Server Actions from Client Components

Two ways: via form `action` prop, or called directly as a function.

### Direct function call

```tsx
"use client";

import { useState, useTransition } from "react";
import { deleteStudent } from "@/actions/students";

export default function DeleteButton({ studentId }: { studentId: number }) {
  const [isPending, startTransition] = useTransition();

  function handleDelete() {
    startTransition(async () => {
      await deleteStudent(studentId);
    });
  }

  return (
    <button
      onClick={handleDelete}
      disabled={isPending}
      className="btn-destructive"
    >
      {isPending ? "Deleting..." : "Delete"}
    </button>
  );
}
```

`useTransition` marks the action as a non-urgent update — React keeps the UI interactive while the server request is in flight, and `isPending` tells you when to show a loading state.

### Binding arguments

Server Actions only receive `FormData` from `<form>`. For additional arguments, use `.bind()`:

```tsx
// actions/students.ts
"use server";

export async function updateGrade(studentId: number, formData: FormData) {
  const grade = parseInt(formData.get("grade") as string);
  await prisma.student.update({ where: { id: studentId }, data: { grade } });
  revalidatePath("/students");
}
```

```tsx
// Usage — bind studentId as the first argument
import { updateGrade } from "@/actions/students";

export default function GradeForm({ studentId }: { studentId: number }) {
  const updateGradeWithId = updateGrade.bind(null, studentId);
  //                                                 ↑ pre-fills the studentId argument

  return (
    <form action={updateGradeWithId}>
      <input name="grade" type="number" />
      <button type="submit">Update Grade</button>
    </form>
  );
}
```

---

## 4. Form Actions

The cleanest way to use Server Actions: pass one directly to a `<form>`'s `action` prop.

```tsx
// Works in both Server Components and Client Components
<form action={serverAction}>
  <input name="..." />
  <button type="submit">Submit</button>
</form>
```

This works without JavaScript — progressive enhancement. The form submits natively if JS hasn't loaded yet, and uses the optimised client-side path once JS is available.

### Multiple actions in one form

```tsx
// Separate form per action — cleanest approach
export default function StudentActions({ student }) {
  const archiveWithId = archiveStudent.bind(null, student.id);
  const promoteWithId = promoteStudent.bind(null, student.id);

  return (
    <div className="flex gap-2">
      <form action={archiveWithId}>
        <button type="submit">Archive</button>
      </form>
      <form action={promoteWithId}>
        <button type="submit">Promote</button>
      </form>
    </div>
  );
}
```

---

## 5. useActionState — Pending and Error States

`useActionState` (React 19 / Next.js 15) wires up a Server Action with pending state, error handling, and previous state in one hook.

```tsx
"use client";

import { useActionState } from "react";
import { createAnnouncement } from "@/actions/announcements";

// Server Action signature for useActionState:
// (prevState: State, formData: FormData) => Promise<State>
type State = {
  error?: string;
  success?: boolean;
};

export default function AnnouncementForm() {
  const [state, action, isPending] = useActionState<State, FormData>(
    createAnnouncement,
    {}, // initial state
  );

  return (
    <form action={action} className="space-y-4">
      <input
        name="title"
        type="text"
        placeholder="Announcement title"
        required
      />
      <textarea name="content" placeholder="Announcement content" required />

      {state.error && <p className="text-destructive text-sm">{state.error}</p>}
      {state.success && (
        <p className="text-success text-sm">Announcement published!</p>
      )}

      <button type="submit" disabled={isPending}>
        {isPending ? "Publishing..." : "Publish"}
      </button>
    </form>
  );
}
```

```tsx
// actions/announcements.ts
"use server";

import { prisma } from "@/lib/prisma";
import { revalidatePath } from "next/cache";

type State = { error?: string; success?: boolean };

export async function createAnnouncement(
  prevState: State,
  formData: FormData,
): Promise<State> {
  const title = formData.get("title") as string;
  const content = formData.get("content") as string;

  if (!title || !content) {
    return { error: "Title and content are required." };
  }

  try {
    await prisma.announcement.create({
      data: { title, content, publishedAt: new Date() },
    });
    revalidatePath("/");
    return { success: true };
  } catch {
    return { error: "Failed to publish. Please try again." };
  }
}
```

The Server Action returns a state object. `useActionState` makes that state available in the component. No separate `useState` for errors or success messages needed.

---

## 6. useOptimistic — Instant UI Updates

`useOptimistic` shows an immediate update in the UI while the server request is still in flight. If the server succeeds, the optimistic update becomes the real one. If it fails, the UI rolls back.

```tsx
"use client";

import { useOptimistic, useTransition } from "react";
import { toggleLike } from "@/actions/news";

type Article = { id: number; title: string; likeCount: number; liked: boolean };

export default function ArticleCard({ article }: { article: Article }) {
  const [isPending, startTransition] = useTransition();
  const [optimisticArticle, addOptimistic] = useOptimistic(
    article,
    (state, newLikedState: boolean) => ({
      ...state,
      liked: newLikedState,
      likeCount: newLikedState ? state.likeCount + 1 : state.likeCount - 1,
    }),
  );

  function handleLike() {
    startTransition(async () => {
      addOptimistic(!optimisticArticle.liked); // instant UI update
      await toggleLike(article.id); // actual server call
    });
  }

  return (
    <div className="article-card">
      <h2>{article.title}</h2>
      <button onClick={handleLike} disabled={isPending}>
        {optimisticArticle.liked ? "❤️" : "🤍"} {optimisticArticle.likeCount}
      </button>
    </div>
  );
}
```

The heart toggles instantly when clicked. If the server call fails, the UI reverts to the real `article` prop automatically.

---

## 7. Revalidating Data After Mutations

After a Server Action mutates data, you need to tell Next.js to re-render the affected pages. Otherwise users see stale data.

```tsx
"use server";

import { revalidatePath, revalidateTag } from "next/cache";
import { redirect } from "next/navigation";

// Revalidate a specific path
export async function deletePost(id: number) {
  await prisma.post.delete({ where: { id } });
  revalidatePath("/news");          // re-render /news
  revalidatePath(`/news/${id}`);    // re-render the specific post page
}

// Revalidate all pages tagged with "news"
export async function publishPost(id: number) {
  await prisma.post.update({ where: { id }, data: { published: true } });
  revalidateTag("news");            // re-renders any page using the "news" cache tag
}

// Redirect after mutation
export async function createStudent(formData: FormData) {
  const student = await prisma.student.create({ data: { ... } });
  revalidatePath("/students");
  redirect(`/students/${student.id}`);  // navigate to the new student's page
}
```

### Revalidation scope

```
revalidatePath("/students")        — exact path /students
revalidatePath("/students", "page") — same as above (explicit)
revalidatePath("/students", "layout") — revalidates the layout and all pages under it
revalidatePath("/")                — revalidates everything (nuclear option)
revalidateTag("students")          — revalidates all fetch() calls with { next: { tags: ["students"] } }
```

---

## 8. Server Action Patterns

### CRUD actions file

```tsx
// actions/news.ts
"use server";

import { prisma } from "@/lib/prisma";
import { revalidatePath } from "next/cache";
import { redirect } from "next/navigation";
import { getSession } from "@/lib/auth";

// Auth check helper
async function requireAdmin() {
  const session = await getSession();
  if (!session?.user.isAdmin) {
    throw new Error("Unauthorized");
  }
  return session;
}

export async function createNews(formData: FormData) {
  await requireAdmin();

  const title = formData.get("title") as string;
  const content = formData.get("content") as string;
  const slug = title.toLowerCase().replace(/\s+/g, "-");

  const post = await prisma.post.create({
    data: { title, content, slug, authorId: (await getSession())!.user.id },
  });

  revalidatePath("/news");
  redirect(`/news/${post.slug}`);
}

export async function updateNews(id: number, formData: FormData) {
  await requireAdmin();

  await prisma.post.update({
    where: { id },
    data: {
      title: formData.get("title") as string,
      content: formData.get("content") as string,
    },
  });

  revalidatePath("/news");
  revalidatePath(`/news/${id}`);
}

export async function deleteNews(id: number) {
  await requireAdmin();
  await prisma.post.delete({ where: { id } });
  revalidatePath("/news");
  redirect("/news");
}
```

### Return values

```tsx
// Server Actions can return values — accessible in Client Components
"use server";

export async function createStudent(formData: FormData) {
  const student = await prisma.student.create({ data: { ... } });
  return { id: student.id, name: student.name };
}

// In Client Component
"use client";
import { createStudent } from "@/actions/students";

async function handleSubmit(formData: FormData) {
  const newStudent = await createStudent(formData);
  // newStudent = { id: 42, name: "Ashan" }
  router.push(`/students/${newStudent.id}`);
}
```

---

## 9. Validation with Zod

Always validate Server Action inputs — they're callable from any client, including malicious ones.

```tsx
// actions/students.ts
"use server";

import { z } from "zod";
import { prisma } from "@/lib/prisma";
import { revalidatePath } from "next/cache";

const CreateStudentSchema = z.object({
  name: z.string().min(2).max(100),
  grade: z.coerce.number().int().min(1).max(13),
  email: z.string().email().optional(),
});

type CreateStudentState = {
  errors?: Record<string, string[]>;
  message?: string;
  success?: boolean;
};

export async function createStudent(
  prevState: CreateStudentState,
  formData: FormData,
): Promise<CreateStudentState> {
  // Parse and validate
  const result = CreateStudentSchema.safeParse({
    name: formData.get("name"),
    grade: formData.get("grade"),
    email: formData.get("email"),
  });

  if (!result.success) {
    return {
      errors: result.error.flatten().fieldErrors,
      message: "Please fix the errors below.",
    };
  }

  // Safe to use result.data now
  const { name, grade, email } = result.data;

  await prisma.student.create({
    data: { name, grade, email },
  });

  revalidatePath("/students");
  return { success: true };
}
```

```tsx
// Client Component — render field-level errors
"use client";

import { useActionState } from "react";
import { createStudent } from "@/actions/students";

export default function AddStudentForm() {
  const [state, action, isPending] = useActionState(createStudent, {});

  return (
    <form action={action} className="space-y-4">
      <div>
        <input name="name" type="text" placeholder="Full name" />
        {state.errors?.name && (
          <p className="text-destructive text-xs mt-1">
            {state.errors.name[0]}
          </p>
        )}
      </div>

      <div>
        <input name="grade" type="number" placeholder="Grade (1–13)" />
        {state.errors?.grade && (
          <p className="text-destructive text-xs mt-1">
            {state.errors.grade[0]}
          </p>
        )}
      </div>

      {state.message && (
        <p className="text-muted-foreground text-sm">{state.message}</p>
      )}

      <button type="submit" disabled={isPending}>
        {isPending ? "Adding..." : "Add Student"}
      </button>
    </form>
  );
}
```

---

## Summary

```
Server Actions:
  Async functions marked "use server" — run on the server, callable from the client
  Next.js handles the HTTP transport automatically
  Replace the API route + fetch() pattern for mutations

Defining:
  File-level: "use server" at top — all exports are Server Actions
  Inline: "use server" inside the function — for simple one-off cases

Calling from Client Components:
  Via form action prop: <form action={serverAction}>
  Directly: await serverAction(args) inside useTransition

useActionState:
  useActionState(action, initialState) → [state, actionWithState, isPending]
  Action signature: (prevState, formData) => Promise<State>
  For forms that need error/success messages without extra useState

useOptimistic:
  Instant UI update while server request is in flight
  Auto-rollback if the server action fails
  useOptimistic(realState, updateFn) → [optimisticState, addOptimistic]

Revalidation after mutations:
  revalidatePath("/path")     — re-render a specific page
  revalidateTag("tag")        — re-render all pages using a fetch tag
  redirect("/path")           — navigate after mutation

Always validate with Zod:
  Server Actions are callable from any client
  Use safeParse() — return errors, don't throw
  Map errors back to field-level messages in the Client Component
```

---

_Next: [06 — Navigation & Linking](https://claude.ai/chat/06%20-%20Navigation%20%26%20Linking.md)_
