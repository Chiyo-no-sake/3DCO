if __name__ == "__main__":
    main()

def main():
  #./3DC0 -v1 assets/dae/1.dae 
  print("Starting test scenes conversion")

  #gather directories
  baseDir = 'tests/assets'
  formatsDirs = ['dae', 'fbx', 'gltf', 'obj']
  