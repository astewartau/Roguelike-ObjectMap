#pragma once
#include <vector>
#include <unordered_map>
#include <stdexcept>

/// <summary>
/// Represents a cell position on a 2D map, given by a row and column
/// Cells can be added together to identify other cells
/// </summary>
struct Cell {
	Cell() { column = 0; row = 0; }
	Cell(unsigned int column, unsigned int row) : column(column), row(row) {}
	void Set(unsigned int column, unsigned int row) { this->column = column; this->row = row; }
	Cell& operator+=(const Cell& rhs) { column += rhs.column; row += rhs.row; return *this; }
	Cell operator+(const Cell& rhs) { return Cell(column + rhs.column, row + rhs.row); }
	unsigned int column, row;
};

/// <summary>
/// Represents a map of objects that can be layered on top of a cell-based map
/// Allows for multiple objects per map cell
/// </summary>
template <typename Object>
class ObjectMap {
public:
	/// <summary>
	/// Creates a new ObjectMap with the given dimensions
	/// </summary>
	/// <param name="columns">Number of columns or map width</param>
	/// <param name="rows">Number of rows or map height</param>
	ObjectMap(unsigned int columns, unsigned int rows);

	/// <summary>
	/// Gets the objects located at the given map cell
	/// </summary>
	/// <param name="row">The row of the cell to inspect</param>
	/// <param name="column">The column of the cell to inspect</param>
	/// <returns>
	/// A pointer to a vector of objects residing at the given cell.
	/// Returns nullptr if there are no objects at the cell.
	/// </returns>
	const std::vector<Object*>* At(unsigned int column, unsigned int row) const;

	/// <summary>
	/// Checks whether the ObjectMap contains the given object
	/// </summary>
	/// <param name="object">A pointer to the object to check for</param>
	/// <returns>True if the ObjectMap contains the object</returns>
	bool Contains(Object* object) const;

	/// <summary>
	/// Adds the given object to the ObjectMap at the given cell
	/// </summary>
	/// <param name="object">The object to add to the map</param>
	/// <param name="row">The row of the cell to add the object to</param>
	/// <param name="column">The column of the cell to add the object to</param>
	/// <returns>True if successful, false if the object is already in the ObjectMap</returns>
	bool Add(Object* object, unsigned int column, unsigned int row);

	/// <summary>
	/// Moves the given object by some number of rows and columns
	/// </summary>
	/// <param name="object">The object to move</param>
	/// <param name="rows">The number of rows to move the object by</param>
	/// <param name="columns">The number of columns to move the object by</param>
	/// <returns>True if successful, false if the object does not exist in the ObjectMap</returns>
	bool MoveBy(Object* object, int columns, int rows);

	/// <summary>
	/// Moves the given object to the given cell
	/// </summary>
	/// <param name="object">The object to move</param>
	/// <param name="row">The row of the cell to move the object to</param>
	/// <param name="column">The column of the cell to move the object to</param>
	/// <returns>True if successful, false if the object does not exist in the ObjectMap</returns>
	bool MoveTo(Object* object, unsigned int column, unsigned int row);

	/// <summary>
	/// Gets the position of the given object
	/// </summary>
	/// <param name="object">A pointer to the object to check the position of</param>
	/// <returns>
	/// A pointer to the position of the object.
	/// Returns nullptr if the object does not exist in the ObjectMap.
	/// </returns>
	const Cell* GetPosition(Object* object) const;
private:
	/// <summary>
	/// The number of columns in the ObjectMap
	/// </summary>
	const unsigned int _COLUMNS;

	/// <summary>
	/// The number of rows on the ObjectMap
	/// </summary>
	const unsigned int _ROWS;

	/// <summary>
	/// A container allowing object access via cell positions
	/// Provides the ability to iterate across sections of the map
	/// Useful for object culling and rendering
	/// Useful for object lookup when the position is known
	/// Example: _map[a][b] is a vector objects positioned at the map cell (x=a,y=b)
	/// </summary>
	std::vector<std::vector<Object*>> _tiles;

	/// <summary>
	/// A 1D container of all objects and pointers to their positions
	/// Useful for quickly checking whether an object exists
	/// Useful for quickly getting the location of an object
	/// </summary>
	std::unordered_map<Object*, Cell> _objects;
};

/// <summary>
/// Custom exception class for ObjectMap runtime errors
/// </summary>
class ObjectMapException : public std::runtime_error {
public:
	ObjectMapException(std::string message)
		: runtime_error("ObjectMapException: " + message) {}
};

///
/// ObjectMap.tpp
/// The implementation has not been separated into a .cpp file because templated
/// functions must be implemented in header files.
///
/// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
///
#include <algorithm>

template <typename Object>
ObjectMap<Object>::ObjectMap(unsigned int columns, unsigned int rows) :
	_ROWS(rows), _COLUMNS(columns) {

	if (columns == 0 || rows == 0) {
		throw ObjectMapException("Invalid number of rows or columns");
	} else {
		if (rows * columns <= (_tiles.max_size() / sizeof(std::vector<Object*>))) {
			// Resize the 2D part of the ObjectMap so that it matches the given size
			_tiles.resize(columns * rows);
		} else {
			throw ObjectMapException("Number of map cells exceeds the maximum allowed");
		}
	}
}

template <typename Object>
const std::vector<Object*>* ObjectMap<Object>::At(unsigned int column, unsigned int row) const {
	// Ensure that the given position is valid
	if (column < _COLUMNS && row < _ROWS) {
		// Return the objects at that position
		return &_tiles[row * _COLUMNS + column];
	}
	return nullptr;
}

template <typename Object>
bool ObjectMap<Object>::Contains(Object* object) const {
	// Search for the given object via the 1D container
	return _objects.find(object) != _objects.end();
}

template <typename Object>
bool ObjectMap<Object>::Add(Object* object, unsigned int column, unsigned int row) {
	// Ensure that the object is not a duplicate and the position is valid
	if (!Contains(object) && (column < _COLUMNS && row < _ROWS)) {
		_objects[object] = Cell(column, row);
		_tiles[row * _COLUMNS + column].push_back(object);
		return true;
	}
	return false;
}

template <typename Object>
bool ObjectMap<Object>::MoveBy(Object* object, int columns, int rows) {
	// Given the object's position, calculate the new position
	Cell newPosition = _objects[object] + Cell(columns, rows);

	// Check that the new position is within the map bounds
	if (newPosition.column < _COLUMNS && newPosition.row < _ROWS) { // TODO
		// Move the object to the new position
		return MoveTo(object, newPosition.column, newPosition.row);
	}
	return false;
}

template <typename Object>
bool ObjectMap<Object>::MoveTo(Object* object, unsigned int column, unsigned int row) {
	// Ensure that the object exists and the position is valid
	if (Contains(object) && (column < _COLUMNS && row < _ROWS)) {
		// Get the position reference of the object
		Cell* position = &_objects[object];

		// Erase the object from its current position in the map
		auto *oldTile = &_tiles[position->row * _COLUMNS + position->column];
		oldTile->erase(std::remove(oldTile->begin(), oldTile->end(), object), oldTile->end());

		// Add the object to its new position on the map
		_tiles[row * _COLUMNS + column].push_back(object);

		// Set the position of the object for fast lookup
		position->Set(column, row);

		return true;
	}
	return false;
}

template <typename Object>
const Cell* ObjectMap<Object>::GetPosition(Object* object) const {
	// Ensure that the object is on the map
	if (Contains(object)) {
		return &_objects.at(object);
	}
	return nullptr;
}
