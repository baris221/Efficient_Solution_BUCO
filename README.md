# Efficient Solutions for Ordinal BUCO

**Algorithms for enumerating Pareto-efficient solutions of an unconstrained binary combinatorial optimization problem with ordinal item categories and a weight objective.**

This repository contains C and Julia implementations of four solution approaches, together with benchmark instances. The main goal is to compute the **nondominated objective vectors** associated with subsets of items when item quality is expressed by **ordered categories** and total weight is to be minimized.

## 1. Problem description

Consider a set of $n$ items $N=\{1,\ldots,n\}$. Each item $i$ has:

- a positive integer weight $w_i$;
- an ordinal quality category $c_i\in\{1,\ldots,k\}$, where category 1 is preferred to category 2, and so on;
- a binary decision variable $x_i\in\{0,1\}$ indicating whether the item is selected.

There is **no fixed knapsack capacity** in the default formulation. Instead, the aim is to select items that are as good as possible in the ordinal sense while keeping their total weight low.

### Cumulative representation of ordinal quality

The category of an item is represented by a cumulative binary vector. With three ordered categories, for example:

| Item category | Cumulative vector |
| --- | --- |
| 1 (best) | $(1,1,1)$ |
| 2 | $(0,1,1)$ |
| 3 (worst) | $(0,0,1)$ |

For each $r=1,\ldots,k$, let

$$
f_r(x)=\sum_{i\in N:\,c_i\le r}x_i.
$$

Thus, $f_r(x)$ counts the selected items belonging to the **best $r$ categories**. The total selected weight is

$$
w(x)=\sum_{i\in N}w_i x_i.
$$

The problem can be written as the multiobjective maximization problem

$$
\max_{x\in\{0,1\}^{n}}\big(f_1(x),\ldots,f_k(x),-w(x)\big).
$$

A solution is **Pareto-efficient** if no other selection is at least as good in every objective and strictly better in at least one objective.

**Implementation detail:** the C programs store the last objective as $W_{\mathrm{all}}-w(x)$, where $W_{\mathrm{all}}=\sum_i w_i$, rather than as $-w(x)$. These are equivalent for Pareto dominance because they differ by a constant. The Julia program uses $-w(x)$ directly.

## 2. Implemented approaches

| Directory | Method | Main implementation | Summary |
| --- | --- | --- | --- |
| [`DP_BUCO/`](DP_BUCO/) | Dynamic programming (DP) | `knapsackDP.c` | Processes categories and maintains nondominated states indexed by total weight. |
| [`TS_BUCO/`](TS_BUCO/) | Tree search (TS) | `knapsackTS.c` | Recursively explores the number of selected items per category, using pruning rules. |
| [`FKSS_BUCO/`](FKSS_BUCO/) | Category-profile enumeration (FKSS) | `Greedy.c`, `VectorEnum.c` | Enumerates cumulative category profiles, constructs a minimum-weight selection for each feasible profile, and filters dominated vectors. |
| [`TV_BUCO/`](TV_BUCO/) | Tamby–Vanderpooten (TV) | `TambySolver.jl` | Uses the Tamby–Vanderpooten multiobjective enumeration algorithm through Julia's MultiObjectiveAlgorithms.jl, JuMP, and Gurobi. |

### Dynamic programming (`DP_BUCO`)

The DP processes categories one at a time. For each category, it considers selecting zero items or a prefix of its lightest items. States are indexed by their exact total weight and store nondominated ordinal objective vectors. After processing all categories, the weight-indexed states are combined into a final nondominated set.

The dynamic program uses an **ND-Tree** archive to support dominance checks and Pareto filtering.

### Tree search (`TS_BUCO`)

The tree-search implementation branches on how many items to select from each category. Because items within a category have identical ordinal contributions, a given category count is represented by selecting the lightest items in that category. The search includes pruning tests based on exchanging a selected item for a lighter item from a better category.

### FKSS profile enumeration (`FKSS_BUCO`)

`VectorEnum.c` generates nondecreasing cumulative profiles. `Greedy.c` checks whether each profile is feasible given the number of available items in each category, then computes its minimum possible weight by selecting the lightest items of each category. The resulting vectors are inserted into an ND-Tree archive.

Although the corresponding function is named `GreedyAlgorithm`, **the current implementation enumerates candidate category profiles** rather than repeatedly applying a conventional value-to-weight-ratio greedy heuristic. The number of candidate profiles can grow quickly with the number of items and categories.

### Tamby–Vanderpooten (`TV_BUCO`)

The Julia implementation models the binary problem using JuMP and calls `MOA.TambyVanderpooten()` with Gurobi as the underlying mixed-integer optimizer. By default, it maximizes the cumulative ordinal objectives and the negative selected weight, without a capacity constraint. It prints the objective vector, selected items, and total weight for each returned solution.

The Julia file also defines optional arguments for a capacity-constrained variant and a solution limit; using these options requires editing the final `solve_instance(...)` call or invoking the function from Julia.

## 3. Repository structure

```text
Efficient_Solution_BUCO/
├── DP_BUCO/
│   ├── knapsackDP.c / knapsackDP.h
│   ├── read_instance.c / read_instance.h
│   ├── main.c
│   ├── NDTree/
│   ├── makefile
│   └── myScript.sh
├── TS_BUCO/
│   ├── knapsackTS.c / knapsackTS.h
│   ├── read_instance.c / read_instance.h
│   ├── main.c
│   ├── NDTree/
│   ├── makefile
│   └── myScript.sh
├── FKSS_BUCO/
│   ├── Greedy.c / Greedy.h
│   ├── VectorEnum.c / VectorEnum.h
│   ├── read_instance.c / read_instance.h
│   ├── main.c
│   ├── NDTree/
│   ├── makefile
│   └── myScript.sh
├── TV_BUCO/
│   ├── TambySolver.jl
│   └── myScript.sh
├── Instance_UN_<k>_<n>/
│   └── instance_1.txt ... instance_30.txt
├── Instance_CO_<k>_<n>/
│   └── instance_1.txt ... instance_30.txt
├── makefile
├── README.md
└── LICENSE
```

The `NDTree/` directories contain the shared-style C implementation of an archive for nondominated solutions (`treeNDS`, `tabNDS`, `tabSol`, and `dominance`). Each C solver has its own copy.

## 4. Requirements and installation

### C implementations

- GCC (or a compatible C compiler)
- GNU Make
- Bash for the batch scripts

Clone the repository and compile all three C implementations:

```bash
git clone https://github.com/baris221/Efficient_Solution_BUCO.git
cd Efficient_Solution_BUCO
make
```

The root `make` target builds:

- `DP_BUCO/mainDP`
- `TS_BUCO/mainTS`
- `FKSS_BUCO/mainGreedy`

To compile only one solver, use `make dp`, `make ts`, or `make greedy` at the repository root. To remove the compiled C targets and object files, run:

```bash
make clean
```

The root Makefile **does not build the Julia solver**.

### Julia implementation

Install Julia, Gurobi (including a working license), and these Julia packages:

```julia
import Pkg
Pkg.add(["JuMP", "Gurobi", "MultiObjectiveAlgorithms"])
```

The Julia code imports `JuMP`, `Gurobi`, and `MultiObjectiveAlgorithms` directly. No separate C compilation is required for this solver.

## 5. Running the algorithms

All commands below start from the **repository root** unless a `cd` command is shown.

### Run one instance with a C solver

**Dynamic programming**

```bash
cd DP_BUCO
./mainDP ../Instance_UN_3_100/instance_1.txt result.txt
```

**Tree search**

```bash
cd TS_BUCO
./mainTS ../Instance_UN_3_100/instance_1.txt result.txt
```

**FKSS enumeration**

```bash
cd FKSS_BUCO
./mainGreedy ../Instance_UN_3_100/instance_1.txt result.txt
```

Run each example from the indicated solver directory (return to the repository root before changing to another directory). Each executable accepts the same two command-line arguments:

```text
./<executable> <instance_file> <result_file>
```

It prints execution time and the number of nondominated vectors to the terminal, and writes the nondominated objective vectors to the result file.

### Run a benchmark family with a C solver

Each solver has a `myScript.sh` that expects three arguments:

```text
bash myScript.sh <UN|CO> <number_of_categories> <number_of_items>
```

For example, after building the executables:

```bash
cd DP_BUCO
bash myScript.sh UN 3 100
```

This processes `Instance_UN_3_100/instance_1.txt` through `instance_30.txt` and creates:

- `Results_UN_3_100/res_1.txt` through `res_30.txt`, containing nondominated objective vectors;
- `log_UN_3_100.txt`, containing the appended execution-time and solution-count messages.

The same interface works from `TS_BUCO/` and `FKSS_BUCO/`. **Run each script from its own solver directory**, since it invokes the executable with a relative path such as `./mainDP`. The scripts use Bash syntax and assume 30 instance files per family.

### Run the Julia solver

From the repository root:

```bash
julia TV_BUCO/TambySolver.jl Instance_UN_3_100/instance_1.txt
```

Alternatively, run a batch from inside `TV_BUCO/`:

```bash
cd TV_BUCO
bash myScript.sh UN 3 100
```

The Julia batch script writes solver output into `Results_UN_3_100/res_1.txt` through `res_30.txt`.

## 6. Benchmark instances

Benchmark directories use the naming convention:

```text
Instance_<type>_<k>_<n>/
```

where:

- `type = UN`: uncorrelated instance family;
- `type = CO`: correlated instance family;
- `k`: number of ordinal categories (and cumulative quality objectives);
- `n`: number of items.

The current repository contains **22 benchmark families with 30 instances each (660 instance files)**, covering different values of `k` and `n`.

### Instance file format

An instance is a plain-text file with the following layout:

```text
VARIABLES 4
NB_OBJ 3
ITEMS
[1, 1, 1] 10
[0, 1, 1] 8
[0, 1, 1] 12
[0, 0, 1] 5
```

Here:

- `VARIABLES` gives the total number of items;
- `NB_OBJ` gives the number of cumulative ordinal objectives (not counting weight);
- each subsequent row specifies the item's cumulative category vector followed by its weight.

The C input parsers infer the category index from the sum of the cumulative coefficients. They therefore expect **step-shaped category vectors** of the form shown above, not arbitrary profit vectors.

**Important input convention:** The C algorithms construct the minimum-weight representative of a category profile by taking a prefix of that category's items. Therefore, **items within each category should appear in nondecreasing order of weight**, as in the provided benchmark files. If you generate new instances, preserve this ordering.

## 7. Output and comparison

For a C solver, each line of the result file is an objective vector:

```text
f_1 f_2 ... f_k (W_all - selected_weight)
```

The C programs currently call `writeTreeNDS(..., 0, ...)`, so these result files **contain objective vectors only**, not the full item-selection vectors. The last coordinate is a transformed weight-minimization objective, *not* a remaining capacity in a constrained knapsack model.

The terminal also reports:

```text
Execution time: ... seconds
Number of non dominated solutions: ...
```

(The precise capitalization and spacing differ slightly between solvers.)

The Julia solver prints objective vectors with **negative weight** as the last coordinate and additionally reports the selected item indices. When comparing Julia output with C output, account for this constant shift in the weight coordinate, and compare objective vectors rather than file ordering.

## 8. Notes and limitations

- These are research implementations focused on **Pareto-front enumeration** and comparative algorithmic experiments.
- The default problem is **unconstrained** in the sense that there is no externally specified capacity limit. The DP uses the sum of all item weights as the upper bound of its internal weight-state range.
- Input ordering matters for the C solvers: items must be ordered by nondecreasing weight within each category.
- Performance can depend strongly on the number of categories, number of items, weight distribution, and size of the nondominated set.
- The batch scripts append to existing C log files; remove or rename a previous log if you want a fresh experimental record.
- The Julia implementation depends on Gurobi and an appropriate Gurobi license.

## 9. Author and license

Developed by **Baris Kaftancioglu** for research on ordinal combinatorial optimization and Pareto-efficient solution enumeration.

Distributed under the [MIT License](LICENSE). See `LICENSE` for the full terms.

