/******************************************************************************
 * Project: mazerunner-core                                                   *
 * File:    maze.h                                                            *
 * File Created: Wednesday, 12th October 2022 9:47:23 pm                      *
 * Author: Peter Harrison                                                     *
 * -----                                                                      *
 * Last Modified: Tuesday, 1st November 2022 10:35:58 am                      *
 * -----                                                                      *
 * Copyright 2022 - 2022 Peter Harrison, Micromouseonline                     *
 * -----                                                                      *
 * Licence:                                                                   *
 *     Use of this source code is governed by an MIT-style                    *
 *     license that can be found in the LICENSE file or at                    *
 *     https://opensource.org/licenses/MIT.                                   *
 ******************************************************************************/

#ifndef MAZE_H
#define MAZE_H

#include "src/queue.h"
#include "src/serial.h"
#include "src/utils.h"
#include <stdint.h>

#define MAZE_WIDTH 16
#define GOAL 0x22
#define START 0x00

// directions for mapping
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define VISITED 0xF0

#define INVALID_DIRECTION (0)
#define MAX_COST 255

const char dirChars[] = "^>v<*";

class Maze {

public:
  Maze() {
  }

  void set_maze_goal(uint8_t goal_cell) {
    m_goal = goal_cell;
  }

  uint8_t maze_goal() {
    return m_goal;
  }

  void mark_cell_visited(uint8_t cell) {
    m_walls[cell] |= VISITED;
  }

  bool cell_is_visited(uint8_t cell) {
    return (m_walls[cell] & VISITED) == VISITED;
  }

  bool is_exit(uint8_t cell, uint8_t direction) {
    return ((m_walls[cell] & (1 << direction)) == 0);
  }

  bool is_wall(uint8_t cell, uint8_t direction) {
    return ((m_walls[cell] & (1 << direction)) != 0);
  }
  /***
   * Set a single wall in the maze. Each wall is set from two directions
   * so that it is consistent when seen from the neighbouring cell.
   *
   * The wall is set unconditionally regardless of whether there is
   * already a wall present
   *
   * No check is made on the provided value for direction
   */
  void set_wall_present(uint8_t cell, uint8_t direction) {
    uint16_t nextCell = neighbour(cell, direction);
    switch (direction) {
      case NORTH:
        m_walls[cell] |= (1 << NORTH);
        m_walls[nextCell] |= (1 << SOUTH);
        break;
      case EAST:
        m_walls[cell] |= (1 << EAST);
        m_walls[nextCell] |= (1 << WEST);
        break;
      case SOUTH:
        m_walls[cell] |= (1 << SOUTH);
        m_walls[nextCell] |= (1 << NORTH);
        break;
      case WEST:
        m_walls[cell] |= (1 << WEST);
        m_walls[nextCell] |= (1 << EAST);
        break;
      default:; // do nothing - although this is an error
        break;
    }
  }

  /***
   * Clear a single wall in the maze. Each wall is cleared from two directions
   * so that it is consistent when seen from the neighbouring cell.
   *
   * The wall is cleared unconditionally regardless of whether there is
   * already a wall present
   *
   * No check is made on the provided value for direction. Take care not
   * to clear m_walls around maze boundary.
   */
  void set_wall_absent(uint8_t cell, uint8_t direction) {
    uint16_t nextCell = neighbour(cell, direction);
    switch (direction) {
      case NORTH:
        m_walls[cell] &= ~(1 << NORTH);
        m_walls[nextCell] &= ~(1 << SOUTH);
        break;
      case EAST:
        m_walls[cell] &= ~(1 << EAST);
        m_walls[nextCell] &= ~(1 << WEST);
        break;
      case SOUTH:
        m_walls[cell] &= ~(1 << SOUTH);
        m_walls[nextCell] &= ~(1 << NORTH);
        break;
      case WEST:
        m_walls[cell] &= ~(1 << WEST);
        m_walls[nextCell] &= ~(1 << EAST);
        break;
      default:; // do nothing - although this is an error
        break;
    }
  }

  /***
   * Initialise a maze and the costs with border m_walls and the start cell
   *
   * If a test maze is provided, the m_walls will all be set up from that
   * No attempt is made to verufy the correctness of a test maze.
   *
   */
  void initialise_maze() {
    for (int i = 0; i < 256; i++) {
      m_cost[i] = 0;
      m_walls[i] = 0;
    }
    // place the boundary m_walls.
    for (uint8_t i = 0; i < 16; i++) {
      set_wall_present(i, WEST);
      set_wall_present(15 * 16 + i, EAST);
      set_wall_present(i * 16, SOUTH);
      set_wall_present((16 * i + 15), NORTH);
    }
    // and the start cell m_walls.
    set_wall_present(START, EAST);
    set_wall_absent(START, NORTH);
  }

  uint8_t cell_north(uint8_t cell) {
    uint8_t nextCell = (cell + (1));
    return nextCell;
  }

  uint8_t cell_east(uint8_t cell) {
    uint8_t nextCell = (cell + (16));
    return nextCell;
  }

  uint8_t cell_south(uint8_t cell) {
    uint8_t nextCell = (cell + (255));
    return nextCell;
  }

  uint8_t cell_west(uint8_t cell) {
    uint8_t nextCell = (cell + (240));
    return nextCell;
  }

  uint8_t neighbour(uint8_t cell, uint8_t direction) {
    uint16_t next;
    switch (direction) {
      case NORTH:
        next = cell_north(cell);
        break;
      case EAST:
        next = cell_east(cell);
        break;
      case SOUTH:
        next = cell_south(cell);
        break;
      case WEST:
        next = cell_west(cell);
        break;
      default:
        next = MAX_COST;
    }
    return next;
  }

  /***
   * Assumes the maze has been flooded
   */
  uint8_t neighbour_cost(uint8_t cell, uint8_t direction) {
    uint8_t result = MAX_COST;
    uint8_t wallData = m_walls[cell];
    switch (direction) {
      case NORTH:
        if ((wallData & (1 << NORTH)) == 0) {
          result = m_cost[cell_north(cell)];
        }
        break;
      case EAST:
        if ((wallData & (1 << EAST)) == 0) {
          result = m_cost[cell_east(cell)];
        }
        break;
      case SOUTH:
        if ((wallData & (1 << SOUTH)) == 0) {
          result = m_cost[cell_south(cell)];
        }
        break;
      case WEST:
        if ((wallData & (1 << WEST)) == 0) {
          result = m_cost[cell_west(cell)];
        }
        break;
      default:
        // TODO: this is an error. We should handle it.
        break;
    }
    return result;
  }

  /***
   * Very simple cell counting flood fills m_cost array with the
   * manhattan distance from every cell to the target.
   *
   * Although the queue looks complicated, this is a fast flood that
   * examines each accessible cell exactly once. Consequently, it runs
   * in fairly constant time, taking 5.3ms when there are no interrupts.
   *
   * @param target - the cell from which all distances are calculated
   */
  void flood_maze(uint8_t target) {
    for (int i = 0; i < 256; i++) {
      m_cost[i] = MAX_COST;
    }
    Queue<uint8_t> queue;
    m_cost[target] = 0;
    queue.add(target);
    while (queue.size() > 0) {
      uint8_t here = queue.head();
      uint16_t newCost = m_cost[here] + 1;

      for (uint8_t direction = 0; direction < 4; direction++) {
        if (is_exit(here, direction)) {
          uint16_t nextCell = neighbour(here, direction);
          if (m_cost[nextCell] > newCost) {
            m_cost[nextCell] = newCost;
            queue.add(nextCell);
          }
        }
      }
    }
  }

  /***
   * Algorithm looks around the current cell and records the smallest
   * neighbour and its direction. By starting with the supplied direction,
   * then looking right, then left, the result will preferentially be
   * ahead if there are multiple neighbours with the same m_cost.
   *
   * @param cell
   * @param startDirection
   * @return
   */
  uint8_t direction_to_smallest(uint8_t cell, uint8_t startDirection) {
    uint8_t nextDirection = startDirection;
    uint8_t smallestDirection = INVALID_DIRECTION;
    uint16_t nextCost;
    uint16_t smallestCost = m_cost[cell];
    nextCost = neighbour_cost(cell, nextDirection);
    if (nextCost < smallestCost) {
      smallestCost = nextCost;
      smallestDirection = nextDirection;
    };
    nextDirection = (startDirection + 1) % 4; // right
    nextCost = neighbour_cost(cell, nextDirection);
    if (nextCost < smallestCost) {
      smallestCost = nextCost;
      smallestDirection = nextDirection;
    };
    nextDirection = (startDirection + 3) % 4; // left
    nextCost = neighbour_cost(cell, nextDirection);
    if (nextCost < smallestCost) {
      smallestCost = nextCost;
      smallestDirection = nextDirection;
    };
    nextDirection = (startDirection + 2) % 4; // behind
    nextCost = neighbour_cost(cell, nextDirection);
    if (nextCost < smallestCost) {
      smallestCost = nextCost;
      smallestDirection = nextDirection;
    };
    if (smallestCost == MAX_COST) {
      smallestDirection = 0;
    }
    return smallestDirection;
  }

  void printNorthWalls(int row) {
    for (int col = 0; col < 16; col++) {
      unsigned char cell = row + 16 * col;
      console.print('o');
      if (is_wall(cell, NORTH)) {
        console.print(("---"));
      } else {
        console.print(("   "));
      }
    }
    console.println('o');
  }

  void printSouthWalls(int row) {
    for (int col = 0; col < 16; col++) {
      unsigned char cell = row + 16 * col;
      console.print('o');
      if (is_wall(cell, SOUTH)) {
        console.print(("---"));
      } else {
        console.print(("   "));
      }
    }
    console.println('o');
  }

  void print_plain() {
    console.println();
    for (int row = 15; row >= 0; row--) {
      printNorthWalls(row);
      for (int col = 0; col < 16; col++) {
        unsigned char cell = static_cast<unsigned char>(row + 16 * col);
        if (is_exit(cell, WEST)) {
          console.print(("    "));
        } else {
          console.print(("|   "));
        }
      }
      console.println('|');
    }
    printSouthWalls(0);
    console.println();
    ;
  }

  void print_with_costs() {
    console.println();
    ;
    for (int row = 15; row >= 0; row--) {
      printNorthWalls(row);
      for (int col = 0; col < 16; col++) {
        unsigned char cell = static_cast<unsigned char>(row + 16 * col);
        if (is_exit(cell, WEST)) {
          console.print(' ');
        } else {
          console.print('|');
        }
        print_justified(m_cost[cell], 3);
      }
      console.println('|');
    }
    printSouthWalls(0);
    console.println();
    ;
  }

  void print_with_directions() {
    console.println();
    flood_maze(maze_goal());
    for (int row = 15; row >= 0; row--) {
      printNorthWalls(row);
      for (int col = 0; col < 16; col++) {
        unsigned char cell = row + 16 * col;
        if (is_wall(cell, WEST)) {
          console.print('|');
        } else {
          console.print(' ');
        }
        unsigned char direction = direction_to_smallest(cell, NORTH);
        if (cell == maze_goal()) {
          direction = 4;
        }
        console.print(' ');
        console.print(dirChars[direction]);
        console.print(' ');
      }
      console.println('|');
    }
    printSouthWalls(0);
    console.println();
    ;
  }

  void print_maze_wall_data() {
    console.println();
    ;
    for (int row = 15; row >= 0; row--) {
      for (int col = 0; col < 16; col++) {
        int cell = row + 16 * col;
        print_hex_2(m_walls[cell]);
        console.print(' ');
      }
      console.println();
      ;
    }
    console.println();
    ;
  }

private:
  uint8_t m_goal = 0x077;
  uint8_t m_cost[256] = {0};
  uint8_t m_walls[256] = {0};
};

extern Maze maze;

#endif // MAZE_H
