HTML uses `<` and `>` as tag delimiters and `&` as entity start. To display these as literal characters, use character references.

## Named Character References

|Character|Entity|Notes|
|---|---|---|
|`<`|`&lt;`|Less-than|
|`>`|`&gt;`|Greater-than|
|`&`|`&amp;`|Ampersand — must escape in text and attribute values|
|`"`|`&quot;`|Double quote — escape inside `"` attribute values|
|`'`|`&apos;`|Single quote — escape inside `'` attribute values|
||`&nbsp;`|Non-breaking space|
|`©`|`&copy;`|Copyright|
|`®`|`&reg;`|Registered trademark|
|`™`|`&trade;`|Trademark|
|`—`|`&mdash;`|Em dash|
|`–`|`&ndash;`|En dash|
|`…`|`&hellip;`|Ellipsis|
|`→`|`&rarr;`|Right arrow|
|`←`|`&larr;`|Left arrow|
|`×`|`&times;`|Multiplication sign|
|`÷`|`&divide;`|Division sign|
|`±`|`&plusmn;`|Plus-minus|
|`°`|`&deg;`|Degree|
|`½`|`&frac12;`|One half|

## Numeric Character References

Any Unicode code point can be referenced numerically:

```html
&#169;     <!-- © — decimal -->
&#x00A9;   <!-- © — hexadecimal -->
```

## When to Use Entities vs. Literal Characters

With UTF-8 encoding (which you should always use), you can type most characters directly. The only characters you _must_ escape are `<`, `>`, and `&` in text content, and `"` or `'` inside attribute values.

```html
<!-- Must escape -->
<p>5 &lt; 10 and 10 &gt; 5</p>
<p>Use &amp; between query params: ?a=1&amp;b=2</p>

<!-- Can write directly (with UTF-8) -->
<p>Temperature: 37°C — well above normal.</p>
<p>Copyright © 2025 CodeCrew</p>
```

---