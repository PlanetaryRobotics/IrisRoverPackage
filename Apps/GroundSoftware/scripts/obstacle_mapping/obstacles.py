import math
import numpy as np

class Obst:
    def __init__(self, id):
        self.id = id
        self.points = []

    def getID(self):
        return self.id
    
    def getPoints(self):
        return self.points

    def pointIsIn(self, x, y):
        for p in self.points:
            if(x, y == p[0], p[1]): return True
        return False

    def addPoint(self, x, y):
        self.points.append([x,y])
