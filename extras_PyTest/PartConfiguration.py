import os

from OCC.Core.STEPControl import STEPControl_Reader
from OCC.Core.IGESControl import IGESControl_Reader
from OCC.Core.TColStd import TColStd_SequenceOfAsciiString
from OCC.Core.IFSelect import IFSelect_RetDone, IFSelect_ItemsByEntity
from OCC.Extend.TopologyUtils import list_of_shapes_to_compound

from OCC.Core.TopoDS import TopoDS_Solid, TopoDS_Compound, TopoDS_Shell
from OCC.Core.BRep import BRep_Builder 
from OCC.Extend.TopologyUtils import TopologyExplorer
from OCC.Core.BRepCheck import BRepCheck_Analyzer
from OCC.Core.BRepBuilderAPI import BRepBuilderAPI_Sewing, BRepBuilderAPI_MakeSolid
from OCC.Core.BRep import BRep_Tool

from Exceptions import *


# Constants
Materials = ["alu-5083", "alu-6082", "pom-c-white", "pom-c-black"]
MaterialsThicknesses = { "alu-5083": [10, 12, 16, 20, 25, 40, 60, 100, 150], "alu-6082": [10, 12, 16, 20, 25, 40, 60, 100, 150], "pom-c-white": [10, 12, 16, 20, 25, 40, 60, 100, 150], "pom-c-black": [10, 12, 16, 20, 25, 40, 60, 100, 150] }


def createConfiguration(qty, material, finish):
    configuration = {"quantity": qty, "material": material, "finish": finish}
    return configuration


# Loads part from path. Extension must be lowercase
def loadPart(modelPath):

    model_solid = None
    filename, extension = os.path.splitext(modelPath)
    
    # .step file
    if (extension == ".step" or extension == ".stp"):
        model_solid, modelUnits = read_step_file_with_units(modelPath)

        # Assert model units as millimetres
        if modelUnits != "millimetre":
            raise ModelFileUnitError("Model units is not in millimetres")
    
    # .iges file
    elif (extension == ".iges" or extension == ".igs"):
        model_solid = read_iges_file_as_solid(modelPath)

        # .iges files are always in mm
    
    # unknown filetype
    else:
        raise ModelFileTypeError('Model type is unknwon (not .step or .iges)')

    # Assert that only 1 solid is present


    # Assert model validity
    check_analyzer = BRepCheck_Analyzer(model_solid)
    if not check_analyzer.IsValid():
        raise ModelValidityError("Model isn't valid")

    # Return model as a solid
    return model_solid


def read_step_file_with_units(filename, verbose=False):
    
    if not os.path.isfile(filename):
        raise FileNotFoundError("%s not found." % filename)

    # Init .step reader
    step_reader = STEPControl_Reader()
    if verbose:
        failsonly = False
        step_reader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity)
        step_reader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity)
    
    # Read file
    read_status = step_reader.ReadFile(filename)
    if read_status != IFSelect_RetDone:
        raise ModelReadError("{} was not read properly.".format(filename))

    # Read model units from file
    unitsLength = TColStd_SequenceOfAsciiString()
    unitsAngle = TColStd_SequenceOfAsciiString()
    unitsSolidAngle = TColStd_SequenceOfAsciiString()
    step_reader.FileUnits(unitsLength, unitsAngle, unitsSolidAngle)
    units = unitsLength.Value(1).PrintToString()

    # Transfer shapes
    transfer_result = step_reader.TransferRoots()
    if not transfer_result:
        raise ModelTransferError("Transfer failed.")

    # Assert that shapes are transfered
    _nbs = step_reader.NbShapes()
    if _nbs == 0:
        raise ModelNoShapesError("No shape to transfer.")
    
    elif _nbs > 1:
        raise ModelMultipleShapesError("Multiple shapes found in {}".format(filename))

    # Found 1 shape/solid
    step_shape = step_reader.Shape(1)
    
    return step_shape, units


def read_iges_file_as_solid(filename, verbose=False, visible_only=False):
    
    # Assert file exists
    if not os.path.isfile(filename):
        raise FileNotFoundError("{} not found.".format(filename))

    # Init iges reader
    iges_reader = IGESControl_Reader()
    iges_reader.SetReadVisible(visible_only)
    if verbose:
        failsonly = False
        iges_reader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity)
        iges_reader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity)

    # Read file
    read_status = iges_reader.ReadFile(filename)
    if read_status != IFSelect_RetDone:
        raise ModelReadError("{} was not read properly.".format(filename))

    # Transfer shapes
    iges_reader.TransferRoots()
    
    # Assert that shapes are transferred for all roots
    nbr = iges_reader.NbRootsForTransfer()
    for _ in range(1, nbr+1):
        nbs = iges_reader.NbShapes()
        if nbs == 0:
            raise ValueError("Shapes in {} was not transferred properly.".format(filename))

    # Load shape(compound) and sew to solid
    iges_shape = iges_reader.OneShape()
    iges_solid = iges_compound_to_solid(iges_shape)
    
    return iges_solid


# Computes solid from iges compund using BRepBuilderAPI_Sewing
def iges_compound_to_solid(compound):
    builder_sewing = BRepBuilderAPI_Sewing(1.0e-03)
    builder_sewing.Load(compound)
    builder_sewing.Perform()
    sewed_shape = builder_sewing.SewedShape()
    
    solid_builder = BRepBuilderAPI_MakeSolid()
    topo_expl = TopologyExplorer(sewed_shape)
    for shell in topo_expl.shells():
        solid_builder.Add(shell)
    
    if solid_builder.IsDone():
        return solid_builder.Solid()
    
    else:
        raise ModelNoSolidError("No solid was made from iges compound")

    

    