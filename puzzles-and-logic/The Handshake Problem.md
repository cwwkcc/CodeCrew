![[business-handshake.jpg]]

**The Scenario:** හිතන්න room එකක් ඇතුළේ ළමයි 10 දෙනෙක් ඉන්නවා කියලා. හැමෝම අනිත් හැමෝමත් එක්ක එක සැරයක් විතරක් අතට අත දෙනවා (**Handshake** කරනවා).

**The Challenge:** එතන සිද්ධ වෙන මුළු handshake ගණන එකින් එක ගණන් කරන්නේ නැතුව සොයන්නේ කෙසේද?

---

### 1. Step-by-Step Logic (Linear Thinking)

පියවරෙන් පියවර බැලුවොත් සිදුවන්නේ මෙයයි:

- **1st person** අනිත් 9 දෙනාටම handshake දෙනවා. (Handshakes 9)

- **2nd person** දැනටමත් 1st person සමඟ handshake කර ඇති බැවින්, අලුතෙන් දීමට ඉතිරි වන්නේ 8 දෙනෙකි. (Handshakes 8)

- මේ විදිහට අන්තිම එක්කෙනා වෙනකම් ගියොත් ලැබෙන එකතුව: **9+8+7+6+5+4+3+2+1=45**

### 2. Triangular Number System (The Pattern)

මෙය **Triangular Number** රටාවට (1,3,6,10,15...) හරියටම ගැලපේ. පුද්ගලයින් n සංඛ්‍යාවක් සිටී නම්, සිදුවන මුළු handshakes ගණන (n−1) වන Triangular අගයට සමාන වේ.

### 3. Overcounting / Double Counting Logic

මෙය වඩාත් කාර්යක්ෂම ගණිතමය ක්‍රමයකි:

- ළමයි 10 දෙනාම එක්කෙනෙක්ට 9 බැගින් අතට අත දුන්නොත් මුළු වාර ගණන 10×9=90 කි.

- නමුත් මෙහිදී **A**, **B** ට අතට අත දීම සහ **B**, **A** ට අතට අත දීම යන දෙකම ගණනය වී ඇත (Double Counted).

- එම නිසා නියම උත්තරය ලබා ගැනීමට 2න් බෙදිය යුතුය: 210×9​=45.

- **Formula:**  $\frac{n(n−1)}{2}$

### 4. Combined Maths: Combinations (nCr)

**Permutations and Combinations** පාඩමට අනුව, මෙය පුද්ගලයින් 10 දෙනෙකුගෙන් 2 බැගින් තෝරාගත හැකි සංයෝජන ගණනයි (10C2):

$nCr = \frac{n!}{r!(n-r)!}$

$nCr = \frac{10!}{2!(10-2)!}$

---

### 💻 Programming & Time Complexity

මෙම concept එක **Competitive Programming** වලදී **Complexity** තේරුම් ගැනීමට ඉතා වැදගත් වේ.

- **O(N²) Complexity:** ඔයා **C++** වලින් `for loops` දෙකක් (Nested Loops) පාවිච්චි කරලා හැම connection එකක්ම check කරනවා නම්, ළමයි ගණන (N) වැඩි වෙද්දී handshake ගණන "වර්ගජ" (Square) අගයකින් වැඩි වේ.

- **O(1) Complexity:** ඉහත සඳහන් කළ n(n−1)/2 සූත්‍රය (Formula) පාවිච්චි කරන්නේ නම්, ළමයි ගණන කොතරම් විශාල වුවත් **Constant Time** එකකින් පිළිතුර ලබාගත හැක.

---

> **Note:** **NOI** වැනි තරග වලදී ප්‍රශ්නයක් විසඳීමට පෙර එය computer එකේ run වීමට ගතවන කාලය (Efficiency) තීරණය කරන්නේ මෙවැනි ගණිතමය සංකල්ප භාවිතා කරලයි.
