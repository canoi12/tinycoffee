foreign class Filesystem {
  foreign static read(filename)
  foreign static write(filename, text)
  foreign static fileExists(filename)
  foreign static getPath()
  foreign static resolvePath(name)
  foreign static mkdir(name)
  foreign static rmdir(name)
}
