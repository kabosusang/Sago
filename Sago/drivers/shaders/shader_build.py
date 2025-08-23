import os
import subprocess
import sys
import argparse

def compile_glsl_to_spv(entry_point = "main"):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Check glslc
    try:
        subprocess.run(["glslc.exe", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except Exception:
        print("ERROR: No Finnd glslc.exe, Make Sure VulkanSDK Add PATH")
        return
    
    stage_mapping = {
        "_vert.": ("vertex", "-fshader-stage=vertex"),
        "_frag.": ("fragment", "-fshader-stage=fragment"),
        "_comp.": ("compute", "-fshader-stage=compute"),
    }

    for root, dirs, files in os.walk(script_dir):
        for file in files:
            if file.endswith(".glsl"):
                glsl_path = os.path.join(root, file)
                
                stage_flag = None
                for suffix, (stage, flag) in stage_mapping.items():
                    if suffix in file:
                        stage_flag = flag
                        break
                
                if not stage_flag:
                    print(f"Warring: No Find {file} Type, Skip~")
                    continue
                
                spv_path = os.path.splitext(glsl_path)[0] + ".spv"
                print(f"Compile: {glsl_path} -> {spv_path}")
                
                try:
                    subprocess.run([
                        "glslc.exe",
                        stage_flag,
                        glsl_path,
                        "-o", spv_path,
                        f"-fentry-point={entry_point}", 
                        "--target-env=vulkan1.4"
                    ], check=True)
                except subprocess.CalledProcessError as e:
                    print(f"ERROR (CODE: {e.returncode}): {file}")
                   
if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='GLSL Shader Compiler')
    parser.add_argument('-e', '--entry', type=str, default='main', 
                       help='Shader entry point name (default: main)')
    parser.add_argument('-v', '--version', action='version', version='GLSL Compiler 1.0')

    args = parser.parse_args()

    print("GLSL Compile")
    print("=" * 50)
    print(f"Using entry point: {args.entry}")
    compile_glsl_to_spv(args.entry)
    print("=" * 50)
    input("Enter Quit...")