#!/usr/bin/python3

import PyApplicationCore

import time

class MyMod(PyApplicationCore.ApplicationModule) :
  def mainLoop(self) :
    while True:
      print(self.getName() + "::mainLoop()")
      time.sleep(1.1)
      self.interruption_point()


class MyMod2(PyApplicationCore.ApplicationModule) :
  def mainLoop(self) :
    for i in range(1,5):
      print(self.getName() + "::mainLoop(): " + str(i))
      time.sleep(1.2)
      self.interruption_point()

    print(thisDoesNotExist)


mod = MyMod("SomeName")
mod2 = MyMod2("SomeName2")
