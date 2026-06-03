A regular expression is a pattern that describes a set of strings. In JavaScript, regex is used for validation, searching, extracting data from strings, and replacing content. The syntax looks dense at first but follows strict rules — once you know the pieces, you can read any pattern.

---

## Table of Contents

1. [Creating a Regex](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#1-creating-a-regex)
2. [Flags](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#2-flags)
3. [Character Classes](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#3-character-classes)
4. [Anchors and Boundaries](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#4-anchors-and-boundaries)
5. [Quantifiers](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#5-quantifiers)
6. [Groups and Capturing](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#6-groups-and-capturing)
7. [Alternation and Lookahead](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#7-alternation-and-lookahead)
8. [String Methods that Use Regex](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#8-string-methods-that-use-regex)
9. [The RegExp Object](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#9-the-regexp-object)
10. [Common Patterns](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#10-common-patterns)
11. [Common Mistakes](https://claude.ai/chat/3a84ff5e-238b-491e-850f-8e213256ce0e#11-common-mistakes)

---

## 1. Creating a Regex

Two syntaxes — literal (preferred) and constructor:

```javascript
// Literal — defined at compile time, evaluated once
const pattern = /hello/;
const withFlags = /hello/gi;

// Constructor — defined at runtime, useful for dynamic patterns
const dynamic = new RegExp('hello');
const fromVar  = new RegExp(userInput, 'i');   // flag as second argument
```

Use the literal syntax unless the pattern needs to be built from a variable.

```javascript
// Test if a pattern matches
/hello/.test('say hello world');   // true
/hello/.test('say goodbye');       // false
```

---

## 2. Flags

Flags modify how the pattern is applied. They go after the closing `/`:

|Flag|Name|Effect|
|---|---|---|
|`g`|global|Find all matches, not just the first|
|`i`|case-insensitive|`A` matches `a`|
|`m`|multiline|`^` and `$` match start/end of each line|
|`s`|dotAll|`.` matches newlines too|
|`u`|unicode|Enable full Unicode support (recommended)|
|`d`|indices|Include match start/end indices in results|

```javascript
'Hello HELLO hello'.match(/hello/g)    // ['hello'] — only one, no g
'Hello HELLO hello'.match(/hello/gi)   // ['Hello', 'HELLO', 'hello']

// m flag — ^ and $ per line
const text = `first line\nsecond line`;
text.match(/^\w+/gm)   // ['first', 'second']
text.match(/^\w+/g)    // ['first'] — without m, ^ only matches string start
```

---

## 3. Character Classes

Match a category of characters:

```javascript
// . — any character except newline (with s flag, includes newline)
/h.t/.test('hat')   // true
/h.t/.test('hit')   // true
/h.t/.test('h\nt')  // false (without s flag)

// \d — digit [0-9]
// \D — non-digit
/\d+/.exec('Room 42')     // ['42']
/\D+/.exec('Room 42')     // ['Room ']

// \w — word character [a-zA-Z0-9_]
// \W — non-word character
/\w+/.exec('hello_world') // ['hello_world']

// \s — whitespace (space, tab, newline, etc.)
// \S — non-whitespace
'  hello  '.replace(/\s+/g, '')   // 'hello'

// \b — word boundary (position, not a character)
// \B — non-word boundary
/\bcat\b/.test('cat')       // true
/\bcat\b/.test('catch')     // false — 'cat' is not a whole word here
/\bcat\b/.test('the cat')   // true
```

### Custom Character Classes `[ ]`

```javascript
// Match any character listed
/[aeiou]/     // any vowel
/[0-9]/       // any digit (same as \d)
/[a-z]/       // any lowercase letter
/[A-Za-z]/    // any letter
/[A-Za-z0-9_]/ // same as \w

// Negate with ^ inside brackets
/[^aeiou]/    // any NON-vowel
/[^0-9]/      // any non-digit (same as \D)

// Special characters lose most meaning inside []
/[.+*?]/      // matches literal . + * or ?

// Examples
/[aeiou]/i.test('Hello')    // true — 'e' matches
/[^a-z]/i.test('Hello123')  // true — '1' is not a letter
```

---

## 4. Anchors and Boundaries

Anchors match a position, not a character:

```javascript
// ^ — start of string (or start of line with m flag)
/^Hello/.test('Hello world')    // true
/^Hello/.test('Say Hello')      // false

// $ — end of string (or end of line with m flag)
/world$/.test('Hello world')    // true
/world$/.test('world domination') // false

// \b — word boundary
/\bword\b/.test('a word here')   // true
/\bword\b/.test('password')      // false — 'word' is inside 'password'

// Validate full string with ^ and $
/^\d{5}$/.test('12345')          // true  — exactly 5 digits
/^\d{5}$/.test('1234')           // false — too short
/^\d{5}$/.test('123456')         // false — too long
/^\d{5}$/.test('1234a')          // false — contains non-digit
```

---

## 5. Quantifiers

Control how many times the preceding element repeats:

|Quantifier|Meaning|
|---|---|
|`*`|0 or more|
|`+`|1 or more|
|`?`|0 or 1 (optional)|
|`{n}`|Exactly n|
|`{n,}`|n or more|
|`{n,m}`|Between n and m|

```javascript
/go*gle/.test('ggle')    // true — 0 o's
/go*gle/.test('google')  // true — 2 o's
/go+gle/.test('ggle')    // false — needs at least 1 o
/go+gle/.test('google')  // true

/colou?r/.test('color')   // true — u is optional
/colou?r/.test('colour')  // true

/\d{4}/.test('2025')      // true — exactly 4 digits
/\d{2,4}/.test('25')      // true
/\d{2,4}/.test('2025')    // true
/\d{2,4}/.test('2')       // false — only 1 digit
```

### Greedy vs Lazy

By default quantifiers are **greedy** — they match as much as possible. Adding `?` after makes them **lazy** — match as little as possible:

```javascript
const html = '<b>bold</b> and <b>more bold</b>';

// Greedy — matches as much as possible between < and >
html.match(/<.+>/)    // ['<b>bold</b> and <b>more bold</b>'] — too much

// Lazy — matches as little as possible
html.match(/<.+?>/)   // ['<b>'] — just the first tag
html.match(/<.+?>/g)  // ['<b>', '</b>', '<b>', '</b>']
```

---

## 6. Groups and Capturing

### Capturing Groups `( )`

Groups capture a portion of the match for later use:

```javascript
// match() returns [fullMatch, group1, group2, ...]
const date = '2025-11-15';
const result = date.match(/(\d{4})-(\d{2})-(\d{2})/);
// result[0]  → '2025-11-15'   (full match)
// result[1]  → '2025'         (group 1)
// result[2]  → '11'           (group 2)
// result[3]  → '15'           (group 3)
```

### Named Capturing Groups `(?<name> )`

```javascript
const result = '2025-11-15'.match(/(?<year>\d{4})-(?<month>\d{2})-(?<day>\d{2})/);
result.groups.year    // '2025'
result.groups.month   // '11'
result.groups.day     // '15'
```

### Non-Capturing Groups `(?: )`

Group without capturing — useful for applying quantifiers to multiple characters:

```javascript
// Repeat 'ha' as a unit — no capture
/(?:ha)+/.test('hahaha')  // true
/(?:ha)+/.exec('hahaha')  // ['hahaha'] — not saved as a group

// vs capturing
/(ha)+/.exec('hahaha')    // ['hahaha', 'ha'] — 'ha' captured
```

### Backreferences `\1`, `\2`, `\k<name>`

Reference a previously captured group within the same pattern:

```javascript
// Match repeated words
/(\b\w+\b) \1/.test('the the')   // true — word repeated
/(\b\w+\b) \1/.test('the cat')   // false

// Match opening and closing HTML tags
/<(\w+)>.*?<\/\1>/.test('<div>content</div>')   // true
/<(\w+)>.*?<\/\1>/.test('<div>content</span>')  // false — tags don't match

// Named backreference
/(?<tag>\w+).*\k<tag>/.test('divsomediv')   // true
```

---

## 7. Alternation and Lookahead

### Alternation `|`

Match one pattern OR another:

```javascript
/cat|dog/.test('I have a cat')   // true
/cat|dog/.test('I have a dog')   // true
/cat|dog/.test('I have a fish')  // false

// Group alternation to limit its scope
/^(cat|dog)$/.test('cat')   // true
/^(cat|dog)$/.test('cats')  // false
```

### Lookahead and Lookbehind

Look ahead or behind without including the match in the result:

```javascript
// Positive lookahead (?=...) — match A only if followed by B
/\d+(?= dollars)/.exec('100 dollars')  // ['100'] — number before ' dollars'
/\d+(?= dollars)/.exec('100 euros')    // null

// Negative lookahead (?!...) — match A only if NOT followed by B
/\d+(?! dollars)/.exec('100 euros')    // ['100']
/\d+(?! dollars)/.exec('100 dollars')  // null

// Positive lookbehind (?<=...) — match B only if preceded by A
/(?<=\$)\d+/.exec('$100')    // ['100']
/(?<=\$)\d+/.exec('€100')    // null

// Negative lookbehind (?<!...) — match B only if NOT preceded by A
/(?<!\$)\d+/.exec('€100')    // ['100']
/(?<!\$)\d+/.exec('$100')    // null
```

---

## 8. String Methods that Use Regex

### `test()` — Boolean Match Check

```javascript
/^\d+$/.test('12345')    // true
/^\d+$/.test('123ab')    // false
```

### `match()` — Find Matches

```javascript
// Without g flag — returns first match with groups
'2025-11-15'.match(/(\d{4})-(\d{2})-(\d{2})/)
// ['2025-11-15', '2025', '11', '15', index: 0, ...]

// With g flag — returns all matches, no groups
'one 1, two 2, three 3'.match(/\d+/g)
// ['1', '2', '3']
```

### `matchAll()` — Iterator of All Matches with Groups

```javascript
const str = 'key1=val1&key2=val2';
const matches = [...str.matchAll(/(\w+)=(\w+)/g)];
// matches[0][1]  → 'key1'
// matches[0][2]  → 'val1'
// matches[1][1]  → 'key2'
// matches[1][2]  → 'val2'

// With named groups
const results = [...str.matchAll(/(?<key>\w+)=(?<value>\w+)/g)];
results[0].groups   // { key: 'key1', value: 'val1' }
```

`matchAll()` requires the `g` flag.

### `replace()` / `replaceAll()`

```javascript
// Replace first match
'hello world'.replace(/o/, '0')           // 'hell0 world'

// Replace all matches (g flag)
'hello world'.replace(/o/g, '0')          // 'hell0 w0rld'
'hello world'.replaceAll('o', '0')        // same — replaceAll with string

// Replace with captured groups
'2025-11-15'.replace(
  /(\d{4})-(\d{2})-(\d{2})/,
  '$3/$2/$1'                               // rearrange using $1 $2 $3
)   // '15/11/2025'

// Replace with named groups
'2025-11-15'.replace(
  /(?<y>\d{4})-(?<m>\d{2})-(?<d>\d{2})/,
  '$<d>/$<m>/$<y>'
)   // '15/11/2025'

// Replace with a function
'hello world'.replace(/\b\w/g, char => char.toUpperCase())
// 'Hello World'

// Remove HTML tags
'<b>bold</b> text'.replace(/<[^>]+>/g, '')   // 'bold text'
```

### `search()` — Index of First Match

```javascript
'hello world'.search(/world/)   // 6
'hello world'.search(/xyz/)     // -1
```

### `split()` — Split by Pattern

```javascript
'one  two   three'.split(/\s+/)    // ['one', 'two', 'three']
'2025-11-15'.split(/[-\/]/)        // ['2025', '11', '15']
```

---

## 9. The RegExp Object

### Methods

```javascript
const regex = /\d+/g;

// exec() — returns one match at a time, advances lastIndex
let match;
const str = 'room 42, seat 7';
while ((match = regex.exec(str)) !== null) {
  console.log(match[0], 'at index', match.index);
  // '42' at index 5
  // '7' at index 14
}

// test() — boolean
regex.test('abc 123')   // true (also advances lastIndex with g flag)
```

### The `lastIndex` Trap

A regex with the `g` flag remembers where it left off via `lastIndex`. Reusing a stateful regex causes bugs:

```javascript
const re = /\d+/g;
re.test('abc 1');   // true  — lastIndex moves to 6
re.test('abc 1');   // false — starts searching from index 6, finds nothing
re.lastIndex = 0;   // reset manually
re.test('abc 1');   // true again

// Avoid the trap: create a new regex each time, or use string methods
'abc 1'.match(/\d+/)   // always starts fresh — no lastIndex issue
```

---

## 10. Common Patterns

```javascript
// Email (reasonable approximation — full RFC 5322 is much more complex)
const email = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
email.test('user@example.com')    // true
email.test('invalid@')            // false

// URL
const url = /^https?:\/\/[\w.-]+(?:\.[\w.-]+)+[\w\-._~:/?#[\]@!$&'()*+,;=]+$/i;

// Phone (Sri Lankan — 07X-XXXXXXX)
const slPhone = /^(?:\+94|0)?7[01245678]\d{7}$/;
slPhone.test('0771234567')    // true
slPhone.test('+94771234567')  // true

// Postal code (US ZIP)
const usZip = /^\d{5}(-\d{4})?$/;

// Strong password (min 8 chars, uppercase, lowercase, digit, special char)
const strongPassword = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{8,}$/;

// Username (3-20 chars, letters, digits, underscores, hyphens)
const username = /^[a-zA-Z0-9_-]{3,20}$/;

// ISO date (YYYY-MM-DD)
const isoDate = /^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$/;

// Hex colour
const hexColor = /^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/;
hexColor.test('#ff0000')   // true
hexColor.test('#fff')      // true
hexColor.test('ff0000')    // false — missing #

// Extract all URLs from text
const urlExtractor = /https?:\/\/[^\s"'<>]+/gi;
text.match(urlExtractor);

// Strip HTML tags
str.replace(/<[^>]*>/g, '');

// Slug (URL-friendly string)
function slugify(str) {
  return str
    .toLowerCase()
    .replace(/[^\w\s-]/g, '')    // remove non-word chars
    .replace(/[\s_-]+/g, '-')   // spaces and underscores to hyphens
    .replace(/^-+|-+$/g, '');   // trim leading/trailing hyphens
}
slugify('Hello World! How are you?')   // 'hello-world-how-are-you'

// Trim whitespace (pre-ES2019 — now use str.trimStart() / str.trimEnd())
str.replace(/^\s+|\s+$/g, '');

// Camel case to kebab case
'camelCaseString'.replace(/([A-Z])/g, '-$1').toLowerCase()
// 'camel-case-string'
```

---

## 11. Common Mistakes

```javascript
// WRONG — forgetting to escape special characters
/3.14/.test('3X14')    // true — . matches any character
/3\.14/.test('3X14')   // false — \. matches literal dot

// WRONG — reusing a g-flag regex without resetting lastIndex
const re = /\w+/g;
re.test('hello');  // true — lastIndex = 5
re.test('hello');  // false — starts at index 5

// WRONG — using .match() without g flag expecting all results
'a1 b2 c3'.match(/\d/)     // ['1'] — only first match
'a1 b2 c3'.match(/\d/g)    // ['1', '2', '3'] — all matches

// WRONG — using regex for HTML parsing
// Regex cannot handle nested or malformed HTML — use the DOM parser instead
document.createElement('div').innerHTML = html;
// Then use querySelector to extract content

// WRONG — catastrophic backtracking
// Some patterns cause exponential time on certain inputs:
/^(a+)+$/.test('aaaaaaaaaaaaaaaaaaaab')   // hangs — catastrophic backtracking
// Use atomic groups or possessive quantifiers where supported, or redesign pattern

// WRONG — assuming \d matches only ASCII digits
// With u flag, \d still only matches [0-9]
// Arabic-Indic numerals (٣) do NOT match \d
// Use /[0-9]/ explicitly if you want only ASCII
```

---

_Next: [Browser APIs](https://claude.ai/chat/13%20-%20Browser%20APIs.md)_