import math
import numpy as np

obstacles = [] #list of all obstacles

e = '.' #e : empty pixal

#0 = e pixel, # = obstacle ID


def getObstList():
    return obstacles

def getObstNum():
    return len(obstacles)

def printObstIDs(obstacles):
    print("Obst IDs:", end=" ")
    for o in obstacles:
        print(o.getID(), end=" ")
    print();

def printObstPoints(obstacles):
    for o in obstacles:
        print(o.getID(), ": ", end="")
        for p in o.getPoints():
            print(p, end=" ")
        print("");
            

def isValidPxl(img, x, y):
    return (x >= 0) & (x < img.shape[0]) & (y >= 0) & (y < img.shape[1])

def isEdge(img, x, y):
    if img[x][y] == e: return False
   
    if not (isValidPxl(img, x-1, y) & isValidPxl(img, x+1, y) & isValidPxl(img, x, y-1) & isValidPxl(img, x, y+1)):
        return True

    if (img[x-1][y] != img[x][y]) | (img[x+1][y] != img[x][y]) | (img[x][y-1] != img[x][y]) | (img[x][y+1] != img[x][y]):
        return True

    return False

def updateObst(img):
    for row in img:
        for p in row:
            if(p != e):
                newObst = True
                for o in obstacles:
                    if p == o.getID(): 
                        newObst = False
                        break
                if(newObst): 
                    obstacles.append(Obst(p))
                    print(obstacles[-1].getID())

    for x in range(len(img)):
        for y in range(len(img[0])):
            if isEdge(img, x, y):
                for o in obstacles:
                    if img[x][y] == o.getID():  
                        o.addPoint(x, y)

def drawEdges(img):
    edgeImg = np.full(img.shape, e, dtype=str)
    for o in obstacles:
        for p in o.getPoints():
            edgeImg[p[0]][p[1]] = o.getID()
    for row in edgeImg:
        for p in row:
            print(p, end=" ")
        print("")
    return edgeImg
            
