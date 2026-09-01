## Assignment Overview

Build a small multi-page blog: a list of posts fetched from an API, individual post pages reachable by URL, and a "new post" form with real validation. This is the assignment where React stops being a single page and becomes an actual application — routing, server data, and forms all working together.

---

## Covers

`react/15  - React Router`, `react/16 - Data Fetching & TanStack Query`, `react/17 - Forms & Validation (React Hook form + Zod)`

---

## The API

Use [JSONPlaceholder](https://jsonplaceholder.typicode.com) again:

```
GET  https://jsonplaceholder.typicode.com/posts        (list of 100 posts)
GET  https://jsonplaceholder.typicode.com/posts/:id    (a single post)
POST https://jsonplaceholder.typicode.com/posts        (fake-creates a post, doesn't really persist)
```

---

## Part 1 — Routing

Set up three routes:

```jsx
<Routes>
  <Route path="/" element={<PostList />} />
  <Route path="/posts/:postId" element={<PostDetail />} />
  <Route path="/new" element={<NewPostForm />} />
</Routes>
```

- `PostList` shows a list of post titles, each linking to `/posts/:postId`
- `PostDetail` reads the `:postId` param from the URL and fetches that specific post
- A `<Link>` (not a plain `<a>` — this matters, see the Common Mistakes note in Module 15) to `/new` should be visible from the list page
- **Bonus requirement**: a post ID that doesn't exist (try `/posts/9999`) should show a clear "Post not found" message, not a blank page or a crash

---

## Part 2 — Data Fetching with TanStack Query

Replace any manual `useEffect` + `fetch` + loading-state juggling with TanStack Query:

```jsx
function PostList() {
  const {
    data: posts,
    isLoading,
    isError,
  } = useQuery({
    queryKey: ["posts"],
    queryFn: () =>
      fetch("https://jsonplaceholder.typicode.com/posts").then((res) =>
        res.json(),
      ),
  });

  // render loading / error / success states
}
```

- `PostList` and `PostDetail` should both use `useQuery`
- Handle all three states explicitly: loading, error, and success — no state should render as a blank screen
- **Use the correct `queryKey` structure for `PostDetail`** — it needs to include the post ID (`["post", postId]`), not just `["post"]`, or TanStack Query will incorrectly treat every different post as the same cached query

---

## Part 3 — Form Validation with React Hook Form + Zod

Build the "New Post" form with a real schema:

```jsx
import { z } from "zod";
import { zodResolver } from "@hookform/resolvers/zod";
import { useForm } from "react-hook-form";

const postSchema = z.object({
  title: z.string().min(5, "Title must be at least 5 characters"),
  body: z.string().min(20, "Post body must be at least 20 characters"),
});

function NewPostForm() {
  const {
    register,
    handleSubmit,
    formState: { errors },
  } = useForm({
    resolver: zodResolver(postSchema),
  });

  // your submit handler: POST to the API (it won't really persist —
  // JSONPlaceholder fakes a 201 response with an id), then show a
  // success message
}
```

- Both fields must show their specific validation error message when invalid
- Submitting a valid form should call the API and show a confirmation (JSONPlaceholder returns a fake new `id` — display it: `"Post created with id: 101"`)
- The submit button should be disabled while the request is in flight

---

## Constraints

- **No manual `fetch` + `useState`/`useEffect` combinations for data fetching** — that pattern was the previous assignment's territory; this one is specifically about using TanStack Query correctly, including its built-in loading/error states.
- **Validation errors must come from the Zod schema**, not hand-written `if` checks scattered through the submit handler — the whole point of `zodResolver` is that the schema is the single source of truth for what's valid.
- Route navigation must use React Router's `<Link>`/`useNavigate`, never `window.location.href` (which causes a full page reload, defeating the point of a single-page app).

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — Reading the URL param: `const { postId } = useParams();` inside `PostDetail`, then pass `postId` into your `useQuery` call's `queryFn`.

> 💡 **Hint 2** — "Post not found": JSONPlaceholder actually returns an empty object `{}` (not a 404) for an out-of-range ID — check for a missing `data.id` after a successful fetch, rather than only relying on `isError`.

> 💡 **Hint 3** — Register a field with React Hook Form: `<input {...register("title")} />` — then show its error with `{errors.title && <p>{errors.title.message}</p>}`.

---

## Submission Guidelines

1. All component/route files
2. A short note on the difference between `isLoading` and `isError` from TanStack Query, and where you handled each in your components

---

## Bonus Challenge 🏆

- 🔄 Add a "Refresh" button on `PostList` that manually triggers a refetch using TanStack Query's `refetch` function.
- 🧭 Add a 404 catch-all route (`path="*"`) for any URL that doesn't match one of your three defined routes.
- 🗂️ Compare this manual `<Routes>` setup to how routing works in the Next.js curriculum's file-based App Router (`react/27` covers exactly this comparison) — write two or three sentences on the biggest practical difference you notice.

---

## Learning Objectives

- ✓ Set up client-side routing with dynamic URL parameters.
- ✓ Use TanStack Query for server data instead of manual fetch/loading-state management, including correct query keys.
- ✓ Build a schema-validated form with React Hook Form and Zod, with real per-field error messages.

**Good Luck! 🚀**
