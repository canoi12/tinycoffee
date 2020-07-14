foreign class Sound {
  construct load(filename) {}
  construct load(filename, type) {}
  foreign play()
  foreign stop()
  foreign pause()
  foreign isPlaying
  foreign isPaused
}

foreign class Audio {
  foreign static setMasterVolume(volume)
  foreign static startDevice()
  foreign static stopDevice()
  foreign static STREAM
  foreign static STATIC
}
