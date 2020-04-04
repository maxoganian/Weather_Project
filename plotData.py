"""
Python module for parsing and plotting values found in the
logs created by the Weather_Project.

Installation:
    (install python 3)
    python -m venv weather-env
    source weather-env/bin/activate
    pip install matplotlib

Usage:
    source weather-env/bin/activate
    python plotData.py <path/to/logfile.txt>
"""

import matplotlib.pylab as plt

def parseDataLines(key, lines):
    "Take lines from file and return just numerical data we want"

    # get only the lines we care about, getting rid of the
    # trailing '\n'
    dataLines = [l[:-1] for l in lines if key in l]

    # Ex: 'moisture1: 34.0' -> 34.0
    # get the actual value:
    data = [int(l.split(':')[1].strip()) for l in dataLines]
    
    return data

def plotDataLines(key, lines):
    "Plots data that begins with given key in given lines from file."

    data = parseDataLines(key, lines)

    print("Plotting %s with %d points" % (key, len(data)))

    # now plot it
    f = plt.figure()
    ax = f.gca()
    ax.plot(data)
    title = key.replace(" ", "_")
    plt.title(key)
    plt.savefig(key)

def plotData(filename):
    "Read given file and plot each type of data."

    with open(filename, 'r') as f:
        ls = f.readlines()

    # data looks like this:    
    #------------100
    #.7
    #isTooDry: 1
    #Moisture 1: 118
    #Too Dry Value 1: 146
    #Moisture 2: 121
    #Too Dry Value 2: 155

    keys = [
        "Moisture 1",
        "Moisture 2",
        "Too Dry Value 1",
        "Too Dry Value 2",
        "isTooDry",
    ]
    for key in keys:
        plotDataLines(key, ls)

def main():
    import sys
    filename = sys.argv[1]
    plotData(filename)

if __name__ == '__main__':
    main()
