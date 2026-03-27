## Assignment Overview

Create an HTML page for a student portal. Think of it like a dashboard page you would log into at school — it shows your deadlines, announcements, results, and lets you submit an assignment. There is no CSS and no JavaScript. Everything is built using HTML only.

This assignment covers **all** the HTML topics we have learned so far. Work through it one section at a time — do not try to do everything at once.

---

## What You Need to Use

This assignment requires you to use:

- ✅ Document boilerplate: `<!DOCTYPE html>`, `<html lang>`, `<head>`, `<body>`
- ✅ Metadata tags inside `<head>`: `<meta charset>`, `<meta name="viewport">`, `<title>`, `<meta name="description">`
- ✅ Page structure tags: `<header>`, `<nav>`, `<main>`, `<section>`, `<article>`, `<footer>`
- ✅ Text tags: `<h1>`–`<h3>`, `<p>`, `<strong>`, `<em>`, `<small>`, `<mark>`, `<abbr>`, `<time>`
- ✅ List tags: `<ul>`, `<ol>`, `<li>`, `<dl>`, `<dt>`, `<dd>`
- ✅ Link tags: `<a href>` with `aria-current="page"` and `aria-label`
- ✅ Table tags: `<table>`, `<caption>`, `<thead>`, `<tbody>`, `<tfoot>`, `<th>`, `<td>` with `colspan` and `rowspan`
- ✅ Form tags: `<form>`, `<fieldset>`, `<legend>`, `<label>`, `<input>`, `<select>`, `<option>`, `<optgroup>`, `<textarea>`, `<datalist>`, `<button>`
- ✅ Accordion tags: `<details>`, `<summary>`
- ✅ Media tags: `<figure>`, `<figcaption>`, `<video>`, `<source>`, `<track>`, `<iframe>`
- ✅ Open Graph `<meta>` tags (for social media sharing previews)
- ✅ **No CSS. No JavaScript. No `style` attributes anywhere.**

---

## Assignment Requirements

### Your page must have these 8 sections:

1. **Document Shell**
    
    - The standard HTML boilerplate with `lang="en-LK"` on the `<html>` tag
    - A complete `<head>` with the charset, viewport, title, description, author, canonical link, and Open Graph tags
2. **Navigation**
    
    - A skip link — a special link at the very top of the page that jumps to the main content
    - A `<header>` with a logo image and a `<nav>` containing 5 links: Dashboard, Assignments, Results, Resources, Profile
    - A second `<nav>` showing breadcrumbs: Home → Dashboard
3. **Dashboard — Main Content**
    
    - A `<h1>` welcome heading
    - Three `<section>` blocks: Upcoming Deadlines (a numbered list), Announcements (separate article blocks), Quick Stats (a definition list)
4. **Results Table**
    
    - A table showing exam results for two terms and at least 6 subjects
    - Must use `colspan`, `rowspan`, and a `scope` attribute on every single `<th>`
5. **Assignment Submission Form**
    
    - Three groups of inputs (each wrapped in `<fieldset>` with a `<legend>` title): Student Information, Assignment Details, Submission Options
    - Must include a dropdown with grouped options, a text field with autocomplete suggestions, a textarea, a file upload, radio buttons, and checkboxes
6. **Learning Resources Accordion**
    
    - Five collapsible panels using `<details>` and `<summary>`
    - The "C++ Books" panel must be open when the page loads
7. **Embedded Content**
    
    - An image section using `<picture>` (for serving different image formats)
    - A video section using `<video>` with subtitle support
    - An embedded help page using `<iframe>`
8. **Footer**
    
    - A `<footer>` with school contact details inside an `<address>` tag

---

## Page Structure

Your finished page should:

1. **Open and close correctly** — every tag that opens must close in the right order
2. **Be readable without CSS** — the structure alone should make the page understandable
3. **Have a `<label>` for every input** — connect each label to its input using matching `for` and `id` values
4. **Have `scope` on every `<th>`** — this tells browsers whether the heading applies to a row or a column

---

## Sample Output

Here is the skeleton of what your finished file should look like. Every `<!-- comment -->` is a section you need to fill in:

```html
<!DOCTYPE html>
<html lang="en-LK">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Dashboard — CodeCrew Student Portal</title>
  <meta name="description" content="Your student dashboard — assignments, results, resources.">
  <meta name="author" content="Your Name">
  <link rel="canonical" href="https://portal.codecrew.lk/dashboard">
  <meta property="og:title" content="Dashboard — CodeCrew Student Portal">
  <meta property="og:type" content="website">
  <meta property="og:image" content="https://portal.codecrew.lk/og-image.jpg">
  <meta property="og:description" content="Your student dashboard for CodeCrew.">
  <link rel="icon" href="/favicon.ico">
</head>
<body>

  <a href="#main-content">Skip to main content</a>

  <header>
    <img src="/logo.png" alt="CodeCrew" width="120" height="40">
    <nav aria-label="Main navigation">
      <ul>
        <li><a href="/dashboard" aria-current="page">Dashboard</a></li>
        <li><a href="/assignments">Assignments</a></li>
        <li><a href="/results">Results</a></li>
        <li><a href="/resources">Resources</a></li>
        <li><a href="/profile">Profile</a></li>
      </ul>
    </nav>
  </header>

  <nav aria-label="Breadcrumb navigation">
    <ol>
      <li><a href="/">Home</a></li>
      <li>Dashboard</li>
    </ol>
  </nav>

  <main id="main-content">

    <h1>Welcome back, [Your Name]</h1>

    <section>
      <h2>Upcoming Deadlines</h2>
      <!-- numbered list goes here -->
    </section>

    <section>
      <h2>Announcements</h2>
      <!-- article elements go here -->
    </section>

    <section>
      <h2>Quick Stats</h2>
      <!-- description list goes here -->
    </section>

    <section>
      <h2>Exam Results</h2>
      <!-- table goes here -->
    </section>

    <section>
      <h2>Submit Assignment</h2>
      <!-- form goes here -->
    </section>

    <section>
      <h2>Learning Resources</h2>
      <!-- details/summary panels go here -->
    </section>

    <section>
      <h2>Media</h2>
      <!-- picture, video, and iframe go here -->
    </section>

  </main>

  <footer>
    <!-- address goes here -->
  </footer>

</body>
</html>
```

---

## Detailed Instructions

### Step 1: Write the Document Shell

Type this out first. Open it in a browser and make sure it shows a blank page with no errors before you continue:

```html
<!DOCTYPE html>
<html lang="en-LK">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Dashboard — CodeCrew Student Portal</title>
</head>
<body>

</body>
</html>
```

**Important**: `<meta charset="UTF-8">` must always be the **very first line inside `<head>`**, even before `<title>`. The browser needs to read the encoding before anything else.

### Step 2: Complete the `<head>`

Now add the rest of the metadata. Paste these lines between `<meta name="viewport">` and `</head>`:

```html
<meta name="description" content="Your student dashboard — view assignments, results, and resources.">
<meta name="author" content="Your Name">
<link rel="canonical" href="https://portal.codecrew.lk/dashboard">
<meta property="og:title" content="Dashboard — CodeCrew Student Portal">
<meta property="og:type" content="website">
<meta property="og:image" content="https://portal.codecrew.lk/og-image.jpg">
<meta property="og:description" content="Your student dashboard for CodeCrew.">
<link rel="icon" href="/favicon.ico">
```

The `og:` tags control how the page looks when someone shares the link on WhatsApp, Facebook, or LinkedIn — you have seen those preview cards with a title, image, and description. That is what these tags produce.

### Step 3: Build the Navigation

The skip link is a plain `<a>` tag. It must be the **first thing inside `<body>`**, before the `<header>`. It lets keyboard users jump straight to the content and skip the navigation:

```html
<body>
  <a href="#main-content">Skip to main content</a>

  <header>
    <img src="/logo.png" alt="CodeCrew" width="120" height="40">
    <nav aria-label="Main navigation">
      <ul>
        <li><a href="/dashboard" aria-current="page">Dashboard</a></li>
        <li><a href="/assignments">Assignments</a></li>
        <li><a href="/results">Results</a></li>
        <li><a href="/resources">Resources</a></li>
        <li><a href="/profile">Profile</a></li>
      </ul>
    </nav>
  </header>

  <nav aria-label="Breadcrumb navigation">
    <ol>
      <li><a href="/">Home</a></li>
      <li>Dashboard</li>   <!-- plain text, NOT a link — you are already on this page -->
    </ol>
  </nav>
```

Notice the two `<nav>` elements have different `aria-label` values. This is so a screen reader can tell them apart — one is the main menu, one is the breadcrumb trail.

### Step 4: Build the Dashboard Sections

**Upcoming Deadlines** — use `<ol>` (ordered list) because the deadlines have a specific order, earliest first. Each item uses `<strong>` for the title, `<time>` for the date, and `<small>` for the subject:

```html
<section>
  <h2>Upcoming Deadlines</h2>
  <ol>
    <li>
      <strong>Data Structures Assignment 3</strong>
      — due <time datetime="2026-04-10">10 April 2026</time>
      <small>Computer Science</small>
    </li>
    <li>
      <strong>Essay: The Industrial Revolution</strong>
      — due <time datetime="2026-04-14">14 April 2026</time>
      <small>History</small>
    </li>
    <!-- add 2 more items -->
  </ol>
</section>
```

**Announcements** — each announcement is its own `<article>` because each one is a standalone piece of content. Use `<mark>` to highlight something important in one of them, and `<abbr>` to show a short form with its full meaning in another:

```html
<section>
  <h2>Announcements</h2>

  <article>
    <h3>Term Test Timetable Released</h3>
    <p>The <mark>Term 1 test timetable</mark> is now on the notice board.</p>
    <footer>
      Posted by <cite>Mrs. Tharindrie Perera</cite>
      on <time datetime="2026-03-15">15 March 2026</time>
    </footer>
  </article>

  <article>
    <h3>ICT Society Meeting</h3>
    <p>The <abbr title="Kannangara ICT Society">KITS</abbr> meeting is this Friday at 3 PM.</p>
    <footer>
      Posted by <cite>Mr. Bandara</cite>
      on <time datetime="2026-03-18">18 March 2026</time>
    </footer>
  </article>

  <!-- add one more article -->
</section>
```

**Quick Stats** — a description list (`<dl>`) is perfect for label–value pairs like these. `<dt>` is the label (definition term), `<dd>` is the value (definition description):

```html
<section>
  <h2>Quick Stats</h2>
  <dl>
    <dt>Active Assignments</dt>
    <dd>4</dd>

    <dt>Completed This Term</dt>
    <dd>12</dd>

    <dt>Average Score</dt>
    <dd>78%</dd>

    <dt>Rank in Class</dt>
    <dd>5th</dd>

    <dt>Attendance</dt>
    <dd>94%</dd>

    <dt>Books Borrowed</dt>
    <dd>3</dd>
  </dl>
</section>
```

### Step 5: Build the Results Table

The trickiest part of this assignment. Build it one row at a time.

`rowspan="6"` on the Term header means that one cell stretches down to cover 6 rows — one for each subject. `colspan="2"` on the Grade header means that one cell stretches across two columns — Score and Letter:

```html
<table>
  <caption>Term Examination Results — 2026</caption>

  <thead>
    <tr>
      <th scope="col">Term</th>
      <th scope="col">Subject</th>
      <th scope="colgroup" colspan="2">Grade</th>
    </tr>
    <tr>
      <th scope="col">—</th>
      <th scope="col">—</th>
      <th scope="col">Score</th>
      <th scope="col">Letter</th>
    </tr>
  </thead>

  <tbody>
    <tr>
      <th scope="rowgroup" rowspan="6">Term 1</th>  <!-- spans 6 rows below it -->
      <th scope="row">Mathematics</th>
      <td>88</td>
      <td>A</td>
    </tr>
    <tr>
      <!-- no Term cell here — the rowspan above covers this row -->
      <th scope="row">Science</th>
      <td>74</td>
      <td>B</td>
    </tr>
    <!-- add 4 more subjects for Term 1 -->

    <tr>
      <th scope="rowgroup" rowspan="6">Term 2</th>  <!-- new rowspan for Term 2 -->
      <th scope="row">Mathematics</th>
      <td>91</td>
      <td>A+</td>
    </tr>
    <!-- add 5 more subjects for Term 2 -->
  </tbody>

  <tfoot>
    <tr>
      <th scope="row" colspan="2">Term 1 Average</th>
      <td>82</td>
      <td>A</td>
    </tr>
    <tr>
      <th scope="row" colspan="2">Term 2 Average</th>
      <td>85</td>
      <td>A</td>
    </tr>
  </tfoot>

</table>
```

### Step 6: Build the Form

The form has three `<fieldset>` blocks. Each fieldset groups related inputs together and must have a `<legend>` as its first child — the legend is the group's title.

Every single `<input>`, `<select>`, and `<textarea>` needs a `<label>`. The label's `for` value must exactly match the input's `id` value:

```html
<form action="/submit" method="post">

  <fieldset>
    <legend>Student Information</legend>

    <label for="full-name">Full Name</label>
    <input type="text" id="full-name" name="full_name" autocomplete="name" required>

    <label for="index-no">Index Number</label>
    <input type="text" id="index-no" name="index_number"
           pattern="[0-9]{4,6}" title="Must be a 4 to 6 digit number" required>

    <label for="email">School Email</label>
    <input type="email" id="email" name="email" autocomplete="email" required>

    <label for="grade">Grade</label>
    <select id="grade" name="grade" required>
      <option value="">-- Select your grade --</option>
      <optgroup label="Junior (8–10)">
        <option value="8">Grade 8</option>
        <option value="9">Grade 9</option>
        <option value="10">Grade 10</option>
      </optgroup>
      <optgroup label="Senior (11–13)">
        <option value="11">Grade 11</option>
        <option value="12">Grade 12</option>
        <option value="13">Grade 13</option>
      </optgroup>
    </select>
  </fieldset>

  <fieldset>
    <legend>Assignment Details</legend>

    <!-- datalist gives the user autocomplete suggestions while still letting them
         type anything they want — it is NOT the same as a <select> dropdown -->
    <label for="subject">Subject</label>
    <input type="text" id="subject" name="subject" list="subject-options" required>
    <datalist id="subject-options">
      <option value="Mathematics">
      <option value="Science">
      <option value="Computer Science">
      <option value="English">
      <option value="History">
      <option value="Commerce">
    </datalist>

    <label for="term">Term</label>
    <select id="term" name="term" required>
      <option value="">-- Select term --</option>
      <option value="1">First Term</option>
      <option value="2">Second Term</option>
      <option value="3">Third Term</option>
    </select>

    <label for="comments">Comments</label>
    <textarea id="comments" name="comments" rows="4" maxlength="500"></textarea>

    <label for="file">Submission File (PDF or DOCX only)</label>
    <input type="file" id="file" name="file" accept=".pdf,.docx">
  </fieldset>

  <fieldset>
    <legend>Submission Options</legend>

    <!-- all three radio buttons share the same name="submission_type"
         so the browser knows they belong to the same group -->
    <label><input type="radio" name="submission_type" value="original" checked> Original Work</label>
    <label><input type="radio" name="submission_type" value="revision"> Revision</label>
    <label><input type="radio" name="submission_type" value="late"> Late Submission</label>

    <label>
      <input type="checkbox" name="own_work" value="yes" required>
      I confirm this is my own work
    </label>

    <label>
      <input type="checkbox" name="notify" value="yes">
      Notify me by email when marked
    </label>
  </fieldset>

  <button type="submit">Submit Assignment</button>
  <button type="reset">Clear Form</button>

</form>
```

### Step 7: Build the Accordion

`<details>` is a collapsible panel. `<summary>` is the clickable title. Everything else inside `<details>` is the content that shows or hides. Add `open` to start it expanded:

```html
<details open>
  <summary>C++ Books</summary>
  <ul>
    <li>
      <a href="/resources/cpp-primer.pdf" download>C++ Primer — Download</a>
    </li>
    <li>
      <a href="https://isocpp.org" target="_blank" rel="noopener noreferrer">
        ISO C++ — <q>The home of Standard C++</q>
      </a>
    </li>
    <li>
      <a href="/resources/stroustrup.pdf" download>The C++ Programming Language</a>
    </li>
  </ul>
</details>

<details>
  <summary>Competition Archives</summary>
  <!-- add 3 links here -->
</details>

<details>
  <summary>Video Tutorials</summary>
  <!-- add 3 links here -->
</details>

<details>
  <summary>Practice Sites</summary>
  <!-- add 3 links here -->
</details>

<details>
  <summary>Past Papers</summary>
  <!-- add 3 links here -->
</details>
```

### Step 8: Add Embedded Content

**Picture** — `<picture>` lets the browser pick the best image to load. List the modern formats first (WebP), and put the regular JPEG as the fallback `<img>` at the bottom. The `<img>` is required and must always be the last child of `<picture>`:

```html
<figure>
  <picture>
    <source type="image/webp" srcset="/images/campus.webp">
    <source media="(max-width: 600px)" srcset="/images/campus-mobile.jpg">
    <img src="/images/campus.jpg" alt="Aerial view of the school campus"
         width="800" height="450" loading="lazy">
  </picture>
  <figcaption>Figure 1. C.W.W. Kannangara Central College, Mathugama.</figcaption>
</figure>
```

**Video** — list two `<source>` formats so it works in all browsers. The `<track>` tag adds subtitles. The `<p>` at the bottom is the fallback message for browsers that cannot play video at all:

```html
<figure>
  <video controls width="800" height="450" poster="/videos/tutorial-thumb.jpg">
    <source src="/videos/tutorial.webm" type="video/webm">
    <source src="/videos/tutorial.mp4"  type="video/mp4">
    <track kind="subtitles" src="/videos/tutorial-en.vtt"
           srclang="en" label="English" default>
    <p>Your browser cannot play this video.
       <a href="/videos/tutorial.mp4">Download it here.</a>
    </p>
  </video>
  <figcaption>Figure 2. Introduction to the student portal.</figcaption>
</figure>
```

**iFrame** — embeds another page inside this one. `title` is required for accessibility. `sandbox` restricts what the embedded page is allowed to do for security:

```html
<iframe
  src="/help/faq"
  title="Frequently Asked Questions"
  width="100%"
  height="400"
  loading="lazy"
  sandbox="allow-scripts allow-same-origin">
</iframe>
```

### Step 9: Write the Footer

`<address>` is the tag for contact information. Use `<br>` to put each detail on its own line:

```html
<footer>
  <address>
    C.W.W. Kannangara Central College<br>
    Mathugama, Sri Lanka<br>
    <a href="mailto:info@kannangara.edu.lk">info@kannangara.edu.lk</a>
  </address>
  <p><small>© 2026 CodeCrew — Kannangara ICT Society</small></p>
</footer>
```

---

## Common Mistakes to Avoid

❌ **Putting `<meta charset>` anywhere except first in `<head>`**

```html
<head>
  <title>My Page</title>
  <meta charset="UTF-8">  <!-- WRONG — must come before <title> -->
</head>
```

✅ **Correct**:

```html
<head>
  <meta charset="UTF-8">  <!-- Always the very first line inside <head> -->
  <title>My Page</title>
</head>
```

---

❌ **Not connecting `<label>` to its `<input>` properly**

```html
<label>Full Name</label>
<input type="text" name="name">  <!-- WRONG — the label and input are not connected -->
```

✅ **Correct** — the `for` value must match the `id` value exactly:

```html
<label for="full-name">Full Name</label>
<input type="text" id="full-name" name="name">
```

---

❌ **Forgetting `scope` on `<th>` elements**

```html
<th>Subject</th>  <!-- WRONG — is this a column header or a row header? -->
```

✅ **Correct**:

```html
<th scope="col">Subject</th>   <!-- this heading labels a column -->
<th scope="row">Mathematics</th>  <!-- this heading labels a row -->
```

---

❌ **Using `target="_blank"` without `rel="noopener noreferrer"`**

```html
<a href="https://isocpp.org" target="_blank">Link</a>   <!-- WRONG — security risk -->
```

✅ **Correct**:

```html
<a href="https://isocpp.org" target="_blank" rel="noopener noreferrer">Link</a>
```

---

❌ **Skipping heading levels**

```html
<h1>Portal</h1>
<h3>Deadlines</h3>   <!-- WRONG — you jumped over h2 -->
```

✅ **Correct** — headings must go in order, no skipping:

```html
<h1>Portal</h1>
<h2>Deadlines</h2>
```

---

❌ **Making the current page's breadcrumb a link**

```html
<ol>
  <li><a href="/">Home</a></li>
  <li><a href="/dashboard">Dashboard</a></li>  <!-- WRONG — you are already here -->
</ol>
```

✅ **Correct** — the current page is plain text, not a link:

```html
<ol>
  <li><a href="/">Home</a></li>
  <li>Dashboard</li>
</ol>
```

---

❌ **Using `<fieldset>` without a `<legend>`**

```html
<fieldset>
  <input type="radio" name="type" value="a"> Option A  <!-- WRONG — no title for the group -->
</fieldset>
```

✅ **Correct** — `<legend>` must be the first child of `<fieldset>`:

```html
<fieldset>
  <legend>Submission Type</legend>
  <label><input type="radio" name="type" value="a"> Option A</label>
</fieldset>
```

---

## Tips for Success

💡 **Tip 1**: Do not try to write the whole page at once. Finish one section, open it in a browser, check it looks right, then move to the next

💡 **Tip 2**: Every `id` value in your file must be different — you cannot use the same `id` twice. If two inputs have `id="name"`, the labels will get confused

💡 **Tip 3**: `<details open>` starts expanded. `<details>` without `open` starts collapsed. The only difference is that one word

💡 **Tip 4**: The `<datalist>` only works if its `id` matches the `list` attribute on the `<input>`. If you name them differently, the suggestions will not appear

💡 **Tip 5**: Write fallback text inside `<video>` and `<iframe>` — this is the message users see if their browser cannot load the content

---

## Submission Guidelines

1. **File name**: Save your file as `dashboard.html` Example: dashboard.html`
    
2. **Comments**: Add a comment at the top with:
    
    ```cpp
    // Student Name: [Your Name]
    // Assignment: Student Portal
    // Date: [Today's Date]
    ```
    
    
3. **Test**: Open your file in a browser and go through every section — make sure the table rows line up, the form labels are clickable, and the accordion opens and closes
    
4. **Submit**: Submit your `.html` file
    

---

## Bonus Challenge 🏆

Once you have a clean submission, try both of these for an extra 10 points:

- Add `<meta name="theme-color" content="#3a86ff">` and `<link rel="manifest" href="/manifest.webmanifest">` to your `<head>`
- Add `<ins datetime="...">` and `<del datetime="...">` inside one announcement to show that it was edited — like a visible correction

---

## Need Help?

If you get stuck:

1. ✅ Is `<meta charset="UTF-8">` the very first line inside `<head>`?
2. ✅ Does every `<input>` have a `<label>` where the `for` matches the `id`?
3. ✅ Does every `<th>` have a `scope` attribute?
4. ✅ Do both `<nav>` elements have different `aria-label` text?
5. ✅ Does the C++ Books `<details>` have the word `open` in it?

---

## Learning Objectives

By completing this assignment, you will demonstrate:

- ✓ Understanding of the HTML document boilerplate and why each line exists
- ✓ Ability to use the correct structural tags for each part of a page
- ✓ Knowledge of text-level tags and when to use each one
- ✓ Ability to build a table with merged cells using `colspan` and `rowspan`
- ✓ Knowledge of all form elements and how to label them correctly
- ✓ Ability to embed images, video, and external pages with proper fallbacks
- ✓ Understanding of basic accessibility — labels, skip links, alt text, aria attributes
- ✓ Ability to produce a valid HTML file that passes the W3C checker

---

## Good Luck! 🚀

Remember: build it section by section, open it in your browser after each step, and paste it into the validator before you submit.

**Happy Coding!** 💻