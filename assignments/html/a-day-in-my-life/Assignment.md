## Assignment Overview

Create a story-based HTML page that describes a day in your life — like a mini blog or diary entry. Think of it as a digital time capsule that uses structure and formatting to tell a story.

This assignment focuses purely on **text content and semantic meaning** in HTML. There is no CSS and no JavaScript. The goal is to make the page engaging and meaningful using HTML structure alone.

---

## What You Need to Use

This assignment requires you to use:

- ✅ **Headings**: `<h1>`, `<h2>`, `<h3>` for logical hierarchy
    
- ✅ **Paragraphs**: `<p>` for storytelling
    
- ✅ **Importance**: `<strong>` to signal serious or important information
    
- ✅ **Emphasis**: `<em>` to convey feelings, tone, or stress
    
- ✅ **Highlighting**: `<mark>` to call attention to specific moments
    
- ✅ **Side Notes**: `<small>` for extra details or "whispered" thoughts
    
- ✅ **Structural Breaks**: `<br>` for dramatic pauses and `<hr>` for time transitions
    
- ✅ **No CSS. No JavaScript. No `style` attributes anywhere.**
    

---

## Assignment Requirements

### Your page must have these 6 sections:

1. **Title (The Hook)**
    
    - Use an `<h1>` tag.
        
    - Must be creative and engaging (e.g., _"The Day the Coffee Ran Out"_ or _"A Coder’s Guide to 5 AM"_).
        
2. **Morning Section**
    
    - An `<h2>` heading titled "Morning".
        
    - A paragraph describing your start to the day.
        
    - Must include `<em>` for feelings and `<strong>` for something critical.
        
3. **Afternoon Section**
    
    - An `<h2>` heading titled "Afternoon".
        
    - Describe an unexpected, funny, or busy moment.
        
    - Must include `<mark>` for a key moment and `<br>` for a dramatic sentence break.
        
4. **Evening Section**
    
    - An `<h2>` heading titled "Evening".
        
    - A reflective paragraph on how the day ended.
        
    - Must include `<strong>` for a lesson learned and `<small>` for a side thought.
        
5. **Quote of the Day**
    
    - An `<h3>` heading titled "Quote of the Day".
        
    - Add a short quote that fits your mood.
        
    - Must wrap the quote in **both** `<em>` and `<strong>`.
        
6. **Final Twist**
    
    - One final concluding paragraph.
        
    - Highlight the most important "moral of the story" using `<mark>`.
        

---

## Page Structure

Your finished page should:

1. **Follow Heading Hierarchy** — Never skip levels (don't go from `<h1>` to `<h3>`).
    
2. **Use Semantic Containers** — Every piece of text must be inside a `<p>` or heading tag.
    
3. **Prioritize Meaning** — Use inline tags (`<strong>`, `<em>`) because the words are important, not just to change the look.
    
4. **Be Clean and Readable** — Even without CSS, the spacing between paragraphs and headings should make the story easy to follow.
    

---

## Sample Output (Skeleton)

Here is the skeleton of your `story.html` file. Every `` is a section you need to fill with your own story:

HTML

```
<!DOCTYPE html>
<html lang="en-LK">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>A Day in My Life — CodeCrew Blog</title>
  <meta name="description" content="A personal story told through semantic HTML.">
</head>
<body>

  <h1></h1>

  <h2>Morning</h2>
  <p>
    </p>

  <hr>

  <h2>Afternoon</h2>
  <p>
    </p>

  <hr>

  <h2>Evening</h2>
  <p>
    </p>

  <h3>Quote of the Day</h3>
  <p>
    </p>

  <p>
    </p>

</body>
</html>
```

---

## Detailed Instructions

### Step 1: Document Setup

Start with the standard boilerplate. Ensure your `<meta charset="UTF-8">` is the first line inside the `<head>`. Set the title to "A Day in My Life — CodeCrew Blog".

### Step 2: The Hook

Write your `<h1>`. Think of it like a YouTube thumbnail or a book title. It should make the reader want to scroll down.

### Step 3: Storytelling with Sections

Build the Morning, Afternoon, and Evening sections one by one. Use the `<hr>` tag between them to represent the passing of time. This creates a visual "separator" even without CSS.

### Step 4: Adding "Texture" with Inline Tags

Go back through your text and add the required tags.

- **Is a sentence emotional?** Wrap it in `<em>`.
    
- **Is a specific time or event vital?** Use `<strong>`.
    
- **Is there a funny realization?** Add it at the end of a paragraph in `<small>`.
    

---

## Common Mistakes to Avoid

❌ **Using tags just for the look**

HTML

````
<strong>I woke up</strong> ```

✅ **Correct** — Using tags for meaning:
```html
I woke up at 5 AM and realized <strong>I had an exam in twenty minutes.</strong>
````

---

❌ **Skipping heading levels**

HTML

````
<h1>The Great Adventure</h1>
<h3>Morning</h3> ```

✅ **Correct**:
```html
<h1>The Great Adventure</h1>
<h2>Morning</h2>
````

---

❌ **Floating text**

HTML

```
<body>
  <h2>Morning</h2>
  I ate a bagel. </body>
```

---

## Tips for Success

💡 **Tip 1**: Make it personal. Whether it’s about a coding bug that took five hours to fix or a great meal, the best stories are specific.

💡 **Tip 2**: Use `<br>` only for impact. Imagine a dramatic pause in a movie—that is where your `<br>` should go.

💡 **Tip 3**: The `<small>` tag is perfect for "breaking the fourth wall" or adding a sarcastic comment about your own day.

---

## Submission Guidelines

1. **File name**: Save your file as `story.html`.
    
2. **Comments**: Add a comment block at the very top:
    
3. **Validation**: Open the file in your browser. Ensure the headings are bold and large, and your `<mark>` highlights are visible.
    

---

## Bonus Challenge 🏆

For an extra 10 points:

- 🔥 Use **nested tags** (e.g., a `<strong>` word inside an `<em>` sentence).
    
- 🎭 Add a massive "Plot Twist" using a combination of `<hr>` and `<mark>`.
    
- 🧠 Use the `<abbr>` tag for any slang or technical terms you use in your story.
    

---

## Learning Objectives

- ✓ Master heading levels and document structure.
    
- ✓ Differentiate between "Physical" and "Semantic" styling.
    
- ✓ Learn to guide a reader's eye using only HTML defaults.
    
- ✓ Demonstrate clean, valid code without the crutch of CSS.
    

**Good Luck! 🚀**