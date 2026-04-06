> shadcn/ui is not a traditional component library — you don't install it as a dependency and import from it. You copy the component source code into your project and own it. This, combined with Radix UI's accessible primitives underneath, gives you the best of both worlds: production-quality accessible components that you can fully customise.

---

## Table of Contents

1. [What shadcn/ui Actually Is](#1-what-shadcnui-actually-is)
2. [Setup](#2-setup)
3. [Core Components Reference](#3-core-components-reference)
4. [Customising Components](#4-customising-components)
5. [Building with Radix Primitives Directly](#5-building-with-radix-primitives-directly)
6. [The cn() Utility](#6-the-cn-utility)
7. [Form Integration (shadcn + RHF + Zod)](#7-form-integration-shadcn--rhf--zod)
8. [Theming and CSS Variables](#8-theming-and-css-variables)
9. [Creating Custom Components in the shadcn Style](#9-creating-custom-components-in-the-shadcn-style)

---

## 1. What shadcn/ui Actually Is

```
Traditional library (e.g. MUI, Chakra):
  npm install @mui/material
  import { Button } from "@mui/material"
  → You use their code. Updating = npm update. Customising = overrides.

shadcn/ui:
  npx shadcn@latest add button
  → Copies Button source into src/components/ui/button.tsx
  → You OWN the code. It's yours to read, modify, and extend.
  → Built on Radix UI primitives (accessibility) + Tailwind CSS (styling)
```

**Radix UI** provides headless, accessible component primitives — the behaviour and ARIA attributes, no styling. **shadcn/ui** adds Tailwind CSS styling on top. You get accessibility for free, styling you can change completely.

---

## 2. Setup

```bash
# In a Next.js project
npx shadcn@latest init

# Then add components one at a time
npx shadcn@latest add button
npx shadcn@latest add input
npx shadcn@latest add dialog
npx shadcn@latest add select
npx shadcn@latest add form
npx shadcn@latest add table
npx shadcn@latest add dropdown-menu
npx shadcn@latest add toast
```

Each command copies files into `src/components/ui/`. The component code is now yours — read it, understand it, modify it.

---

## 3. Core Components Reference

### Button

```tsx
import { Button } from "@/components/ui/button";

<Button>Default</Button>
<Button variant="destructive">Delete</Button>
<Button variant="outline">Cancel</Button>
<Button variant="secondary">Secondary</Button>
<Button variant="ghost">Ghost</Button>
<Button variant="link">Link style</Button>

<Button size="sm">Small</Button>
<Button size="lg">Large</Button>
<Button size="icon"><TrashIcon /></Button>

<Button disabled>Disabled</Button>
<Button disabled>
  <Loader2Icon className="mr-2 h-4 w-4 animate-spin" />
  Loading...
</Button>
```

### Input

```tsx
import { Input } from "@/components/ui/input";

<Input type="email" placeholder="Enter your email" />
<Input type="password" />
<Input disabled />

// Typically wrapped with Label and error display
<div className="space-y-2">
  <Label htmlFor="email">Email</Label>
  <Input id="email" type="email" {...register("email")} />
  {errors.email && <p className="text-sm text-destructive">{errors.email.message}</p>}
</div>
```

### Dialog (Modal)

```tsx
import {
  Dialog, DialogContent, DialogDescription,
  DialogFooter, DialogHeader, DialogTitle, DialogTrigger,
} from "@/components/ui/dialog";

function DeleteStudentDialog({ student, onConfirm }: { student: Student; onConfirm: () => void }) {
  const [open, setOpen] = useState(false);

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger asChild>
        <Button variant="destructive" size="sm">Delete</Button>
      </DialogTrigger>

      <DialogContent>
        <DialogHeader>
          <DialogTitle>Delete Student</DialogTitle>
          <DialogDescription>
            Are you sure you want to delete {student.name}? This cannot be undone.
          </DialogDescription>
        </DialogHeader>

        <DialogFooter>
          <Button variant="outline" onClick={() => setOpen(false)}>Cancel</Button>
          <Button
            variant="destructive"
            onClick={() => { onConfirm(); setOpen(false); }}
          >
            Delete
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}
```

### Select

```tsx
import {
  Select, SelectContent, SelectItem,
  SelectTrigger, SelectValue,
} from "@/components/ui/select";

<Select value={grade} onValueChange={setGrade}>
  <SelectTrigger>
    <SelectValue placeholder="Select grade" />
  </SelectTrigger>
  <SelectContent>
    {[6,7,8,9,10,11,12,13].map(g => (
      <SelectItem key={g} value={String(g)}>Grade {g}</SelectItem>
    ))}
  </SelectContent>
</Select>
```

### Dropdown Menu

```tsx
import {
  DropdownMenu, DropdownMenuContent, DropdownMenuItem,
  DropdownMenuLabel, DropdownMenuSeparator, DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";

<DropdownMenu>
  <DropdownMenuTrigger asChild>
    <Button variant="ghost" size="icon">
      <MoreHorizontalIcon />
    </Button>
  </DropdownMenuTrigger>
  <DropdownMenuContent align="end">
    <DropdownMenuLabel>Actions</DropdownMenuLabel>
    <DropdownMenuItem onClick={handleView}>View profile</DropdownMenuItem>
    <DropdownMenuItem onClick={handleEdit}>Edit</DropdownMenuItem>
    <DropdownMenuSeparator />
    <DropdownMenuItem onClick={handleDelete} className="text-destructive">
      Delete
    </DropdownMenuItem>
  </DropdownMenuContent>
</DropdownMenu>
```

### Table

```tsx
import {
  Table, TableBody, TableCaption, TableCell,
  TableHead, TableHeader, TableRow,
} from "@/components/ui/table";

<Table>
  <TableCaption>Grade 11 students — A/L stream</TableCaption>
  <TableHeader>
    <TableRow>
      <TableHead>Name</TableHead>
      <TableHead>Subject Stream</TableHead>
      <TableHead className="text-right">Average Score</TableHead>
    </TableRow>
  </TableHeader>
  <TableBody>
    {students.map(student => (
      <TableRow key={student.id}>
        <TableCell className="font-medium">{student.name}</TableCell>
        <TableCell>{student.stream}</TableCell>
        <TableCell className="text-right">{student.averageScore}%</TableCell>
      </TableRow>
    ))}
  </TableBody>
</Table>
```

### Toast (Sonner)

```tsx
// shadcn uses Sonner for toasts
import { toast } from "sonner";
import { Toaster } from "@/components/ui/sonner";

// In your layout
<Toaster position="top-right" richColors />

// In any component
toast.success("Student saved successfully");
toast.error("Failed to save: " + error.message);
toast.info("Changes will take effect next login");
toast.warning("Score is below passing threshold");
toast.loading("Saving...", { id: "save" });
toast.dismiss("save");
```

---

## 4. Customising Components

Since you own the code, customisation is straightforward:

```tsx
// src/components/ui/button.tsx — the actual source you own
const buttonVariants = cva(
  "inline-flex items-center justify-center gap-2 whitespace-nowrap rounded-md text-sm font-medium...",
  {
    variants: {
      variant: {
        default:     "bg-primary text-primary-foreground shadow hover:bg-primary/90",
        destructive: "bg-destructive text-destructive-foreground shadow-sm hover:bg-destructive/90",
        outline:     "border border-input bg-background shadow-sm hover:bg-accent...",
        // Add your own variant:
        school: "bg-[#c9a84c] text-white shadow hover:bg-[#b8973b]",
      },
      size: {
        default: "h-9 px-4 py-2",
        sm:      "h-8 rounded-md px-3 text-xs",
        lg:      "h-10 rounded-md px-8",
        icon:    "h-9 w-9",
        // Add your own size:
        xl:      "h-12 rounded-md px-10 text-base",
      },
    },
  }
);

// Now you can use:
<Button variant="school">Apply Now</Button>
<Button size="xl">Large CTA</Button>
```

---

## 5. Building with Radix Primitives Directly

When shadcn doesn't have a component you need, use Radix directly.

```bash
npm install @radix-ui/react-accordion
npm install @radix-ui/react-tabs
npm install @radix-ui/react-slider
```

```tsx
// Building an accessible accordion with Radix + Tailwind
import * as Accordion from "@radix-ui/react-accordion";
import { ChevronDownIcon } from "lucide-react";

function FAQ({ items }: { items: { question: string; answer: string }[] }) {
  return (
    <Accordion.Root type="single" collapsible className="space-y-2">
      {items.map((item, i) => (
        <Accordion.Item
          key={i}
          value={String(i)}
          className="border rounded-lg overflow-hidden"
        >
          <Accordion.Header>
            <Accordion.Trigger className="flex w-full items-center justify-between p-4 font-medium text-left hover:bg-muted/50 transition-colors data-[state=open]:bg-muted/50">
              {item.question}
              <ChevronDownIcon className="h-4 w-4 transition-transform data-[state=open]:rotate-180" />
              {/* data-[state=open] is Radix's way of exposing state to CSS */}
            </Accordion.Trigger>
          </Accordion.Header>
          <Accordion.Content className="overflow-hidden data-[state=closed]:animate-accordion-up data-[state=open]:animate-accordion-down">
            <div className="p-4 pt-0 text-muted-foreground">
              {item.answer}
            </div>
          </Accordion.Content>
        </Accordion.Item>
      ))}
    </Accordion.Root>
  );
}
// Radix handles: keyboard navigation, ARIA attributes, focus management
// You handle: all styling
```

---

## 6. The `cn()` Utility

`cn()` merges Tailwind classes intelligently — later classes override earlier ones of the same property.

```tsx
import { cn } from "@/lib/utils";
// cn = clsx + tailwind-merge

// Basic merging
cn("px-4 py-2", "px-6")        // "py-2 px-6"  — px-6 overrides px-4
cn("text-sm", "text-lg")       // "text-lg"    — text-lg overrides text-sm
cn("bg-red-500", "bg-blue-500") // "bg-blue-500" — later wins

// Conditional classes
cn(
  "base-classes",
  isActive && "active-classes",      // adds if truthy
  hasError ? "error" : "no-error",   // ternary
  { "hidden": !isVisible },          // object syntax
)

// In components — the standard pattern
function Badge({
  className,
  variant = "default",
  children,
}: BadgeProps) {
  return (
    <div
      className={cn(
        "inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-semibold",
        variant === "success" && "bg-green-100 text-green-800",
        variant === "error"   && "bg-red-100 text-red-800",
        variant === "default" && "bg-primary/10 text-primary",
        className  // allows caller to override/extend
      )}
    >
      {children}
    </div>
  );
}
```

---

## 7. Form Integration (shadcn + RHF + Zod)

shadcn/ui ships a Form component that integrates React Hook Form with accessible form fields.

```tsx
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { z } from "zod";
import {
  Form, FormControl, FormDescription,
  FormField, FormItem, FormLabel, FormMessage,
} from "@/components/ui/form";
import { Input }  from "@/components/ui/input";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Button } from "@/components/ui/button";

const schema = z.object({
  studentName: z.string().min(2, "Name must be at least 2 characters"),
  grade:       z.string({ required_error: "Please select a grade" }),
  parentEmail: z.string().email("Invalid email address"),
});

type FormData = z.infer<typeof schema>;

function AdmissionsForm() {
  const form = useForm<FormData>({
    resolver: zodResolver(schema),
    defaultValues: { studentName: "", grade: "", parentEmail: "" },
  });

  async function onSubmit(data: FormData) {
    await submitAdmissions(data);
  }

  return (
    <Form {...form}>
      <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-6">

        <FormField
          control={form.control}
          name="studentName"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Student Name</FormLabel>
              <FormControl>
                <Input placeholder="Full name" {...field} />
              </FormControl>
              <FormDescription>As it appears on the birth certificate</FormDescription>
              <FormMessage />  {/* shows error.message automatically */}
            </FormItem>
          )}
        />

        <FormField
          control={form.control}
          name="grade"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Grade Applying For</FormLabel>
              <Select onValueChange={field.onChange} defaultValue={field.value}>
                <FormControl>
                  <SelectTrigger>
                    <SelectValue placeholder="Select grade" />
                  </SelectTrigger>
                </FormControl>
                <SelectContent>
                  {[1,2,3,4,5,6,7,8,9,10,11,12,13].map(g => (
                    <SelectItem key={g} value={String(g)}>Grade {g}</SelectItem>
                  ))}
                </SelectContent>
              </Select>
              <FormMessage />
            </FormItem>
          )}
        />

        <FormField
          control={form.control}
          name="parentEmail"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Parent's Email</FormLabel>
              <FormControl>
                <Input type="email" placeholder="parent@email.com" {...field} />
              </FormControl>
              <FormMessage />
            </FormItem>
          )}
        />

        <Button type="submit" disabled={form.formState.isSubmitting}>
          {form.formState.isSubmitting ? "Submitting..." : "Submit Application"}
        </Button>

      </form>
    </Form>
  );
}
```

---

## 8. Theming and CSS Variables

shadcn/ui uses CSS variables for theming. All colours are defined in `globals.css`.

```css
/* src/app/globals.css */
:root {
  --background: 0 0% 100%;
  --foreground: 222.2 84% 4.9%;
  --primary: 222.2 47.4% 11.2%;
  --primary-foreground: 210 40% 98%;
  --secondary: 210 40% 96.1%;
  --muted: 210 40% 96.1%;
  --accent: 210 40% 96.1%;
  --destructive: 0 84.2% 60.2%;
  --border: 214.3 31.8% 91.4%;
  --ring: 222.2 84% 4.9%;
  /* ... */
}

.dark {
  --background: 222.2 84% 4.9%;
  --foreground: 210 40% 98%;
  --primary: 210 40% 98%;
  /* ... */
}
```

Customise by changing these values. The CWWKCC palette would replace `--primary` with the school's gold colour:

```css
:root {
  --primary: 43 60% 55%;            /* #c9a84c in HSL */
  --primary-foreground: 0 0% 100%;  /* white text on primary */
}
```

---

## 9. Creating Custom Components in the shadcn Style

When building new components for Project Nexus, follow the same patterns.

```tsx
// src/components/ui/score-badge.tsx
import { cva, type VariantProps } from "class-variance-authority";
import { cn } from "@/lib/utils";

const scoreBadgeVariants = cva(
  "inline-flex items-center rounded-md px-2 py-1 text-xs font-semibold",
  {
    variants: {
      grade: {
        A: "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200",
        B: "bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200",
        C: "bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200",
        F: "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200",
      },
    },
    defaultVariants: { grade: "C" },
  }
);

interface ScoreBadgeProps extends VariantProps<typeof scoreBadgeVariants> {
  score:     number;
  className?: string;
}

function getGrade(score: number): "A" | "B" | "C" | "F" {
  if (score >= 75) return "A";
  if (score >= 65) return "B";
  if (score >= 50) return "C";
  return "F";
}

export function ScoreBadge({ score, className }: ScoreBadgeProps) {
  const grade = getGrade(score);
  return (
    <span className={cn(scoreBadgeVariants({ grade }), className)}>
      {score}% — {grade}
    </span>
  );
}

// Usage
<ScoreBadge score={91} />          // green "91% — A"
<ScoreBadge score={42} />          // red "42% — F"
<ScoreBadge score={68} className="ml-2" />  // extendable
```

---

## Summary

```
shadcn/ui:
  Not a dependency — it's source code copied into your project
  You own and modify the component code directly
  Built on: Radix UI (accessibility) + Tailwind CSS (styling)

Key commands:
  npx shadcn@latest init           — set up the project
  npx shadcn@latest add button     — copy button component into ui/
  npx shadcn@latest add form       — copies form + label components

Core components:
  Button, Input, Label             — basic inputs
  Dialog                           — modal with trigger, content, header, footer
  Select                           — dropdown with trigger, content, items
  DropdownMenu                     — context menu / action menu
  Table                            — data table
  Toast (Sonner)                   — notification toasts

Customisation:
  Edit ui/*.tsx directly — it's your code
  Add variants to cva() — new button/badge variants
  Change globals.css CSS variables — theme colours

Radix primitives:
  Use when shadcn doesn't have a component
  @radix-ui/react-accordion, -tabs, -slider, etc.
  data-[state=open] — Radix state exposed as CSS attributes

cn() utility:
  Merges Tailwind classes, later overrides earlier
  cn("base", isActive && "active", { hidden: !show }, className)
  Accept className in every component for extensibility

Form integration:
  <Form>, <FormField>, <FormItem>, <FormLabel>, <FormControl>, <FormMessage>
  FormMessage automatically shows the field's error.message
  FormField.render receives { field } — spread onto input

Theming:
  CSS variables in globals.css — :root and .dark
  Change --primary to your brand colour
  All components use these variables automatically

Custom components:
  cva() for variants — same pattern as shadcn uses internally
  cn() for class merging
  Accept className for extensibility
  Export from src/components/ui/ alongside shadcn components
```