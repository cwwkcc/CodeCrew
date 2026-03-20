Tables are for **tabular data** — information that has a meaningful row/column relationship. Never use tables for page layout (a pre-CSS habit that died in 2000).

---

## Full Table Structure

```html
<table>
  <caption>Q3 Sales by Region (units)</caption>
  <thead>
    <tr>
      <th scope="col">Region</th>
      <th scope="col">July</th>
      <th scope="col">August</th>
      <th scope="col">September</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th scope="row">North</th>
      <td>1,200</td>
      <td>1,450</td>
      <td>1,310</td>
    </tr>
    <tr>
      <th scope="row">South</th>
      <td>980</td>
      <td>1,020</td>
      <td>890</td>
    </tr>
  </tbody>
  <tfoot>
    <tr>
      <th scope="row">Total</th>
      <td>2,180</td>
      <td>2,470</td>
      <td>2,200</td>
    </tr>
  </tfoot>
</table>
```

---

## Element Reference

|Element|Purpose|
|---|---|
|`<table>`|Root container|
|`<caption>`|Table title — must be first child of `<table>`|
|`<thead>`|Header row group|
|`<tbody>`|Body row group (can be multiple)|
|`<tfoot>`|Footer row group|
|`<tr>`|Table row|
|`<th>`|Header cell — carries semantic weight|
|`<td>`|Data cell|

---

## `scope` on `<th>`

The `scope` attribute tells assistive technology which cells a header applies to:

|Value|Meaning|
|---|---|
|`scope="col"`|Header for its column|
|`scope="row"`|Header for its row|
|`scope="colgroup"`|Header spans multiple columns|
|`scope="rowgroup"`|Header spans multiple rows|

---

## Spanning: `colspan` and `rowspan`

```html
<table>
  <caption>Exam Schedule</caption>
  <thead>
    <tr>
      <th scope="col">Subject</th>
      <th scope="colgroup" colspan="2">Morning Session</th>
      <th scope="colgroup" colspan="2">Afternoon Session</th>
    </tr>
    <tr>
      <th scope="col">—</th>
      <th scope="col">Room</th>
      <th scope="col">Time</th>
      <th scope="col">Room</th>
      <th scope="col">Time</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th scope="row">Mathematics</th>
      <td>A1</td>
      <td>09:00</td>
      <td colspan="2">—</td>
    </tr>
    <tr>
      <th scope="row" rowspan="2">Science</th>
      <td>B2</td>
      <td>09:30</td>
      <td>C1</td>
      <td>14:00</td>
    </tr>
    <tr>
      <!-- no <th> here — rowspan="2" covers this row too -->
      <td>B3</td>
      <td>10:00</td>
      <td>C2</td>
      <td>14:30</td>
    </tr>
  </tbody>
</table>
```

- `colspan="N"` — cell extends across N columns
- `rowspan="N"` — cell extends down N rows
- When a cell uses rowspan, the subsequent rows must omit those cells

---

## Column Groups: `<colgroup>` and `<col>`

Allow you to apply attributes to whole columns without repeating on every cell:

```html
<table>
  <colgroup>
    <col span="1">           <!-- label column -->
    <col span="3">           <!-- data columns -->
  </colgroup>
  ...
</table>
```

---

## Complex Table: `headers` Attribute

For very complex tables where `scope` is insufficient, use `headers` + `id`:

```html
<th id="h-north" scope="row">North</th>
<td headers="h-north h-july">1,200</td>
```

This explicitly associates a data cell with multiple headers.