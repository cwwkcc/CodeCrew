> React Router is the standard routing library for React applications. It maps URLs to components, manages navigation, and handles all the complexity of client-side routing. Even if you primarily use Next.js (which has its own router), understanding React Router gives you the mental model that Next.js App Router builds on.

---

## Table of Contents

1. [What Client-Side Routing Is](#1-what-client-side-routing-is)
2. [Setup and Basic Routing](#2-setup-and-basic-routing)
3. [Route Parameters](#3-route-parameters)
4. [Nested Routes and Layouts](#4-nested-routes-and-layouts)
5. [Navigation](#5-navigation)
6. [Programmatic Navigation](#6-programmatic-navigation)
7. [Search Params](#7-search-params)
8. [Protected Routes](#8-protected-routes)
9. [Data Loading with Loaders](#9-data-loading-with-loaders)
10. [Error Boundaries in Router](#10-error-boundaries-in-router)
11. [React Router vs Next.js App Router](#11-react-router-vs-nextjs-app-router)

---

## 1. What Client-Side Routing Is

In a traditional multi-page app, every URL change causes a full page load — the browser requests a new HTML document from the server.

In a React SPA (Single Page Application), there's only one HTML document. React Router intercepts navigation, updates the URL using the History API, and re-renders the correct components — without a page reload. The user gets a fast, app-like experience.

```
User clicks a link to /students
  Traditional: browser requests /students.html from server → full reload
  React Router: intercepts → updates URL bar → renders <StudentsPage /> → no reload
```

---

## 2. Setup and Basic Routing

```bash
npm install react-router-dom
```

```jsx
// main.jsx
import { BrowserRouter } from "react-router-dom";

ReactDOM.createRoot(document.getElementById("root")).render(
  <BrowserRouter>
    <App />
  </BrowserRouter>,
);
```

```jsx
// App.jsx
import { Routes, Route } from "react-router-dom";

function App() {
  return (
    <Routes>
      <Route path="/" element={<HomePage />} />
      <Route path="/students" element={<StudentsPage />} />
      <Route path="/about" element={<AboutPage />} />
      <Route path="/contact" element={<ContactPage />} />
      <Route path="*" element={<NotFoundPage />} />
    </Routes>
  );
}
```

`path="*"` is the catch-all — it matches any URL that didn't match earlier routes.

### createBrowserRouter — the modern API (v6.4+)

```jsx
import { createBrowserRouter, RouterProvider } from "react-router-dom";

const router = createBrowserRouter([
  {
    path: "/",
    element: <RootLayout />,
    children: [
      { index: true, element: <HomePage /> },
      { path: "students", element: <StudentsPage /> },
      { path: "about", element: <AboutPage /> },
      { path: "*", element: <NotFoundPage /> },
    ],
  },
]);

ReactDOM.createRoot(document.getElementById("root")).render(
  <RouterProvider router={router} />,
);
```

`createBrowserRouter` enables data APIs (loaders, actions) — covered in section 9.

---

## 3. Route Parameters

Dynamic segments in the path — prefixed with `:`.

```jsx
<Routes>
  <Route path="/students" element={<StudentsPage />} />
  <Route path="/students/:id" element={<StudentDetailPage />} />
  <Route path="/students/:id/edit" element={<StudentEditPage />} />
</Routes>
```

```jsx
// Reading the parameter
import { useParams } from "react-router-dom";

function StudentDetailPage() {
  const { id } = useParams(); // id matches ":id" in the path

  const [student, setStudent] = useState(null);

  useEffect(() => {
    fetchStudent(id).then(setStudent);
  }, [id]);

  if (!student) return <Spinner />;
  return <StudentProfile student={student} />;
}

// Multiple params
// Route: /schools/:schoolId/students/:studentId
function StudentPage() {
  const { schoolId, studentId } = useParams();
}
```

---

## 4. Nested Routes and Layouts

Nested routes render child routes inside a parent layout. The parent renders an `<Outlet />` where the child appears.

```jsx
// routes
<Routes>
  <Route path="/dashboard" element={<DashboardLayout />}>
    <Route index element={<DashboardHome />} />
    <Route path="students" element={<StudentsPage />} />
    <Route path="results" element={<ResultsPage />} />
    <Route path="settings" element={<SettingsPage />} />
  </Route>
</Routes>
```

```jsx
// DashboardLayout.jsx
import { Outlet, NavLink } from "react-router-dom";

function DashboardLayout() {
  return (
    <div className="dashboard">
      <aside className="sidebar">
        <nav>
          <NavLink to="/dashboard" end>
            Overview
          </NavLink>
          <NavLink to="/dashboard/students">Students</NavLink>
          <NavLink to="/dashboard/results"> Results</NavLink>
          <NavLink to="/dashboard/settings">Settings</NavLink>
        </nav>
      </aside>
      <main className="content">
        <Outlet /> {/* child route renders here */}
      </main>
    </div>
  );
}
```

`index` route renders when the parent path matches exactly (no child segment).

`NavLink` is like `Link` but adds an `active` class when its `to` matches the current URL. The `end` prop makes it only match exact paths (without it, `/dashboard` would also be active when on `/dashboard/students`).

---

## 5. Navigation

```jsx
import { Link, NavLink } from "react-router-dom";

// Link — basic navigation
<Link to="/students">Students</Link>
<Link to={`/students/${student.id}`}>View Profile</Link>
<Link to="/about" state={{ from: "homepage" }}>About</Link>

// Relative links (inside nested routes)
<Link to="..">Back</Link>            // go up one level
<Link to="../students">Students</Link>

// NavLink — automatically applies active class
<NavLink to="/students">Students</NavLink>
// adds class="active" when on /students

// Custom active styling
<NavLink
  to="/students"
  className={({ isActive }) => isActive ? "nav-link nav-link-active" : "nav-link"}
  style={({ isActive }) => ({ fontWeight: isActive ? "bold" : "normal" })}
>
  Students
</NavLink>
```

---

## 6. Programmatic Navigation

Navigate in response to events — form submissions, button clicks, auth state changes.

```jsx
import { useNavigate } from "react-router-dom";

function LoginForm() {
  const navigate = useNavigate();

  async function handleSubmit(e) {
    e.preventDefault();
    try {
      await login(credentials);
      navigate("/dashboard"); // go to dashboard after login
    } catch (err) {
      setError(err.message);
    }
  }
}

// With options
navigate("/login"); // push to history
navigate(-1); // go back (like browser back button)
navigate(1); // go forward
navigate("/login", { replace: true }); // replace current history entry (no back button)
navigate("/dashboard", {
  state: { message: "Welcome back!" }, // pass state to the next page
});

// Read state on the destination page
import { useLocation } from "react-router-dom";

function Dashboard() {
  const location = useLocation();
  const message = location.state?.message;

  return (
    <div>
      {message && <Banner message={message} />}
      {/* ... */}
    </div>
  );
}
```

---

## 7. Search Params

URL search params (`?query=ashan&grade=11`) for filters, pagination, and other state that should be shareable via URL.

```jsx
import { useSearchParams } from "react-router-dom";

function StudentsPage() {
  const [searchParams, setSearchParams] = useSearchParams();

  const query = searchParams.get("query") ?? "";
  const grade = searchParams.get("grade") ?? "";
  const page = Number(searchParams.get("page") ?? "1");

  function handleQueryChange(e) {
    setSearchParams((prev) => {
      prev.set("query", e.target.value);
      prev.set("page", "1"); // reset to page 1 on filter change
      return prev;
    });
  }

  function handleGradeChange(e) {
    setSearchParams((prev) => {
      if (e.target.value) prev.set("grade", e.target.value);
      else prev.delete("grade");
      prev.set("page", "1");
      return prev;
    });
  }

  function goToPage(newPage) {
    setSearchParams((prev) => {
      prev.set("page", String(newPage));
      return prev;
    });
  }

  return (
    <div>
      <input
        value={query}
        onChange={handleQueryChange}
        placeholder="Search..."
      />
      <select value={grade} onChange={handleGradeChange}>
        <option value="">All grades</option>
        {[6, 7, 8, 9, 10, 11, 12, 13].map((g) => (
          <option key={g} value={g}>
            Grade {g}
          </option>
        ))}
      </select>

      <StudentGrid query={query} grade={grade} page={page} />
      <Pagination current={page} onPageChange={goToPage} />
    </div>
  );
}
// URL: /students?query=ashan&grade=11&page=2
// Shareable, bookmarkable, browser-back-able
```

---

## 8. Protected Routes

Redirect unauthenticated users to login.

```jsx
import { Navigate, useLocation } from "react-router-dom";
import { useAuth } from "./auth-context";

function RequireAuth({ children }) {
  const { user, isLoading } = useAuth();
  const location = useLocation();

  if (isLoading) return <FullPageSpinner />;

  if (!user) {
    // Redirect to login, preserving the page they tried to visit
    return <Navigate to="/login" state={{ from: location }} replace />;
  }

  return children;
}

// Usage in route config
<Routes>
  <Route path="/login" element={<LoginPage />} />
  <Route
    path="/dashboard"
    element={
      <RequireAuth>
        <DashboardLayout />
      </RequireAuth>
    }
  >
    <Route index element={<DashboardHome />} />
    <Route path="students" element={<StudentsPage />} />
  </Route>
</Routes>;

// After login, redirect back to where they were trying to go
function LoginPage() {
  const navigate = useNavigate();
  const location = useLocation();
  const from = location.state?.from?.pathname ?? "/dashboard";

  async function handleLogin(credentials) {
    await login(credentials);
    navigate(from, { replace: true }); // go to original destination
  }
}
```

### Role-based access

```jsx
function RequireRole({ role, children }) {
  const { user } = useAuth();

  if (!user) return <Navigate to="/login" replace />;
  if (!user.roles.includes(role))
    return <Navigate to="/unauthorised" replace />;

  return children;
}

<Route
  path="/admin"
  element={
    <RequireRole role="admin">
      <AdminPanel />
    </RequireRole>
  }
/>;
```

---

## 9. Data Loading with Loaders

`createBrowserRouter` enables **loaders** — async functions that fetch data before the route renders. No more `isLoading` states in components.

```jsx
const router = createBrowserRouter([
  {
    path: "/students/:id",
    element: <StudentDetailPage />,
    loader: async ({ params }) => {
      const student = await fetchStudent(params.id);
      if (!student) throw new Response("Not Found", { status: 404 });
      return student; // returned value available in the component
    },
    errorElement: <StudentErrorPage />,
  },
]);

// Component — data is already loaded when it renders
import { useLoaderData } from "react-router-dom";

function StudentDetailPage() {
  const student = useLoaderData();
  return <StudentProfile student={student} />;
  // No loading state needed — loader ran before render
}
```

---

## 10. Error Boundaries in Router

```jsx
const router = createBrowserRouter([
  {
    path: "/",
    element: <RootLayout />,
    errorElement: <RootErrorPage />, // catches errors from this route + children
    children: [
      {
        path: "students/:id",
        element: <StudentDetailPage />,
        errorElement: <StudentErrorPage />, // more specific error UI
        loader: studentLoader,
      },
    ],
  },
]);

import { useRouteError, isRouteErrorResponse } from "react-router-dom";

function StudentErrorPage() {
  const error = useRouteError();

  if (isRouteErrorResponse(error)) {
    if (error.status === 404) return <p>Student not found.</p>;
    if (error.status === 403) return <p>You don't have permission.</p>;
  }

  return <p>Something went wrong. Please try again.</p>;
}
```

---

## 11. React Router vs Next.js App Router

Understanding the parallels helps you move between the two systems.

```
React Router (v6)              Next.js App Router (v14+)
────────────────────────────────────────────────────────────
<Route path="/students">       app/students/page.tsx
  element={<StudentsPage />}

<Route path="/students/:id">   app/students/[id]/page.tsx
  element={<StudentPage />}

Nested routes + <Outlet />     app/students/layout.tsx
                               (wraps all /students/* routes)

loader: async ({ params })     async function Page({ params }) {
  => return fetchData()          const data = await fetchData(params.id)
                               } (Server Component)

useLoaderData()                Props passed to page component

useNavigate()                  import { redirect } from "next/navigation"
                               or useRouter().push()

useSearchParams()              useSearchParams() (same hook name!)

<Navigate to="/login" />       redirect("/login") in Server Components
                               router.push("/login") in Client Components

errorElement                   error.tsx in the route segment
```

---

## Summary

```
Client-side routing:
  React Router intercepts navigation, updates URL, re-renders components
  No page reload — fast, app-like experience

Basic setup:
  <BrowserRouter> wraps the app
  <Routes> + <Route path="..." element={<Page />} />
  path="*" for 404

Route params:
  path="/students/:id" → useParams() → { id }
  Params are always strings — parse numbers explicitly

Nested routes:
  Parent renders <Outlet /> where child appears
  <Route index> for the parent's exact path

Navigation:
  <Link to="/path"> — basic navigation
  <NavLink to="/path"> — adds active class automatically
  useNavigate() — programmatic navigation
  navigate(-1) — go back; navigate("/x", { replace: true }) — no history entry

Search params:
  useSearchParams() — [params, setParams]
  params.get("key"), params.set("key", val)
  Shareable, bookmarkable state

Protected routes:
  Component that checks auth, renders children or <Navigate to="/login" />
  Pass { state: { from: location } } to restore destination after login

Loaders (createBrowserRouter):
  Fetch data before the component renders — no loading states
  useLoaderData() to access in component
  errorElement handles loader errors

vs Next.js App Router:
  file-system routing instead of explicit <Route> components
  Server Components run on server — no loader needed
  layout.tsx = shared layout (like parent route with <Outlet />)
```

---

_Next: [16 — Data Fetching & TanStack Query](./16%20-%20Data%20Fetching%20%26%20TanStack%20Query.md)_
