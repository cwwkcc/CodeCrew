## Assignment Overview

Below is a single HTML file for a **CodeCrew Student Hub** page.
It was written by a developer who did not follow proper HTML rules.

Your job is to **find every error, understand why it is wrong, and rewrite the
entire file correctly** using proper semantic HTML.

There is no CSS. There is no JavaScript. Focus entirely on structure and meaning.

---

## The Broken File

Read the code carefully. Some errors are obvious. Some are subtle.

```html
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title></title>
  </head>
  <body>
    <h1>CodeCrew Student Hub</h1>
    <h3>Your guide to everything HTML</h3>

    <div>
      <a href="#">Home</a>
      <a href="#">Resources</a>
      <a href="#">Schedule</a>
      <a href="#">Contact</a>
    </div>

    <hr />

    <h2 id="resources">Useful Resources</h2>
    <p>Check out these links to support your learning:</p>

    <p>
      <a href="https://developer.mozilla.org" target="_blank">MDN Web Docs</a>
    </p>
    <p><a href="https://www.w3schools.com" target="_blank">W3Schools</a></p>
    <p><a>HTML Validator</a></p>
    <p><a href="mailto:hello@codecrew.lk" target="_blank">Email Us</a></p>
    <p><a href="/notes.pdf">Download Class Notes</a></p>

    <hr />

    <h2 id="setup">How to Set Up VS Code</h2>
    Follow these steps carefully to set up your coding environment.
    <ul>
      <li>Go to code.visualstudio.com</li>
      <li>Click the Download button</li>
      <li>Run the installer</li>
      <li>Open VS Code after installation</li>
      <li>Install the Live Server extension</li>
    </ul>

    <hr />

    <h2 id="stack">Technologies We Teach</h2>
    <p>Here is what you will learn at CodeCrew:</p>
    <ol>
      <li>HTML</li>
      <li>CSS</li>
      <li>JavaScript</li>
      <li>React</li>
      <li>Node.js</li>
    </ol>

    <hr />

    <h2 id="frontend-backend">Frontend vs Backend</h2>
    <p>The web is divided into two sides:</p>
    <ul>
      <li>Frontend</li>
      <ul>
        <li>HTML</li>
        <li>CSS</li>
        <li>JavaScript</li>
      </ul>
      <li>Backend</li>
      <ul>
        <li>Node.js</li>
        <li>Databases</li>
      </ul>
    </ul>

    <hr />

    <h2 id="glossary">Key Terms</h2>
    <p>Important words every developer must know:</p>
    <ul>
      <li>HTML: HyperText Markup Language</li>
      <li>CSS: Cascading Style Sheets</li>
      <li>DOM: Document Object Model</li>
      <li>HTTP: The protocol used to transfer web documents</li>
    </ul>

    <hr />

    <h2 id="schedule">Class Schedule</h2>
    <table border="1">
      <tr>
        <td>Day</td>
        <td>Subject</td>
        <td>Time</td>
        <td>Room</td>
      </tr>
      <tr>
        <td>Monday</td>
        <td>HTML</td>
        <td>09:00</td>
        <td>Lab 1</td>
      </tr>
      <tr>
        <td>Tuesday</td>
        <td>CSS</td>
        <td>09:00</td>
        <td>Lab 1</td>
      </tr>
      <tr>
        <td>Wednesday</td>
        <td>JavaScript</td>
        <td>10:00</td>
        <td>Lab 2</td>
      </tr>
      <tr>
        <td>Thursday</td>
        <td>React</td>
        <td>10:00</td>
        <td>Lab 2</td>
      </tr>
    </table>

    <hr />

    <h2 id="contact">Contact</h2>
    <p>
      <b
        >Important: The next class is on Friday 18 April at 9AM. Do not be
        late.</b
      >
    </p>
    <p>
      Have questions? <i>We are always happy to help.</i> Reach out any time.
    </p>
    <br />
    <br />
    <p>
      You can also find us on
      <a href="https://github.com/codecrew" target="_blank">GitHub</a>.
    </p>
  </body>
</html>
```

---

## Your Task

### Step 1 — Hunt the Errors

Go through the file from top to bottom. For each error you find, write:

- **What line / section** the error is in
- **What is wrong**
- **What the fix should be**

Use this format in a plain text or `.md` file:

```
Error 1
Location : <head> section
Problem  : ...
Fix      : ...
```

* There are **15 errors** hidden in this file. 
### Step 2 — Rewrite the File

Once you have identified all the errors, rewrite the complete HTML file from
scratch with every error fixed. Do not patch line by line — rewrite cleanly.

Save your fixed file as `profile-fixed.html`.

---

## Hints (Read Only If Stuck)

> 💡 **Hint 1** — Check the `<html>` and `<head>` tags very carefully. Is anything missing?

> 💡 **Hint 2** — Look at every `<a>` tag. Does each one have the right attributes for what it is trying to do?

> 💡 **Hint 3** — Before each list, ask yourself: does the order matter here? Is this a list of steps, options, or definitions?

> 💡 **Hint 4** — Look at the nested list structure. Where exactly does the inner `<ul>` sit in the code — and where should it sit?

> 💡 **Hint 5** — Count the structural parts of the table. What is missing?

---

## Submission Guidelines

1. **Your error list**: written in a `.md` or `.txt` file named `errors.md`
2. **Your fixed file**: saved as `profile-fixed.html`
3. **Comment block** at the top of `profile-fixed.html`:

```html
<!--
  Name    : Your Full Name
  Errors  : How many errors you found
  Date    : Date of submission
-->
```

---

## Bonus Challenge 🏆


- 🔍 There is a **floating text** error that is easy to miss. Find it and fix it.
- 📋 Improve the table by adding a `<tfoot>` row that shows the total number of sessions.
- 🔗 The navigation bar works visually but has two semantic problems. Identify both.

---

## Learning Objectives

- ✓ Read and analyse existing HTML code critically.
- ✓ Identify structural, semantic, and attribute-level errors.
- ✓ Apply correct usage of links, lists, and tables from memory.
- ✓ Rewrite broken code cleanly without carrying over old mistakes.

**Good Luck! 🚀**
