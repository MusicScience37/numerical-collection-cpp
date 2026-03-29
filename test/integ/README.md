# Integration Tests

| Directory          | Tested Modules | Description                                                                                                                   |
| :----------------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------- |
| `assertions`       | `util`         | Tests of assertions.                                                                                                          |
| `linear_solvers`   | `linear`       | Tests of solvers for linear systems.                                                                                          |
| `logging_test`     | `logging`      | Integration tests of logging. Tests using configuration files are implemented.                                                |
| `oct_math`         | `multi_double` | Tests of mathematics with octuple-precision floating-point numbers.                                                           |
| `ode_comb`         | `ode`          | Combination tests of ODE solvers using several problems.                                                                      |
| `quad_math`        | `multi_double` | Tests of mathematics with quadruple-precision floating-point numbers.                                                         |
| `rbf_fd_comb`      | `rbf`          | Combination tests of RBF-FD method for different dimensions, polynomial degrees, and RBFs.                                    |
| `rbf_fd_equations` | `rbf`          | Tests of RBF-FD method for solving some PDEs. This will include solving time-dependent PDEs using `ode` module in the future. |
| `rbf_interp_comb`  | `rbf`          | Combination tests of RBF interpolation for different dimensions, operators to apply, polynomial degrees, and RBFs.            |
