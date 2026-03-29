# Properties of Formulas in ODE Solvers

| Type        | Formula                                      | Explicit | Order | Stiffly-Accurate | Stability | DAE Support | Notes / Keywords     |
| :---------- | :------------------------------------------- | :------- | :---- | :--------------- | :-------- | :---------- | :------------------- |
| Runge-Kutta | ARK4(3)6L[2]SA-ERK {cite:p}`Kennedy2003`     | Yes      | 4, 3  | No               |           |             | Additive Runge-Kutta |
| Runge-Kutta | ARK4(3)6L[2]SA-ESDIRK {cite:p}`Kennedy2003`  | No       | 4, 3  | Yes              | L-stable  |             | Additive Runge-Kutta |
| Runge-Kutta | ARK5(4)8L[2]SA-ESDIRK {cite:p}`Kennedy2003`  | No       | 5, 4  | Yes              | L-stable  |             | Additive Runge-Kutta |
| Runge-Kutta | DOPRI5 {cite:p}`Hairer1991`                  | Yes      | 5, 4  | No               |           |             |                      |
| Runge-Kutta | ESDIRK45c {cite:p}`Jorgensen2018`            | No       | 4, 5  | Yes              | L-stable  |             |                      |
| Runge-Kutta | RK4 {cite:p}`Hairer1993`                     | Yes      | 4     | No               |           |             |                      |
| Runge-Kutta | RKF45 {cite:p}`Hairer1993`                   | Yes      | 4, 5  | No               |           |             |                      |
| Runge-Kutta | 4th order SDIRK {cite:p}`Hairer1991`         | No       | 4, 3  | Yes              | L-stable  |             |                      |
| Runge-Kutta | Tanaka Formula 1 {cite:p}`Togawa2007`        | No       | 3, 1  | No               |           |             |                      |
| Runge-Kutta | Tanaka Formula 2 {cite:p}`Togawa2007`        | No       | 4, 2  | No               |           |             |                      |
| Rosenbrock  | RODASP                                       | No       | 4, ?  | Yes              | L-stable  | Index 1     |                      |
| Rosenbrock  | RODASPR {cite:p}`Rang2015`                   | No       | 4, ?  | Yes              | L-stable  | Index 2 ?   |                      |
| Rosenbrock  | ROS3w {cite:p}`Rang2005`                     | No       | 3, 2  | No               | L-stable  | Index 1     |                      |
| Rosenbrock  | ROS34PW3 {cite:p}`Rang2005`                  | No       | 4, 2  | No               | A-stable  | Index 1     |                      |
| AVF         | AVF order 2 {cite:p}`Quispel2008`            | No       | 2     |                  |           |             |                      |
| AVF         | AVF order 3 {cite:p}`Quispel2008`            | No       | 3     |                  |           |             |                      |
| AVF         | AVF order 4 {cite:p}`Quispel2008`            | No       | 4     |                  |           |             |                      |
| Symplectic  | Leap-frog                                    | Yes      | 2     |                  |           |             |                      |
| Symplectic  | Fourth-order formula in {cite:p}`Forest1990` | Yes      | 4     |                  |           |             |                      |
| Others      | Implicit Euler                               | No       | 1     |                  |           |             |                      |

- Order: The order of the formula.
  Two orders separated by a comma indicate that the method is an embedded method.
- Stiffly-Accurate: The last stage of the method is the same as the final solution.
  This property is desirable for stiff problems.
  This property implicitly means that the method is implicit.
- A-stable: The stability region includes the entire left half of the complex plane.
  This property is desirable for stiff problems.
- L-stable: A-stable and the stability function goes to zero as the argument goes to infinity.
  This property is desirable for stiff problems.
- DAE Support: The method can be applied to differential-algebraic equations (DAEs) with the specified index.
  The index of RODASPR is not explicitly stated in the reference,
  but numerical experiments to index 2 problems are reported in the reference.
