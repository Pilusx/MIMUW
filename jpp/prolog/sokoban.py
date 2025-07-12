from pyswip import Prolog, Functor
from gym_sokoban.envs.sokoban_env_fast import SokobanEnvFast
import time
import pandas as pd

def format_value(value):
    output = ""
    if isinstance(value, list):
        output = "[ " + ", ".join([format_value(val) for val in value]) + " ]"
    elif isinstance(value, Functor) and value.arity == 2:
        output = "{0}{1}{2}".format(value.args[0], value.name, value.args[1])
    else:
        output = "{}".format(value)

    return output

def format_result(result):
    result = list(result)

    if len(result) == 0:
        return "false."

    if len(result) == 1 and len(result[0]) == 0:
        return "true."

    output = ""
    for res in result:
        tmpOutput = []
        for var in res:
            tmpOutput.append(format_value(res[var]))
        output += ", ".join(tmpOutput) + " ;\n"
    output = output[:-3] + " ."

    return output


def flatten(container):
    for i in container:
        if isinstance(i, (list,tuple)):
            for j in flatten(i):
                yield j
        else:
            yield i


def map_moves(move):
    if move == "up":
        return 3
    elif move == "down":
        return 2
    elif move == "left":
        return 0
    elif move == "right":
        return 1

def find_solution(size=8, num_boxes=2, time_limit=10, seed=0):
    dim_room = (size, size)

    env = SokobanEnvFast(dim_room=dim_room,
                         num_boxes=num_boxes,
                         seed=seed,
                         penalty_for_step=0)
    # The encoding of the board is described in README
    board = env.reset()

    wall = board[:,:,0] # this is a one-hot encoding of walls
    # For readibility first we deal with tops and then with rights
    tops = []
    for i in range(dim_room[0]):
        for j in range(dim_room[1]-1):
            if wall[i,j] == 0 and wall[i,j+1] == 0:
                tops.append("top(x{}y{},x{}y{})".format(i,j,i,j+1))

    rights = []
    for i in range(dim_room[0]-1):
        for j in range(dim_room[1]):
            if wall[i,j] == 0 and wall[i+1,j] == 0:
                rights.append("right(x{}y{},x{}y{})".format(i,j,i+1,j))

    boxes_initial_locations = board[:,:,4]
    boxes_initial = []
    for i in range(dim_room[0]):
        for j in range(dim_room[1]):
            if boxes_initial_locations[i,j] == 1:
                boxes_initial.append("box(x{}y{})".format(i,j))

    boxes_target_locations = board[:,:,2]
    boxes_target = []
    for i in range(dim_room[0]):
        for j in range(dim_room[1]):
            if boxes_target_locations[i,j] == 1:
                boxes_target.append("solution(x{}y{})".format(i,j))

    sokoban_initial_location = board[:,:,5] + board[:,:,6]
    for i in range(dim_room[0]):
        for j in range(dim_room[1]):
            if sokoban_initial_location[i,j] == 1:
                sokoban_string = "sokoban(x{}y{})".format(i,j)
                break

    tops_string = "[" + ','.join(tops) + ']'
    rights_string = "[" + ','.join(rights) + ']'
    boxes_initial_string = "[" + ','.join(boxes_initial) + ']'
    boxes_target_string = "[" + ','.join(boxes_target) + ']'

    prolog = Prolog()
    prolog.consult("sokoban.pl")
    query = "call_with_time_limit({},solve([{},{},{},{},{}],Solution))".format(time_limit,
                                                                               tops_string,
                                                                               rights_string,
                                                                               boxes_initial_string,
                                                                               boxes_target_string,
                                                                               sokoban_string)
    print(query)
    try:
        q = list(prolog.query(query))
        solution = format_result(q).replace('[', '').replace(']', '').replace(' ', '').replace('.', '').split(',')
        #print("\nMoves:", solution)

        rewards = []
        actions = []

        for index in range(len(solution)):
            move = solution[index]
            action = map_moves(move)
            actions.append(action)
            observation, reward, done, info = env.step(action)
            rewards.append(reward)
        print("Last return {}".format(rewards[-1]))
        if rewards[-1] >= 10:
            return 1, actions
        else:
            return 0, []
    except Exception as e:
        print(e)
        return 0, []


if __name__ == "__main__":
    number_of_trials = 100
    time_start = time.time()

    df = pd.DataFrame(columns=['seed', 'actions'])

    results = 0
    for seed in range(number_of_trials):
        seed+=3863890000
        print("Current trial {} result {}".format(seed, results))
        new_result, actions = find_solution(size=8, num_boxes=2, time_limit=20, seed=seed)
        results += new_result
        df = df.append({'seed' : seed , 'actions' : actions} , ignore_index=True)


    print("Number of solutions: {}".format(results))
    print("Total time: {}".format(time.time() - time_start))
    df.to_csv('results.csv')
