---

## The Golden Rule

```
Normal mode  → MOVING and EDITING
Insert mode  → TYPING
Visual mode  → SELECTING
```

---

## Switching Modes

```
Normal → Insert
  i     insert before cursor
  a     insert after cursor
  I     insert at start of line
  A     insert at end of line
  o     new line below + insert
  O     new line above + insert
  s     delete character + insert
  S     delete entire line + insert
  cw    delete word + insert
  C     delete to end of line + insert
  cc    delete entire line + insert

Insert → Normal
  jj    (your custom remap — fastest)
  Esc   (default)

Normal → Visual
  v     character visual mode
  V     line visual mode
  Ctrl+v  block visual mode

Visual → Normal
  jj    or Esc
```

---

## Navigation — Normal Mode

### Basic movement

```
h     ←  left
j     ↓  down
k     ↑  up
l     →  right
```

### Word movement

```
w     jump to start of next word
W     jump to start of next WORD (ignores punctuation)
b     jump to start of previous word
B     jump to start of previous WORD
e     jump to end of current word
E     jump to end of current WORD
```

### Line movement

```
0     start of line (absolute)
^     first non-blank character of line
$     end of line
gg    top of file
G     bottom of file
5G    go to line 5
```

### Screen movement

```
Ctrl+d    scroll half page down
Ctrl+u    scroll half page up
Ctrl+f    scroll full page down
Ctrl+b    scroll full page up
zz        center screen on cursor
zt        scroll so cursor is at top
zb        scroll so cursor is at bottom
```

### Jumping

```
%     jump to matching bracket/paren/brace
*     jump to next occurrence of word under cursor
#     jump to previous occurrence of word under cursor
``    jump back to last position
```

---

## Numbers Multiply Everything

```
5j      move down 5 lines
3w      jump forward 3 words
2dd     delete 2 lines
10k     move up 10 lines
4dw     delete 4 words
6yy     copy 6 lines
```

---

## Editing — Normal Mode

### Deleting

```
x       delete character under cursor
X       delete character before cursor
dd      delete entire line
D       delete to end of line
dw      delete word
db      delete word backwards
d$      delete to end of line
d0      delete to start of line
dgg     delete to top of file
dG      delete to bottom of file
```

### Copying (Yanking)

```
yy      copy entire line
yw      copy word
y$      copy to end of line
y0      copy to start of line
ygg     copy to top of file
yG      copy to bottom of file
```

### Pasting

```
p       paste below / after cursor
P       paste above / before cursor
```

### Changing (delete + enter insert mode)

```
cw      change word
C       change to end of line
cc      change entire line
c$      change to end of line
c0      change to start of line
```

### Undo / Redo

```
u         undo
Ctrl+r    redo
.         repeat last action (extremely powerful)
```

### Indenting

```
>>    indent line right
<<    indent line left
=     auto-indent (in visual mode, auto-indent selection)
```

---

## Insert Mode Shortcuts

```
Ctrl+w    delete word before cursor
Ctrl+u    delete to start of line
Ctrl+h    delete character before cursor (backspace)
```

---

## Visual Mode

### Selecting

```
v         start character selection
V         start line selection
Ctrl+v    start block selection
ggVG      select entire file
```

### After selecting

```
d     delete selection
y     copy selection
c     change selection (delete + insert)
>     indent right
<     indent left
=     auto-indent
~     toggle case
u     lowercase
U     uppercase
```

---

## Search

```
/word     search forward for "word"
?word     search backward for "word"
n         next match
N         previous match
*         search for word under cursor (forward)
#         search for word under cursor (backward)

Ctrl+n    clear search highlight (your custom remap)
```

---

## Find and Replace

```
:s/old/new/         replace first occurrence on current line
:s/old/new/g        replace all occurrences on current line
:%s/old/new/g       replace all occurrences in entire file
:%s/old/new/gc      replace all with confirmation
```

---

## Working with Lines

```
J         join line below to current line
o         new line below
O         new line above
dd        delete line
yy        copy line
p         paste below
P         paste above
>>        indent right
<<        indent left
```

---

## Marks (Bookmarks)

```
ma        set mark "a" at current position
`a        jump to mark "a"
''        jump to last jump position
```

---

## Text Objects (Very Powerful)

```
Pattern:  verb + i/a + object
  i = inside (excludes surrounding characters)
  a = around (includes surrounding characters)

Objects:
  w   word
  s   sentence
  p   paragraph
  "   double quotes
  '   single quotes
  `   backticks
  )   parentheses
  ]   square brackets
  }   curly braces
  >   angle brackets
  t   HTML tag

Examples:
  diw   delete inside word
  daw   delete around word (includes space)
  di"   delete inside double quotes
  da"   delete around double quotes (includes quotes)
  di)   delete inside parentheses
  da)   delete around parentheses (includes parens)
  di}   delete inside curly braces
  ci"   change inside double quotes (delete + insert)
  yi)   copy inside parentheses
  vi}   select inside curly braces

Real world usage:
  You have:  const name = "John"
  Cursor inside "John"
  ci"  → deletes John, puts you in insert mode inside the quotes
  Type new value → done

  You have:  function hello(name, age)
  Cursor anywhere inside parens
  di)  → deletes name, age
  da)  → deletes (name, age)
```

---

## VS Code Specific (vscodevim)

```
Ctrl+d    add next occurrence to multi-cursor selection
gh        show hover (same as hovering with mouse)
gd        go to definition
gD        go to declaration
gr        go to references
gi        go to implementation
Ctrl+o    go back (after gd or gD)

Space+d   delete line (your custom remap → dd)
```

---

## Command Mode

```
:w          save file
:q          quit
:wq         save and quit
:q!         quit without saving
:e filename open file
:split      split window horizontally
:vsplit     split window vertically
:nohl       clear search highlighting
```

---

## EasyMotion (your plugin)

```
Trigger: Space Space

Space Space w    → highlights words forward, press letter to jump
Space Space b    → highlights words backward, press letter to jump
Space Space s    → search for 2 characters and jump
Space Space j    → jump to line below
Space Space k    → jump to line above
```

---

## The . (Dot) Command

The most powerful command in Vim. Repeats your last action.

```
Example:
  dw      delete word
  .       delete next word
  .       delete next word
  .       keeps deleting words

Example:
  ci"     change inside quotes → type new value → jj
  move to next string
  .       repeats the change inside quotes action
```

---

## Common Patterns You Will Use Daily

```
Rename a variable:
  *         find all occurrences
  ciw       change inner word
  type new name
  jj
  n         next occurrence
  .         repeat rename
  n.n.n.    keep repeating

Delete a function argument:
  di)       delete inside parentheses

Change a string value:
  ci"       change inside double quotes
  ci'       change inside single quotes

Select a whole function body:
  vi}       visually select inside curly braces

Duplicate a line:
  yy        copy line
  p         paste below

Move a line down:
  dd        delete line
  p         paste below

Indent a block:
  V         line visual mode
  select lines with j
  >         indent right
```

---

## Learning Order

```
Week 1    i, jj, h, j, k, l, w, b, 0, $, dd, u, Cmd+S
Week 2    o, O, yy, p, dw, cw, gg, G, x
Week 3    numbers (5j, 3w), /, n, N, *, Visual mode
Week 4    Text objects (ci", di), da}, etc.)
Week 5    . (dot command), marks, EasyMotion
Week 6+   Everything else as needed
```

---

_The goal is never to memorize this sheet. The goal is to need it less every week._
