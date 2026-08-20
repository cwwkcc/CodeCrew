> Zustand is a small, fast, and flexible global state library. It solves the problems that Context has at scale: performance (fine-grained subscriptions), simplicity (no Provider wrapping), and ergonomics (actions live next to state). It's the right tool for client state that multiple unrelated components need.

---

## Table of Contents

1. [Why Zustand over Context](#1-why-zustand-over-context)
2. [Creating a Store](#2-creating-a-store)
3. [Selectors — Fine-Grained Subscriptions](#3-selectors--fine-grained-subscriptions)
4. [Actions and State Mutations](#4-actions-and-state-mutations)
5. [Async Actions](#5-async-actions)
6. [Slices — Organising Large Stores](#6-slices--organising-large-stores)
7. [Persistence with Middleware](#7-persistence-with-middleware)
8. [DevTools](#8-devtools)
9. [Real-World Store Examples](#9-real-world-store-examples)
10. [When to Use Zustand vs Context vs TanStack Query](#10-when-to-use-zustand-vs-context-vs-tanstack-query)

---

## 1. Why Zustand over Context

```
Context:
  Any context consumer re-renders when ANY part of the context value changes
  Requires Provider wrapping — components must be inside the tree
  No built-in devtools
  Gets complex with many actions and state slices

Zustand:
  Components only re-render when the specific slice they SELECT changes
  No Provider needed — import the hook anywhere
  Redux DevTools support built-in
  Flat, composable API
```

```jsx
// Context: ThemeContext changes → ALL consumers re-render
// Even components that only use user.name re-render when theme changes

// Zustand: each component selects exactly what it needs
const name = useAppStore((state) => state.user.name); // only re-renders when name changes
const theme = useAppStore((state) => state.theme); // only re-renders when theme changes
// Completely independent — no cross-contamination
```

---

## 2. Creating a Store

```bash
npm install zustand
```

```typescript
// store.ts
import { create } from "zustand";

interface UIStore {
  // State
  isSidebarOpen: boolean;
  activeModal: string | null;
  theme: "light" | "dark";

  // Actions
  openSidebar: () => void;
  closeSidebar: () => void;
  toggleSidebar: () => void;
  openModal: (modalName: string) => void;
  closeModal: () => void;
  setTheme: (theme: "light" | "dark") => void;
}

export const useUIStore = create<UIStore>((set) => ({
  // Initial state
  isSidebarOpen: true,
  activeModal: null,
  theme: "light",

  // Actions — call set() to update state
  openSidebar: () => set({ isSidebarOpen: true }),
  closeSidebar: () => set({ isSidebarOpen: false }),
  toggleSidebar: () =>
    set((state) => ({ isSidebarOpen: !state.isSidebarOpen })),

  openModal: (modalName) => set({ activeModal: modalName }),
  closeModal: () => set({ activeModal: null }),

  setTheme: (theme) => set({ theme }),
}));
```

```typescript
// Usage — anywhere, no Provider needed
import { useUIStore } from "./store";

function Sidebar() {
  const isOpen = useUIStore(state => state.isSidebarOpen);
  return <aside className={isOpen ? "open" : "closed"}>...</aside>;
}

function Header() {
  const toggleSidebar = useUIStore(state => state.toggleSidebar);
  return <button onClick={toggleSidebar}>☰</button>;
}

function Modal() {
  const { activeModal, closeModal } = useUIStore(state => ({
    activeModal: state.activeModal,
    closeModal:  state.closeModal,
  }));
  if (!activeModal) return null;
  return <ModalContainer name={activeModal} onClose={closeModal} />;
}
```

---

## 3. Selectors — Fine-Grained Subscriptions

The selector is the function passed to `useUIStore()`. It determines what slice of state the component subscribes to.

```typescript
// ✓ Select only what you need — re-renders only when that value changes
const isSidebarOpen = useUIStore((state) => state.isSidebarOpen);
const theme = useUIStore((state) => state.theme);
const openModal = useUIStore((state) => state.openModal);

// ✓ Multiple values — but re-renders when EITHER changes
const { isSidebarOpen, theme } = useUIStore((state) => ({
  isSidebarOpen: state.isSidebarOpen,
  theme: state.theme,
}));
// Zustand uses shallow equality for object selectors — add shallow if needed
import { shallow } from "zustand/shallow";
const { isSidebarOpen, theme } = useUIStore(
  (state) => ({ isSidebarOpen: state.isSidebarOpen, theme: state.theme }),
  shallow,
);

// ✗ Selecting entire store — re-renders on ANY state change
const store = useUIStore(); // don't do this unless you genuinely need everything
```

### Computed selectors

```typescript
// Derive values from store state directly in the selector
const hasActiveModal = useUIStore((state) => state.activeModal !== null);
const isDarkMode = useUIStore((state) => state.theme === "dark");
const canDismissModal = useUIStore(
  (state) => state.activeModal !== null && !state.isModalPersistent,
);
```

---

## 4. Actions and State Mutations

```typescript
const useCartStore = create<CartStore>((set, get) => ({
  items: [],
  discountCode: null,

  addItem: (product, quantity = 1) =>
    set((state) => {
      const existing = state.items.find((i) => i.id === product.id);

      if (existing) {
        return {
          items: state.items.map((i) =>
            i.id === product.id ? { ...i, quantity: i.quantity + quantity } : i,
          ),
        };
      }

      return { items: [...state.items, { ...product, quantity }] };
    }),

  removeItem: (id) =>
    set((state) => ({
      items: state.items.filter((i) => i.id !== id),
    })),

  updateQuantity: (id, quantity) =>
    set((state) => ({
      items:
        quantity <= 0
          ? state.items.filter((i) => i.id !== id)
          : state.items.map((i) => (i.id === id ? { ...i, quantity } : i)),
    })),

  clearCart: () => set({ items: [], discountCode: null }),

  // get() reads current state without subscribing
  applyDiscount: (code) => {
    const validCodes = { KITS10: 0.1, SCHOOL20: 0.2 };
    const discount = validCodes[code];

    if (!discount) {
      return { error: "Invalid discount code" };
    }

    set({ discountCode: { code, discount } });
    return { error: null };
  },

  // Computed value in the store (alternatively, compute in selector)
  get total() {
    const state = get();
    const subtotal = state.items.reduce(
      (sum, i) => sum + i.price * i.quantity,
      0,
    );
    const discount = state.discountCode?.discount ?? 0;
    return subtotal * (1 - discount);
  },
}));
```

---

## 5. Async Actions

```typescript
interface StudentStore {
  students: Student[];
  isLoading: boolean;
  error: string | null;
  fetchStudents: (filters?: Filters) => Promise<void>;
  addStudent: (data: NewStudent) => Promise<Student>;
  deleteStudent: (id: string) => Promise<void>;
}

const useStudentStore = create<StudentStore>((set, get) => ({
  students:  [],
  isLoading: false,
  error:     null,

  fetchStudents: async (filters) => {
    set({ isLoading: true, error: null });
    try {
      const students = await fetchStudentsApi(filters);
      set({ students, isLoading: false });
    } catch (err) {
      set({ error: err.message, isLoading: false });
    }
  },

  addStudent: async (data) => {
    const newStudent = await createStudentApi(data);
    set(state => ({ students: [...state.students, newStudent] }));
    return newStudent;
  },

  deleteStudent: async (id) => {
    // Optimistic update
    const previous = get().students;
    set(state => ({ students: state.students.filter(s => s.id !== id) }));

    try {
      await deleteStudentApi(id);
    } catch (err) {
      set({ students: previous });  // roll back
      throw err;
    }
  },
}));

// Usage
function StudentsPage() {
  const { students, isLoading, error, fetchStudents } = useStudentStore();

  useEffect(() => {
    fetchStudents();
  }, [fetchStudents]);

  if (isLoading) return <Spinner />;
  if (error)     return <ErrorMessage message={error} />;
  return <StudentGrid students={students} />;
}
```

> **Note:** For server data (data that lives in a database), prefer TanStack Query over Zustand. Zustand is better for client-only state. Async Zustand actions are useful for non-HTTP async operations (WebSocket, IndexedDB, background computation).

---

## 6. Slices — Organising Large Stores

For large apps, split the store into logical slices that are combined.

```typescript
// slices/ui.ts
type UISlice = {
  isSidebarOpen: boolean;
  toggleSidebar: () => void;
  activeModal: string | null;
  openModal: (name: string) => void;
  closeModal: () => void;
};

const createUISlice = (set: SetState): UISlice => ({
  isSidebarOpen: true,
  toggleSidebar: () =>
    set((state) => ({ isSidebarOpen: !state.isSidebarOpen })),
  activeModal: null,
  openModal: (name) => set({ activeModal: name }),
  closeModal: () => set({ activeModal: null }),
});

// slices/cart.ts
type CartSlice = {
  cartItems: CartItem[];
  addToCart: (item: CartItem) => void;
  removeFromCart: (id: string) => void;
  clearCart: () => void;
};

const createCartSlice = (set: SetState): CartSlice => ({
  cartItems: [],
  addToCart: (item) =>
    set((state) => ({ cartItems: [...state.cartItems, item] })),
  removeFromCart: (id) =>
    set((state) => ({ cartItems: state.cartItems.filter((i) => i.id !== id) })),
  clearCart: () => set({ cartItems: [] }),
});

// store.ts — combine slices
type AppStore = UISlice & CartSlice;

export const useAppStore = create<AppStore>()((...args) => ({
  ...createUISlice(...args),
  ...createCartSlice(...args),
}));

// Each component selects from the combined store
const isSidebarOpen = useAppStore((state) => state.isSidebarOpen);
const cartItems = useAppStore((state) => state.cartItems);
```

---

## 7. Persistence with Middleware

```typescript
import { create } from "zustand";
import { persist, createJSONStorage } from "zustand/middleware";

const useSettingsStore = create(
  persist(
    (set) => ({
      theme: "dark" as "light" | "dark",
      language: "en",
      fontSize: 16,

      setTheme: (theme: "light" | "dark") => set({ theme }),
      setLanguage: (language: string) => set({ language }),
      setFontSize: (fontSize: number) => set({ fontSize }),
    }),
    {
      name: "app-settings", // localStorage key
      storage: createJSONStorage(() => localStorage),
      partialize: (state) => ({
        // only persist these fields
        theme: state.theme,
        language: state.language,
        fontSize: state.fontSize,
        // don't persist actions — they're recreated
      }),
    },
  ),
);
// State automatically saved to localStorage on every change
// State restored from localStorage on app start
```

---

## 8. DevTools

```typescript
import { devtools } from "zustand/middleware";

const useAppStore = create(
  devtools(
    (set) => ({
      count: 0,
      increment: () =>
        set((state) => ({ count: state.count + 1 }), false, "increment"),
      //                                                                    ↑ action name in devtools
    }),
    { name: "AppStore" }, // store name in devtools
  ),
);
// Open Redux DevTools in browser → see all state changes with action names
```

---

## 9. Real-World Store Examples

### Notification store

```typescript
interface Notification {
  id: string;
  type: "success" | "error" | "info" | "warning";
  message: string;
}

const useNotificationStore = create<{
  notifications: Notification[];
  add: (notif: Omit<Notification, "id">) => void;
  dismiss: (id: string) => void;
  clear: () => void;
}>((set) => ({
  notifications: [],

  add: (notif) => {
    const id = crypto.randomUUID();
    set((state) => ({
      notifications: [...state.notifications, { ...notif, id }],
    }));
    // Auto-dismiss after 4 seconds
    setTimeout(() => {
      set((state) => ({
        notifications: state.notifications.filter((n) => n.id !== id),
      }));
    }, 4000);
  },

  dismiss: (id) =>
    set((state) => ({
      notifications: state.notifications.filter((n) => n.id !== id),
    })),

  clear: () => set({ notifications: [] }),
}));

// Utility function — callable outside React
export const notify = {
  success: (message: string) =>
    useNotificationStore.getState().add({ type: "success", message }),
  error: (message: string) =>
    useNotificationStore.getState().add({ type: "error", message }),
  info: (message: string) =>
    useNotificationStore.getState().add({ type: "info", message }),
};

// Usage in non-component code (API layer, etc.)
try {
  await saveData();
  notify.success("Saved successfully");
} catch (err) {
  notify.error("Failed to save: " + err.message);
}
```

### Auth store

```typescript
const useAuthStore = create(
  persist(
    (set, get) => ({
      user: null as User | null,
      accessToken: null as string | null,

      setAuth: (user: User, token: string) => set({ user, accessToken: token }),

      logout: () => {
        set({ user: null, accessToken: null });
        // Clear other stores on logout
        useCartStore.getState().clearCart();
      },

      isAuthenticated: () => get().user !== null,
      hasRole: (role: string) => get().user?.roles.includes(role) ?? false,
    }),
    {
      name: "auth",
      partialize: (state) => ({
        user: state.user,
        accessToken: state.accessToken,
      }),
    },
  ),
);
```

---

## 10. When to Use Zustand vs Context vs TanStack Query

```
Data type                        Tool
─────────────────────────────────────────────────────────────────────
Server data (from DB/API)        TanStack Query
  - Student list
  - Course details
  - Exam results

Global UI state                  Zustand
  - Sidebar open/closed
  - Active modal
  - Notification queue
  - Shopping cart
  - User preferences (theme, language)

Auth state                       Zustand + persist
  - Current user
  - Access token

Subtree-scoped state             Context
  - Wizard step data
  - Form context (RHF does this internally)
  - Feature-specific state used in one part of the app

Local component state            useState / useReducer
  - Input value
  - Expanded/collapsed
  - Tab selection
```

---

## Summary

```
Zustand vs Context:
  Context: any change → all consumers re-render
  Zustand: selector determines what triggers re-renders — fine-grained

create():
  create<Type>((set, get) => ({ state, actions }))
  set({ key: value }) — partial update (merges, not replaces)
  set(state => ({ key: newValue })) — updater function

Selectors:
  useStore(state => state.field)  — re-renders only when field changes
  Don't select entire store — subscribe to what you need

set() rules:
  Always create new references: set(s => ({ arr: [...s.arr, item] }))
  get() reads current state without subscribing

Async actions:
  set loading, try/await, set result or error
  For server data: prefer TanStack Query
  Zustand async: for non-HTTP operations (WebSocket, local storage)

Slices:
  createXSlice = (set, get) => ({ ...state, ...actions })
  Combine: create((...args) => ({ ...createA(...args), ...createB(...args) }))

Middleware:
  persist() — save/restore from localStorage/sessionStorage
  devtools() — Redux DevTools integration

Read store outside React:
  useStore.getState()     — read current state
  useStore.setState({})   — update state
  useStore.subscribe(fn)  — subscribe to changes
  Useful for: API layers, notification utilities, logout cleanup
```

---

_Next: [19 — Animations (Framer Motion)](<./19%20-%20Animations%20(Framer%20Motion).md>)_
