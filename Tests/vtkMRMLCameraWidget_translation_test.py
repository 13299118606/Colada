# Get all necesssary objects
threeDViewWidget = slicer.app.layoutManager().threeDWidget(0)
view = threeDViewWidget.threeDView()
viewNode = view.mrmlViewNode()
renderer = view.renderWindow().GetRenderers().GetItemAsObject(0)
camera = renderer.GetActiveCamera()
cameraDisplayableManager = view.displayableManagerByClassName("vtkMRMLCameraDisplayableManager")
cameraWidget = cameraDisplayableManager.GetCameraWidget()


# Automatically send events to make a translation like user usually do interactively
def do_translate():
    # Process CLICK ON (click mouse 1 btn)
    ev = slicer.vtkMRMLInteractionEventData()
    ev.SetRenderer(renderer)
    ev.SetViewNode(viewNode)
    ev.SetType(12)
    ev.SetKeySym("Shift_L")
    ev.SetModifiers(1)	# Shift btn
    ev.SetMouseMovedSinceButtonDown(False)
    ev.SetWorldPosition([0, 0, 0]) 
    ev.SetDisplayPosition([0, 0])
    cameraWidget.ProcessInteractionEvent(ev)
    
    # Process DRAG
    ev = slicer.vtkMRMLInteractionEventData()
    ev.SetRenderer(renderer)
    ev.SetViewNode(viewNode)
    ev.SetType(26)
    ev.SetKeySym("Shift_L")
    ev.SetModifiers(1)	# Shift btn
    ev.SetMouseMovedSinceButtonDown(True)
    ev.SetWorldPosition([0, 0, 10])
    ev.SetDisplayPosition([0, 100])
    cameraWidget.ProcessInteractionEvent(ev)
    
    # Process CLICK OFF (release the mouse 1 btn)
    ev = slicer.vtkMRMLInteractionEventData()
    ev.SetRenderer(renderer)
    ev.SetViewNode(viewNode)
    ev.SetType(13)
    ev.SetKeySym("Shift_L")
    ev.SetModifiers(1)	# Shift btn
    ev.SetMouseMovedSinceButtonDown(True)
    ev.SetWorldPosition([0, 0, 10])
    ev.SetDisplayPosition([0, 100])
    cameraWidget.ProcessInteractionEvent(ev)


# list_out = list_a - list_b
def substract_lists(a: list, b: list) -> list:
    if len(a) != len(b):
        return
    out = [0] * len(a)
    for i in range(0, len(a)):
        out[i] = a[i]-b[i]
    return out


# Calculate `delta_cam_pos` before changing aspect ratio
cam_pos_old = camera.GetPosition()
do_translate()
cam_pos_new = camera.GetPosition()
delta_cam_pos = substract_lists(cam_pos_new, cam_pos_old)

# Change aspect ratio
transform = vtk.vtkTransform()
transform.Scale(1, 1, 10)
camera.SetModelTransformMatrix(transform.GetMatrix())

# Calculate `delta_translated_cam_pos` after changing aspect ratio
translated_cam_pos_old = camera.GetPosition()
do_translate()
translated_cam_pos_new= camera.GetPosition()
delta_translated_cam_pos = substract_lists(translated_cam_pos_new, translated_cam_pos_old)


# return `True` if `list_a` contains almost the same elements as `list_b`
def compare_lists(a: list, b: list) -> bool:
    if len(a) != len(b):
        return
    from math import isclose
    out = True
    for i in range(0, len(a)):
        out = out and isclose(a[i], b[i])
    return out


# Delta translations calculated before scaling camera and after that should be equal
compare_lists(delta_cam_pos, delta_translated_cam_pos)