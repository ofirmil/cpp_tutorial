import time
import robot_brain

motors_positions_cache = []
def Log(motors_positions, robot_positions):
    print(f"robot_positions: {len(robot_positions)}")
    print(f"motors_positions: {len(motors_positions)}")
    if len(robot_positions) > 0:
        print(f"first robot pos {robot_positions[0]}")
        print(f"last robot pos {robot_positions[len(robot_positions) - 1]}")
    return robot_brain.VectorDouble([3.1, 3.2, 3.3, 3.4, 3.5])


def SaveCacheAndClear(motors_positions, robot_positions):
    print(f"robot_positions: {len(robot_positions)}")
    print(f"motors_positions: {len(motors_positions)}")
    if len(motors_positions) > 0:
        motors_positions.clear()
    if len(robot_positions) > 0:
        robot_positions.clear()
    SaveCacheAndClear.counter += 1.0
    return robot_brain.VectorDouble([SaveCacheAndClear.counter] * 5)
SaveCacheAndClear.counter = 0.0

robot = robot_brain.brain_t("tests/config_test.yaml")

robot.run()
time.sleep(2)

print("### Set log function for first time ###")
robot.set_net_function("LOG", Log)
robot.set_active_net_function("LOG")
time.sleep(2)

print("### Set log function for second time ###")
robot.set_net_function("LOG", Log)

time.sleep(2)

robot.set_net_function("INCREMENT_BY_2", SaveCacheAndClear)
# print("### Setting  SaveCacheAndClear() ###")
# robot.set_active_net_function("INCREMENT_BY_2")
# time.sleep(3)

# print("### stop() ###")
robot.stop()
print("### stop() Done ###")
