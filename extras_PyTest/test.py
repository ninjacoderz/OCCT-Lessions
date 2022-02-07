import logging
logging.basicConfig(level=logging.INFO)

import math, os, random, time, sys

# PyOCC Imports
from OCC.Extend.TopologyUtils import TopologyExplorer
from OCC.Core.GeomAbs import GeomAbs_Plane, GeomAbs_Cylinder, GeomAbs_Cone, GeomAbs_Sphere, GeomAbs_Torus, GeomAbs_BezierSurface, GeomAbs_BSplineSurface, GeomAbs_SurfaceOfRevolution, GeomAbs_SurfaceOfExtrusion, GeomAbs_OffsetSurface, GeomAbs_OtherSurface, GeomAbs_Circle, GeomAbs_IsoU
from OCC.Core.BRepAdaptor import BRepAdaptor_Surface, BRepAdaptor_Curve
from OCC.Core.TopoDS import TopoDS_Face, TopoDS_Edge
from OCC.Core.gp import gp_Pnt, gp_Pln, gp_Ax1, gp_Dir, gp_Vec
from OCC.Core.TopAbs import TopAbs_ON, TopAbs_OUT, TopAbs_IN, TopAbs_UNKNOWN
from OCC.Core.GProp import GProp_GProps
from OCC.Core.BRepGProp import brepgprop_VolumeProperties, brepgprop_SurfaceProperties, brepgprop_LinearProperties
from OCC.Core.Bnd import Bnd_Box
from OCC.Core.BRepBndLib import brepbndlib_Add, brepbndlib_AddOptimal

# __main__ imports
from PartConfiguration import *
from OCC.Display.SimpleGui import init_display
from OCC.Core.Quantity import Quantity_Color, Quantity_TOC_RGB


from OCC.Display.SimpleGui import init_display
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeBox

# display, start_display, add_menu, add_function_to_menu = init_display()
# my_box = BRepPrimAPI_MakeBox(10., 20., 30.).Shape()

# display.DisplayShape(my_box, update=True)
# start_display()


states = { TopAbs_ON: "TopAbs_ON", TopAbs_OUT: "TopAbs_OUT", TopAbs_IN: "TopAbs_IN", TopAbs_UNKNOWN: "TopAbs_UNKNOWN" }
SurfaceTypeNames = { GeomAbs_Plane: "Plane", GeomAbs_Cylinder: "Cylinder", GeomAbs_Cone: "Cone", GeomAbs_Sphere: "Sphere", GeomAbs_Torus: "Torus", GeomAbs_BezierSurface: "BezierSurface", GeomAbs_BSplineSurface: "BSplineSurface", GeomAbs_SurfaceOfRevolution: "SurfaceOfRevolution", GeomAbs_SurfaceOfExtrusion: "SurfaceOfExtrusion", GeomAbs_OffsetSurface: "OffsetSurface", GeomAbs_OtherSurface: "OtherSurface" }

RefAxisXP = gp_Dir( 1,  0,  0)
RefAxisXN = gp_Dir(-1,  0,  0)
RefAxisYP = gp_Dir( 0,  1,  0)
RefAxisYN = gp_Dir( 0, -1,  0)
RefAxisZP = gp_Dir( 0,  0,  1)
RefAxisZN = gp_Dir( 0,  0, -1)
RefAxisUnknown = 0
RefAxes = [ RefAxisZP, RefAxisZN, RefAxisXP, RefAxisXN, RefAxisYP, RefAxisYN ]
RefAxesNames = { RefAxisZP: "Z+", RefAxisZN: "Z-", RefAxisXP: "X+", RefAxisXN: "X-", RefAxisYP: "Y+", RefAxisYN: "Y-", RefAxisUnknown: "Unknown" }

# Constants
MachiningTimePerVolume = 1/10000.0    # For roughing calculations
MachiningTimeBBPerVolume = 1/25000.0
MachiningTimePerArea = 1/3000.0    # For finishing calculations
MachiningTimePerFeatureRadii = 1.0   # For details calculations
MachiningTimePer3DArea = 1/150.0    # For 3d milling calculations
MachiningTimePer3DSurface = 1/2.5 # For 3d milling calculations


# Top level Part analysis
def partAnalysis(modelShape, display=False):
    analysis = {"codeVersion": "0.0"}
    
    # Analyze model
    modelAnalysis(modelShape, analysis)
    printModelAnalysis(analysis)
    
    # Analyze stock
    stockAnalysis(analysis)
    printStockAnalysis(analysis)

    # Analyze machining
    machiningAnalysis(modelShape, analysis)
    printMachiningAnalysis(analysis)

    # # Show faces 
    # print("\nDisplay")
    # t = TopologyExplorer(modelShape)
    # display, start_display, add_menu, add_function_to_menu = init_display()
    # for face in t.faces():
    #     r = 0.3
    #     g = 0.3
    #     b = 0.3
    #     if face in analysis["_modelLinearFaces"]:
    #         b = 0.9
    #     elif face in analysis["_model3DFaces"]:
    #         r = 0.9
    #     display.DisplayColoredShape(face, color=Quantity_Color(r, g, b, Quantity_TOC_RGB))
    # for edge in t.edges():
    #     display.DisplayColoredShape(edge, color=Quantity_Color(0, 0, 0, Quantity_TOC_RGB))
    # start_display()

    # Remove internal analysis parameters
    analysis.pop("_modelFaces")
    analysis.pop("_modelFaceAreas")
    analysis.pop("_modelLinearFaces")
    analysis.pop("_model3DFaces")

    return analysis

# Model analysis
def modelAnalysis(modelShape, analysis):

    t = TopologyExplorer(modelShape)
    
    analysis["modelNumEdges"], analysis["modelNumFaces"], analysis["modelNumSolids"] = countShapeFeatures(modelShape)
    analysis["modelDimX"], analysis["modelDimY"], analysis["modelDimZ"] = calcShapeDimensions(modelShape)
    analysis["modelVolume"] = calcShapeVolume(modelShape)
    analysis["modelSurfaceArea"] = calcShapeSurfaceArea(modelShape)
    analysis["modelEdgeLength"] = calcShapeEdgeLength(modelShape)
    analysis["_modelFaces"], analysis["_modelFaceAreas"], analysis["_modelLinearFaces"], analysis["_model3DFaces"] = analyseShapeFaces(modelShape)
    #analysis["model3DSurfaceArea"] = calcFacesSurfaceArea(analysis["_model3DFaces"], analysis["_modelFaceAreas"])
    analysis["modelNumUniqueFaces"] = countUniqueFaces(t.faces(), analysis["_modelFaceAreas"])
    analysis["model3DSurfaceArea"], analysis["model3DSurfaceCount"] = calcFacesSurfaceArea(analysis["_model3DFaces"], analysis["_modelFaceAreas"])
    analysis['surface_area_xp'], analysis['surface_area_xn'], analysis['surface_area_yp'], analysis['surface_area_yn'], analysis['surface_area_zp'], analysis['surface_area_zn'] = analyseSurfaceArea(modelShape)

    logging.debug(f"Faces: {analysis['modelNumFaces']}")

def printModelAnalysis(analysis):
    strModelDimensions = "  Dimensions: {:0.1f}mm X {:0.1f}mm X {:0.1f}mm".format(analysis["modelDimX"], analysis["modelDimY"], analysis["modelDimZ"])
    strVolume = "  Volume: {:0.0f}mm^3".format(analysis["modelVolume"])
    strSurfaceArea = "  Surface Area: {:0.0f}mm^2".format(analysis["modelSurfaceArea"])
    strDddSurfaceArea = "  3D Surface Area: {:0.0f}mm^2".format(analysis["model3DSurfaceArea"])
    strEdgeLength = "  Edge Length: {:0.0f}mm".format(analysis["modelEdgeLength"])
    strModelFeatures = "  Features (Faces/Edges): {}/{}".format(analysis["modelNumFaces"], analysis["modelNumEdges"])
    numUniqueFaces = analysis["modelNumUniqueFaces"]
    logging.info(f"numUniqueFaces: {numUniqueFaces}")
    print("\nModelAnalysis:\n{}\n{}\n{}\n{}\n{}".format(strModelDimensions, strVolume, strSurfaceArea, strDddSurfaceArea, strEdgeLength, strModelFeatures))

def countFaceTypes(faces):
    faceTypeCounts = { GeomAbs_Plane: 0, GeomAbs_Cylinder: 0, GeomAbs_Cone: 0, GeomAbs_Sphere: 0, GeomAbs_Torus: 0, GeomAbs_BezierSurface: 0, GeomAbs_BSplineSurface: 0, GeomAbs_SurfaceOfRevolution: 0, GeomAbs_SurfaceOfExtrusion: 0, GeomAbs_OffsetSurface: 0, GeomAbs_OtherSurface: 0 }
    for face in faces:
        surface = BRepAdaptor_Surface(face, True)
        faceTypeCounts[surface.GetType()] += 1
        #print(surface.GetType())
    
    strTypeCounts = "Surface Type Counts:\n"
    for surfaceType, surfaceTypeName in SurfaceTypeNames.items():
        strTypeCounts += "{} {}s\n".format(faceTypeCounts[surfaceType], surfaceTypeName)

    return strTypeCounts

def countShapeFeatures(shape):
    t = TopologyExplorer(shape)

    return t.number_of_edges(), t.number_of_faces(), t.number_of_solids()

def countUniqueFaces(faces, faceAreas):
    uniqueFaces = {}

    start = time.time()
    # Test all faces
    for face in faces:
        surface = BRepAdaptor_Surface(face, True)
        surfaceIsIdentical = False
        # Compare with existing faces
        for uniqueFace, uniqueSurface in uniqueFaces.items():
            if face.Orientation() == uniqueFace.Orientation():
                if areSurfacesidentical(surface, uniqueSurface, faceAreas):
                    surfaceIsIdentical = True
                    break

        if not surfaceIsIdentical:
            uniqueFaces[face] = surface

    end = time.time()
    logging.debug(f"Time: {end-start}")
    logging.debug("Unique Faces: " + countFaceTypes(uniqueFaces))
    return len(uniqueFaces)

def areSurfacesidentical(s1, s2, faceAreas):

    if s1.GetType() != s2.GetType():
        return False
    elif abs(s1.FirstUParameter() - s2.FirstUParameter()) > 1e-6:
        return False
    elif abs(s1.FirstVParameter() - s2.FirstVParameter()) > 1e-6:
        return False
    elif abs(s1.LastUParameter() - s2.LastUParameter()) > 1e-6:
        return False
    elif abs(s1.LastVParameter() - s2.LastVParameter()) > 1e-6:
        return False
    elif abs(faceAreas[s1.Face()] - faceAreas[s2.Face()]) > 1e-3:
        return False

    surfaceType = s1.GetType()
    # Test Planes
    if surfaceType == GeomAbs_Plane:
        st1 = s1.Plane()
        st2 = s2.Plane()

        if st1.Axis().Angle(st2.Axis()) > 1e-6:
            return False
        return True

    # Test Cylinders
    elif surfaceType == GeomAbs_Cylinder:
        st1 = s1.Cylinder()
        st2 = s2.Cylinder()
        if st1.Axis().Angle(st2.Axis()) > 1e-6:
            return False
        elif abs(st1.Radius() - st2.Radius()) > 1e-6:
            return False
        return True

    # Test Cones
    elif surfaceType == GeomAbs_Cone:
        st1 = s1.Cone()
        st2 = s2.Cone()
        if st1.Axis().Angle(st2.Axis()) > 1e-6:
            return False
        elif abs(st1.RefRadius() - st2.RefRadius()) > 1e-6:
            return False
        elif abs(st1.SemiAngle() - st2.SemiAngle()) > 1e-6:
            return False
        return True
    # Test Spheres
    elif surfaceType == GeomAbs_Sphere:
        st1 = s1.Sphere()
        st2 = s2.Sphere()
        if abs(st1.Radius() - st2.Radius()) > 1e-6:
            return False
        return True

    # Test Toruss
    elif surfaceType == GeomAbs_Torus:
        st1 = s1.Torus()
        st2 = s2.Torus()
        if st1.Axis().Angle(st2.Axis()) > 1e-6:
            return False
        elif abs(st1.MinorRadius() - st2.MinorRadius()) > 1e-6:
            return False
        elif abs(st1.MajorRadius() - st2.MajorRadius()) > 1e-6:
            return False
        return True
    # Test BezierSurfaces
    elif surfaceType == GeomAbs_BezierSurface:
        st1 = s1.Bezier()
        st2 = s2.Bezier()
        logging.debug(st1)
        return True

    # Test BSplineSurfaces
    elif surfaceType == GeomAbs_BSplineSurface:
        st1 = s1.BSpline()
        st2 = s2.BSpline()
        if st1.NbUKnots() - st2.NbUKnots() != 0:
            return False
        elif st1.NbUPoles() - st2.NbUPoles() != 0:
            return False
        elif st1.NbVKnots() - st2.NbVKnots() != 0:
            return False
        elif st1.NbVPoles() - st2.NbVPoles() != 0:
            return False
        elif st1.UDegree() - st2.UDegree() > 1e-6:
            return False
        elif st1.VDegree() - st2.VDegree() > 1e-6:
            return False
        for ui in range(st1.NbUKnots()):
            if st1.UKnots()[ui] - st2.UKnots()[ui] > 1e-6:
                return False
        for vi in range(st1.NbVKnots()):
            if st1.VKnots()[vi] - st2.VKnots()[vi] > 1e-6:
                return False

        # Dont check poles, as poles contain location data and can vary in sequence
        return True

    # Test SurfaceOfRevolutions
    elif surfaceType == GeomAbs_SurfaceOfRevolution:
        #TODO
        return False

    # Test SurfaceOfExtrusions
    elif surfaceType == GeomAbs_SurfaceOfExtrusion:
        #TODO
        return False

    # Test OffsetSurfaces
    elif surfaceType == GeomAbs_OffsetSurface:
        #TODO
        return False

    # Test OtherSurfaces
    elif surfaceType == GeomAbs_OtherSurface:
        #TODO
        return False

    # Unknown shapes
    else:
        return False
    return True

def calcShapeDimensions(shape, tol=1e-3):
    
    # Built-in boundingbox calculation doesn't return correct result from iges files
    #t = time.time()
    boundingbox = Bnd_Box()
    boundingbox.SetGap(tol)
    brepbndlib_AddOptimal(shape, boundingbox, False)
    xmin, ymin, zmin, xmax, ymax, zmax = boundingbox.Get()
    #print(time.time() - t)

    # topo_expl = TopologyExplorer(shape)
    # xmin = 1.0e6
    # xmax = -1.0e6
    # ymin = 1.0e6
    # ymax = -1.0e6
    # zmin = 1.0e6
    # zmax = -1.0e6
    
    # for vertex in topo_expl.vertices():
    #     pnt = BRep_Tool.Pnt(vertex)
    #     xmin = min(xmin, pnt.X())
    #     xmax = max(xmax, pnt.X())
    #     ymin = min(ymin, pnt.Y())
    #     ymax = max(ymax, pnt.Y())
    #     zmin = min(zmin, pnt.Z())
    #     zmax = max(zmax, pnt.Z())

    dims = [xmax-xmin, ymax-ymin, zmax-zmin]
    dims.sort(reverse=True)
    return dims[0], dims[1], dims[2]

def calcShapeEdgeLength(shape):
    properties = GProp_GProps()
    
    brepgprop_LinearProperties(shape, properties)
    shapeEdgeLength = properties.Mass()
    
    return abs(shapeEdgeLength)

def calcShapeSurfaceArea(shape):
    properties = GProp_GProps()
    
    brepgprop_SurfaceProperties(shape, properties)
    shapeSurfaceArea = properties.Mass()
    
    return abs(shapeSurfaceArea)

def calcFacesSurfaceArea(faces, faceAreas):
    surfaceArea = 0.0
    
    for face in faces:
        surfaceArea += faceAreas[face]

    return abs(surfaceArea), len(faces)

def calcShapeVolume(shape):
    properties = GProp_GProps()
    
    brepgprop_VolumeProperties(shape, properties)
    shapeVolume = properties.Mass()
    
    return abs(shapeVolume)

def analyseShapeFaces(shape):
    t = TopologyExplorer(shape)
    shapeFaces = []
    shapeFaceAreas = {}
    shapeLinearFaces = []
    shape3DFaces = []

    for face in t.faces():
        shapeFaces.append(face)
        shapeFaceAreas[face] = calcShapeSurfaceArea(face)

        if faceIs3DMill(face):
            shape3DFaces.append(face)
        else:
            shapeLinearFaces.append(face)

    return shapeFaces, shapeFaceAreas, shapeLinearFaces, shape3DFaces

def faceIs3DMill(face):

    surface = BRepAdaptor_Surface(face, True)
    surfaceType = surface.GetType()

    if surfaceType == GeomAbs_Plane:
        plane = surface.Plane()
        planeDirection = plane.Axis().Direction()
        
        # Plane is linear face if:
        for refAxis in RefAxes:
            # Normal to any reference axis
            if refAxis.IsParallel(planeDirection, 1e-6):
                return False
            
            # 45 degree to any reference axis
            if math.isclose(refAxis.Angle(planeDirection), math.pi/4, abs_tol=1e-6):
                return False

    elif surfaceType == GeomAbs_Cylinder:
        cylinder = surface.Cylinder()
        cylinderDirection = cylinder.Axis().Direction()
        
        # Cylinder is linear face if:
        for refAxis in RefAxes:
            # Normal to any reference axis
            if refAxis.IsParallel(cylinderDirection, 1e-6):
                return False

    elif surfaceType == GeomAbs_Cone:
        cone = surface.Cone()
        coneDirection = cone.Axis().Direction()
        
        # Plane is linear face if:
        for refAxis in RefAxes:
            # Normal to any reference axis and 45Degree
            if refAxis.IsParallel(coneDirection, 1e-6) and math.isclose(cone.SemiAngle(), math.pi/4, abs_tol=1e-6):
                return False

    elif surfaceType == GeomAbs_Torus:
        torus = surface.Torus()
        torusDirection = torus.Axis().Direction()
        # Torus may be linear if axis direction is along a RefAxis:
        for refAxis in RefAxes:
            # Normal to any reference axis and 45Degree
            if refAxis.IsParallel(torusDirection, 1e-6):
                if math.isclose(torus.MinorRadius(), 1.0, abs_tol=1e-3) or math.isclose(torus.MinorRadius(), 2.0, abs_tol=1e-3) or math.isclose(torus.MinorRadius(), 3.0, abs_tol=1e-3) or math.isclose(torus.MinorRadius(), 4.0, abs_tol=1e-3) or math.isclose(torus.MinorRadius(), 5.0, abs_tol=1e-3):
                    return False
                else:
                    return True

    # Other facetypes are always 3D mill
    else:
        return True

def analyseSurfaceArea(shape):

    return 1.0, 1.0, 1.0, 1.0, 1.0, 1.0


# Stock analysis
def stockAnalysis(analysis):
    analysis["stockDimX"] = analysis["modelDimX"] + 20.0
    analysis["stockDimY"] = analysis["modelDimY"] + 20.0
    
    # Find best suited material height
    analysis["stockDimZ"] = analysis["modelDimZ"] + 12.0
    # allowedThicknesses = MaterialsThicknesses["alu-5083"]
    # for thickness in allowedThicknesses:
    #     if (analysis["modelDimZ"] + 10.0) <= thickness:
    #         analysis["stockDimZ"] = thickness
    #         break

    analysis["stockVolume"] = analysis["stockDimX"] * analysis["stockDimY"] * analysis["stockDimZ"]
    
def printStockAnalysis(analysis):
    strStockDimensions = "  Dimensions: {:0.1f}mm X {:0.1f}mm X {:0.1f}mm".format(analysis["stockDimX"], analysis["stockDimY"], analysis["stockDimZ"])
    strVolume = "  Volume: {:0.0f}mm^3".format(analysis["stockVolume"])
    print("StockAnalysis:\n{}\n{}".format(strStockDimensions, strVolume))

# Machining analysis
def machiningAnalysis(modelShape, analysis):
    #analysis["machiningJobs"] = calcShapeJobs(modelShape, analysis)
    analysis["machiningJobs"] = 2 #calcShapeJobs(modelShape, analysis) # Doesn't work
    analysis["machiningTimeBBRoughingMinutes"], analysis["machiningTimeModelRoughingMinutes"], analysis["machiningTimeFinishMinutes"], analysis["machiningTime3DMinutes"], analysis["machiningTimeFeatureMinutes"] = calcShapeMachiningTime(analysis)
    analysis["machiningTimeMinutes"] = analysis["machiningTimeBBRoughingMinutes"] + analysis["machiningTimeModelRoughingMinutes"] + analysis["machiningTimeFinishMinutes"] + analysis["machiningTime3DMinutes"] + analysis["machiningTimeFeatureMinutes"]

def printMachiningAnalysis(analysis):
    strJobs = "  Jobs: {}".format(analysis["machiningJobs"])
    strMachiningTimeBBRoughing = "  BB-Roughing: {:0.1f} minutes".format(analysis["machiningTimeBBRoughingMinutes"])
    strMachiningTimeModelRoughing = "  Model-Roughing: {:0.1f} minutes".format(analysis["machiningTimeModelRoughingMinutes"])
    strMachiningTimeFinish = "  Finish: {:0.1f} minutes".format(analysis["machiningTimeFinishMinutes"])
    strMachiningTime3d = "  3D Milling: {:0.1f} minutes".format(analysis["machiningTime3DMinutes"])
    strMachiningTimefeature = "  Feature: {:0.1f} minutes".format(analysis["machiningTimeFeatureMinutes"])
    strMachiningTime = "  Total: {:0.1f} minutes".format(analysis["machiningTimeMinutes"])
    print("MachiningAnalysis:\n{}\n{}\n{}\n{}\n{}\n{}\n{}".format(strJobs, strMachiningTimeBBRoughing, strMachiningTimeModelRoughing, strMachiningTimeFinish, strMachiningTime3d, strMachiningTimefeature, strMachiningTime))

def calcShapeJobs(modelShape, analysis):
    t = TopologyExplorer(modelShape)
    logging.debug("\nMachining Job Analysis:")
    
    
    # Detect possible millsides of each face
    millableFaces = {RefAxisZP: [], RefAxisZN: [], RefAxisXP: [], RefAxisXN: [], RefAxisYP: [], RefAxisYN: [], RefAxisUnknown: []}
    for face in t.faces():     
        faceIsMilleable = False

        for refAxis in RefAxes:
            if isFaceMillableFromAxis(face, refAxis):
                millableFaces[refAxis].append(face)
                faceIsMilleable = True
        
        if not faceIsMilleable:
            millableFaces[RefAxisUnknown].append(face)

    # Remove faces that can be milled from the axis with the most milleable area
    #milledSides = 0
    for axis in millableFaces.keys():
        logging.debug(f"{len(millableFaces[axis])} faces can be milled from {RefAxesNames[axis]}")


    # Start by milling from front and back of flattest surface first
    if analysis['modelDimX'] <= analysis['modelDimY'] and analysis['modelDimX'] <= analysis['modelDimZ']:
        # Remove faces milled from current reference axis
        milledFaces = millableFaces[RefAxisXP] + millableFaces[RefAxisXN]
        millableFaces.pop(RefAxisXP)
        millableFaces.pop(RefAxisXN)
        for face in milledFaces:
            for refAxisFaces2 in millableFaces.values():
                if (face in refAxisFaces2):
                    refAxisFaces2.remove(face)
    elif analysis['modelDimY'] <= analysis['modelDimX'] and analysis['modelDimY'] <= analysis['modelDimZ']:
        # Remove faces milled from current reference axis
        milledFaces = millableFaces[RefAxisYP] + millableFaces[RefAxisYN]
        millableFaces.pop(RefAxisYP)
        millableFaces.pop(RefAxisYN)
        for face in milledFaces:
            for refAxisFaces2 in millableFaces.values():
                if (face in refAxisFaces2):
                    refAxisFaces2.remove(face)
    elif analysis['modelDimZ'] <= analysis['modelDimX'] and analysis['modelDimZ'] <= analysis['modelDimY']:
        # Remove faces milled from current reference axis
        milledFaces = millableFaces[RefAxisZP] + millableFaces[RefAxisZN]
        millableFaces.pop(RefAxisZP)
        millableFaces.pop(RefAxisZN)
        for face in milledFaces:
            for refAxisFaces2 in millableFaces.values():
                if (face in refAxisFaces2):
                    refAxisFaces2.remove(face)
    milledSides = 2
    
    while any( len(c) > 0 for c in millableFaces.values() ):
        # for refAxis, refAxisFaces in millableFaces.items():
        #     refAxisFaceArea = 0
        #     for face in refAxisFaces:
        #         refAxisFaceArea += analysis["_modelFaceAreas"][face]
        #     logging.debug("    Faces millable from {}: {} {:0.1f}mm^2".format(RefAxesNames[refAxis], len(refAxisFaces), refAxisFaceArea))

        # Calculate total surface area of each reference axis
        refAxes = list(millableFaces.keys())
        refAxesFaces = list(millableFaces.values())
        refAxesAreas = []
        for refAxisFaces in refAxesFaces:
            area = 0.0
            for face in refAxisFaces:
                area += analysis["_modelFaceAreas"][face]
            refAxesAreas.append(area)
        
        for i in range(len(refAxesAreas)):
            logging.debug("    Area millable from {}: {} faces total {:0.1f}mm^2".format(RefAxesNames[refAxes[i]], len(refAxesFaces[i]), refAxesAreas[i]))

        for i in range(len(refAxesAreas)):
            logging.debug("    Area millable from {}: {} faces total {:0.1f}mm^2".format(RefAxesNames[refAxes[i]], len(refAxesFaces[i]), refAxesAreas[i]))

        maxAreaRefAxis = refAxes[ refAxesAreas.index(max(refAxesAreas)) ]
        maxAreaRefAxisFaces = refAxesFaces[ refAxesAreas.index(max(refAxesAreas)) ]
        # Remove current axis from axis list
        millableFaces.pop(maxAreaRefAxis)

        # Remove faces that were milled from current axis
        for face in maxAreaRefAxisFaces:
            for refAxisFaces2 in millableFaces.values():
                if (face in refAxisFaces2):
                    refAxisFaces2.remove(face)

        milledSides += 1
        logging.debug("  After {}: {} faces has not been milled yet".format(RefAxesNames[maxAreaRefAxis], "?"))
        

    return milledSides

def calcShapeMachiningTime(analysis):

    bbRoughingTimeMinutes = (analysis["stockVolume"] - (analysis["modelDimX"] * analysis["modelDimY"] * analysis["modelDimZ"]) ) * MachiningTimeBBPerVolume
    modelRoughingTimeMinutes = ((analysis["modelDimX"] * analysis["modelDimY"] * analysis["modelDimZ"]) - analysis["modelVolume"]) * MachiningTimePerVolume
    finishTimeMinutes = (analysis["modelSurfaceArea"] - analysis["model3DSurfaceArea"]) * MachiningTimePerArea
    # TODO: Base feature time on individual features and their radii
    featureTimeMinutes = 0.0
    # TODO: Base 3d milling time on individual features and their surface area
    #mill3dTimeMinutes = analysis["model3DSurfaceArea"] * MachiningTimePer3DArea
    mill3dTimeMinutes = analysis["model3DSurfaceArea"] * MachiningTimePer3DArea + analysis["model3DSurfaceCount"] * MachiningTimePer3DSurface

    logging.info(f"bbRoughingTimeMinutes: {bbRoughingTimeMinutes:0.1f}")
    logging.info(f"modelRoughingTimeMinutes: {modelRoughingTimeMinutes:0.1f}")
    logging.info(f"finishTimeMinutes: {finishTimeMinutes:0.1f}")
    logging.info(f"featureTimeMinutes: {featureTimeMinutes:0.1f}")
    logging.info(f"mill3dTimeMinutes: {mill3dTimeMinutes:0.1f}")
    
    return bbRoughingTimeMinutes, modelRoughingTimeMinutes, finishTimeMinutes, mill3dTimeMinutes, featureTimeMinutes





# Old functions
def dirIsRefAxis(direction):
    for refAxis in RefAxes:
        if (direction.IsEqual(refAxis, 1e-6)):
            return True
    
    return False

def dirIsOrthogonal(dir1, dir2, tol=1e-6):
    minAngle = math.pi/2 - tol
    maxAngle = math.pi/2 + tol
    angle = dir1.Angle(dir2)

    return (angle >= minAngle) and (angle <= maxAngle)

def isFaceMillableFromAxis(face, refAxis):
    surface = BRepAdaptor_Surface(face, True)
    surfaceType = surface.GetType()

    # Can only handle planes
    if surfaceType == GeomAbs_Plane:
        plane = surface.Plane()
        
        if plane.Axis().Direction().isEqual(refAxis, 1e-6):
            return face.Orientation()
        elif plane.Axis().Direction().IsOpposite(refAxis, 1e-6):
            return not face.Orientation()
        elif dirIsOrthogonal(plane.Axis().Direction(), refAxis):
            return True
        
    # elif surfaceType == GeomAbs_Cylinder:
    #     cylinder = surface.Cylinder()
    #     return cylinder.Axis().Direction().IsParallel(refAxis, 1e-6)
    
    return False

def axisArea(refAxis):
    axisArea = 0.0
    
    for face in faceList:
        if (face in millTops[refAxis]) or (face in millSides[refAxis]) or (face in mill3d[refAxis]):
            axisArea += faceAreas[face]
    
    return axisArea

def determineMillOrder(faces):
    # Make list of faces to be milled
    for face in faces:
        faceList.append(face)

    sortedRefAxes = RefAxes + [RefAxisUnknown]
    while (len(faceList) != 0):
        # Sort reference axes based on area they can mill    
        sortedRefAxes.sort(reverse=True, key=axisArea)

        refAxis = sortedRefAxes[0]
        print ("Milling {}".format(RefAxesNames[refAxis]))
        for face in millTops[refAxis] + millSides[refAxis] + mill3d[refAxis]:
            if (face in faceList):
                faceList.remove(face)

        print("After {}: {} faces has not been milled yet".format(RefAxesNames[refAxis], len(faceList)))
        
        # Remove current axis from axis list
        sortedRefAxes.pop(0)
    
     # modelPath = 'test-assets/Test Small.step'.lower()
    # modelPath = 'test-assets/Test-Nontouching-Squares.step'.lower()
    modelPath = 'test-assets/Price/DFM Analysis Test v1.step'.lower()
    #modelPath = 'test-assets/Price/Test2 v2.step'.lower()
    #modelPath = 'test-assets/Price/Test3 v2.step'.lower()
    #modelPath = 'test-assets/Price/Test4 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test5 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test6 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test7 v2.step'.lower()
    #modelPath = 'test-assets/Price/Test8 v3.step'.lower()
    #modelPath = 'test-assets/Price/Test9 v1.step'.lower()
    #modelPath = 'test-assets/Price/Test10 v1.step'.lower()
    #modelPath = 'test-assets/Price/Test10-scale1000 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test11.step'.lower()
    # modelPath = 'test-assets/Price/Test12.step'.lower()



if __name__ == '__main__':
    
    #modelPath = 'test-assets/DFM Analysis Test v12.step'.lower()
    #modelPath = 'test-assets/z-axis_mellemplade_SW8.IGS'.lower()
    # modelPath = 'test-assets/Test Small.step'.lower()
    # modelPath = 'test-assets/Test-Nontouching-Squares.step'.lower()
    # modelPath = 'D:/Work/ASitus/data/cad/ANC101.stp'.lower()
    #modelPath = 'test-assets/Price/Test2 v2.step'.lower()
    #modelPath = 'test-assets/Price/Test3 v2.step'.lower()
    #modelPath = 'test-assets/Price/Test4 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test5 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test6 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test7 v2.step'.lower()
    #modelPath = 'test-assets/Price/Test8 v3.step'.lower()
    #modelPath = 'test-assets/Price/Test9 v1.step'.lower()
    #modelPath = 'test-assets/Price/Test10 v1.step'.lower()
    #modelPath = 'test-assets/Price/Test10-scale1000 v2.step'.lower()
    # modelPath = 'test-assets/Price/Test11.step'.lower()
    # modelPath = 'test-assets/Price/Test12.step'.lower()

    #modelPath ='D:\Work\ASitus_Extenstions\data\dfm\Vaerks\DFM_Analysis_Test_v1.step'
    #modelPath ='D:\Work\ASitus_Extenstions\data\dfm\Vaerks\z-axis_mellemplade.step'
    #modelPath = 'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\ANC101.stp' 
    #modelPath = 'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\DFM_Analysis_Test_v2.step'
    #modelPath = 'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\gehause_rohteil.stp'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Mill\NX-Mill-1.stp'
    #modelPath = 'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\motor_adapter_v5.step'
    #modelPath = 'D:\Work\ASitus_Extenstions\data\dfm\Fractory\FR_149066__0.stp'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\01_accessibility.stp'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\02 dybt spor.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\03 skarpe hjoerner.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\04 tynd vaeg.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\05 udhaeng.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\06 Rundt udhaeng (F360).step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\20021_Milling 07-05-2020 v3.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\bugs\DFM0054_211101_Seal_V4_part1.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\bugs\DFM0054_211101_Seal_v4_part2.step'
    #modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\bugs\DFM0055_20021_Milling_07-05-2020_v3.step'
    modelPath = r'D:\Work\ASitus_Extenstions\data\dfm\Vaerks\bugs\DFM0055_Lifting_disc_2.1_-_Color_v1.step'
    
    # Load Model
    try:
        modelShape = loadPart(modelPath)
    
    # Error Handling
    except:
        raise
    
    # Continue if no exceptions were raised
    else:
        partAnalysis = partAnalysis(modelShape, display=False)
        printModelAnalysis(partAnalysis)

        #display, start_display, add_menu, add_function_to_menu = init_display()
        #display.DisplayShape(modelShape, update=True)
        #start_display()
