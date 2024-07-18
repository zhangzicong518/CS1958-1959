import DR20API
import numpy as np
from heapq import *
import matplotlib.pyplot as plt
import math

### START CODE HERE ###
# This code block is optional. You can define your utility function and class in this block if necessary.

eps = 0.5  # control the eps of distance
penalty = 0.2
depth_limit = 200  # iteration depth
step_len = 0.25  # steplen of move


# Euclidean Distance
def heuristics(current_node, goal_node):
    return ((current_node[1] - goal_node[1]) ** 2 + (current_node[0] - goal_node[0]) ** 2) ** 0.5


class Node:
    def __init__(self, current_pos, ori, layer, parent, f, g):
        self.x = current_pos[0]
        self.y = current_pos[1]
        self.ori = ori
        self.parent = parent
        self.f = f
        self.g = g
        self.layer = layer

    def __lt__(self, other):
        return self.f < other.f


###  END CODE HERE  ###


# Hybrid_A_star considering sterr, obstacles, distance
def Hybrid_A_star(current_map, current_pos, ori, goal_pos):
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
    ### START CODE HERE ###

    path = []
    open_list = []
    closed_list = set()

    heappush(
        open_list,
        Node(current_pos, ori, 0, None, heuristics(current_pos, goal_pos), 0),
    )

    while open_list:
        current_node = heappop(open_list)
        current_pos = [current_node.x, current_node.y]

        if reach_goal(current_pos, goal_pos) or current_node.layer >= depth_limit:  # use the idea of IDA*, add layer to control depth of iteration
            while current_node is not None:
                path.append((current_node.x, current_node.y))
                current_node = current_node.parent
            return path[::-1]

        if current_pos[0] >= 110 or current_pos[1] >= 110:  # avoid crssoing border
            continue

        # small ori adjustment peforms better
        steering = [-0.05 * math.pi, 0, 0.05 * math.pi]
        cost_steering = [0.2, 0, 0.2]

        for i, ori in enumerate(steering):
            steering_cost = cost_steering[i]
            ori += current_node.ori

            next_pos_x, next_pos_y = round(current_node.x + step_len * math.cos(ori), 2), round(
                current_node.y + step_len * math.sin(ori), 2
            )  # real pos of car
            next_dx, next_dy = int(next_pos_x), int(next_pos_y)  # nearest int pos of car

            obs = (
                current_map[next_dx][next_dy]
                + current_map[next_dx + 1][next_dy]
                + current_map[next_dx][next_dy + 1]
                + current_map[next_dx + 1][next_dy + 1]
            )

            if 0 < next_pos_x < 110 and 0 < next_pos_y < 110 and not obs and (next_pos_x, next_pos_y) not in closed_list:
                heappush(
                    open_list,
                    Node(
                        [next_pos_x, next_pos_y],
                        ori,
                        current_node.layer + 1,
                        current_node,
                        heuristics([next_pos_x, next_pos_y], goal_pos) + current_node.g + steering_cost + penalty,
                        current_node.g + steering_cost + penalty,  # add penalty can avoid frequent sterr
                    ),
                )
                closed_list.add((next_pos_x, next_pos_y))

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
    dis = heuristics(current_pos, goal_pos)
    return dis <= eps


def planner(current_map, current_pos, current_ori, goal_pos):
    return Hybrid_A_star(current_map, current_pos, current_ori, goal_pos)


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
        path = planner(current_map, current_pos, current_ori, goal_pos)

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

    # visualize
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

###  END CODE HERE  ###
