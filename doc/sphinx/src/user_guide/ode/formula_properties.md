# Properties of Formulas in ODE Solvers

For classes implementing formulas, see [Algorithm List](../../algorithm_list.md#ode-solvers-ode-module)

## General Properties of Formulas

| Type        | Formula                                      | Explicit | Stages | Order  |
| :---------- | :------------------------------------------- | :------- | :----- | :----- |
| Runge-Kutta | ARK4(3)6L[2]SA-ERK {cite:p}`Kennedy2003`     | Yes      | 6      | 4, 3   |
| Runge-Kutta | ARK4(3)6L[2]SA-ESDIRK {cite:p}`Kennedy2003`  | No       | 6      | 4, 3   |
| Runge-Kutta | ARK5(4)8L[2]SA-ESDIRK {cite:p}`Kennedy2003`  | No       | 8      | 5, 4   |
| Runge-Kutta | DOPRI5 {cite:p}`Hairer1991`                  | Yes      | 7      | 5, 4   |
| Runge-Kutta | ESDIRK45c {cite:p}`Jorgensen2018`            | No       | 6      | 4, 5   |
| Runge-Kutta | RK4 {cite:p}`Hairer1993`                     | Yes      | 4      | 4      |
| Runge-Kutta | RKF45 {cite:p}`Hairer1993`                   | Yes      | 6      | 4, 5   |
| Runge-Kutta | 4th order SDIRK {cite:p}`Hairer1991`         | No       | 5      | 4, 3   |
| Runge-Kutta | Tanaka Formula 1 {cite:p}`Togawa2007`        | No       | 2      | 3, 1   |
| Runge-Kutta | Tanaka Formula 2 {cite:p}`Togawa2007`        | No       | 3      | 4, 2   |
| Runge-Kutta | Lobatto IIIC order 4 {cite:p}`Hairer1991`    | No       | 3      | 4      |
| Runge-Kutta | Lobatto IIIC order 6 {cite:p}`Hairer1991`    | No       | 4      | 6      |
| Runge-Kutta | Radau IIA order 3 {cite:p}`Hairer1991`       | No       | 2      | 3      |
| Runge-Kutta | Radau IIA order 5 {cite:p}`Hairer1991`       | No       | 3      | 5      |
| Runge-Kutta | Radau IIA other orders {cite:p}`Hairer1991`  | No       | $s$    | $2s-1$ |
| Rosenbrock  | RODASP                                       | No       | 6      | 4, ?   |
| Rosenbrock  | RODASPR {cite:p}`Rang2015`                   | No       | 6      | 4, ?   |
| Rosenbrock  | ROS3w {cite:p}`Rang2005`                     | No       | 3      | 3, 2   |
| Rosenbrock  | ROS34PW3 {cite:p}`Rang2005`                  | No       | 4      | 4, 2   |
| Rosenbrock  | ROS34PRW {cite:p}`Rang2015`                  | No       | 4      | 3, 2   |
| AVF         | AVF order 2 {cite:p}`Quispel2008`            | No       | 1      | 2      |
| AVF         | AVF order 3 {cite:p}`Quispel2008`            | No       | 1      | 3      |
| AVF         | AVF order 4 {cite:p}`Quispel2008`            | No       | 1      | 4      |
| Symplectic  | Leap-frog                                    | Yes      | 3      | 2      |
| Symplectic  | Fourth-order formula in {cite:p}`Forest1990` | Yes      | 7      | 4      |
| Others      | Implicit Euler                               | No       | 1      | 1      |

- Order: The order of the formula.
  Two orders separated by a comma indicate that the method is an embedded method.
  This property is desirable for stiff problems.
  A formula is L-stable if it is A-stable and stiffly-accurate (converse is not necessarily true).

## Properties of Implicit Runge-Kutta and Rosenbrock Formulas

| Type        | Formula                                     | Stiffly-Accurate | A-stable | B-stable | L-stable | W-method |
| :---------- | :------------------------------------------ | :--------------- | :------- | :------- | :------- | :------- |
| Runge-Kutta | ARK4(3)6L[2]SA-ESDIRK {cite:p}`Kennedy2003` | Yes              | Yes      |          | Yes      |          |
| Runge-Kutta | ARK5(4)8L[2]SA-ESDIRK {cite:p}`Kennedy2003` | Yes              | Yes      |          | Yes      |          |
| Runge-Kutta | ESDIRK45c {cite:p}`Jorgensen2018`           | Yes              | Yes      |          | Yes      |          |
| Runge-Kutta | 4th order SDIRK {cite:p}`Hairer1991`        | Yes              | Yes      |          | Yes      |          |
| Runge-Kutta | Tanaka Formula 1 {cite:p}`Togawa2007`       | No               |          |          |          |          |
| Runge-Kutta | Tanaka Formula 2 {cite:p}`Togawa2007`       | No               |          |          |          |          |
| Runge-Kutta | Lobatto IIIC order 4 {cite:p}`Hairer1991`   | Yes              | Yes      | Yes      | Yes      |          |
| Runge-Kutta | Lobatto IIIC order 6 {cite:p}`Hairer1991`   | Yes              | Yes      | Yes      | Yes      |          |
| Runge-Kutta | Radau IIA order 3 {cite:p}`Hairer1991`      | Yes              | Yes      | Yes      | Yes      |          |
| Runge-Kutta | Radau IIA order 5 {cite:p}`Hairer1991`      | Yes              | Yes      | Yes      | Yes      |          |
| Runge-Kutta | Radau IIA other orders {cite:p}`Hairer1991` | Yes              | Yes      | Yes      | Yes      |          |
| Rosenbrock  | RODASP                                      | Yes              | Yes      |          | Yes      |          |
| Rosenbrock  | RODASPR {cite:p}`Rang2015`                  | Yes              | Yes      |          | Yes      |          |
| Rosenbrock  | ROS3w {cite:p}`Rang2005`                    | No               | Yes      |          | Yes      | W-method |
| Rosenbrock  | ROS34PW3 {cite:p}`Rang2005`                 | No               | Yes      |          |          | W-method |
| Rosenbrock  | ROS34PRW {cite:p}`Rang2015`                 | No               | Yes      |          |          | W-method |

- Stiffly-Accurate: The last stage of the method is the same as the final solution.
  This property is desirable for stiff problems.
  This property implicitly means that the method is implicit.
- A-stable: The stability region includes the entire left half of the complex plane.
  This property is desirable for stiff problems.
- B-stable: A-stable and some additional conditions are satisfied.
- L-stable: A-stable and the stability function goes to zero as the argument goes to infinity.
- W-method: Approximate Jacobian can be used instead of the exact Jacobian.
  This property only applies to Rosenbrock methods.

## Support of Mass Matrices

| Type        | Formula                                      | Constant Mass | Changeable Mass | DAE     |
| :---------- | :------------------------------------------- | :------------ | :-------------- | :------ |
| Runge-Kutta | ARK4(3)6L[2]SA-ERK {cite:p}`Kennedy2003`     | No            | No              | None    |
| Runge-Kutta | ARK4(3)6L[2]SA-ESDIRK {cite:p}`Kennedy2003`  | Yes           | Yes             | None    |
| Runge-Kutta | ARK5(4)8L[2]SA-ESDIRK {cite:p}`Kennedy2003`  | Yes           | Yes             | None    |
| Runge-Kutta | DOPRI5 {cite:p}`Hairer1991`                  | No            | No              | None    |
| Runge-Kutta | ESDIRK45c {cite:p}`Jorgensen2018`            | Yes           | Yes             | None    |
| Runge-Kutta | RK4 {cite:p}`Hairer1993`                     | No            | No              | None    |
| Runge-Kutta | RKF45 {cite:p}`Hairer1993`                   | No            | No              | None    |
| Runge-Kutta | 4th order SDIRK {cite:p}`Hairer1991`         | Yes           | Yes             | Index 2 |
| Runge-Kutta | Tanaka Formula 1 {cite:p}`Togawa2007`        | Yes           | Yes             | ???     |
| Runge-Kutta | Tanaka Formula 2 {cite:p}`Togawa2007`        | Yes           | Yes             | ???     |
| Runge-Kutta | Lobatto IIIC order 4 {cite:p}`Hairer1991`    | Yes           | No              | Index 2 |
| Runge-Kutta | Lobatto IIIC order 6 {cite:p}`Hairer1991`    | Yes           | No              | Index 2 |
| Runge-Kutta | Radau IIA order 3 {cite:p}`Hairer1991`       | Yes           | No              | Index 2 |
| Runge-Kutta | Radau IIA order 5 {cite:p}`Hairer1991`       | Yes           | No              | Index 3 |
| Runge-Kutta | Radau IIA other orders {cite:p}`Hairer1991`  | Yes           | No              | Index 2 |
| Rosenbrock  | RODASP                                       | Yes           | No              | Index 1 |
| Rosenbrock  | RODASPR {cite:p}`Rang2015`                   | Yes           | No              | Index 2 |
| Rosenbrock  | ROS3w {cite:p}`Rang2005`                     | Yes           | No              | Index 1 |
| Rosenbrock  | ROS34PW3 {cite:p}`Rang2005`                  | Yes           | No              | Index 1 |
| Rosenbrock  | ROS34PRW {cite:p}`Rang2015`                  | Yes           | No              | Index 2 |
| AVF         | AVF order 2 {cite:p}`Quispel2008`            | No            | No              | None    |
| AVF         | AVF order 3 {cite:p}`Quispel2008`            | No            | No              | None    |
| AVF         | AVF order 4 {cite:p}`Quispel2008`            | No            | No              | None    |
| Symplectic  | Leap-frog                                    | No            | No              | None    |
| Symplectic  | Fourth-order formula in {cite:p}`Forest1990` | No            | No              | None    |
| Others      | Implicit Euler                               | Yes           | Yes             | ???     |

- Constant Mass: The method can be applied to problems with constant mass matrices.
- Changeable Mass: The method can be applied to problems with mass matrices that change with time or variables.
- DAE: The method can be applied to differential-algebraic equations (DAEs) with the specified index.
  The value of the index are the highest index of DAEs that are solved in literatures.
  At least index 1 is required to use singular mass matrices.
