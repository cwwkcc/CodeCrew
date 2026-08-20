> Framer Motion is the animation library for React. It brings complex animations — page transitions, gesture-driven interactions, shared element transitions, layout animations — down to a declarative API that fits naturally into React's model.

---

## Table of Contents

1. [The Framer Motion Mental Model](#1-the-framer-motion-mental-model)
2. [Setup and motion Components](#2-setup-and-motion-components)
3. [Animate, Initial, and Exit](#3-animate-initial-and-exit)
4. [Variants — Orchestrated Animations](#4-variants--orchestrated-animations)
5. [AnimatePresence — Enter and Exit](#5-animatepresence--enter-and-exit)
6. [Gesture Animations](#6-gesture-animations)
7. [Layout Animations](#7-layout-animations)
8. [useAnimation — Imperative Control](#8-useanimation--imperative-control)
9. [useMotionValue and useTransform](#9-usemotionvalue-and-usetransform)
10. [Scroll Animations](#10-scroll-animations)
11. [Page Transitions](#11-page-transitions)
12. [Performance Guidelines](#12-performance-guidelines)

---

## 1. The Framer Motion Mental Model

Framer Motion is declarative — you describe what a component should look like in different states, and Motion handles the transition between them.

```jsx
// You say: "this box should be here, this size, this color"
// Motion says: "I'll figure out how to get there from where it is now"

<motion.div
  initial={{ opacity: 0, y: 20 }} // start: invisible, 20px down
  animate={{ opacity: 1, y: 0 }} // end: visible, in place
  exit={{ opacity: 0, y: -20 }} // leaving: invisible, 20px up
/>
```

This is the same declarative mental model as React itself — describe the desired state, let the library figure out the transitions.

---

## 2. Setup and `motion` Components

```bash
npm install framer-motion
```

```jsx
import { motion } from "framer-motion";

// motion.div, motion.span, motion.p, motion.img, motion.button...
// motion.* wraps any HTML element with animation capabilities
<motion.div animate={{ x: 100 }}>Slide right 100px</motion.div>
<motion.button whileHover={{ scale: 1.05 }}>Hover me</motion.button>
<motion.img animate={{ rotate: 360 }} />

// Custom components — wrap with motion()
const MotionCard = motion(Card);
<MotionCard animate={{ opacity: 1 }} />
```

---

## 3. Animate, Initial, and Exit

```jsx
// animate — what the component animates TO on mount and when values change
<motion.div animate={{ x: 0, opacity: 1 }} />

// initial — starting state (before first animate)
<motion.div
  initial={{ opacity: 0, y: 20 }}
  animate={{ opacity: 1, y: 0 }}
/>

// transition — how the animation runs
<motion.div
  initial={{ opacity: 0 }}
  animate={{ opacity: 1 }}
  transition={{
    duration: 0.4,       // seconds
    delay:    0.1,       // seconds before starting
    ease:     "easeOut", // "linear" | "easeIn" | "easeOut" | "easeInOut" | [0.6, 0, 0.4, 1]

    // Spring physics (feels more natural than duration-based)
    type:      "spring",
    stiffness: 260,
    damping:   20,
    mass:      1,

    // Repeat
    repeat:         Infinity,
    repeatType:    "reverse",   // "loop" | "reverse" | "mirror"
    repeatDelay:    0.5,
  }}
/>

// Animatable properties
<motion.div
  animate={{
    // Layout
    x: 100,         // translateX in px
    y: -50,         // translateY in px
    scale: 1.2,
    scaleX: 1.5,
    scaleY: 0.8,
    rotate: 45,     // degrees
    rotateX: 30,
    rotateY: 30,
    skew: 10,

    // Appearance
    opacity: 0.5,
    borderRadius: "50%",
    backgroundColor: "#c9a84c",
    color: "#ffffff",
    width: 200,
    height: 200,

    // Box shadow
    boxShadow: "0px 10px 30px rgba(0,0,0,0.3)",
  }}
/>
```

---

## 4. Variants — Orchestrated Animations

Variants let you define animation states by name and coordinate animations across a parent and children.

```jsx
const containerVariants = {
  hidden: { opacity: 0 },
  visible: {
    opacity: 1,
    transition: {
      staggerChildren: 0.1, // delay each child by 0.1s
      delayChildren: 0.2, // wait 0.2s before starting children
    },
  },
};

const itemVariants = {
  hidden: { opacity: 0, y: 20 },
  visible: {
    opacity: 1,
    y: 0,
    transition: { duration: 0.4, ease: "easeOut" },
  },
};

function AnimatedList({ items }) {
  return (
    <motion.ul variants={containerVariants} initial="hidden" animate="visible">
      {items.map((item) => (
        <motion.li key={item.id} variants={itemVariants}>
          {/* Children inherit parent's "hidden"/"visible" state automatically */}
          {item.name}
        </motion.li>
      ))}
    </motion.ul>
  );
}
// Items stagger in one by one with a 0.1s delay between each
```

---

## 5. `AnimatePresence` — Enter and Exit

By default, React immediately removes components from the DOM. `AnimatePresence` lets components animate out before being removed.

```jsx
import { AnimatePresence, motion } from "framer-motion";

function Modal({ isOpen, onClose, children }) {
  return (
    <AnimatePresence>
      {isOpen && (
        <>
          {/* Backdrop */}
          <motion.div
            key="backdrop"
            className="fixed inset-0 bg-black/50"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            onClick={onClose}
          />
          {/* Modal */}
          <motion.div
            key="modal"
            className="fixed inset-0 flex items-center justify-center"
            initial={{ opacity: 0, scale: 0.9, y: 20 }}
            animate={{ opacity: 1, scale: 1, y: 0 }}
            exit={{ opacity: 0, scale: 0.9, y: 20 }}
            transition={{ type: "spring", damping: 25, stiffness: 300 }}
          >
            {children}
          </motion.div>
        </>
      )}
    </AnimatePresence>
  );
}

// Notification stack
function NotificationStack({ notifications }) {
  return (
    <div className="fixed top-4 right-4 space-y-2">
      <AnimatePresence>
        {notifications.map((notif) => (
          <motion.div
            key={notif.id} // key is essential for AnimatePresence
            initial={{ opacity: 0, x: 50, scale: 0.9 }}
            animate={{ opacity: 1, x: 0, scale: 1 }}
            exit={{ opacity: 0, x: 50, scale: 0.9 }}
            layout // animate position when others are added/removed
          >
            <Notification notif={notif} />
          </motion.div>
        ))}
      </AnimatePresence>
    </div>
  );
}
```

**`AnimatePresence` requirements:**

- Wrap elements that mount/unmount
- Each direct child must have a unique `key` prop
- The children must be the animated `motion.*` elements themselves

---

## 6. Gesture Animations

```jsx
// Hover and tap — most common
<motion.button
  whileHover={{ scale: 1.05, backgroundColor: "#d4b56a" }}
  whileTap={{ scale: 0.95 }}
  transition={{ type: "spring", stiffness: 400, damping: 17 }}
>
  Submit
</motion.button>

// Focus (for accessibility — keyboard navigation)
<motion.input whileFocus={{ scale: 1.02, borderColor: "#c9a84c" }} />

// Drag
<motion.div
  drag
  dragConstraints={{ left: -100, right: 100, top: -50, bottom: 50 }}
  dragElastic={0.1}   // how much it can go beyond constraints (0 = hard stop, 1 = free)
  whileDrag={{ scale: 1.1, cursor: "grabbing" }}
>
  Drag me
</motion.div>

// Drag along one axis
<motion.div drag="x" dragConstraints={{ left: 0, right: 300 }}>
  Horizontal slider
</motion.div>

// Swipe to dismiss
function SwipeableCard({ onDismiss }) {
  return (
    <motion.div
      drag="x"
      dragConstraints={{ left: 0, right: 0 }}
      dragElastic={1}
      onDragEnd={(e, { offset, velocity }) => {
        if (Math.abs(offset.x) > 150 || Math.abs(velocity.x) > 500) {
          onDismiss();  // dismissed if swiped far enough or fast enough
        }
      }}
    >
      Card content
    </motion.div>
  );
}
```

---

## 7. Layout Animations

`layout` prop animates between different positions/sizes automatically when the layout changes.

```jsx
// Expanding card — size changes animate smoothly
function ExpandableCard({ title, content }) {
  const [isExpanded, setIsExpanded] = useState(false);

  return (
    <motion.div
      layout
      className="card"
      onClick={() => setIsExpanded(!isExpanded)}
    >
      <motion.h2 layout="position">{title}</motion.h2>
      <AnimatePresence>
        {isExpanded && (
          <motion.p
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
          >
            {content}
          </motion.p>
        )}
      </AnimatePresence>
    </motion.div>
  );
}

// Reordering list — items animate to their new positions
function SortableList({ items }) {
  return (
    <ul>
      <AnimatePresence>
        {items.map((item) => (
          <motion.li
            key={item.id}
            layout
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
          >
            {item.name}
          </motion.li>
        ))}
      </AnimatePresence>
    </ul>
  );
  // When items are reordered (sorted, filtered), they slide to their new positions
}

// Shared layout — element transitions between different DOM positions
// (tab indicator, selected card highlight)
function TabBar({ tabs, activeTab, onTabChange }) {
  return (
    <div className="tab-bar">
      {tabs.map((tab) => (
        <button
          key={tab.id}
          onClick={() => onTabChange(tab.id)}
          className="relative"
        >
          {tab.label}
          {activeTab === tab.id && (
            <motion.div
              layoutId="tab-indicator" // same layoutId = shared element transition
              className="absolute bottom-0 left-0 right-0 h-0.5 bg-primary"
            />
          )}
        </button>
      ))}
    </div>
  );
  // The indicator slides from one tab to another as activeTab changes
}
```

---

## 8. `useAnimation` — Imperative Control

```jsx
import { useAnimation } from "framer-motion";

function ShakeOnError({ hasError, children }) {
  const controls = useAnimation();

  useEffect(() => {
    if (hasError) {
      controls.start({
        x: [0, -10, 10, -10, 10, -5, 5, 0],
        transition: { duration: 0.5 },
      });
    }
  }, [hasError, controls]);

  return <motion.div animate={controls}>{children}</motion.div>;
}

// Sequence — run animations one after another
async function playSequence() {
  await controls.start({ scale: 1.2, transition: { duration: 0.2 } });
  await controls.start({ scale: 1, transition: { duration: 0.2 } });
  await controls.start({ x: 100, transition: { duration: 0.4 } });
  controls.start({ opacity: 0 }); // last one without await — fire and forget
}
```

---

## 9. `useMotionValue` and `useTransform`

For values that animate without triggering React re-renders (pure JS-driven animations).

```jsx
import { useMotionValue, useTransform, motion } from "framer-motion";

function ParallaxCard() {
  const x = useMotionValue(0); // tracks mouse position
  const y = useMotionValue(0);

  // Transform mouse position → rotation (no re-renders)
  const rotateX = useTransform(y, [-100, 100], [10, -10]);
  const rotateY = useTransform(x, [-100, 100], [-10, 10]);

  function handleMouseMove(e) {
    const rect = e.currentTarget.getBoundingClientRect();
    const centerX = rect.left + rect.width / 2;
    const centerY = rect.top + rect.height / 2;
    x.set(e.clientX - centerX);
    y.set(e.clientY - centerY);
  }

  function handleMouseLeave() {
    x.set(0);
    y.set(0);
  }

  return (
    <motion.div
      style={{ rotateX, rotateY, transformStyle: "preserve-3d" }}
      onMouseMove={handleMouseMove}
      onMouseLeave={handleMouseLeave}
    >
      3D card that follows the mouse
    </motion.div>
  );
}
```

---

## 10. Scroll Animations

```jsx
import { useScroll, useTransform, motion } from "framer-motion";

// Scroll progress of the entire page
function ParallaxHero() {
  const { scrollY } = useScroll();
  const y = useTransform(scrollY, [0, 500], [0, 150]); // parallax

  return (
    <div style={{ overflow: "hidden", height: 500 }}>
      <motion.img
        src="/hero.jpg"
        style={{ y }} // moves slower than scroll = parallax
      />
    </div>
  );
}

// Animate as element enters viewport
import { useInView } from "framer-motion";

function FadeInSection({ children }) {
  const ref = useRef(null);
  const isInView = useInView(ref, { once: true, margin: "-100px" });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, y: 50 }}
      animate={isInView ? { opacity: 1, y: 0 } : { opacity: 0, y: 50 }}
      transition={{ duration: 0.6, ease: "easeOut" }}
    >
      {children}
    </motion.div>
  );
}

// Scroll-linked progress bar
function ReadingProgress() {
  const { scrollYProgress } = useScroll();

  return (
    <motion.div
      className="fixed top-0 left-0 right-0 h-1 bg-primary origin-left"
      style={{ scaleX: scrollYProgress }} // width = scroll percentage
    />
  );
}
```

---

## 11. Page Transitions

```jsx
// With React Router v6
import { AnimatePresence } from "framer-motion";
import { useLocation, Routes, Route } from "react-router-dom";

const pageVariants = {
  initial: { opacity: 0, y: 20 },
  enter: { opacity: 1, y: 0 },
  exit: { opacity: 0, y: -20 },
};

function AnimatedRoutes() {
  const location = useLocation();

  return (
    <AnimatePresence mode="wait">
      <Routes location={location} key={location.pathname}>
        <Route
          path="/"
          element={
            <PageWrapper>
              <HomePage />
            </PageWrapper>
          }
        />
        <Route
          path="/students"
          element={
            <PageWrapper>
              <StudentsPage />
            </PageWrapper>
          }
        />
        <Route
          path="/about"
          element={
            <PageWrapper>
              <AboutPage />
            </PageWrapper>
          }
        />
      </Routes>
    </AnimatePresence>
  );
}

function PageWrapper({ children }) {
  return (
    <motion.div
      variants={pageVariants}
      initial="initial"
      animate="enter"
      exit="exit"
      transition={{ duration: 0.3, ease: "easeInOut" }}
    >
      {children}
    </motion.div>
  );
}
```

---

## 12. Performance Guidelines

```
Always use transform and opacity for animations:
  ✓ x, y, scale, rotate, opacity
  — GPU-accelerated, don't cause layout recalculation

Avoid animating layout properties:
  ✗ width, height, top, left, margin, padding
  — cause layout recalculation, expensive

Use will-change sparingly:
  style={{ willChange: "transform" }}  — hints to browser
  Only add when needed — can hurt if overused

Reduce motion for accessibility:
  import { useReducedMotion } from "framer-motion";
  const shouldReduceMotion = useReducedMotion();
  // Respect user's "reduce motion" system preference

Prefer variants for orchestrated animations:
  variants + staggerChildren > complex nested animations

Don't animate what's off-screen:
  Use useInView to only animate when element is visible
  AnimatePresence exit animations run before DOM removal
```

```jsx
// Accessible animation component
function AccessibleMotion({ children, ...props }) {
  const shouldReduceMotion = useReducedMotion();

  if (shouldReduceMotion) {
    return <>{children}</>; // no animation if user prefers reduced motion
  }

  return <motion.div {...props}>{children}</motion.div>;
}
```

---

## Summary

```
Core API:
  motion.div, motion.button, etc. — animatable HTML elements
  motion(Component) — wrap custom components

Three animation props:
  initial   — starting state (before first animation)
  animate   — target state
  exit      — state when removed (requires AnimatePresence parent)

transition:
  duration, delay, ease — timing
  type: "spring", stiffness, damping — physics-based (more natural)
  repeat, repeatType — looping

Variants:
  Named animation states: { hidden: {...}, visible: {...} }
  parent.animate = "visible" propagates to children automatically
  staggerChildren: delay each child — cascade effects

AnimatePresence:
  Wraps conditionally rendered elements
  Lets exit animations complete before DOM removal
  Each child must have a unique key

Gestures:
  whileHover, whileTap, whileDrag, whileFocus
  drag, dragConstraints, onDragEnd — swipeable elements

Layout:
  layout prop — animate size/position changes
  layoutId — shared element transitions between DOM positions
  layout="position" — only animate position (not size)

useAnimation:
  controls = useAnimation()
  controls.start({}) — programmatic animation
  await controls.start({}) — sequential animations

Performance:
  Animate transform + opacity only (GPU-accelerated)
  Avoid width, height, margin — cause layout recalculation
  useReducedMotion() — respect accessibility preferences
  useInView() — only animate when visible
```

---

_Next: [20 — Performance Optimization](./20%20-%20Performance%20Optimization.md)_
