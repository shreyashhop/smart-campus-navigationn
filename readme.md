Smart Campus Navigation & Emergency Route Management System

An interactive, production-ready single-page web application designed as an academic Data Structures and Algorithms (DSA) portfolio. This system models a college campus as a weighted, dynamic graph, allowing users to calculate shortest paths, manage transit requests using queues, keep track of search history using stacks, and toggle real-time road obstructions.

?? Key Features

Shortest Path Navigation: Compute optimal paths between $11$ real-world campus nodes and $16$ pre-mapped road networks using Dijkstra's Algorithm.

Step-by-Step Walkthrough Debugger: Watch the expansion of nodes, min-heap priority queue states, and distance relaxation arrays update in real-time.

Emergency Mode Hotkeys: Instantly map the shortest route from your location to the nearest available Health Center, Security Post, or Campus Gate.

Dynamic Road Blocking: Simulate real-time construction or accidents. Click roads directly on the interactive map canvas to disable them; the system instantly recalculates alternative routes.

Runtime Structure Inspector: Interactive tabbed viewer showing live internal structures of the running program (Adjacency List, FIFO Request Queue, LIFO Call History Stack).

??? Implemented Data Structures & Algorithms

1. Weighted Graph (Adjacency List)

Purpose: Model campus locations and roads.

Implementation: Custom weighted adjacency list mapping each Node ID to a collection of neighbors. Supports dynamic edge properties (weight, blocking/unblocking).

2. Custom HashMap (Chaining Resolution)

Purpose: Map arbitrary text location names to Node IDs.

Implementation: An array-based hash table using a modulo arithmetic hashing function (_hash(key)) with linked collision buckets to showcase chaining collision resolution.

3. Binary Min-Heap (Priority Queue)

Purpose: Optimizes node selection during Dijkstra's Algorithm from $O(V^2)$ to $O(E \log V)$.

Implementation: Built completely from scratch without external dependencies. Features standard tree-balancing methods (parent-child index lookups, bubbleUp, bubbleDown, swaps).

4. Search History (Stack)

Purpose: Tracks previously computed paths.

Implementation: Standard LIFO structure allowing users to .push() computed paths, .pop() previous histories, or click a historical frame in the UI to instantly re-calculate that search.

5. Transit Buffer (Queue)

Purpose: Manages incoming navigation requests in an organized FIFO format.

Implementation: A array-backed FIFO queue supporting .enqueue() (adding new transit requests) and .dequeue() (processing the head request instantly).

?? Tech Stack & Dependencies

Frontend Design: HTML5, Tailwind CSS, FontAwesome Icon Library

Interactive Engine: Native HTML5 2D Canvas Context (supporting custom zoom, scroll pan, and node drag-and-drop transformations)

Logic Layer: Pure JavaScript (ES6+ standard)

?? Repository Structure

Ensure your repository is organized as follows:

+-- index.html   # Main self-contained application file containing UI & logic
+-- README.md    # Documentation file (this file)
