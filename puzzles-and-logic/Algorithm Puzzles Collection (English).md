## 1. The Missing Number Problem

**The Scenario:** You have an array containing numbers from 1 to 100, but one number is missing. The array is unsorted and you can only traverse it once.

**The Challenge:** Find the missing number in the most efficient way possible with O(n) time complexity and O(1) space complexity.

**The Solution:** This problem can be solved using mathematical formula. We use the **Sum Formula** for consecutive numbers.

#### **Step 1: Understanding the Formula**

- The sum of first n natural numbers = `n × (n + 1) / 2`
- For numbers 1 to 100: Sum = `100 × 101 / 2 = 5050`

#### **Step 2: Calculate Actual Sum**

- Traverse the array once and calculate the sum of all present numbers
- Let's call this `actual_sum`

#### **Step 3: Find the Missing Number**

- Missing number = `5050 - actual_sum`
- This gives us the answer in just one pass!

**Example:**

- If array has numbers 1 to 100 but missing 67
- Expected sum = 5050
- Actual sum = 4983
- Missing number = 5050 - 4983 = 67

---

### **What's the Logic Here?**

This demonstrates **Mathematical Optimization** in programming:

- **Time Complexity:** O(n) - We traverse the array only once
- **Space Complexity:** O(1) - We use only two variables regardless of array size
- **Smart Approach:** Instead of sorting or comparing, we use mathematics

#### **Programming Impact:**

- Shows how mathematical formulas can optimize algorithms
- Demonstrates the power of **constant space** solutions
- Important for coding interviews and competitive programming

---

## 2. The Two Sum Problem

**The Scenario:** You're given an array of integers and a target sum. You need to find two numbers in the array that add up to the target.

**The Challenge:** Find the indices of two numbers that add up to the target in the most efficient way. Each input has exactly one solution, and you cannot use the same element twice.

**The Solution:** While a brute force approach would take O(n²), we can solve this in O(n) using a **Hash Map (Dictionary)**.

#### **Step 1: Understanding the Approach**

- As we traverse the array, for each number, we check if its complement (target - current number) exists in our hash map
- If it exists, we found our answer!
- If not, we store the current number with its index in the hash map

#### **Step 2: Implementation Logic**

```
For each number in array:
    complement = target - current_number

    If complement exists in hash_map:
        Return [hash_map[complement], current_index]

    Store current_number with its index in hash_map
```

#### **Step 3: Example Walkthrough**

Array: `[2, 7, 11, 15]`, Target: `9`

- Index 0: number = 2, complement = 7 (not in map yet), store {2: 0}
- Index 1: number = 7, complement = 2 (found in map!), return [0, 1]

---

### **What's the Logic Here?**

This showcases **Hash Table** optimization:

- **Time Complexity:** O(n) - Single pass through array
- **Space Complexity:** O(n) - We store at most n elements
- **Trade-off:** We use extra space to gain speed

#### **Programming Impact:**

- Hash maps are powerful tools for **lookup optimization**
- Demonstrates **space-time tradeoff** principle
- Common pattern in many interview problems

---

## 3. The Stock Trading Problem

**The Scenario:** You have an array of stock prices where each element represents the price on that day. You can buy once and sell once.

**The Challenge:** Find the maximum profit you can achieve. If no profit is possible, return 0. You must buy before you sell.

**The Solution:** This can be solved with a **Single Pass** algorithm using two variables.

#### **Step 1: Track Minimum Price**

- Keep track of the minimum price seen so far
- This represents the best day to buy

#### **Step 2: Calculate Maximum Profit**

- For each day, calculate potential profit: `current_price - min_price`
- Keep track of the maximum profit seen
- Update minimum price if current price is lower

#### **Step 3: The Algorithm**

```
min_price = infinity
max_profit = 0

For each price in array:
    If price < min_price:
        min_price = price

    current_profit = price - min_price

    If current_profit > max_profit:
        max_profit = current_profit

Return max_profit
```

**Example:** Prices: `[7, 1, 5, 3, 6, 4]`

- Day 1: min=7, profit=0
- Day 2: min=1, profit=0 (bought at 1)
- Day 3: min=1, profit=4 (sell at 5)
- Day 4: min=1, profit=4
- Day 5: min=1, profit=5 (sell at 6) ← Maximum!
- Day 6: min=1, profit=5

Answer: Buy at 1, sell at 6 → Profit = 5

---

### **What's the Logic Here?**

This demonstrates **Dynamic Programming** thinking:

- **Greedy Approach:** Always buy at the lowest price seen so far
- **Optimal Substructure:** Best profit up to day i depends on best profit up to day i-1
- **Time Complexity:** O(n) - Single pass
- **Space Complexity:** O(1) - Only two variables

#### **Programming Impact:**

- Shows how to solve problems with **single pass** instead of nested loops
- Introduces **dynamic programming** concepts
- Common in real-world scenarios and interviews

---

## 4. The Island Counting Problem

**The Scenario:** You have a 2D grid map of '1's (land) and '0's (water). An island is formed by connecting adjacent lands horizontally or vertically.

**The Challenge:** Count the number of distinct islands in the grid.

**The Solution:** This problem requires **Graph Traversal** using either DFS (Depth-First Search) or BFS (Breadth-First Search).

#### **Step 1: Understanding the Approach**

- Iterate through each cell in the grid
- When we find a '1' (land), we've found a new island
- Use DFS/BFS to mark all connected land cells as visited
- Increment island counter

#### **Step 2: The DFS Algorithm**

```
island_count = 0

For each cell in grid:
    If cell is '1' and not visited:
        island_count += 1
        Mark this cell and all connected land cells as visited using DFS

Return island_count
```

#### **Step 3: DFS Function**

```
Function DFS(row, col):
    If out of bounds OR cell is '0' OR already visited:
        Return

    Mark current cell as visited

    // Explore all 4 directions
    DFS(row-1, col)  // Up
    DFS(row+1, col)  // Down
    DFS(row, col-1)  // Left
    DFS(row, col+1)  // Right
```

**Example:**

```
Grid:
1 1 0 0 0
1 1 0 0 0
0 0 1 0 0
0 0 0 1 1

Islands: 3
- Top-left cluster (4 cells)
- Middle single cell
- Bottom-right cluster (2 cells)
```

---

### **What's the Logic Here?**

This introduces **Graph Theory** concepts:

- **DFS/BFS:** Fundamental graph traversal algorithms
- **Connected Components:** Finding groups of connected nodes
- **Time Complexity:** O(rows × cols) - Visit each cell once
- **Space Complexity:** O(rows × cols) - Recursion stack in worst case

#### **Programming Impact:**

- Essential for understanding **graph algorithms**
- Used in image processing, network analysis, game development
- Foundation for more complex problems like shortest path

---

## 5. The Palindrome Partitioning Problem

**The Scenario:** Given a string, you need to partition it into substrings such that every substring is a palindrome.

**The Challenge:** Find the minimum number of cuts needed to partition the string into palindromes.

**The Solution:** This requires **Dynamic Programming** with palindrome detection.

#### **Step 1: Palindrome Detection**

- First, build a 2D table to store whether substring[i to j] is a palindrome
- Use dynamic programming to fill this table efficiently

#### **Step 2: Minimum Cuts Calculation**

- Create a DP array where `cuts[i]` = minimum cuts needed for substring[0 to i]
- For each position, try all possible partitions
- If substring is already a palindrome, no cut needed
- Otherwise, find the minimum cuts by trying all positions

#### **Step 3: The Algorithm**

```
// Build palindrome table
For length = 1 to n:
    For i = 0 to n-length:
        j = i + length - 1
        isPalindrome[i][j] = check if substring(i,j) is palindrome

// Calculate minimum cuts
cuts[0] = 0
For i = 1 to n:
    If isPalindrome[0][i]:
        cuts[i] = 0
    Else:
        cuts[i] = infinity
        For j = 0 to i-1:
            If isPalindrome[j+1][i]:
                cuts[i] = min(cuts[i], cuts[j] + 1)
```

**Example:** String: "aab"

- Palindrome substrings: "a", "a", "b", "aa"
- Minimum cuts: 1 ("aa" | "b")

String: "aabbc"

- Minimum cuts: 2 ("aa" | "bb" | "c")

---

### **What's the Logic Here?**

This demonstrates **Advanced Dynamic Programming**:

- **Two-Stage DP:** First detect palindromes, then find minimum cuts
- **Optimal Substructure:** Optimal solution contains optimal solutions to subproblems
- **Time Complexity:** O(n²) - Building tables
- **Space Complexity:** O(n²) - Storing palindrome table

#### **Programming Impact:**

- Shows how to break complex problems into stages
- Introduces **2D dynamic programming**
- Common in string manipulation problems

---

## 6. The Meeting Rooms Problem

**The Scenario:** You're given an array of meeting time intervals where each interval is [start_time, end_time].

**The Challenge:** Find the minimum number of conference rooms required to schedule all meetings.

**The Solution:** This can be solved using **Sorting and Priority Queue (Min Heap)**.

#### **Step 1: Sort by Start Time**

- Sort all meetings by their start time
- This allows us to process meetings in chronological order

#### **Step 2: Use Priority Queue**

- Priority queue stores the end times of ongoing meetings
- The size of the queue represents active meeting rooms needed

#### **Step 3: The Algorithm**

```
Sort meetings by start_time

min_heap = empty priority queue
max_rooms = 0

For each meeting in sorted order:
    // Remove all meetings that have ended
    While heap is not empty AND heap.top() <= current_meeting.start:
        heap.pop()

    // Add current meeting's end time
    heap.push(current_meeting.end)

    // Update maximum rooms needed
    max_rooms = max(max_rooms, heap.size())

Return max_rooms
```

**Example:** Meetings: `[[0,30], [5,10], [15,20]]`

- Meeting 1: [0,30] → rooms needed = 1, heap = [30]
- Meeting 2: [5,10] → rooms needed = 2, heap = [10, 30]
- Meeting 3: [15,20] → meeting at 10 ended, rooms needed = 2, heap = [20, 30]

Answer: 2 rooms required

---

### **What's the Logic Here?**

This combines multiple concepts:

- **Sorting:** Preprocessing data for efficient processing
- **Priority Queue:** Efficiently tracking earliest ending meeting
- **Greedy Algorithm:** Always reuse the room that becomes free earliest
- **Time Complexity:** O(n log n) - Sorting + heap operations
- **Space Complexity:** O(n) - Heap can store all meetings

#### **Programming Impact:**

- Shows real-world application of **data structures**
- Demonstrates **interval scheduling** problems
- Common in system design and resource allocation

---

## 7. The Longest Increasing Subsequence (LIS)

**The Scenario:** You have an array of integers. A subsequence is a sequence that can be derived from the array by deleting some or no elements without changing the order.

**The Challenge:** Find the length of the longest strictly increasing subsequence.

**The Solution:** There are two approaches - Dynamic Programming O(n²) and Binary Search O(n log n).

#### **Step 1: Dynamic Programming Approach**

- Create a DP array where `dp[i]` = length of LIS ending at index i
- For each element, check all previous elements
- If previous element is smaller, we can extend that subsequence

#### **Step 2: DP Algorithm**

```
dp = array of size n, initialized to 1

For i = 1 to n-1:
    For j = 0 to i-1:
        If arr[j] < arr[i]:
            dp[i] = max(dp[i], dp[j] + 1)

Return max value in dp array
```

#### **Step 3: Optimized Binary Search Approach**

For better performance, we can use binary search:

```
tails = empty array  // tails[i] = smallest tail of all increasing subsequences of length i+1

For each number in array:
    position = binary_search(tails, number)

    If position == tails.length:
        tails.append(number)
    Else:
        tails[position] = number

Return tails.length
```

**Example:** Array: `[10, 9, 2, 5, 3, 7, 101, 18]`

LIS: `[2, 3, 7, 18]` or `[2, 3, 7, 101]` Length: 4

---

### **What's the Logic Here?**

This showcases algorithm optimization:

- **DP Solution:** Clear logic, O(n²) time
- **Binary Search Solution:** Harder to understand, O(n log n) time
- **Trade-off:** Complexity vs. Efficiency

#### **Programming Impact:**

- Demonstrates how to **optimize algorithms**
- Shows the power of **binary search** beyond just searching
- Foundation for many sequence problems

---

## 8. The Coin Change Problem

**The Scenario:** You have coins of different denominations and a total amount of money.

**The Challenge:** Find the minimum number of coins needed to make up that amount. If it's impossible, return -1.

**The Solution:** This is a classic **Dynamic Programming** problem demonstrating **optimal substructure**.

#### **Step 1: Define the Subproblem**

- Let `dp[i]` = minimum coins needed to make amount i
- For each amount, try using each coin denomination
- Choose the option that gives minimum coins

#### **Step 2: The Recurrence Relation**

```
dp[0] = 0  // Zero coins needed for amount 0

For amount = 1 to target:
    dp[amount] = infinity

    For each coin in coins:
        If coin <= amount:
            dp[amount] = min(dp[amount], dp[amount - coin] + 1)

Return dp[target] if dp[target] != infinity, else return -1
```

#### **Step 3: Example Walkthrough**

Coins: `[1, 2, 5]`, Amount: `11`

| Amount    | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  |
| --------- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Min Coins | 0   | 1   | 1   | 2   | 2   | 1   | 2   | 2   | 3   | 3   | 2   | 3   |

Answer: 3 coins (5 + 5 + 1)

---

### **What's the Logic Here?**

This is fundamental DP:

- **Bottom-up Approach:** Build solution from smallest subproblems
- **Optimal Substructure:** Optimal solution contains optimal solutions to subproblems
- **Overlapping Subproblems:** Same subproblems solved multiple times
- **Time Complexity:** O(amount × number of coins)
- **Space Complexity:** O(amount)

#### **Programming Impact:**

- Core concept in **dynamic programming**
- Used in resource optimization problems
- Foundation for more complex DP problems

---

## 9. The K-th Largest Element

**The Scenario:** You have an unsorted array of integers.

**The Challenge:** Find the k-th largest element in the array efficiently.

**The Solution:** While sorting gives O(n log n), we can achieve O(n) average case using **QuickSelect** algorithm.

#### **Step 1: QuickSelect Algorithm**

QuickSelect is based on QuickSort's partition logic:

- Choose a pivot element
- Partition array so elements larger than pivot are on left, smaller on right
- If pivot is at position k, we found our answer
- Otherwise, recurse on the relevant partition

#### **Step 2: The Algorithm**

```
Function quickSelect(arr, left, right, k):
    If left == right:
        Return arr[left]

    // Choose pivot and partition
    pivot_index = partition(arr, left, right)

    If k == pivot_index:
        Return arr[k]
    Else if k < pivot_index:
        Return quickSelect(arr, left, pivot_index - 1, k)
    Else:
        Return quickSelect(arr, pivot_index + 1, right, k)
```

#### **Step 3: Alternative - Min Heap Approach**

For smaller k values, using a min heap of size k is simpler:

```
min_heap = first k elements

For remaining elements:
    If element > heap.top():
        heap.pop()
        heap.push(element)

Return heap.top()
```

**Example:** Array: `[3, 2, 1, 5, 6, 4]`, k = 2

Using QuickSelect or Heap, we find: 5 (second largest)

---

### **What's the Logic Here?**

This shows algorithm selection:

- **QuickSelect:** O(n) average, O(n²) worst case, O(log n) space
- **Heap:** O(n log k) time, O(k) space - better when k is small
- **Sorting:** O(n log n) time - simplest but not optimal

#### **Programming Impact:**

- Shows how to **choose the right algorithm** based on constraints
- Demonstrates **divide and conquer**
- Important for performance-critical applications

---

## 10. The Word Ladder Problem

**The Scenario:** You're given two words (start and end) and a dictionary. Each transformation changes exactly one letter. All intermediate words must be in the dictionary.

**The Challenge:** Find the shortest transformation sequence from start word to end word. If no path exists, return 0.

**The Solution:** This is a **Breadth-First Search (BFS)** problem on an implicit graph.

#### **Step 1: Model as Graph**

- Each word is a node
- Two words are connected if they differ by exactly one letter
- We need shortest path → BFS is perfect!

#### **Step 2: BFS Algorithm**

```
queue = [start_word]
visited = {start_word}
level = 1

While queue is not empty:
    level_size = queue.size()

    For i = 0 to level_size:
        current_word = queue.dequeue()

        If current_word == end_word:
            Return level

        // Try changing each character
        For each position in word:
            For each letter a-z:
                new_word = current_word with letter at position

                If new_word in dictionary AND new_word not visited:
                    queue.enqueue(new_word)
                    visited.add(new_word)

    level += 1

Return 0  // No path found
```

**Example:** Start: "hit", End: "cog" Dictionary: ["hot", "dot", "dog", "lot", "log", "cog"]

Transformation: hit → hot → dot → dog → cog

Answer: 5 (length of sequence)

---

### **What's the Logic Here?**

This demonstrates **BFS on implicit graphs**:

- **Graph Construction:** Not built explicitly, generated on-the-fly
- **BFS Guarantees:** Shortest path in unweighted graphs
- **Time Complexity:** O(M² × N) where M = word length, N = dictionary size
- **Space Complexity:** O(N) for queue and visited set

#### **Programming Impact:**

- Shows BFS application beyond tree traversal
- Demonstrates **implicit graph** problems
- Common in word games and transformation problems

---

## Summary of Key Concepts

These puzzles demonstrate fundamental algorithms and techniques:

1. **Mathematical Optimization** - Missing Number
2. **Hash Maps** - Two Sum
3. **Greedy/DP** - Stock Trading
4. **Graph Traversal (DFS)** - Island Counting
5. **Advanced DP** - Palindrome Partitioning
6. **Sorting + Priority Queue** - Meeting Rooms
7. **DP + Binary Search** - Longest Increasing Subsequence
8. **Classic DP** - Coin Change
9. **Divide and Conquer** - K-th Largest Element
10. **BFS** - Word Ladder

Each puzzle builds problem-solving skills essential for:

- Competitive Programming
- Technical Interviews
- Real-world Software Development
- Algorithm Design and Analysis
