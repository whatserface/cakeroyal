import os
import shutil
import glob

UE4_ENGINE_PATH = r'D:\Soft\UE_4.26'
cwd = os.getcwd()
PROJECT_NAME = cwd[cwd.rfind('\\')+1:]


os.remove(os.path.join(cwd, f'{PROJECT_NAME}.sln'))
try:
    shutil.rmtree(os.path.join(cwd, 'DerivedDataCache'))
except:
    print("DerivedDataCache folder doesn't exist")

try:
    shutil.rmtree(os.path.join(cwd, '.vs'))
except:
    print(".vs folder doesn't exist")

try:
    shutil.rmtree(os.path.join(cwd, 'Binaries'))
except:
    print("Binaries folder doesn't exist", end='\n\n')

for fold in ('Intermediate', 'Saved'):
    work_dir = os.path.join(cwd, fold)
    for file in glob.iglob(f'{work_dir}/*'):
        if file != os.path.join(work_dir, 'Config'):
            if os.path.isdir(file):
                shutil.rmtree(file)
            else:
                os.remove(file)

os.system(rf'{UE4_ENGINE_PATH}\Engine\Binaries\DotNET\UnrealBuildTool.exe -projectfiles -project="{os.getcwd()}\{PROJECT_NAME}.uproject" -game -rocket -progress')