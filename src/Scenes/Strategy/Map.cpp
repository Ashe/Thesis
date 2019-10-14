// Strategy/Map.pp
// A struct containing the game's map

#include "Map.h"

// Allow outputting of map
std::ostream& 
Strategy::operator<<(std::ostream& os, const Map& m) {

  // 1. Size
  os << m.size.x << ',' << m.size.y << '\n';

  // 2. MP and AP
  os << m.startingMP << ',' << m.startingAP << '\n';

  // 3. Prepare to map objects
  os << '{' << '\n';

  // 4. Map objects (index, team, object)
  for (const auto& kvp : m.field) {
    os  << '('
        << kvp.first << ',' 
        << kvp.second.first << ',' 
        << static_cast<int>(kvp.second.second) 
        << ')' << '\n';
  }

  // 5. Terminator character followed by newline
  os << '}' << '\n';

  // Return the stream
  return os;
}

// Load a map from a stream
std::istream& 
Strategy::operator>> (std::istream& is, Map& m) {

  // Dump variable
  char c;

  // 1. Size
  is >> m.size.x >> c >> m.size.y;

  // 2. MP and AP
  is >> m.startingMP >> c >> m.startingAP;

  // 3. Prepare receive map objects
  is >> c ;
  m.field.clear();

  // 4. Receive map objects while not '}'
  is >> c;
  unsigned int counter = 0;
  while (counter < m.size.x * m.size.y && c != '}') {

    // Ensure this loop only iterates w*h times
    counter += 1;

    // Declare temporary data
    unsigned int index;
    Team team;
    int object;

    // Receive data
    is  >> index >> c
        >> team >> c
        >> object >> c;

    // Insert data to field
    m.field[index] = std::make_pair(team, static_cast<Object>(object));

    // Check to see if the next line contains '(' or '{'
    is >> c;
  }

  // 5. Consume final endline character
  is >> c;

  // Return instream
  return is;
}
