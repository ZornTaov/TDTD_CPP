# MTL File Reader
# (c) 2020 Atom
# Reads the entries of an .MTL file and generates a material linked to the image maps.
# 10/01/20

import unreal
import os, re, platform

def returnValidUnrealMaterialName(passedItem):
    # Replace any illegal characters for names here.
    s = re.sub("[^0-9a-zA-Z\.]+", "_", passedItem)
    s = s.replace(".","_")
    return s

def importListOfImageMaps(passedList, passedTargetFolder):
    lst_texture2D = []
    data = unreal.AutomatedAssetImportData()
    data.set_editor_property('destination_path', passedTargetFolder) # Unreal game folder.
    data.set_editor_property('filenames', passedList)
    lst_texture2D = unreal.AssetToolsHelpers.get_asset_tools().import_assets_automated(data)
    return lst_texture2D

def returnImageListByToken(passedLines, passedToken, passedTexturePath=""):
    # Collect list of image files associated with this MTL file.
    lst_images = []
    for line in passedLines:
        s = line.lstrip() # Remove leading TAB or spacing if present.
        ary = s.split(' ') # Split by spaces.
        if ary[0] == passedToken:
            s = os.path.join(passedTexturePath, ary[1])
            lst_images.append(s)
    return lst_images

def createNewImageMapOpaqueMaterial(passedAssetPath, passedMaterialName, passedDiffuseTexture, passedNormalTexture, passedDiffuseColor, passedSpec, passedRough):
    # Create a material.
    mat_closure = None
    if unreal.EditorAssetLibrary.does_asset_exist(passedAssetPath):
        print("%s Material already exists, Recreating." % passedMaterialName)
        mat_closure = unreal.EditorAssetLibrary.load_asset(passedAssetPath)
        unreal.MaterialEditingLibrary.delete_all_material_expressions(mat_closure)
    else:
	    assetTools = unreal.AssetToolsHelpers.get_asset_tools()
	    mf = unreal.MaterialFactoryNew()
	    mat_closure = assetTools.create_asset(passedMaterialName, passedAssetPath, unreal.Material, mf)
	    if mat_closure == None:
	    	print("%s Material already exists, skipping." % passedMaterialName)
	    	return
    
    # Make a texture diffuse node.
    if passedDiffuseTexture!=None:
        # Add an image node.
        ts_node_diffuse = unreal.MaterialEditingLibrary.create_material_expression(mat_closure,unreal.MaterialExpressionTextureSample,-384,-200)
        ts_node_diffuse.texture = passedDiffuseTexture
        unreal.MaterialEditingLibrary.connect_material_property(ts_node_diffuse, "RGBA", unreal.MaterialProperty.MP_BASE_COLOR)
    else:
        # Add a color constant node.
        ts_node_diffuse = unreal.MaterialEditingLibrary.create_material_expression(mat_closure,unreal.MaterialExpressionConstant3Vector,-384,-200)
        value = unreal.LinearColor(float(passedDiffuseColor[0]),float(passedDiffuseColor[1]),float(passedDiffuseColor[2]),1.0)
        ts_node_diffuse.set_editor_property("constant", value)
        unreal.MaterialEditingLibrary.connect_material_property(ts_node_diffuse, "", unreal.MaterialProperty.MP_BASE_COLOR)
        
    # Make a texture normal node.
    if passedNormalTexture!=None:
        ts_node_normal = unreal.MaterialEditingLibrary.create_material_expression(mat_closure,unreal.MaterialExpressionTextureSample,-384,200)
        unreal.MaterialEditingLibrary.connect_material_property(ts_node_normal, "RGB", unreal.MaterialProperty.MP_NORMAL)
        # Change this sampler color sample type to work with normal types.
        ts_node_normal.sampler_type = unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL
        ts_node_normal.texture = passedNormalTexture
    
    # Make a constant float node.
    ts_node_roughness = unreal.MaterialEditingLibrary.create_material_expression(mat_closure,unreal.MaterialExpressionConstant,-125,150)
    ts_node_roughness.set_editor_property('R', passedRough)
    unreal.MaterialEditingLibrary.connect_material_property(ts_node_roughness, "", unreal.MaterialProperty.MP_ROUGHNESS)

    # Make a constant float node.
    ts_node_specular = unreal.MaterialEditingLibrary.create_material_expression(mat_closure,unreal.MaterialExpressionConstant,-125,50)
    ts_node_specular.set_editor_property('R', passedSpec)
    unreal.MaterialEditingLibrary.connect_material_property(ts_node_specular, "", unreal.MaterialProperty.MP_SPECULAR)
    
    unreal.MaterialEditingLibrary.recompile_material(mat_closure)

# Program begins here.
asset_path = "/Game"
material_path = "%s/Geometry/Meshes/ModularAssets/Modular_Terrain_Hilly/Materials" % asset_path
texture_path = "%s/Geometry/Meshes/ModularAssets/Modular_Terrain_Hilly/Textures" % asset_path

# Select which disk file to scan.
import glob

mtlFiles = glob.glob('D:/Dev/3d/Modular Assets/Modular Terrain Hilly/*.mtl')
for mtl in mtlFiles:
    # file_path = r'D:/Dev/3d/Modular Assets/Modular Terrain Hilly'
    # file_name = "%s/%s" % (file_path, "/Grass_Flat.mtl")
    with open(mtl, 'r') as f:
        lines = f.read().splitlines() # Remove slash n character at the end of each line.
    f.close()
    
    print("scanning %s" % mtl)
    # Collect list of image files associated with this MTL file.
    lst_result = returnImageListByToken(lines, "map_Kd", mtl)
    lst_diffuse_maps = list(dict.fromkeys(lst_result)) #Remove doubles.
    lst_textures_diffuse = importListOfImageMaps(lst_diffuse_maps, texture_path)
    
    lst_result = returnImageListByToken(lines, "map_Kn", mtl)
    lst_normal_maps = list(dict.fromkeys(lst_result)) #Remove doubles.
    lst_textures_normal = importListOfImageMaps(lst_normal_maps, texture_path)
    
    # Defaults
    mat_name = ""
    material_pending = False
    opacity = 1.0
    ior = 1.0
    reflection_weight = 0.1
    reflection_roughness = 0.23
    diffuse_color = [0,0,0]
    specular_color = [0,0,0]
    map_diffuse = ""
    map_normal = ""
    
    for line in lines:
        s = line.lstrip() # Remove leading TAB or spacing if present.
        ary = s.split(' ') # Split by spaces.
        if ary[0] == 'newmtl':
            if material_pending:
                # We encountered a new material, but have a pending one we need to generate.
                shader_name = returnValidUnrealMaterialName("%s" % mat_name)
                print("Detected material [%s]" % shader_name)
                print("diff color[%s]." % diffuse_color)
                print("map diff[%s]." % map_diffuse)
                print("map nor[%s]." % map_normal)
                
                txture_diffuse = None # If there is no map, a solid color will be used instead.
                if len(map_diffuse) > 0:
                    # Search for map_diffuse texture.
                    for i,entry in enumerate(lst_diffuse_maps):
                        if entry.find(map_diffuse)!=-1:
                            # Found a match, pass texture to material creation routine.
                            txture_diffuse = lst_textures_diffuse
                            break
                
                createNewImageMapOpaqueMaterial(material_path, shader_name, txture_diffuse, None, diffuse_color, 0.1, 0.5)
                
                # Reset defaults.
                material_pending = False
                opacity = 1.0
                ior = 1.0
                reflection_weight = 0.1
                reflection_roughness = 0.23
                diffuse_color = [0,0,0]
                specular_color = [0,0,0]
                map_diffuse = ""
                map_normal = ""
        
            mat_name = ary[1] # Kind of assuming that only one space will be between the token and the value.
            material_pending = True
        
        if ary[0] == 'Ks':
            specular_color = [ary[1],ary[2],ary[3]]
        if ary[0] == 'Kd':
            diffuse_color = [ary[1],ary[2],ary[3]]
        if ary[0] == 'Ns':
            reflection_weight = float(ary[1])/100
        if ary[0] == 'Ni':
            ior = ary[1]
        if ary[0] == 'd':
            opacity = ary[1]
        if ary[0] == 'map_Kd':
            # Found a diffuse map.
            map_diffuse = ary[1]
        if ary[0] == 'map_Kn':
            # Found a normal map.
            map_normal = ary[1]
        
    # Write out the last material if it is still pending.
    if material_pending:
        # We encountered a new material, but have a pending one we need to generate.
        shader_name = returnValidUnrealMaterialName("%s" % mat_name)
        print("Detected material [%s]" % shader_name)
        print("diff color[%s]." % diffuse_color)
        print("map diff[%s]." % map_diffuse)
        print("map nor[%s]." % map_normal)
    
        txture_diffuse = None # If there is no map, a solid color will be used instead.
        if len(map_diffuse) > 0:
            # Search for map_diffuse texture.
            for i,entry in enumerate(lst_diffuse_maps):
                if entry.find(map_diffuse)!=-1:
                    # Found a match, pass texture to material creation routine.
                    txture_diffuse = lst_textures_diffuse
                    break
        
        createNewImageMapOpaqueMaterial(material_path, shader_name, txture_diffuse, None, diffuse_color, 0.1, 0.5)
    print("MTL file scanned.")