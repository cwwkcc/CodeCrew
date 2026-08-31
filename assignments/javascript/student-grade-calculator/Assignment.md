## Assignment Overview

Build a small command-line-style program (running in the browser console, or a simple page with a "Run" button) that manages a class roster: adds students, calculates grade statistics, and formats a report. No DOM UI required for the core logic — this assignment is entirely about functions, data structures, and working with objects/arrays correctly.

---

## Covers

`javascript/00 - Foundations`, `javascript/01 - Loops, Data Structures & Objects`, `javascript/02 - Functions, Scope, JSON, Dates & Timers`

---

## Starting Data

```js
const students = [
  { name: "Ashan Perera", grades: [78, 82, 91, 68] },
  { name: "Nadeesha Silva", grades: [92, 95, 89, 97] },
  { name: "Thanuga Fernando", grades: [55, 61, 48, 70] },
  { name: "Vindi Jayasuriya", grades: [88, 76, 81, 79] },
];
```

---

## Your Task

Write the following functions. Each one should be a **pure function** — given the same input, it always returns the same output, with no reliance on variables outside its own parameters (Module 02's scope material is exactly what this tests).

### 1. `average(grades)`

Takes an array of numbers, returns their average, rounded to 1 decimal place.

```js
average([78, 82, 91, 68]); // → 79.75 → rounds to 79.8
```

### 2. `letterGrade(average)`

Takes an average, returns a letter grade using this scale:

```
90+     → "A"
80–89   → "B"
70–79   → "C"
60–69   → "D"
below 60 → "F"
```

### 3. `classReport(students)`

Takes the full `students` array, returns a **new array** (don't mutate the original) of objects shaped like:

```js
{ name: "Ashan Perera", average: 79.8, letterGrade: "C" }
```

### 4. `topStudent(students)`

Returns the name of the student with the highest average. If there's a tie, return the first one encountered.

### 5. `classAverage(students)`

Returns the average of all students' averages, rounded to 1 decimal place.

### 6. `formatReportDate()`

Using the built-in `Date` object (Module 02), return today's date as a string in the format `DD/MM/YYYY`. Don't hardcode a date — it must reflect whenever the function is actually run.

---

## Putting It Together

```js
console.log(`Class Report — Generated ${formatReportDate()}`);
console.log("---");
classReport(students).forEach(({ name, average, letterGrade }) => {
  console.log(`${name}: ${average} (${letterGrade})`);
});
console.log("---");
console.log(`Top student: ${topStudent(students)}`);
console.log(`Class average: ${classAverage(students)}`);
```

---

## Constraints

- **No mutating the original `students` array or any student object.** `classReport` must return new objects — check this by logging `students` again after calling your functions and confirming it's unchanged.
- **No global/shared mutable variables holding intermediate results.** Every function should compute what it needs from its own parameters.
- Use `Array.prototype` methods (`.map()`, `.reduce()`, `.filter()`) rather than manually-written `for` loops wherever they make the code clearer — but a plain `for` loop is fine if you genuinely find it clearer for a specific piece.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — `average()` is a natural fit for `.reduce()`: sum the array, then divide by its length. Rounding to 1 decimal place: `Math.round(value * 10) / 10`.

> 💡 **Hint 2** — `classReport` is a `.map()` over `students`, where each student is transformed into a new, differently-shaped object — this is exactly the kind of transformation `.map()` exists for.

> 💡 **Hint 3** — For "returning a new array without mutating the original," make sure you're not doing `students.map(s => { s.average = ...; return s; })` — that mutates `s` in place even though `.map()` returns a new array. Build a genuinely new object instead: `{ ...s, average: ... }` or `{ name: s.name, average: ... }`.

---

## Submission Guidelines

1. `gradebook.js` — all six functions
2. A short comment block at the top explaining which of the six functions you found hardest to keep "pure," and why

---

## Bonus Challenge 🏆

- 📊 Add a `gradeDistribution(students)` function returning a count of how many students got each letter grade (`{ A: 1, B: 2, C: 1, D: 0, F: 0 }`).
- 🗓️ Add a `daysUntilExamDay(examDate)` function using `Date` arithmetic to calculate days remaining until a given date.
- 💾 Serialize the full class report to a JSON string with `JSON.stringify()`, then parse it back with `JSON.parse()` and confirm the round-trip produces an equivalent (not identical-reference) object.

---

## Learning Objectives

- ✓ Write pure functions with no reliance on external mutable state.
- ✓ Choose the right array method (`map`/`filter`/`reduce`) for a given transformation.
- ✓ Work with nested objects and arrays without accidentally mutating them.
- ✓ Use `Date` and `JSON` correctly for real, practical tasks.

**Good Luck! 🚀**
