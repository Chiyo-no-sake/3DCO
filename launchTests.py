from os import listdir
from os.path import isfile, join
import subprocess
import time
import pyautogui
import pygetwindow
import PIL
import os

# Specify formats directories here
baseDir = join('tests', 'assets')
formatsDirs = ['dae', 'fbx', 'gltf', 'obj']

def findOvoViewerWindow():
  allwindows = pygetwindow.getAllTitles()
  for w in allwindows:
    if("OVO viewer" in w):
      return pygetwindow.getWindowsWithTitle(w)[0]
  
  return None

def takeScreenshot(window, outFile):
  x2,y2 = pyautogui.size()
  x2,y2=int(str(x2)),int(str(y2))
  x3 = x2 // 2
  y3 = y2 // 2
  window.resizeTo(x3,y3)
  # top-left
  window.moveTo(0, 0)
  window.activate()
  time.sleep(1)
  p = pyautogui.screenshot()
  p.save("tmp.png")
  im = PIL.Image.open('tmp.png')
  im_crop = im.crop((0, 0, x3, y3))
  im_crop.save(outFile + ".png", quality=100)
  os.remove("tmp.png")

def main():
  #./3DC0 -v1 assets/dae/1.dae 
  print("Starting 3DCO tests")

  #gather executable
  executablePath = join('bin','Release')
  executableName = '3DCO.exe'
  executableFile = join(executablePath, executableName)
  if(not isfile(executableFile)):
    raise "3DCO must be compiled in release mode in order to execute tests"
  
  for f in formatsDirs:
    # for each directory
    path = join(baseDir, f)
    sceneFiles = [f for f in listdir(path) if isfile(join(path, f))]
    print("Testing " + f + " files")
    for file in sceneFiles:
      if(file.endswith("OVO")):
        continue

      filePath = join(path, file)
      command = executableFile + " " + "-v2 " + join('..','..',filePath)
      print("Running " + command)

      converterProcess = subprocess.Popen(args = command, cwd = executablePath)
      exitCode = converterProcess.wait()
      
      if(exitCode != 0):
        print("Error occured converting file", filePath)
        continue

      print("File has been converted")
      print("Starting ovoviewer to check for correctness")

      command = join('tests', 'vendor','ovoviewer.exe')
      targetOVO = join(join('..','..',path),file.replace(f, "OVO"))
      print("Executing: " + command + " " + targetOVO)

      viewerProcess = subprocess.Popen([command, targetOVO], cwd = join('tests', 'vendor'))
      time.sleep(3)

      # get window and take screenshot
      ovoWindow = findOvoViewerWindow()
      if(ovoWindow == None):
        print("Ovoviewer crashed")
        continue
      
      if not os.path.exists(join('tests', 'screenshots')):
        os.makedirs(join('tests', 'screenshots'))

      takeScreenshot(ovoWindow, join('tests', 'screenshots', file.replace(f, "OVO")))

      if(viewerProcess.poll() == None):
        viewerProcess.terminate()
      elif(viewerProcess.poll() != 0):
        print("Ovoviewer crashed")
      
      print("Ovoviewer read correctly the file " + targetOVO)
      

if __name__ == "__main__":
    main()

