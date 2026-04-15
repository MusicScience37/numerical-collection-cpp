# Properties of Formulas in ODE Solvers

For classes implementing formulas, see [Algorithm List](../../algorithm_list.md#ode-solvers-ode-module)

| Type        | Formula                                               | Explicit | Order     | Stiffly-Accurate | Stability          | DAE Support | Supported Mass                    | Notes / Keywords                |
| :---------- | :---------------------------------------------------- | :------- | :-------- | :--------------- | :----------------- | :---------- | :-------------------------------- | :------------------------------ |
| Runge-Kutta | ARK4(3)6L[2]SA-ERK {cite:p}`Kennedy2003`              | Yes      | 4, 3      | No               |                    |             | None                              | Additive Runge-Kutta            |
| Runge-Kutta | ARK4(3)6L[2]SA-ESDIRK {cite:p}`Kennedy2003`           | No       | 4, 3      | Yes              | L-stable           |             | Invertible, changeable            | Additive Runge-Kutta            |
| Runge-Kutta | ARK5(4)8L[2]SA-ESDIRK {cite:p}`Kennedy2003`           | No       | 5, 4      | Yes              | L-stable           |             | Invertible, changeable            | Additive Runge-Kutta            |
| Runge-Kutta | DOPRI5 {cite:p}`Hairer1991`                           | Yes      | 5, 4      | No               |                    |             | None                              |                                 |
| Runge-Kutta | ESDIRK45c {cite:p}`Jorgensen2018`                     | No       | 4, 5      | Yes              | L-stable           |             | Invertible, changeable            |                                 |
| Runge-Kutta | RK4 {cite:p}`Hairer1993`                              | Yes      | 4         | No               |                    |             | None                              |                                 |
| Runge-Kutta | RKF45 {cite:p}`Hairer1993`                            | Yes      | 4, 5      | No               |                    |             | None                              |                                 |
| Runge-Kutta | 4th order SDIRK {cite:p}`Hairer1991`                  | No       | 4, 3      | Yes              | L-stable           | Index 2     | Invertible / singular, changeable |                                 |
| Runge-Kutta | Tanaka Formula 1 {cite:p}`Togawa2007`                 | No       | 3, 1      | No               |                    |             | Invertible, changeable            |                                 |
| Runge-Kutta | Tanaka Formula 2 {cite:p}`Togawa2007`                 | No       | 4, 2      | No               |                    |             | Invertible, changeable            |                                 |
| Runge-Kutta | Lobatto IIIC method of order 4 {cite:p}`Hairer1991`   | No       | 4         | Yes              | B-stable, L-stable | Index 2     |                                   | Invertible / singular, constant |
| Runge-Kutta | Lobatto IIIC method of order 6 {cite:p}`Hairer1991`   | No       | 6         | Yes              | B-stable, L-stable | Index 2     |                                   | Invertible / singular, constant |
| Runge-Kutta | Radau IIA method of order 3 {cite:p}`Hairer1991`      | No       | 3         | Yes              | B-stable, L-stable | Index 2     | Invertible / singular, constant   |                                 |
| Runge-Kutta | Radau IIA method of order 5 {cite:p}`Hairer1991`      | No       | 5         | Yes              | B-stable, L-stable | Index 3     | Invertible / singular, constant   |                                 |
| Runge-Kutta | Radau IIA method of other orders {cite:p}`Hairer1991` | No       | (various) | Yes              | B-stable, L-stable | Index ?     | Invertible / singular, constant   |                                 |
| Rosenbrock  | RODASP                                                | No       | 4, ?      | Yes              | L-stable           | Index 1     | Invertible / singular, constant   |                                 |
| Rosenbrock  | RODASPR {cite:p}`Rang2015`                            | No       | 4, ?      | Yes              | L-stable           | Index 2     | Invertible / singular, constant   |                                 |
| Rosenbrock  | ROS3w {cite:p}`Rang2005`                              | No       | 3, 2      | No               | L-stable           | Index 1     | Invertible / singular, constant   | W-method                        |
| Rosenbrock  | ROS34PW3 {cite:p}`Rang2005`                           | No       | 4, 2      | No               | A-stable           | Index 1     | Invertible / singular, constant   | W-method                        |
| Rosenbrock  | ROS34PRW {cite:p}`Rang2015`                           | No       | 3, 2      | No               | A-stable           | Index 2     | Invertible / singular, constant   | W-method                        |
| AVF         | AVF order 2 {cite:p}`Quispel2008`                     | No       | 2         |                  |                    |             | None                              |                                 |
| AVF         | AVF order 3 {cite:p}`Quispel2008`                     | No       | 3         |                  |                    |             | None                              |                                 |
| AVF         | AVF order 4 {cite:p}`Quispel2008`                     | No       | 4         |                  |                    |             | None                              |                                 |
| Symplectic  | Leap-frog                                             | Yes      | 2         |                  |                    |             | None                              |                                 |
| Symplectic  | Fourth-order formula in {cite:p}`Forest1990`          | Yes      | 4         |                  |                    |             | None                              |                                 |
| Others      | Implicit Euler                                        | No       | 1         |                  |                    |             | Invertible, changeable            |                                 |

- Order: The order of the formula.
  Two orders separated by a comma indicate that the method is an embedded method.
- Stiffly-Accurate: The last stage of the method is the same as the final solution.
  This property is desirable for stiff problems.
  This property implicitly means that the method is implicit.
- A-stable: The stability region includes the entire left half of the complex plane.
  This property is desirable for stiff problems.
- B-stable: A-stable and some additional conditions are satisfied.
- L-stable: A-stable and the stability function goes to zero as the argument goes to infinity.
  This property is desirable for stiff problems.
  A formula is L-stable if it is A-stable and stiffly-accurate (converse is not necessarily true).
- DAE Support: The method can be applied to differential-algebraic equations (DAEs) with the specified index.
  The value of the index are the highest index of DAEs that are solved in literatures.
