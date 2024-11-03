#include "task_dag.h"
#include "gtest/gtest.h"

namespace TMPSF {
TEST(DAG, BASIC_TEST1) {
    auto dag = new TaskDAG();
    dag->init_random();
    dag->dag_print();
    // dag->edges_print();
    dag->visualize_dag("dag.dot");
}
} // namespace TMPSF
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
