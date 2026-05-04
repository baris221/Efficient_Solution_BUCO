import sys

def parse_mokp_instance(input_file, output_file):
    with open(input_file, "r") as f:
        lines = [line.strip() for line in f if line.strip()]

    # --- Extract header values ---
    #capacity = int(lines[0].split()[1])
    n_vars = int(lines[0].split()[1])
    nb_obj = int(lines[1].split()[1])

    # --- Extract items ---
    items_start = lines.index("ITEMS") + 1
    items = []
    capacity=0

    for line in lines[items_start:]:
        # Format: [a, b, c] weight
        vec_part, w = line.split("]")
        vec_str = vec_part.replace("[", "")
        profits = list(map(int, vec_str.split(",")))
        weight = int(w.strip())
        capacity=capacity+weight
        items.append((profits, weight))
    

    # --- Write LP file ---
    with open(output_file, "w") as f:
        f.write("Maximize multi-objectives\n\n")
        # OBJECTIVES
        for obj in range(nb_obj):
            obj_terms = [f"{items[i][0][obj]} x{i+1}" for i in range(n_vars)]
            f.write(f"OBJ{obj+1}: Priority={nb_obj-obj} Weight=1 AbsTol=0 RelTol=0 \n")
            f.write(" + ".join(obj_terms) + "\n")
            f.write("\n")
        f.write("\n")

        # MINIMIZE WEIGHT
        w_terms = [f"{-1*items[i][1]} x{i+1}" for i in range(n_vars)]
        f.write(f"OBJ{nb_obj+1}: Priority={nb_obj-obj} Weight=1 AbsTol=0 RelTol=0 \n")
        f.write(" + ".join(w_terms) + "\n") # Multiply by -1 if you want to minimize weight
        f.write("\n")

        # CAPACITY CONSTRAINT
        #w_terms = [f"{items[i][1]} x{i+1}" for i in range(n_vars)]
        #f.write("subject to \n")
        #f.write("c1: " + " + ".join(w_terms) + f" <= {capacity};\n\n")

        # VARIABLE DOMAINS
        f.write("Binary \n")
        for i in range(n_vars):
            f.write(f" x{i+1}")
        
        f.write("\n")
        f.write("End\n")


# -------- Example usage --------

#parse_mokp_instance("Instance_UN_3_100/instance_1.txt", "mokp1.lp")
#print("Generated mokp1.lp")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_PL.py <input_instance_file> <output_lp_file>")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        parse_mokp_instance(input_file, output_file)
        #print(f"Generated {output_file} from {input_file}")