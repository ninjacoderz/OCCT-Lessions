from OCC.Core.BRepMesh import BRepMesh_IncrementalMesh
from OCC.Core.TopLoc import TopLoc_Location
from OCC.Core.TopoDS import topods_Face
from OCC.Core.BRep import BRep_Tool
from OCC.Core.TopExp import TopExp_Explorer
from OCC.Core.TopAbs import TopAbs_FACE
from OCC.Core.STEPControl import STEPControl_Reader
from OCC.Core.TopoDS import TopoDS_Shape
from OCC.Core.TopExp import TopExp_Explorer
from OCC.Core.TopAbs import TopAbs_FACE
from OCC.Display.SimpleGui import init_display

# Function to read a shape from a STEP file
def read_step_file(filename):
    step_reader = STEPControl_Reader()
    status = step_reader.ReadFile(filename)
    if status == 1:
        step_reader.TransferRoot()
        shape = step_reader.OneShape()
    return shape

# Replace 'your_step_file.step' with the path to your STEP file
shape = read_step_file('C:/Users/serge/Desktop/part.step')

display, start_display, add_menu, add_function_to_menu = init_display()
display.DisplayShape(shape, update=True)
start_display()