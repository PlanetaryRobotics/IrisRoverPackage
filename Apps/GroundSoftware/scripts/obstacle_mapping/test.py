def main():
    img = np.array([[".",1,e,e,e], [1,1,1,e,e], [1,1,1,e,2], [1,1,e,2,2]])
    img2 = np.array([[4, 4, e, e, e, 1, 1, 1, e, e, 6, 6],
                     [4, 4, 4, e, 1, 1, 1, 1, e, e, 6, 6],
                     [e, 4, 4, e, e, 1, 1, 1, e, e, e, 6],
                     [e, 4, 2, 2, e, e, 1, 1, e, e, e, e],
                     [e, e, 2, 2, 2, e, e, e, e, e, e, e],
                     [3, 3, 3, 2, 2, e, e, e, 5, 5, e, e],
                     [3, 3, 3, 3, e, e, e, 5, 5, 5, 5, e],
                     [3, 3, 3, 3, e, e, e, 5, 5, 5, 5, 5],
                     [3, 3, 3, 3, 3, e, e, 5, 5, 5, 5, 5],
                     [e, 3, 3, 3, 3, 3, e, e, 5, 5, 5, 5],
                     [e, 3, 3, 3, 3, e, e, e, e, 5, 5, e],
                     [e, 3, 3, 3, e, e, e, e, e, e, e, e]])
    test = np.array([[e,e,1,e,e], [e,1,1,1,e], [e,e,1,e,e]])

    for row in img2:
        for p in row:
            print(p, end=" ")
        print("")
    print("")
    updateObst(img2)
    print("Num of Obst: ", getObstNum())
    printObstIDs(getObstList())
    printObstPoints(getObstList())
    drawEdges(img2)

    

if __name__ == "__main__":
    main()