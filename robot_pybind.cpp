#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include <iostream>

#include "optimus.h"

PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<double>>);
int add(int i, int j) { return i + j; }

namespace robot {
struct Pet {
  Pet(const std::string &name) : name(name) {}
  void setName(const std::string &name_) { name = name_; }
  const std::string &getName() const { return name; }

 private:
  std::string name;
};
}  // namespace robot

template <class Container>
ostream &dump_container(ostream &os, const Container &container) {
  os << "{ ";
  for (const auto &v : container) {
    os << v;
    os << " ";
  }
  os << "}";
  return os;
}

void calculate_motors_positions(
    const std::function<std::vector<double>()> &functor) {
  std::vector<double> res = functor();
  dump_container(std::cout, res);
}

namespace py = pybind11;

PYBIND11_MODULE(robot_brain, m) {
  // py::bind_vector<std::vector<double>>(m, "VectorDouble",
  // py::buffer_protocol()); py::bind_vector<std::vector<double>>(m,
  // "VectorVectorDouble", py::buffer_protocol());
  py::bind_vector<std::vector<double>>(m, "VectorDouble");
  py::bind_vector<std::vector<std::vector<double>>>(m, "VectorVectorDouble");

  m.doc() = "pybind11 robot brain plugin";  // optional module docstring
  m.def("add", &add, "A function that adds two numbers");

  using namespace robot;
  py::class_<Pet>(m, "Pet")
      .def(py::init<const std::string &>())
      .def("setName", &Pet::setName)
      .def("getName", &Pet::getName);

  py::class_<::robot::brain_t>(m, "brain_t")
      .def(py::init<const std::string &>())
      .def("run", &robot::brain_t::run)
      .def("stop", &robot::brain_t::stop)
      .def("set_net_function", &robot::brain_t::set_net_function)
      .def("set_active_net_function", &robot::brain_t::set_active_net_function);
}
