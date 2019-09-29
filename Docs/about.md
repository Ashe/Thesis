# Pathfinding to the right decision - Ashley Smith
## Overview

This project aims to investigate further uses for established pathfinding algorithms. Traditionally, algorithms such as A* are about getting from $A$ to $B$ when given some data like a roadmap of a city or a grid of tiles or points for a video game. When implemented, these algorithms will be given this data as a graph of nodes where the edges represent things like distance, maximum speed or the traversal time, all of which are based on moving from one node to the next. In this project, I want to attempt to substitute these types for *any* type in a similar way to how we use algebra to factor out variables. In functional programming, functions themselves can be treated as variables, and so by adopting similar methods we ignore the details about *how* and *why* we are pathfinding and *what with* in order to use A* in other situations.

## Uses

In video games, pathfinding takes a backseat when it comes to decision making. In games like worms, an enemy might decide that attacking the player is a reasonable thing to do, but then proceeds to path awkwardly into an open area, thus exposing themselves for attack. In the Sims, characters sometimes make silly judgements such as going next door to eat food because there's none on the fridge. Asking questions such as 'how far away am I from $x$' requires explicit consideration by game programmers. Taking things further, the idea of specifically catering to these use cases becomes a rigid process without complicating the engine's infrastructure could make programming things like agent personalities significantly difficult. If the distance to a point of interest was valued in a similar way to how the interest itself is valued, agents will respond more and more naturally to distance as values are tweaked.

I can't be certain as to what other types of games this algorithm would be suited towards. You *could* apply it to games such as Tic-Tac-Toe, but it is important to remember that only one decision can be made per turn in these types of games, and so the generated path isn't much of a path but more of a single decision consisting of 'which edge leads to the best node'. While the algorithm has done it's job, it hasn't been used to it's fullest and so a different algorithm would be better suited. Games where the AI needs to make multiple decisions would be far better suited, as the decision to take an action would be it's own edge leading to a node, which could then be traversed normally by an algorithm like A*.

## Features
* **Pathfinding and decision making in one:** By combining pathfinding with decision making, more signals and responses are available by default. If the programmer wants, they can provide the agent with spacial data to improve the quality of decision making, and non-spacial data to improve the quality of pathfinding. This all happens without the need for the programmer to implement a behaviour tree.
* **Easy to adapt to new games:** The algorithm has been adjusted in a functional way so that all game-specific classes and rules are factored out. This is to demonstrate how simple it is to use A* in this way for any situation as you only need to implement the algorithm itself once, just like how easy it is to get A* running for traditional pathfinding in the first place.
* **As realistic as you want to program:** By using A*, the act of decision making **doesn't require** (but still can involve) recursively calculating all possible outcomes up front like the min-max algorithm. Instead, it looks at the game one state at a time (in Tic-Tac-Toe, one state = one turn, whereas in other games a turn could involve multiple states). This is much more realistic, as it decides what to do based on the evaluation functions provided to it. If the programmer's evaluation function predicts the future in some way, then so can the agent. While this isn't automatic, it is customisable. For instance, in Tic-Tac-Toe, the current agent tries to prevent ending its turn with it's opponent having 2 in a row while being uncontested (and thus it knows that it'll lose the game on it's next turn if it doesn't act). 

## Re-implementation
### Summary

```cpp
// Evaluates options and returns a stack of actions to take
// Templates: thought STATE, ACTION, decision COST
template <class S, class A, class C>
std::pair<bool, std::stack<A>> decide(
    const S& startingState,
    const C& minimumCost,
    const C& maximumCost,
    std::function<std::vector<A>(const S&)> getPossibleActions,
    std::function<bool(const S&, const S&)> isStateEndpoint,
    std::function<C(const S&)> heuristic,
    std::function<C(const S&, const S&, const A&)> weighAction,
    std::function<std::pair<bool, const S>(const S&, const A&)> takeAction,
    std::function<bool(const C&, const C&)> compareCost) {
```

When changing A* into a decision making algorithm instead of a pathfinding one, it's arguments should become generic in order to accommodate the possible options and allow for easier reusing of code. To do this, C++ templates are used.

`State` is a representation of the variables used to keep track of things that the agent needs to know in order to make their decisions. They can represent anything from the position and value of pieces in a board game to the status of the agent during play. The agent doesn't have to be omniscient - by filtering `State` to only represent the agent's immediate surroundings and their own health bar, they won't attempt to do anything unrealistic such as shoot projectiles from across the battlefield into areas they shouldn't see when the player is on low health.

`Action`s are acts that impact the `State` in some way, such as navigating, healing, expending ammunition to fire at enemies or possibly waiting for time to pass. It is up to decide which `Action`s are available at any moment in time, as some games don't allow you to perform things in any order (in XCOM, you can move and then shoot, but not shoot and then move). This is what the agent will be deciding between, and an `std::stack` of `Action`s will be the outcome of successfully executed call to the amended A* algorithm.

`Cost` is a representation of the price to pay for taking an action. While sometimes it'd be better to model this as a 'points' mechanic and choosing the highest one, A* cannot optimise an additive situation where the goal is to be the highest scoring route as any route could have a high-value node at the end. Using `Cost` however, implies that evaluating any routes with a worse `Cost` than another is a waste of time, and so the algorithm can make optimisations. `Cost` is the factor A* uses to determine that one route is better than another, and so by manipulating the `Cost` of `Action`s and also tweaking how the agent values one `Cost` over another the programmer has great control over the agent's values and aims when making a decision. 

The functions that `decide` accepts all significantly change how the algorithm chooses what to do.  Below, I've written a more in-depth description of what amendments have been made to the standard A* algorithm in order to factor out the types that I have.

> It is **important** to mention that the decision to adjust A* to be generic does not mean that I am cutting out traditional pathfinding. It is quite the opposite - by making it generic, the programmer has more power over how the algorithm operates and can therefore use it to generate a path while accepting more than just spacial data or by doing more than simply moving.

### Step 1: Separating edges from nodes
A* takes a graph of nodes, starting and destination nodes $A$ and $B$, and a heuristic function $h$ for calculating the remaining distance or travel time. This must be amended if A* is going to make decisions. To make this easier, I'm going to write as if we're keeping the theme of a video game's AI agent getting from $A$ to $B$ but changing the concept of *generating the path* to *deciding which path to take*. 

Normally in these situations the nodes contain coordinates to the locations they represent in the game (that's if the node itself isn't a coordinate). This makes it trivial to find the distance between connected nodes $X$ and $Y$, as you can simply calculate displacement $d=Y - X$. However, going back to our amended version, we want to separate the *action* from the *result of taking it* - in this case, the `Action` of moving with displacement $d$ and our new `State` with our location at $Y$. It is very easy to combine these two types when they're both in vector form, so its important to declare up front that it is **not required** for an `Action` to be of the same type as a `State`.

Now we have to amend the algorithm with `Action` and `State` taken into consideration. The nodes of the graph to traverse are `State`s and the edges between them are `Action`s. This gives us some possible interactions such as many `Action`s leading us to the same `State`, although in the current example it'd be impossible to move in a different direction and end up in the same place as if the agent had decided to move differently.

```mermaid
graph TD;
    A[Injured, enemy close] -- Kill Enemy -->B[Injured, no enemy];
    A-- Run away -->C[Injured];
    A-- Use medkit -->D[Die];
    A-- Stand Still -->D;
    B-- Use medkit-->E[Safe];
    C-- Use medkit -->E;
```

### Step 2: Revisiting the graph
The next thing that needs attention is the graph itself. Rather than passing in the graph of nodes, it'd make things a lot more flexible if the programmer could decide how this graph is found. One of the main reasons I think this is a good move is that we know longer know what a `State` or `Action` really is, and so the algorithm no longer has a say as to what `Action`s are available to take. In terms of the standard implementation, the `Action` that can be taken from any given `State` would simply be derived from the neighbours of the current node. However, what if some `Action`s are one way? What if you can only take an `Action` from a specific state, or after taking a chain of `Action`s previously?

```cpp
std::function<std::vector<A>(const S&)> getPossibleActions
```

All of these would normally require special consideration, so instead of providing any kind of graph, we will allow the programmer to inject their graph through a function. In the test harness, this function is called `getPossibleActions`. This function will return a list of actions for any given state. It is then up to the programmer to pack enough information into the state such that they can derive what the agent could do at this point in time. For a traditional graph, this would just be a traditional look up function in an `std::map` of connections. However, this also allows for more abstract concepts - in Tic-Tac-Toe, there is no navigation. Instead, the possible `Action`s returned by this function are actually coordinates representing the different moves the AI could take. 

```mermaid
graph LR;
    A[1, 1]-- Walk 1, 0 across grass-->B[2, 1];
    B-- Walk -1, 0 across grass -->A;
    B-- Walk -1, -1 down stairs -->C[1, 0]
    C-- Walk 1 1 up stairs -->B;
    C-- Descend 0 -1 down ladder -->D[1, -1];
    D-- Ascend 0 1 up ladder -->C;
    A-- Jump down pit 0, -2 -->D;
```

### Step 3: Removing the notion of a destination
```cpp
std::function<bool(const S&, const S&)> isStateEndpoint
```
It would be very difficult to provide a single `State` as a destination for a problem like this. Again, I factored out the process of comparing a given `State` to a destination into the function `isStateEndpoint`, as in, 'should the decision making process be terminated once this `State` has been reached?'

For our example, this function would return `true` when the coordinates contained in the given `State` match those desired. Again though, you can take this further as multiple `State`s can be endpoints - in the Tic-Tac-Toe scene, every action leads to an endpoint as only one move can be made at a time. The condition that makes this function return `true` is simply that the turn tracker has been incremented.

```mermaid
graph TD;
    A[X's Turn: Empty]-- Top left--> B[O's Turn: top left]
    A-- Top center -->C[O's Turn: top center]
    A-- Top right -->D[O's Turn: top right]
    A-- other moves -->E[...]
```

### Step 4: Abstracting weight of an edge
With abstract `State`s and `Action`s in play, a problem arises from trying to choose what to do. The next amendment is this idea of `Cost`. Again, we don't care what a `State` or an `Action` consists or, nor do we care about the `Cost` of taking a given action. It is important to note that doing the same `Action` from a different `State` may result in a different `Cost` - an obvious example being the throwing of a grenade in the agent's own base versus the opponent's. On it's own, the `Cost` class doesn't do much, but it is important to multiple functions in this implementation.

```cpp
std::function<bool(const C&, const C&)> compareCost
```

It isn't obvious what low or high `Cost` instances are, so the programmer should also provide the equivalent to $0$ and $\infty$ if the `Cost` type was an `integer` in the variables `minimumCost` and `maximumCost` . A* works by ignoring routes that have a calculated `Cost` higher than the others, always working with the lowest `Cost` route. The programmer will also need to provide the function `compareCost` to tell A* which `Cost` instance is the better one.

### Step 5: Understanding the heuristic function
```cpp
std::function<C(const S&)> heuristic
```
The programmer will need to provide the function `heuristic`. This will estimate the `Cost` of finishing the decision making process after pathing through the given node. This is used by A* to optimise the order of which routes are evaluated. Nothing has changed from the traditional A* implementation, but it's important to note that this function evaluates a `State` and so more thought may be needed. The Tic-Tac-Toe scene does not need a fleshed out `heuristic` function as only one decision needs to be made, and so the returned `Cost` is equivalent to the `minimumCost` exemplar variable as there's no decision making beyond the single `Action`.

### Step 6: Evaluating an action
```cpp
std::function<C(const S&, const S&, const A&)> weighAction
```
The function `weighAction` takes two `State`s and an `Action`. The first `State` is the current one, whereas the second is the resulting `State` after the given `Action` has been taken. This function is critical as it is what separates the good decisions from the bad. For navigation, this is trivial as you can simply use distance, travel time or something similar. 

In the Tic-Tac-Toe example, the `Cost` class uses the idea of `logicPenalties` that penalise the agent for making bad decisions. While it is down to the programmer to create this function, these checks are all encapsulated in this one function, allowing the programmer to have different evaluation functions used in multiple agents. The penalties are completely arbitrary and tweaking the values can completely change what the agent values. Randomness could also be implemented at this stage by increasing the cost by a random amount to slightly influence decisions between `Action`s with similar `Cost`s.

```mermaid
graph LR;
    A[One single enemy ahead]-- Move close -->B[Enemy close];
    A-- Move to cover -->C[Enemy ahead, protected];
    A-- Run away -->D[100% safe]
    B-- Shotgun -->E[70% chance to kill, exposed];
    C-- Shotgun-->F[30% chance to hit, protected];
```
A small scenario has been drawn in the chart above detailing an encounter with an enemy in a game like Worms. In reality there'd be more weapons, more locations to move to and more enemies. If you were asked what you'd do, your answer would be something along the lines of 'it depends'. Everything this decision depends on can be placed into the `State` to be considered. Now the question becomes 'what do you value more?' and this is something that can be calculated. The programmer can hard code or even dynamically provide the `Cost` of all three endpoints. If the agent values staying alive, possibly because they have low health or for any other reason, running away is the winner. However, if the penalty of leaving the enemy alive outweighs one's survival rate, maybe going in close is the one. If the distances are extreme or maybe it's costly either way, maybe the agent's lowest `Cost` option is to try and shoot them but stay protected. 

### Step 7: Generating actions from states
```cpp
std::function<std::pair<bool, const S>(const S&, const A&)> takeAction
```
Finally, the programmer will provide the `takeAction` function. When given a `State` and an `Action` to take, this function will return an `std::pair` containing a `bool` acting as a flag for if the operation was successful and the resulting `State`. If the `bool` is `true`, then the `State` is valid and can be further evaluated. In navigation, this function would simply combine the current location with the displacement to get the next location. In Tic-Tac-Toe, this function would place a mark on the requested tile if it is unoccupied, alternate the current player and increment the turn counter. 

This function is used to generate the possible future `State`s for every `Action` provided by `getPossibleActions`. After checking that these `Action`s do indeed lead to a valid `State`, the `Action`s are then weighed with `weighAction` and their `Cost`s are compared with `compareCost`. Everything else works like normal A*, managing `std::map`s of `State`s to `Cost`s etc.

