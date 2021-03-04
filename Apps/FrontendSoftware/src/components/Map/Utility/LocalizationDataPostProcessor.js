export function getReorganizedList(oldList, newList) {
    if (oldList.length === 0) {
        return newList;
    }

    let lastElem = newList[newList.length - 1];

    // If the new last elem is in the right position: 
    if (lastElem.data.collectionTime > oldList[oldList.length - 1].data.collectionTime) {
        oldList.push(lastElem);
        return oldList;
    }

    // Else do a binary search to find the right index
    let res = binaryFind.call(oldList, lastElem);
    if (!res.found) {
        oldList.splice(res.index, 0, lastElem);
    } else {
        oldList.push(lastElem);
    }

    return oldList;
}

// https://stackoverflow.com/questions/20351959/how-to-keep-javascript-array-sorted-without-sorting-it/20352387#20352387
function binaryFind(searchElement) {
    'use strict';

    var minIndex = 0;
    var maxIndex = this.length - 1;
    var currentIndex;
    var currentElement;

    while (minIndex <= maxIndex) {
        currentIndex = (minIndex + maxIndex) / 2 | 0;
        currentElement = this[currentIndex];

        if (currentElement.data.collectionTime < searchElement.data.collectionTime) {
            minIndex = currentIndex + 1;
        }
        else if (currentElement.data.collectionTime > searchElement.data.collectionTime) {
            maxIndex = currentIndex - 1;
        }
        else {
            return { // Modification
                found: true,
                index: currentIndex
            };
        }
    }      

    return { // Modification
        found: false,
        index: currentElement < searchElement ? currentIndex + 1 : currentIndex
    };
}