![Executable preview](/Docs/img/strategy_game.png)

# Masters Thesis
This thesis is titled **"The path to the right decision: An investigation into using heuristic algorithms for decision making"**. In this project, I wrote implemented the A* algorithm generically and attempted to use it to power AI opponents in a strategy game. 

## Abstract

The complexity and depth of video games are always growing, with AI being an important aspect of many modern-day games. Game AI has a variety of uses from being used as a plot device in a narrative game to being a replacement for human opponents in a competitive game, but the approach of choice for developing game AI is the traditional state machine. Many techniques such as behaviour trees aim to improve development to be more flexible and modular while being still being easy to implement. Pathfinding is a key element in the majority of these AIs; a decision would be made to navigate somewhere, and an algorithm such as A* is used to generate a path for the AI to follow to get to its destination. The problem with this is that there's no way of knowing if the destination can be reached and the way of getting there without pre-calculation. Moreover, information found when pathfinding, such as the destination being inaccessible, cannot be given back to the decision making system without restarting the whole process, occasionally resulting in unexpected behaviour. 

This paper investigated how the pathfinding and decision making processes could be combined to make a new approach to game AI using the A* algorithm. Through substitution of types, A* can search a graph of data that isn't necessarily spatial to generate a sequence of actions to perform rather than locations to move to. Unfortunately, the testing of the prototypes made with this approach was hindered by problems caused by the amount of nodes being expanded and processed by A*. This, combined with the difficult methods for configuring the AI, ultimately meant that the AIs created for this paper would be unsuitable for non-academic use-cases unless the problems identified were solved, minimised or eliminated in some way.

## Read the full thesis
The [**entire thesis**](https://github.com/Ashe/Thesis/blob/master/Docs/thesis.pdf) is stored in this repository.

Feel free to download the [**latest release**](https://github.com/Ashe/Thesis/releases) for a Windows executable as well as a copy of the thesis. 
