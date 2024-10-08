#!/usr/bin/python3

import PyApplicationCore

import time

class MyMod(PyApplicationCore.ApplicationModule) :

  myOutput = PyApplicationCore.ScalarAccessor(PyApplicationCore.DataType.int32, "/Var1")
  myInput = PyApplicationCore.ScalarAccessor(PyApplicationCore.DataType.float32, "/Var2")

  def mainLoop(self) :
    val = 0
    while True:
      print(self.getName() + "::mainLoop()")
      time.sleep(0.25)
      self.myOutput.setAndWrite(val)
      val = self.myInput.readAndGet()
      print(self.getName() + " got: " + str(val))


class MyMod2(PyApplicationCore.ApplicationModule) :

  myInput = PyApplicationCore.ScalarAccessor(PyApplicationCore.DataType.int32, "/Var1")
  myOutput = PyApplicationCore.ScalarAccessor(PyApplicationCore.DataType.float32, "/Var2")

  def mainLoop(self) :
    while True:
      print(self.getName() + "::mainLoop()")
      val = self.myInput.readAndGet()
      print(self.getName() + " got: " + str(val))
      self.myOutput.setAndWrite(val + 1.2)
      if val == 15 :
        print(thisDoesNotExist)


mod = MyMod("SomeName")
mod2 = MyMod2("SomeName2")
