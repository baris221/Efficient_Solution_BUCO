# Solves a binary multi-objective instance using JuMP and MOA.
#
# One-time installation in Julia:
#   import Pkg
#   Pkg.add(["JuMP", "Gurobi", "MultiObjectiveAlgorithms"])
#
# Execution:
#   julia mokp_moa_gurobi.jl instance_1.txt

using JuMP
import Gurobi
import MultiObjectiveAlgorithms as MOA

"""
    read_instance(path)

Reads an instance with the following format:

VARIABLES n
NB_OBJ p
ITEMS
[c_1, ..., c_p] weight
...

Returns:
- `profits`: a p × n matrix, where `profits[k, i]` is the coefficient
             of item i in objective k;
- `weights`: the vector of item weights.
"""
function read_instance(path::String)
    lines = readlines(path)

    n = nothing
    nb_obj = nothing
    item_lines = String[]
    reading_items = false

    for line in lines
        s = strip(line)
        isempty(s) && continue

        if startswith(s, "VARIABLES")
            n = parse(Int, split(s)[2])
        elseif startswith(s, "NB_OBJ")
            nb_obj = parse(Int, split(s)[2])
        elseif startswith(s, "ITEMS")
            reading_items = true
        elseif reading_items
            push!(item_lines, s)
        end
    end

    n === nothing && error("The VARIABLES line is missing.")
    nb_obj === nothing && error("The NB_OBJ line is missing.")

    if length(item_lines) != n
        @warn "The number of ITEMS lines does not match VARIABLES." declared = n found = length(item_lines)
    end

    n_items = length(item_lines)
    profits = zeros(Int, nb_obj, n_items)
    weights = zeros(Int, n_items)

    for (i, s) in enumerate(item_lines)
        m = match(r"^\[([^\]]+)\]\s+(-?\d+)\s*$", s)
        m === nothing && error("Invalid item line: $s")

        coeffs = parse.(Int, strip.(split(m.captures[1], ",")))
        length(coeffs) == nb_obj ||
            error("Item $i: incorrect number of coefficients.")

        profits[:, i] .= coeffs
        weights[i] = parse(Int, m.captures[2])
    end

    return profits, weights
end

"""
    solve_instance(
        path;
        capacity=nothing,
        weight_as_objective=true,
        solution_limit=nothing,
    )

When `weight_as_objective = true`, the provided LP file is reproduced:

    Maximize (objective_1, objective_2, -weight)

To solve a standard bi-objective knapsack problem with a capacity
constraint, use, for example:

    solve_instance(
        "instance_1.txt";
        capacity=1000,
        weight_as_objective=false,
    )

This gives:

    Maximize (objective_1, objective_2)
    subject to sum(weight_i * x_i) <= 1000.
"""
function solve_instance(
    path::String;
    capacity::Union{Nothing,Int}=nothing,
    weight_as_objective::Bool=true,
    solution_limit::Union{Nothing,Int}=nothing,
)
    profits, weights = read_instance(path)
    nb_obj, n = size(profits)

    model = Model(() -> MOA.Optimizer(Gurobi.Optimizer))

    # Tamby-Vanderpooten is designed for discrete optimization problems
    # and supports more than two objectives.
    set_attribute(model, MOA.Algorithm(), MOA.TambyVanderpooten())

    # Use the following line to run the Kirlik-Sayin algorithm instead:
    # set_attribute(model, MOA.Algorithm(), MOA.KirlikSayin())

    # Optional: limit the number of returned non-dominated solutions.
    if solution_limit !== nothing
        set_attribute(model, MOA.SolutionLimit(), solution_limit)
    end

    @variable(model, x[1:n], Bin)

    if capacity !== nothing
        @constraint(
            model,
            sum(weights[i] * x[i] for i in 1:n) <= capacity,
        )
    end

    objective_exprs = [
        sum(profits[k, i] * x[i] for i in 1:n)
        for k in 1:nb_obj
    ]

    if weight_as_objective
        # Transform weight minimization into maximization by maximizing
        # the negative total weight.
        push!(
            objective_exprs,
            -sum(weights[i] * x[i] for i in 1:n),
        )
    end

    @objective(model, Max, objective_exprs)

    optimize!(model)

    println("\nTermination status: ", termination_status(model))
    println("Number of returned solutions: ", result_count(model))

    for r in 1:result_count(model)
        z = objective_value(model; result=r)
        selected = [
            i for i in 1:n
            if value(x[i]; result=r) > 0.5
        ]
        total_weight = sum(weights[i] for i in selected)

        println("\nSolution ", r)
        println("  Objective vector = ", z)
        println("  Total weight = ", total_weight)
        println("  Selected items = ", selected)
    end

    return model, x, profits, weights
end

# -----------------------
# Main program
# -----------------------

instance_file = length(ARGS) >= 1 ?
    ARGS[1] :
    "Instance_UN_3_100/instance_1.txt"

# This version reproduces the LP file:
# Maximize (objective_1, objective_2, -weight), without a capacity constraint.
solve_instance(instance_file; weight_as_objective=true)

# For the standard knapsack version, comment out the previous line
# and use:
#
# solve_instance(
#     instance_file;
#     capacity=1000,
#     weight_as_objective=false,
# )