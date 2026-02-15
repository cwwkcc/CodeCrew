![[Gold Bars.webp]]

**The Scenario:** You hire a worker for 7 days and must pay them one gold bar per day. You have a gold bar that can be divided into exactly 7 equal segments. However, you can only make 2 cuts total in the gold bar.

**The Challenge:** How do you arrange the cuts and payment so that the worker receives correct payment at the end of each day? The worker must have possession of their payment each day (you can exchange pieces back and forth).

**The Solution:** Cut the bar into pieces of lengths 1, 2, and 4 segments.

#### **The Strategy:**

**Making the Cuts:**

- Cut 1: Separate 1 segment from the bar → Pieces: (1) and (6)
- Cut 2: From the 6-segment piece, separate 2 segments → Pieces: (1), (2), and (4)

**Daily Payments:**

|Day|You Give|You Take Back|Worker Has|
|---|---|---|---|
|1|1|-|1|
|2|2|1|2|
|3|1|-|3 (2+1)|
|4|4|2, 1|4|
|5|1|-|5 (4+1)|
|6|2|1|6 (4+2)|
|7|1|-|7 (4+2+1)|

---

### **What's the Logic Here?**

This demonstrates **Binary Representation** and **Powers of 2**:

- **Binary Numbers:** With pieces 1, 2, 4, you can make any number 1-7
    
    - 1 = 1
    - 2 = 2
    - 3 = 2 + 1
    - 4 = 4
    - 5 = 4 + 1
    - 6 = 4 + 2
    - 7 = 4 + 2 + 1
- **Greedy Algorithm:** Can exchange pieces to maintain correct daily payment
    
- **Optimal Partitioning:** Minimum cuts for maximum flexibility
    

#### **Programming Impact:**

- **Binary System:** Foundation of computer data representation
- **Bit Manipulation:** Using powers of 2 to represent any number
- **Optimization:** Achieving maximum coverage with minimum resources
- **Dynamic State:** Managing changing conditions (exchanges)
