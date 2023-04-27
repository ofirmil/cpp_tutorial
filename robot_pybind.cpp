#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "optimus.h"

int add(int i, int j) {
  return i + j;
}

namespace robot {
struct Pet {
  Pet(const std::string &name) : name(name) {}
  void setName(const std::string &name_) { name = name_; }
  const std::string &getName() const { return name; }

 private:
  std::string name;
};
}  // namespace robot

void calculate_motor_positions(
    const std::function<std::vector<double>()> &functor) {
  std::vector<double> res = functor();
  robot::dump_container(std::cout, res);
}

namespace py = pybind11;
PYBIND11_MODULE(robot_brain, m) {
  m.doc() = "pybind11 robot brain plugin";  // optional module docstring
  m.def("add", &add, "A function that adds two numbers");
  m.def("calculate_motor_positions", &calculate_motor_positions,
        "calculate the motor_positions");

  using namespace robot;
  py::class_<Pet>(m, "Pet")
      .def(py::init<const std::string &>())
      .def("setName", &Pet::setName)
      .def("getName", &Pet::getName);
  m.def("calculate_motors_pos") py::class_<::robot::brain_t>(m, "brain_t")
      .def(py::init<const std::string &>())
      .def("set_net_function", &robot::brain_t::set_net_function);
}
