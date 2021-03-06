# Roguelike ObjectMap

## Installation
Just add [ObjectMap.hpp](https://github.com/astewartau/Roguelike-ObjectMap/blob/master/ObjectMap.hpp) to your project!

## Background
When programming 2D cell-based video games, and especially roguelikes, map information can *usually* be stored using relatively simple data structures.

For example, map information is often stored as a simple array of integers, or even an array of booleans. A value of `1` or `True` might mean a cell contains a wall, and a value of `0` or `False` might mean the cell is empty. Of course, more integer values could be used to check for more terrain types. Often, if a game is more complicated, a custom `Tile` object with more properties could be used in place of integers.

The problem becomes more challenging at the next step, when we must decide how to store character and item positions. It's important to realise that characters and items are *not* actually part of the map, and can instead be thought of as a separate, dynamic layer that sits on top of a map. This is because items are picked up, characters move around, and often we want to allow many objects to exist in the same cell.

## Features and speed
This project provides a container that can be used as an item/character layer in a fairly general way, and it should work for most roguelikes that use 2D grids. The following features all work in O(1) time:

- Map cell access
- Containment checking
- Object insertion
- Object movement by amount
- Object movement to known position
- Object position inquiry

## Documentation and basic usage
For detailed documentation, please see [ObjectMap.hpp](https://github.com/astewartau/Roguelike-ObjectMap/blob/master/ObjectMap.hpp).

Here, we create a new 50x60 `ObjectMap<Actor>` and add a player actor to it at `(3,4)`.

```
ObjectMap<Actor> objectMap = new ObjectMap(50, 60)

_player = new Actor('@');

_objectMap->Add(_player, 3, 4);

```
