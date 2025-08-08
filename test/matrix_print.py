import gdb

class MatrixPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        rows = int(self.val["rows"])
        cols = int(self.val["cols"])
        values = self.val["values"]

        elements = []

        for i in range(rows):
            row_elements = []
            for j in range(cols):
                cell = float(values[i * cols + j])
                row_elements.append(f"{cell:.2f}")
            elements.append("[" + ", ".join(row_elements) + "]")
        return "[\n" + ",\n".join(elements) + "\n]"

def lookup_type(val):
    if str(val.type) == "struct Matrix":
        return MatrixPrinter(val)
    if val.type.code == gdb.TYPE_CODE_PTR and str(val.type.target()) == 'struct Matrix':
        return MatrixPrinter(val.dereference())
    return None

gdb.pretty_printers.append(lookup_type)
