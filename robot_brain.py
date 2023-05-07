import time import robot_brain

    def Log(robot_positions, motors_positions) :print(f "robot_positions: {len(robot_positions)}") print(f "motors_positions: {len(motors_positions)}") if len (robot_positions)> 0 :print(f "first robot pos {robot_positions[0]}") print(f "last robot pos {robot_positions[len(robot_positions) - 1]}") return robot_brain.VectorDouble([3.1, 3.2, 3.3, 3.4, 3.5])

#res = robot_brain.add(7, 3)
#print(f "robot_brain.add(7, 3) = {res}")

#pet = robot_brain.Pet("Dog")
#print(pet.getName())
#pet.setName("Cat")
#print(pet.getName())

                                                                                                                                                                                                                                                                                                                                                robot = robot_brain.brain_t("tests/config_test.yaml") robot.set_net_function("LOG", Log) robot.set_active_net_function("LOG") robot.run() time.sleep(5) robot.stop()

#robot = robot_brain.brain_t("tests/config_test.yaml")

#def log_net_function(vector2_of_double_ref, vector_pos_double):
#print(vector2_of_double_ref)
#print(vector_pos_double)
#vector2_of_double_ref[0] = 5.5

#robot.set_net_function("INCREMENT_BY_2", log_net_function)
