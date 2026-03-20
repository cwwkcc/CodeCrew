## Unordered List: `<ul>`

For items where order does not matter:

```html
<ul>
  <li>Apples</li>
  <li>Bananas</li>
  <li>Mangoes</li>
</ul>
```

## Ordered List: `<ol>`

For items where order matters (steps, rankings):

```html
<ol>
  <li>Boil water</li>
  <li>Add pasta</li>
  <li>Cook for 10 minutes</li>
</ol>
```

`<ol>` attributes:

- `start="5"` — begin numbering at 5
- `reversed` — count downward
- `type="a"` — use letters (`a`, `A`, `i`, `I`, `1`)

Individual `<li>` elements can have `value="N"` to override their number.

## Description List: `<dl>`

For term–definition pairs, glossaries, metadata displays:

```html
<dl>
  <dt>CPU</dt>
  <dd>Central Processing Unit — performs the arithmetic and logic operations.</dd>

  <dt>RAM</dt>
  <dd>Random Access Memory — volatile short-term storage for running programs.</dd>

  <dt>HTTP</dt>
  <dt>HyperText Transfer Protocol</dt>
  <dd>The application-layer protocol used to transfer web documents.</dd>
</dl>
```

One `<dt>` can have multiple `<dd>` values. Multiple `<dt>` elements can share one `<dd>`.

## Nesting

Lists can be nested to arbitrary depth. A nested list must go _inside_ the parent `<li>`:

```html
<ul>
  <li>Fruits
    <ul>
      <li>Tropical
        <ul>
          <li>Mango</li>
          <li>Pineapple</li>
        </ul>
      </li>
    </ul>
  </li>
</ul>
```