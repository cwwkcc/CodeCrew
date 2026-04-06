# React Curriculum

React is a JavaScript library for building user interfaces. It's the foundation of the Project Nexus stack — every page, every component, every interactive element you build uses React. This curriculum takes you from zero to production-ready, covering the core library, the hooks system, React 19, and the ecosystem libraries that complete a real application.

---

## Prerequisites

Complete the JavaScript curriculum (`00–08` minimum) before starting here. React is built entirely on JavaScript closures, the event model, the module system, and asynchronous patterns. If those aren't solid, React's behaviour will feel like magic — and not the good kind.

TypeScript basics (`00–02` in the TS curriculum) are helpful but not required for the early files. File `21` covers React + TypeScript together.

---

## Structure

```
CORE (00–07)
  The fundamental building blocks. Components, props, state, rendering.
  Work through these in order — each file assumes the previous.

HOOKS (08–13)
  React's system for adding behaviour to components.
  useEffect, useRef, useContext, useReducer, useMemo, useCallback, custom hooks.

REACT 19 (14)
  New primitives: use(), Actions, useFormStatus, useOptimistic.
  Only makes sense after the full hooks system is understood.

ECOSYSTEM (15–19)
  The libraries that complete a real application.
  Router, TanStack Query, React Hook Form + Zod, Zustand, Framer Motion.

ADVANCED (20–24)
  Performance, TypeScript integration, patterns, testing, shadcn/ui.
  These files assume you've built something with React already.
```

---

## File Map

```
00 - What React Is & The Mental Model
01 - JSX In Depth
02 - Components & Props
03 - State & useState
04 - Rendering & the Virtual DOM
05 - Conditional Rendering & Lists
06 - Event Handling
07 - Forms & Controlled Components
08 - useEffect & Side Effects
09 - useRef
10 - useContext
11 - useReducer
12 - useMemo & useCallback
13 - Custom Hooks
14 - React 19 New Features
15 - React Router
16 - Data Fetching & TanStack Query
17 - Forms & Validation (React Hook Form + Zod)
18 - Global State (Zustand)
19 - Animations (Framer Motion)
20 - Performance Optimization
21 - React with TypeScript
22 - Design Patterns in React
23 - Testing React Components
24 - Component Library (shadcn & Radix)
```

---

## Relation to Project Nexus

Every file in this curriculum has a direct counterpart in the codebase:

- Files `00–07` → every component in `apps/web/src/components/`
- Files `08–13` → data fetching, auth state, scroll animations
- File `14` → Server Actions for the admissions form, optimistic updates
- File `15` → Next.js App Router (which extends React Router concepts)
- File `16` → tRPC + TanStack Query for all data fetching
- File `17` → the A/L admissions form, contact forms
- File `18` → global UI state (sidebar, modals, theme)
- File `19` → hero animations, page transitions
- File `21` → every component (the whole codebase is TypeScript)
- File `24` → shadcn/ui components used throughout