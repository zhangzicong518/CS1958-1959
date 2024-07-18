import DR20API
import numpy as np
import matplotlib.pyplot as plt
import heapq

### START CODE HERE ###
# This code block is optional. You can define your utility function and class in this block if necessary.


class Node:
    def __init__(self, position=None, parent=None, ori=None):
        self.position = position
        self.parent = parent
        self.g = 0
        self.h = 0
        self.f = 0
        self.ori = ori  # store the direction from 0 to 8

    def __eq__(self, other):
        return (self.position[0] == other.position[0]) and (self.position[1] == other.position[1])

    def __lt__(self, other):
        return self.f < other.f


def heuristic(node_pos, goal_pos):
    return abs(node_pos[0] - goal_pos[0]) + abs(node_pos[1] - goal_pos[1])


###  END CODE HERE  ###


def A_star(current_map, current_pos, goal_pos):
    """
    Given current map of the world, current position of the robot and the position of the goal,
    plan a path from current position to the goal using A* algorithm.

    Arguments:
    current_map -- A 120*120 array indicating current map, where 0 indicating traversable and 1 indicating obstacles.
    current_pos -- A 2D vector indicating the current position of the robot.
    goal_pos -- A 2D vector indicating the position of the goal.

    Return:
    path -- A N*2 array representing the planned path by A* algorithm.
    """

    # check cpllision and location
    def collision_check(x, y):
        if x < 0 or x >= len(current_map) or y < 0 or y >= len(current_map[0]):
            return True
        num = (
            current_map[x + 1][y]
            + current_map[x + 1][y + 1]
            + current_map[x + 1][y - 1]
            + current_map[x][y + 1]
            + current_map[x][y - 1]
            + current_map[x - 1][y]
            + current_map[x - 1][y + 1]
            + current_map[x - 1][y - 1]
        )
        return num

    start_node = Node(current_pos)
    goal_node = Node(goal_pos)

    path = []
    open_list = []
    closed_list = set()

    heapq.heappush(open_list, start_node)

    while open_list:
        current_node = heapq.heappop(open_list)

        # trace back to print real path
        if reach_goal(current_node.position, goal_node.position):
            while current_node is not None:
                path.append(current_node.position)
                current_node = current_node.parent
            return path[::-1]

        direction = [(0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (1, -1), (-1, 1), (-1, -1)]

        for i, new_position in enumerate(direction):
            node_position = [current_node.position[0] + new_position[0], current_node.position[1] + new_position[1]]
            sterr_cost = (i != current_node.ori) * 0.2  # sterr penalty

            if collision_check(node_position[0], node_position[1]):
                continue

            if (node_position[0], node_position[1]) in closed_list:  # judge whether visited
                continue

            new_node = Node(node_position, current_node, i)
            new_node.g = current_node.g + heuristic(current_node.position, new_node.position) + sterr_cost  # add sterr penalty
            new_node.h = heuristic(current_node.position, goal_node.position)
            new_node.f = new_node.h + new_node.g

            heapq.heappush(open_list, new_node)
            closed_list.add((node_position[0], node_position[1]))

    return path


def reach_goal(current_pos, goal_pos):
    """
    Given current position of the robot,
    check whether the robot has reached the goal.

    Arguments:
    current_pos -- A 2D vector indicating the current position of the robot.
    goal_pos -- A 2D vector indicating the position of the goal.

    Return:
    is_reached -- A bool variable indicating whether the robot has reached the goal, where True indicating reached.
    """
    is_reached = (current_pos[0] == goal_pos[0]) and (current_pos[1] == goal_pos[1])
    return is_reached


if __name__ == "__main__":
    # Define goal position of the exploration, shown as the gray block in the scene.
    goal_pos = [100, 100]
    controller = DR20API.Controller()

    # Initialize the position of the robot and the map of the world.
    current_pos = controller.get_robot_pos()
    current_map = controller.update_map()

    start_pos = current_pos
    path_x, path_y = [], []

    # Plan-Move-Perceive-Update-Replan loop until the robot reaches the goal.
    while not reach_goal(current_pos, goal_pos):
        # Plan a path based on current map from current position of the robot to the goal.
        current_ori = controller.get_robot_ori()
        path = A_star(current_map, current_pos, goal_pos)
        # Move the robot along the path to a certain distance.
        controller.move_robot(path)
        # Get current position of the robot.
        current_pos = controller.get_robot_pos()
        # Update the map based on the current information of laser scanner and get the updated map.
        current_map = controller.update_map()

        for path_node in path:
            path_x.append(path_node[0])
            path_y.append(path_node[1])
            if reach_goal(path_node, current_pos):
                break

    # visualize map
    obstacles_x, obstacles_y = [], []
    for i in range(120):
        for j in range(120):
            if current_map[i][j] == 1:
                obstacles_x.append(i)
                obstacles_y.append(j)

    plt.plot(path_x, path_y, "-r")
    plt.plot(start_pos[0], start_pos[1], "xr")
    plt.plot(goal_pos[0], goal_pos[1], "xb")
    plt.plot(obstacles_x, obstacles_y, ".k")
    plt.grid(True)
    plt.axis("equal")
    plt.show()

    # Stop the simulation.
    controller.stop_simulation()
