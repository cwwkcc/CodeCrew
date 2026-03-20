![[white and black hats.webp]]

**The Scenario:** A king has imprisoned 100 of his wisest advisors. He lines them up single file, all facing forward, so each person can see everyone in front of them but no one behind. The king places either a black or white hat on each person's head randomly.

Starting from the back of the line, the king asks each prisoner to guess the color of their own hat. They can only say "black" or "white" - nothing else. If they guess correctly, they live. If wrong, they die (silently, so others don't know).

**The Challenge:** The prisoners can discuss a strategy beforehand, but once the hats are placed, they cannot communicate except for saying their guess. What strategy can save the maximum number of prisoners,  and how many of them can be saved?? 

---
What does  “lines them up single file” mean?

**Single file** means:

> People are standing **one behind another in a straight line**, not side-by-side.

Think of:

- a queue at a counter
    
- students standing for morning assembly
    
- people waiting in line for a bus
    

So the advisors are arranged like this:

```
King → [Person 1] [Person 2] [Person 3] ... [Person 100]
```

Everyone is facing **the same direction** (toward the king).

---

 How can each person see everyone in front of them?

Because:

- Everyone is facing forward
    
- You can naturally see the backs of heads of people **ahead of you**
    
- You **cannot** see behind your own head
      

So:

- **Person 100 (last in line)** can see hats of **Persons 1–99**
    
- **Person 50** can see hats of **Persons 1–49**
    
- **Person 1 (front)** sees **no one**
    

Example view:

```
[Front]
P1 → sees nobody
P2 → sees P1’s hat
P3 → sees P1, P2 hats
P4 → sees P1, P2, P3 hats
...
P100 → sees P1 to P99 hats
[Back]
```

---

**The Solution:** The prisoners can save at least 99 out of 100 people, with the first person having a 50% chance.

#### **The Strategy:**

1. **Before the hats are placed:** The prisoners agree on a system:
    
    - Count the number of BLACK hats you can see
    - If the count is EVEN, say "BLACK"
    - If the count is ODD, say "WHITE"
2. **The first prisoner (at the back):**
    
    - Sees all 99 hats in front
    - Counts black hats: let's say 37 (odd number)
    - Says "WHITE" (because count is odd)
    - This prisoner has 50% chance of survival
    - But crucially, they've given information to everyone else!
3. **The second prisoner:**
    
    - Heard "WHITE" meaning odd number of black hats originally
    - Can see 98 hats in front
    - Counts black hats in front: let's say 36 (even)
    - Thinks: "Original was odd (37), now I see even (36), so I must be wearing BLACK"
    - Says "BLACK" and survives!
4. **Each subsequent prisoner:**
    
    - Keeps track of the parity (odd/even) using all previous information
    - Updates based on what they hear and what they see
    - Can deduce their own hat color with 100% certainty

#### **Example Walkthrough:**

Line: (Back) P100, P99, P98, ... P2, P1 (Front) Hats: (Back) B, W, B, B, W, ... (Front)

- P100 counts 50 black hats ahead → EVEN → says "BLACK" → might die, but encodes the information
- P99 heard "BLACK" (even), sees 49 black ahead (odd), knows they're BLACK → survives
- P98 updates parity based on P99 saying "BLACK", counts what's ahead → survives
- And so on...

---

### **What's the Logic Here?**

This demonstrates **Parity Bit** concept from computer science:

- **Information Encoding:** One person sacrifices certainty to encode information
- **Parity Check:** Even/odd counting is a form of error detection
- **Distributed Computing:** Each person processes information based on previous outputs
- **Communication Protocol:** Following a pre-agreed system with minimal bandwidth

#### **Programming Impact:**

- **Error Detection:** Parity bits are used in data transmission
- **Information Theory:** Maximizing information transfer with constraints
- **Protocol Design:** Creating systems where agents cooperate with limited communication
- **Binary Logic:** Using simple binary states to encode complex information

---